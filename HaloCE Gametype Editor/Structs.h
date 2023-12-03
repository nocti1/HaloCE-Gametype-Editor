#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H
#include <Windows.h>
struct Gametype
{
    // Gametype name string
    wchar_t Name[24];

    // Game type 1 = CTF, 2 = Slayer, 3 = Oddball, 4 = King, 5 = Race
    DWORD GameMode;

    // Team Play (0/1)
    DWORD TeamPlay;

    // Parameters binary (0/1): 8 bits
    // 0 = Other players on radar, 1 = Friend indicators on screen, 2 = Infinite grenades, 3 = No shields, 4 = Invisible players
    // 5 = Starting equipment (1 = generic, 0 = custom), 6 = Only friends on radar, 7 = Unknown
    DWORD Parameters;

    // Navigation objectives tracker (0 = motion tracker, 1 = nav points, 2 = none)
    DWORD NavIndicator;

    // Odd man out (0/1)
    DWORD OddManOut;

    // Respawn time growth in ticks (0 = off, 150 = 5 sec, 300 = 10 sec, 450 = 15 sec)
    DWORD RespawnTimeGrowth;

    // Respawn time in ticks (0 = off, 150 = 5 sec, 300 = 10 sec, 450 = 15 sec)
    DWORD RespawnTime;

    // Suicide penalty time in ticks (0 = off, 150 = 5 sec, 300 = 10 sec, 450 = 15 sec)
    DWORD SuicidePenalty;

    // Number of lives for elimination mode (0 = unlimited/off, 1 = 1 life, 3 = 3 lives, 5 = 5 lives)
    DWORD NumLives;

    // Health
    float Health;

    // Score limit (Kills for Slayer, Captures for CTF, Laps for Race, Time for King and Oddball)
    DWORD ScoreLimit;

    // Weapons set (0 = normal, 1 = pistols, 2 = rifles, 3 = plasma, 4 = snipers, 5 = no sniping, 6 = rockets,
    //              7 = shotguns, 8 = short range, 9 = human, 10 = covenant, 11 = classic, 12 = heavy)
    DWORD WeaponSet;

    // Some bitfield magic going on
    DWORD RedVehicles;

    // Some bitfield magic going on
    DWORD BlueVehicles;

    // Vehicle respawn time
    DWORD VehicleRespawnTime;

    // Friendly fire (0/1)
    DWORD FriendlyFire;

    // Friendly fire penalty in ticks (0 = off, 150 = 5 sec, 300 = 10 sec, 450 = 15 sec)
    DWORD FriendlyFirePenalty;

    // Team balance (0/1)
    DWORD TeamBalance;

    // Time limit in ticks (0 = no limit, 18000 = 10 min, 27000 = 15 min, 36000 = 20 min, 45000 = 25 min, 54000 = 30 min, 81000 = 45 min)
    DWORD TimeLimit;

    // Passing over some unknown/too complicated
    DWORD Unknown2[7];

    // "Old CRC-32 1s complement of the first 0×098 bytes in the file"
    DWORD OldCRC;

    // Gametype modifier
    // CTF: assault (0/1)
    // King: moving hill (0/1)
    // Slayer: death bonus (0/1)
    byte GtMod1;

    // Gametype modifier
    // Slayer: kill penalty (0/1)
    byte GtMod2;

    // Gametype modifier
    // CTF: flag must reset (0/1)
    // Slayer: kill in order (0/1)
    byte GtMod3;

    // Gametype modifier
    // CTF: flag at home to score (0/1)
    byte GtMod4;

    // Gametype modifier
    // CTF: single flag (0 = off, 1800 = 1 min, 3600 = 2 min, 5400 = 3 min, 9000 = 5 min, 18000 = 10 min)
    // Race: team scoring (0 = min, 1 = max, 2 = sum)
    // Oddball: speed with ball (0 = slow, 1 = normal, 2 = fast)
    DWORD GtMod5;

    // Some extra shit for Oddball
    DWORD Unknown4[13];

    // "CRC-32 1s CRC of the above information with 1s complement."
    DWORD NewCRC;
};
#endif /* STRUCTS_H */