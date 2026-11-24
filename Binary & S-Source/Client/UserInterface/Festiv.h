#pragma once

#include <Python27/bitset.h>

#ifndef __INC_PACKET_FESTIV_H__
#define __INC_PACKET_FESTIV_H__
#pragma pack(push, 1)
enum EFestivalGuildHeader
{
	HEADER_CG_FESTIVAL = 115,
	HEADER_GC_FESTIVAL
};

enum EFestivalGuildSubHeader
{
	FEST_GUILD_OPEN = 0,
	FEST_GC_MISSION_LIST,
	FEST_CG_ACCEPT_MISSION,
	FEST_GC_GUILD_STATE,
	FEST_CG_CLAIM_PENDING
};

enum EFestivalMissionStatus
{
	FEST_MISSION_AVAILABLE = 0,
	FEST_MISSION_IN_PROGRESS = 1,
	FEST_MISSION_COMPLETED = 2,
	FEST_MISSION_FAILED = 3
};


enum EFestivalMissionListStatus
{
    FESTIVAL_LIST_ACTIVE = 0,
    FESTIVAL_LIST_UNAVAILABLE = 1
};

#pragma pack(push, 1)
typedef struct SPacketCGFestival
{
	BYTE header;
	BYTE subheader;
	DWORD template_id;
}TPacketCGFestivGuild;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SPacketGCFestival
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCFestival;
#pragma pack(pop)

typedef struct SPacketGCFestivalMissionList
{
    BYTE status;
    WORD count;
} TPacketGCFestivalMissionList;

typedef struct SMission
{
	int id;
	std::string title;
	std::string description;
	int points;
	BYTE type;
	int target_vnum;
	int target_count;
	int time_limit;
} TMission;

typedef struct SFestivalMissionPacket
{
	DWORD id;
	char title[101];
	char description[256];
	DWORD points;
	BYTE type;
	DWORD target_vnum;
	DWORD target_count;
	DWORD time_limit;
	BYTE status;
} TFestivalMissionPacket;

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SFestivalGuildStatePacket
{
	DWORD total_points;
	DWORD reward_progress;
	DWORD pending_points;
	BYTE claimed_chests;
} TPacketGCFestivalGuildState;
#pragma pack(pop)

#endif
