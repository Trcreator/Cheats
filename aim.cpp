#include <iostream>
#include <Windows.h>
#include "MemMan.h"
//https://guidedhacking.com/threads/codys-c-csgo-triggerbot-tutorial-external-part1.11401/
//https://guidedhacking.com/donate

#define KeyDOWN -32768
#define KeyUP 0

MemMan MemClass;

struct offsets
{
    DWORD localPlayer = 0xC618AC;
    DWORD forceLMB = 0x307FCF4;
    DWORD entityList = 0x4C3E624;
    DWORD crosshair = 0xB2DC;
    DWORD team = 0xF0;
    DWORD health = 0xFC;
} offset;

struct variables
{
    DWORD localPlayer;
    DWORD gameModule;
    int myTeam;
} val;

void shoot()
{
    MemClass.writeMem<int>(val.gameModule + offset.forceLMB, 5);
    Sleep(20);
    MemClass.writeMem<int>(val.gameModule + offset.forceLMB, 4);
}

bool checkTBot()
{
    int crosshair = MemClass.readMem<int>(val.localPlayer + offset.crosshair);
    if (crosshair != 0 && crosshair < 64)
    {
        DWORD entity = MemClass.readMem<DWORD>(val.gameModule + offset.entityList + ((crosshair - 1) * 0x10));
        int eTeam = MemClass.readMem<int>(entity + offset.team);
        int eHealth = MemClass.readMem<int>(entity + offset.health);
        if (eTeam != val.myTeam && eHealth > 0)
            return true;
        else
            return false;
    }
    else
        return false;
}

void handleTBot()
{
    if (checkTBot())
        shoot();
}

int main()
{
    bool canTBot = false, keyHeld = false;
    int proc = MemClass.getProcess("csgo.exe");

    val.gameModule = MemClass.getModule(proc, "client.dll");
    val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + offset.localPlayer);

    if (val.localPlayer == NULL)
        while (val.localPlayer == NULL)
            val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + offset.localPlayer);

    while (true)
    {
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            val.myTeam = MemClass.readMem<int>(val.localPlayer + offset.team);
            canTBot = !canTBot;
        }

        if (GetAsyncKeyState(VK_XBUTTON1) == KeyDOWN && !keyHeld)
        {
            keyHeld = true;
            canTBot = true;
        }
        if (GetAsyncKeyState(VK_XBUTTON1) == KeyUP && keyHeld)
        {
            keyHeld = false;
            canTBot = false;
        }

        if (canTBot)
            handleTBot();

        Sleep(1);
    }
    return 0;
}