#ifndef __INC_FESTIV_H__
#define __INC_FESTIV_H__

#include <string>

#include "char.h"

namespace Festival
{
    // Identificador do evento.
    static const DWORD FESTIVAL_EVENT_ID = 1;

    // Duração padrão dos ciclos: 24 horas.
    static const DWORD FESTIVAL_DEFAULT_CYCLE_MINUTES = 1440;

    bool DistributeMissionsForAllGuilds(DWORD cycleID);

    bool StartEvent(DWORD cycleDurationMinutes);
    bool CloseEvent();

    void SendOpen(LPCHARACTER ch);
    void SendMission(LPCHARACTER ch);
    bool AcceptMission(LPCHARACTER ch, DWORD templateID);
	
	bool ProcessCycle();
	void OnMetinKilled(LPCHARACTER ch, DWORD metinVnum);
	void OnMonsterKilled(LPCHARACTER ch, DWORD monsterVnum);
	void OnPlayerKilled(LPCHARACTER killer, LPCHARACTER victim, bool isDuel);
	void OnItemCollected(LPCHARACTER ch, DWORD itemVnum, DWORD itemCount);
	
	bool InitializeGuildState(DWORD guildID);
	int GetFestivalConfig(const char* configKey, int defaultValue);
	bool AddGuildPoints(DWORD guildID, DWORD points);
	void SendGuildState(LPCHARACTER ch);
	bool PreparePlayerChestClaims(LPCHARACTER ch);
	bool ClaimPendingChests(LPCHARACTER ch);
}

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

enum EFestivalMissionListStatus
{
    FESTIVAL_LIST_ACTIVE = 0,
    FESTIVAL_LIST_UNAVAILABLE = 1
};

#pragma pack(push, 1)
typedef struct SPacketGCFestivalMissionList
{
    BYTE status;
    WORD count;
} TPacketGCFestivalMissionList;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SPacketCGFestival
{
	BYTE header;
	BYTE subheader;
	DWORD template_id;
} TPacketCGFestivGuild;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SPacketGCFestival
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCFestival;
#pragma pack(pop)

// Dados internos do servidor; nao fazem parte do formato de rede.
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
	BYTE status;
	DWORD progress;
	DWORD remaining_time;
	DWORD player_id;
	bool is_owner;
	std::string player_name;
} TMission;

enum EFestivalMissionStatus
{
	FEST_MISSION_AVAILABLE = 0,
	FEST_MISSION_IN_PROGRESS = 1,
	FEST_MISSION_COMPLETED = 2,
	FEST_MISSION_FAILED = 3
};

#pragma pack(push, 1)
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
	DWORD progress;
	DWORD remaining_time;
	BYTE is_owner;
	char player_name[CHARACTER_NAME_MAX_LEN + 1];
} TFestivalMissionPacket;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(push, 1)
typedef struct SFestivalGuildStatePacket
{
    DWORD total_points;
    DWORD reward_progress;
    DWORD pending_points;
    BYTE claimed_chests;

    DWORD pending_chest_200;
    DWORD pending_chest_500;
    DWORD pending_chest_1000;
    DWORD pending_chest_1500;
    DWORD pending_chest_2000;
	
	DWORD accepted_missions;
} TPacketGCFestivalGuildState;
#pragma pack(pop)
#pragma pack(pop)

#endif
