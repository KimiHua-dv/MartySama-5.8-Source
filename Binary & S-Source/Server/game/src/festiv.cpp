#include "stdafx.h"

#include "char.h"
#include "desc.h"
#include "db.h"
#include "festiv.h"
#include "guild.h"
#include "questmanager.h"

#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace
{
	const size_t FESTIVAL_MAX_MISSIONS = 5;

	SQLResult* GetSelectResult(std::unique_ptr<SQLMsg>& pmsg, const char* operation)
	{
		if (!pmsg)
		{
			sys_err("Festival: DirectQuery retornou NULL em %s", operation);
			return NULL;
		}

		if (pmsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro SQL %u em %s", pmsg->uiSQLErrno, operation);
			return NULL;
		}

		SQLResult* pResult = pmsg->Get();
		if (!pResult || !pResult->pSQLResult)
		{
			sys_err("Festival: resultado SQL invalido em %s", operation);
			return NULL;
		}

		return pResult;
	}

	bool ParseMissionRow(MYSQL_ROW row, TMission& mission)
	{
		if (!row)
			return false;

		for (int i = 0; i < 8; ++i)
		{
			if (row[i] == NULL)
				return false;
		}

		if (row[1][0] == '\0' || row[2][0] == '\0')
			return false;

		mission = TMission();
		str_to_number(mission.id, row[0]);
		mission.title = row[1];
		mission.description = row[2];
		str_to_number(mission.points, row[3]);
		str_to_number(mission.type, row[4]);
		str_to_number(mission.target_vnum, row[5]);
		str_to_number(mission.target_count, row[6]);
		str_to_number(mission.time_limit, row[7]);

		return mission.id > 0
			&& mission.points >= 0
			&& mission.target_vnum >= 0
			&& mission.target_count > 0
			&& mission.time_limit >= 0;
	}

	bool LoadMissionCatalog(std::vector<TMission>& missions)
	{
		missions.clear();

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT id, title, description, points, type, "
			"target_vnum, target_count, time_limit "
			"FROM srv1_player.festival_mission_template "
			"WHERE enabled = 1"
		);

		SQLResult* pResult = GetSelectResult(pmsg, "carregar catalogo");
		if (!pResult)
			return false;

		MYSQL_ROW row = NULL;
		while ((row = mysql_fetch_row(pResult->pSQLResult)))
		{
			TMission mission;
			if (!ParseMissionRow(row, mission))
			{
				sys_err("Festival: template invalido ignorado no catalogo");
				continue;
			}

			missions.push_back(mission);
		}

		if (missions.empty())
		{
			sys_err("Festival: catalogo vazio ou sem templates validos");
			return false;
		}

		return true;
	}

	bool LoadGuildIDs(std::vector<DWORD>& guildIDs)
	{
		guildIDs.clear();

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT id FROM srv1_player.guild WHERE id > 0 ORDER BY id"
		);

		SQLResult* pResult = GetSelectResult(pmsg, "carregar guildas");
		if (!pResult)
			return false;

		MYSQL_ROW row = NULL;
		while ((row = mysql_fetch_row(pResult->pSQLResult)))
		{
			if (!row[0])
				continue;

			DWORD guildID = 0;
			str_to_number(guildID, row[0]);
			if (guildID != 0)
				guildIDs.push_back(guildID);
		}

		if (guildIDs.empty())
		{
			sys_err("Festival: nenhuma guilda encontrada em srv1_player.guild");
			return false;
		}

		return true;
	}

	bool LoadExistingAssignmentCounts(DWORD cycleID,std::map<DWORD, DWORD>& counts)
	{
		counts.clear();

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT guild_id, COUNT(*) "
			"FROM srv1_player.festival_guild_mission "
			"WHERE event_id = %u AND cycle_id = %u "
			"GROUP BY guild_id",
			Festival::FESTIVAL_EVENT_ID,
			cycleID
		);

		SQLResult* pResult = GetSelectResult(
			pmsg,
			"consultar atribuicoes existentes"
		);

		if (!pResult)
			return false;

		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pResult->pSQLResult)))
		{
			if (!row[0] || !row[1])
				continue;

			DWORD guildID = 0;
			DWORD missionCount = 0;

			str_to_number(guildID, row[0]);
			str_to_number(missionCount, row[1]);

			if (guildID != 0 && missionCount != 0)
				counts[guildID] = missionCount;
		}

		return true;
	}

	class CFestivalDistributionLock
	{
	public:
		CFestivalDistributionLock() : m_locked(false)
		{
			m_name[0] = '\0';
		}

		~CFestivalDistributionLock()
		{
			if (!m_locked)
				return;

			auto pmsg = DBManager::instance().DirectQuery(
				"SELECT RELEASE_LOCK('%s')", m_name);
			if (!pmsg || pmsg->uiSQLErrno != 0)
				sys_err("Festival: falha ao liberar lock de distribuicao");
		}

		bool Acquire(DWORD eventID)
		{
			snprintf(m_name, sizeof(m_name), "festival_guild_mission_%u", eventID);

			auto pmsg = DBManager::instance().DirectQuery(
				"SELECT GET_LOCK('%s', 10)", m_name);
			SQLResult* pResult = GetSelectResult(pmsg, "obter lock de distribuicao");
			if (!pResult)
				return false;

			MYSQL_ROW row = mysql_fetch_row(pResult->pSQLResult);
			if (!row || !row[0] || strcmp(row[0], "1") != 0)
			{
				sys_err("Festival: lock de distribuicao ocupado ou indisponivel");
				return false;
			}

			m_locked = true;
			return true;
		}

	private:
		bool m_locked;
		char m_name[64];
	};

	void SelectRandomMissions(
		const std::vector<TMission>& catalog,
		std::vector<TMission>& selected)
	{
		std::vector<TMission> available(catalog);
		selected.clear();

		size_t missionCount = available.size();
		if (missionCount > FESTIVAL_MAX_MISSIONS)
			missionCount = FESTIVAL_MAX_MISSIONS;

		selected.reserve(missionCount);
		while (selected.size() < missionCount)
		{
			const size_t index = static_cast<size_t>(
				number(0, static_cast<int>(available.size()) - 1));

			selected.push_back(available[index]);
			available.erase(available.begin() + index);
		}
	}

	bool SaveGuildAssignments(DWORD guildID,DWORD cycleID,const std::vector<TMission>& missions)
	{
		if (missions.empty())
			return false;

		std::string query =
			"INSERT INTO srv1_player.festival_guild_mission "
			"(event_id, cycle_id, guild_id, template_id, player_id, "
			"status, progress, target_count, points) VALUES ";

		for (size_t i = 0; i < missions.size(); ++i)
		{
			char values[160];

			const int length = snprintf(
				values,
				sizeof(values),
				"%s(%u, %u, %u, %d, NULL, 'AVAILABLE', 0, %d, %d)",
				i == 0 ? "" : ", ",
				Festival::FESTIVAL_EVENT_ID,
				cycleID,
				guildID,
				missions[i].id,
				missions[i].target_count,
				missions[i].points
			);

			if (length < 0 || static_cast<size_t>(length) >= sizeof(values))
			{
				sys_err("Festival: falha ao montar INSERT da guilda %u", guildID);
				return false;
			}

			query += values;
		}

		auto pmsg = DBManager::instance().DirectQuery("%s", query.c_str());

		if (!pmsg || pmsg->uiSQLErrno != 0 || !pmsg->Get())
		{
			sys_err("Festival: falha ao persistir missoes da guilda %u", guildID);
			return false;
		}

		SQLResult* pResult = pmsg->Get();

		if (pResult->uiAffectedRows != missions.size())
			return false;

		sys_log(
			0,
			"Festival: %u missoes gravadas para guilda %u, evento %u, ciclo %u",
			static_cast<unsigned int>(missions.size()),
			guildID,
			Festival::FESTIVAL_EVENT_ID,
			cycleID
		);

		return true;
	}

	void SendMissionPacket(LPDESC d, const std::vector<TMission>& missions)
	{
		if (!d || missions.empty())
			return;

		TPacketGCFestivalMissionList payload = {};
		payload.count = static_cast<WORD>(missions.size());

		const size_t totalSize =
			sizeof(TPacketGCFestival)
			+ sizeof(TPacketGCFestivalMissionList)
			+ missions.size() * sizeof(TFestivalMissionPacket);

		if (totalSize > 0xFFFF)
		{
			sys_err("Festival: pacote de missoes excede o limite");
			return;
		}

		TPacketGCFestival header = {};
		header.header = HEADER_GC_FESTIVAL;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = FEST_GC_MISSION_LIST;

		std::vector<char> buffer(totalSize);
		size_t offset = 0;

		memcpy(buffer.data() + offset, &header, sizeof(header));
		offset += sizeof(header);

		memcpy(buffer.data() + offset, &payload, sizeof(payload));
		offset += sizeof(payload);

		for (size_t i = 0; i < missions.size(); ++i)
		{
			const TMission& source = missions[i];
			TFestivalMissionPacket entry = {};
			entry.id = static_cast<DWORD>(source.id);
			strncpy(entry.title, source.title.c_str(), sizeof(entry.title) - 1);
			strncpy(entry.description, source.description.c_str(), sizeof(entry.description) - 1);
			entry.points = static_cast<DWORD>(source.points);
			entry.type = source.type;
			entry.target_vnum = static_cast<DWORD>(source.target_vnum);
			entry.target_count = static_cast<DWORD>(source.target_count);
			entry.time_limit = static_cast<DWORD>(source.time_limit);
		entry.status = source.status;

			memcpy(buffer.data() + offset, &entry, sizeof(entry));
			offset += sizeof(entry);
		}

		if (offset != totalSize)
		{
			sys_err("Festival: tamanho incorreto do pacote (%u/%u)",
				static_cast<unsigned int>(offset),
				static_cast<unsigned int>(totalSize));
			return;
		}

		d->Packet(buffer.data(), buffer.size());
	}
}

namespace Festival
{
	bool ProcessCycle()
	{
		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT status, current_cycle, cycle_duration_seconds "
			"FROM srv1_player.festival_event "
			"WHERE id = %u LIMIT 1",
			FESTIVAL_EVENT_ID
		);

		SQLResult* pResult = GetSelectResult(
			pmsg,
			"consultar estado do evento"
		);

		if (!pResult)
			return false;

		MYSQL_ROW row = mysql_fetch_row(pResult->pSQLResult);

		if (!row || !row[0] || !row[1] || !row[2])
		{
			sys_err("Festival: dados incompletos no registro do evento");
			return false;
		}

		DWORD currentCycle = 0;
		DWORD cycleDuration = 0;

		str_to_number(currentCycle, row[1]);
		str_to_number(cycleDuration, row[2]);

		if (currentCycle == 0 || cycleDuration == 0)
		{
			sys_err("Festival: ciclo ou duracao invalida");
			return false;
		}
		
		// Recuperacao sera implementada na proxima etapa.
		if (strcmp(row[0], "PROCESSING") == 0)
		{
			// Verifica se o prazo total do evento terminou durante a renovacao.
			auto expirationMsg = DBManager::instance().DirectQuery(
				"SELECT NOW() >= ends_at "
				"FROM srv1_player.festival_event "
				"WHERE id = %u",
				FESTIVAL_EVENT_ID
			);

			SQLResult* expirationResult = GetSelectResult(
				expirationMsg,
				"verificar expiracao durante recuperacao"
			);

			if (!expirationResult)
				return false;

			MYSQL_ROW expirationRow = mysql_fetch_row(
				expirationResult->pSQLResult
			);

			if (!expirationRow || !expirationRow[0])
				return false;

			const bool eventExpired = strcmp(expirationRow[0], "1") == 0;

			if (eventExpired)
			{
				sys_log(0,
					"Festival: evento expirou durante recuperacao do ciclo %u",
					currentCycle
				);

				// 1. Finaliza as missoes pendentes.
				auto missionMsg = DBManager::instance().DirectQuery(
					"UPDATE srv1_player.festival_guild_mission "
					"SET status = 'FAILED' "
					"WHERE event_id = %u "
					"AND status IN ('AVAILABLE', 'IN_PROGRESS')",
					FESTIVAL_EVENT_ID
				);

				if (!missionMsg || missionMsg->uiSQLErrno != 0 || !missionMsg->Get())
				{
					sys_err("Festival: erro ao finalizar missoes durante encerramento");
					return false;
				}

				// 2. Fecha o evento.
				auto closeMsg = DBManager::instance().DirectQuery(
					"UPDATE srv1_player.festival_event "
					"SET status = 'CLOSED', "
					"ends_at = NOW(), "
					"cycle_ends_at = NOW(), "
					"updated_at = NOW() "
					"WHERE id = %u "
					"AND status = 'PROCESSING'",
					FESTIVAL_EVENT_ID
				);

				if (!closeMsg || closeMsg->uiSQLErrno != 0 || !closeMsg->Get())
				{
					sys_err("Festival: erro ao fechar evento durante recuperacao");
					return false;
				}

				SQLResult* closeResult = closeMsg->Get();

				if (closeResult->uiAffectedRows != 1)
				{
					sys_err("Festival: evento nao foi fechado durante recuperacao");
					return false;
				}

				// 3. Desativa a flag do evento.
				quest::CQuestManager::instance().RequestSetEventFlag(
					"festival_guild", 0
				);

				sys_log(0,
					"Festival: evento %u encerrado durante recuperacao",
					FESTIVAL_EVENT_ID
				);

				return true;
			}
			
			if (currentCycle == 0)
			{
				sys_err("Festival: ciclo invalido durante recuperacao");
				return false;
			}

			const DWORD previousCycle = currentCycle - 1;

			sys_log(0,
				"Festival: recuperando renovacao do ciclo %u",
				currentCycle
			);

			// 1. Encerra as missoes pendentes do ciclo anterior.
			auto missionMsg = DBManager::instance().DirectQuery(
				"UPDATE srv1_player.festival_guild_mission "
				"SET status = 'FAILED' "
				"WHERE event_id = %u "
				"AND cycle_id = %u "
				"AND status IN ('AVAILABLE', 'IN_PROGRESS')",
				FESTIVAL_EVENT_ID,
				previousCycle
			);

			if (!missionMsg || missionMsg->uiSQLErrno != 0 || !missionMsg->Get())
			{
				sys_err("Festival: erro ao finalizar ciclo anterior %u",
					previousCycle);
				return false;
			}

			// 2. Distribui as missoes do novo ciclo.
			if (!DistributeMissionsForAllGuilds(currentCycle))
			{
				sys_err("Festival: falha na distribuicao do ciclo %u",
					currentCycle);
				return false;
			}

			// 3. Finaliza a transicao e ativa o novo ciclo.
			auto activateMsg = DBManager::instance().DirectQuery(
				"UPDATE srv1_player.festival_event "
				"SET status = 'ACTIVE', "
				"cycle_started_at = NOW(), "
				"cycle_ends_at = DATE_ADD(NOW(), "
				"INTERVAL cycle_duration_seconds SECOND), "
				"updated_at = NOW() "
				"WHERE id = %u "
				"AND status = 'PROCESSING' "
				"AND current_cycle = %u "
				"AND ends_at > NOW()",
				FESTIVAL_EVENT_ID,
				currentCycle
			);

			if (!activateMsg || activateMsg->uiSQLErrno != 0 || !activateMsg->Get())
			{
				sys_err("Festival: erro ao ativar ciclo %u",
					currentCycle);
				return false;
			}

			SQLResult* activateResult = activateMsg->Get();

			if (activateResult->uiAffectedRows != 1)
			{
				sys_err("Festival: ciclo %u nao foi ativado",
					currentCycle);
				return false;
			}
			
			auto renewalMsg = DBManager::instance().DirectQuery(
				"UPDATE srv1_player.festival_guild_state AS guild "
				"INNER JOIN srv1_player.festival_event AS event "
				"ON event.id = guild.event_id "
				"SET guild.next_renewal_at = event.cycle_ends_at, "
				"guild.renewal_count = event.current_cycle - 1 "
				"WHERE guild.event_id = %u "
				"AND event.current_cycle = %u "
				"AND event.status = 'ACTIVE'",
				FESTIVAL_EVENT_ID,
				currentCycle
			);

			if (!renewalMsg ||
				renewalMsg->uiSQLErrno != 0 ||
				!renewalMsg->Get())
			{
				sys_err("Festival: erro ao atualizar estado das guildas");
				return false;
			}

			quest::CQuestManager::instance().RequestSetEventFlag(
				"festival_guild", 1
			);

			sys_log(0,
				"Festival: ciclo %u iniciado com sucesso",
				currentCycle
			);

			return true;
		}
		
		if (strcmp(row[0], "ACTIVE") != 0)
			return false;
		
		// Verifica missoes individuais cujo prazo terminou.
		auto expirationMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission "
			"SET status = 'FAILED' "
			"WHERE event_id = %u "
			"AND status = 'IN_PROGRESS' "
			"AND expires_at IS NOT NULL "
			"AND expires_at <= NOW()",
			FESTIVAL_EVENT_ID
		);

		if (!expirationMsg ||
			expirationMsg->uiSQLErrno != 0 ||
			!expirationMsg->Get())
		{
			sys_err("Festival: erro ao expirar missoes individuais");
			return false;
		}

		SQLResult* expirationResult = expirationMsg->Get();

		if (expirationResult->uiAffectedRows > 0)
		{
			sys_log(0,
				"Festival: %u missoes individuais expiraram",
				expirationResult->uiAffectedRows
			);
		}

		// Verifica se o evento ou ciclo atingiu o prazo.
		auto timeMsg = DBManager::instance().DirectQuery(
			"SELECT "
			"NOW() >= ends_at, "
			"NOW() >= cycle_ends_at "
			"FROM srv1_player.festival_event "
			"WHERE id = %u",
			FESTIVAL_EVENT_ID
		);

		SQLResult* timeResult = GetSelectResult(
			timeMsg,
			"verificar horarios do evento"
		);

		if (!timeResult)
			return false;

		MYSQL_ROW timeRow = mysql_fetch_row(timeResult->pSQLResult);

		if (!timeRow || !timeRow[0] || !timeRow[1])
			return false;

		const bool eventExpired = strcmp(timeRow[0], "1") == 0;
		const bool cycleExpired = strcmp(timeRow[1], "1") == 0;

		// Encerramento definitivo dos sete dias.
		if (eventExpired)
		{
			auto missionMsg = DBManager::instance().DirectQuery(
				"UPDATE srv1_player.festival_guild_mission "
				"SET status = 'FAILED' "
				"WHERE event_id = %u "
				"AND status IN ('AVAILABLE', 'IN_PROGRESS')",
				FESTIVAL_EVENT_ID
			);

			if (!missionMsg || missionMsg->uiSQLErrno != 0 || !missionMsg->Get())
			{
				sys_err("Festival: erro ao encerrar missoes do evento");
				return false;
			}

			auto closeMsg = DBManager::instance().DirectQuery(
				"UPDATE srv1_player.festival_event "
				"SET status = 'CLOSED', "
				"ends_at = NOW(), "
				"cycle_ends_at = NOW(), "
				"updated_at = NOW() "
				"WHERE id = %u "
				"AND status = 'ACTIVE'",
				FESTIVAL_EVENT_ID
			);

			if (!closeMsg || closeMsg->uiSQLErrno != 0 || !closeMsg->Get())
			{
				sys_err("Festival: erro ao fechar evento");
				return false;
			}

			SQLResult* closeResult = closeMsg->Get();

			if (closeResult->uiAffectedRows != 1)
				return false;

			quest::CQuestManager::instance().RequestSetEventFlag(
				"festival_guild", 0
			);

			sys_log(0,
				"Festival: evento %u encerrado apos os sete dias",
				FESTIVAL_EVENT_ID
			);

			return true;
		}

		if (!cycleExpired)
			return false;

		// Reserva a transicao para impedir processamento duplicado.
		auto cycleMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_event "
			"SET status = 'PROCESSING', "
			"current_cycle = current_cycle + 1, "
			"updated_at = NOW() "
			"WHERE id = %u "
			"AND status = 'ACTIVE' "
			"AND current_cycle = %u "
			"AND cycle_ends_at <= NOW() "
			"AND ends_at > NOW()",
			FESTIVAL_EVENT_ID,
			currentCycle
		);

		if (!cycleMsg || cycleMsg->uiSQLErrno != 0 || !cycleMsg->Get())
		{
			sys_err("Festival: erro ao reservar proximo ciclo");
			return false;
		}

		SQLResult* cycleResult = cycleMsg->Get();

		if (cycleResult->uiAffectedRows != 1)
			return false;

		const DWORD nextCycle = currentCycle + 1;

		sys_log(0,
			"Festival: transicao iniciada. Ciclo anterior: %u, novo ciclo: %u",
			currentCycle,
			nextCycle
		);

		return true;
	}
	
	bool DistributeMissionsForAllGuilds(DWORD cycleID)
	{
		if (cycleID == 0)
			return false;

		CFestivalDistributionLock distributionLock;

		if (!distributionLock.Acquire(FESTIVAL_EVENT_ID))
			return false;

		std::vector<TMission> catalog;

		if (!LoadMissionCatalog(catalog))
			return false;

		std::vector<DWORD> guildIDs;

		if (!LoadGuildIDs(guildIDs))
			return false;

		std::map<DWORD, DWORD> existingCounts;

		if (!LoadExistingAssignmentCounts(cycleID, existingCounts))
			return false;

		for (size_t i = 0; i < guildIDs.size(); ++i)
		{
			const DWORD guildID = guildIDs[i];

			if (!InitializeGuildState(guildID))
				return false;

			std::map<DWORD, DWORD>::const_iterator existing =
				existingCounts.find(guildID);

			if (existing != existingCounts.end() && existing->second > 0)
				continue;

			std::vector<TMission> selected;

			SelectRandomMissions(catalog, selected);

			if (!SaveGuildAssignments(guildID, cycleID, selected))
				return false;
		}

		sys_log(
			0,
			"Festival: distribuicao do evento %u ciclo %u concluida",
			FESTIVAL_EVENT_ID,
			cycleID
		);

		return true;
	}

	bool InitializeGuildState(DWORD guildID)
	{
		if (guildID == 0)
			return false;

		auto msg = DBManager::instance().DirectQuery(
			"INSERT IGNORE INTO srv1_player.festival_guild_state "
			"(event_id, guild_id, next_renewal_at) "
			"SELECT id, %u, cycle_ends_at "
			"FROM srv1_player.festival_event "
			"WHERE id = %u",
			guildID,
			FESTIVAL_EVENT_ID
		);

		if (!msg || msg->uiSQLErrno != 0 || !msg->Get())
		{
			sys_err(
				"Festival: falha ao inicializar estado da guilda %u",
				guildID
			);

			return false;
		}

		auto check = DBManager::instance().DirectQuery(
			"SELECT 1 "
			"FROM srv1_player.festival_guild_state "
			"WHERE event_id = %u AND guild_id = %u "
			"LIMIT 1",
			FESTIVAL_EVENT_ID,
			guildID
		);

		if (!check || check->uiSQLErrno != 0 || !check->Get())
			return false;

		MYSQL_ROW row = mysql_fetch_row(check->Get()->pSQLResult);

		return row != NULL;
	}

	void SendOpen(LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();
		if (!d)
			return;

		TPacketGCFestival packet = {};
		packet.header = HEADER_GC_FESTIVAL;
		packet.size = sizeof(TPacketGCFestival);
		packet.subheader = FEST_GUILD_OPEN;

		d->Packet(&packet, sizeof(packet));
		sys_log(0, "Festival: enviado HEADER_GC_FESTIVAL - OPEN");
	}

	bool AcceptMission(LPCHARACTER ch, DWORD templateID)
	{
		if (!ch || !ch->GetDesc() || templateID == 0)
			return false;

		if (quest::CQuestManager::instance().GetEventFlag("festival_guild") == 0)
		{
			sys_log(0, "Festival: tentativa de aceitar missao com evento fechado");
			return false;
		}

		CGuild* guild = ch->GetGuild();

		if (!guild)
		{
			sys_log(0, "Festival: personagem sem guilda tentou aceitar missao");
			return false;
		}

		const DWORD guildID = guild->GetID();
		const DWORD playerID = ch->GetPlayerID();

		// 1. Inicia a transacao.
		auto transactionMsg = DBManager::instance().DirectQuery(
			"START TRANSACTION"
		);

		if (!transactionMsg || transactionMsg->uiSQLErrno != 0 || !transactionMsg->Get())
		{
			sys_err("Festival: erro ao iniciar transacao de aceitacao");
			return false;
		}

		// 2. Consulta o ciclo atual.
		auto cycleMsg = DBManager::instance().DirectQuery(
			"SELECT current_cycle "
			"FROM srv1_player.festival_event "
			"WHERE id = %u AND status = 'ACTIVE' "
			"LIMIT 1",
			FESTIVAL_EVENT_ID
		);

		SQLResult* cycleResult = GetSelectResult(
			cycleMsg,
			"consultar ciclo para aceitar missao"
		);

		if (!cycleResult)
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		MYSQL_ROW cycleRow = mysql_fetch_row(cycleResult->pSQLResult);

		if (!cycleRow || !cycleRow[0])
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		DWORD currentCycle = 0;
		str_to_number(currentCycle, cycleRow[0]);

		if (currentCycle == 0)
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		// 3. Cria o registro individual, caso ainda nao exista.
		auto insertMsg = DBManager::instance().DirectQuery(
			"INSERT IGNORE INTO srv1_player.festival_player_state "
			"(event_id, guild_id, player_id, cycle_id, accepted_missions) "
			"VALUES (%u, %u, %u, %u, 0)",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			currentCycle
		);

		if (!insertMsg || insertMsg->uiSQLErrno != 0 || !insertMsg->Get())
		{
			sys_err("Festival: erro ao criar estado individual do jogador %u",
				playerID);

			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		// 4. Bloqueia o registro individual.
		auto stateMsg = DBManager::instance().DirectQuery(
			"SELECT accepted_missions "
			"FROM srv1_player.festival_player_state "
			"WHERE event_id = %u "
			"AND guild_id = %u "
			"AND player_id = %u "
			"AND cycle_id = %u "
			"FOR UPDATE",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			currentCycle
		);

		SQLResult* stateResult = GetSelectResult(
			stateMsg,
			"consultar contador individual"
		);

		if (!stateResult)
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		MYSQL_ROW stateRow = mysql_fetch_row(stateResult->pSQLResult);

		if (!stateRow || !stateRow[0])
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		DWORD acceptedMissions = 0;
		str_to_number(acceptedMissions, stateRow[0]);

		// 5. Verifica o limite de duas missoes.
		if (acceptedMissions >= 2)
		{
			DBManager::instance().DirectQuery("ROLLBACK");

			ch->ChatPacket(CHAT_TYPE_INFO,
				"Voce ja aceitou o limite de missoes deste ciclo!");

			return false;
		}

		// 6. Verifica se o jogador ja possui uma missao em andamento.
		auto activeMission = DBManager::instance().DirectQuery(
			"SELECT id "
			"FROM srv1_player.festival_guild_mission "
			"WHERE event_id = %u "
			"AND cycle_id = %u "
			"AND player_id = %u "
			"AND status = 'IN_PROGRESS' "
			"LIMIT 1",
			FESTIVAL_EVENT_ID,
			currentCycle,
			playerID
		);

		if (!activeMission || activeMission->uiSQLErrno != 0 || !activeMission->Get())
		{
			sys_err("Festival: erro SQL ao verificar missoes ativas do player %u",
				playerID);

			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		SQLResult* activeResult = activeMission->Get();

		if (activeResult->uiNumRows > 0)
		{
			DBManager::instance().DirectQuery("ROLLBACK");

			ch->ChatPacket(CHAT_TYPE_INFO,
				"Voce ja possui uma missao em andamento!");

			return false;
		}

		// 7. Aceita a missao.
		auto acceptMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_mission_template AS mission_template "
			"ON mission_template.id = mission.template_id "
			"SET mission.player_id = %u, "
			"mission.status = 'IN_PROGRESS', "
			"mission.accepted_at = NOW(), "
			"mission.expires_at = DATE_ADD(NOW(), "
			"INTERVAL mission_template.time_limit SECOND) "
			"WHERE mission.event_id = %u "
			"AND mission.guild_id = %u "
			"AND mission.template_id = %u "
			"AND mission.cycle_id = %u "
			"AND mission.status = 'AVAILABLE' "
			"AND mission.player_id IS NULL",
			playerID,
			FESTIVAL_EVENT_ID,
			guildID,
			templateID,
			currentCycle
		);

		if (!acceptMsg || acceptMsg->uiSQLErrno != 0 || !acceptMsg->Get())
		{
			sys_err("Festival: erro SQL ao aceitar missao %u para guilda %u",
				templateID,
				guildID);

			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		SQLResult* acceptResult = acceptMsg->Get();

		if (acceptResult->uiAffectedRows != 1)
		{
			DBManager::instance().DirectQuery("ROLLBACK");

			sys_log(0,
				"Festival: missao %u indisponivel para player %u guilda %u",
				templateID,
				playerID,
				guildID);

			return false;
		}

		// 8. Incrementa o contador individual.
		auto updateStateMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_player_state "
			"SET accepted_missions = accepted_missions + 1 "
			"WHERE event_id = %u "
			"AND guild_id = %u "
			"AND player_id = %u "
			"AND cycle_id = %u "
			"AND accepted_missions < 2",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			currentCycle
		);

		if (!updateStateMsg ||
			updateStateMsg->uiSQLErrno != 0 ||
			!updateStateMsg->Get())
		{
			sys_err("Festival: erro ao incrementar contador do jogador %u",
				playerID);

			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		SQLResult* updateStateResult = updateStateMsg->Get();

		if (updateStateResult->uiAffectedRows != 1)
		{
			DBManager::instance().DirectQuery("ROLLBACK");
			return false;
		}

		// 9. Confirma a transacao.
		auto commitMsg = DBManager::instance().DirectQuery(
			"COMMIT"
		);

		if (!commitMsg || commitMsg->uiSQLErrno != 0 || !commitMsg->Get())
		{
			sys_err("Festival: erro ao confirmar aceitacao da missao %u",
				templateID);

			return false;
		}

		sys_log(0,
			"Festival: player %u aceitou missao %u da guilda %u. "
			"Missoes aceitas no ciclo: %u/2",
			playerID,
			templateID,
			guildID,
			acceptedMissions + 1
		);

		return true;
	}
	
	void SendMission(LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();
		if (!d)
			return;

		CGuild* guild = ch->GetGuild();
		if (!guild)
		{
			sys_log(0, "Festival: personagem sem guilda solicitou missoes");
			return;
		}

		const DWORD guildID = guild->GetID();
		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT mission.template_id, template.title, template.description, "
			"mission.points, template.type, template.target_vnum, "
			"mission.target_count, template.time_limit, "
			"CASE mission.status "
			"WHEN 'AVAILABLE' THEN 0 "
			"WHEN 'IN_PROGRESS' THEN 1 "
			"WHEN 'COMPLETED' THEN 2 "
			"WHEN 'FAILED' THEN 3 ELSE 255 END "
			"FROM srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"WHERE mission.event_id = %u "
			"AND mission.guild_id = %u "
			"AND mission.cycle_id = ("
				"SELECT current_cycle "
				"FROM srv1_player.festival_event "
				"WHERE id = %u"
			") "
			"ORDER BY mission.id LIMIT 5",
			FESTIVAL_EVENT_ID,
			guildID,
			FESTIVAL_EVENT_ID
		);

		SQLResult* pResult = GetSelectResult(pmsg, "carregar missoes da guilda");
		if (!pResult)
			return;

		std::vector<TMission> missions;
		MYSQL_ROW row = NULL;
		while ((row = mysql_fetch_row(pResult->pSQLResult)))
		{
			TMission mission;
			if (!ParseMissionRow(row, mission) || !row[8])
			{
				sys_err("Festival: atribuicao invalida ignorada para guilda %u", guildID);
				continue;
			}

			str_to_number(mission.status, row[8]);
			if (mission.status > FEST_MISSION_FAILED)
			{
				sys_err("Festival: status de missao invalido ignorado para guilda %u", guildID);
				continue;
			}

			missions.push_back(mission);
		}

		if (missions.empty())
		{
			sys_log(0,
				"Festival: nenhuma missao persistida para guilda %u no evento %u",
				guildID,
				FESTIVAL_EVENT_ID);
			return;
		}

		SendMissionPacket(d, missions);
	}
	
	bool StartEvent(DWORD cycleDurationMinutes)
	{
		const DWORD EVENT_DURATION_SECONDS = 7 * 24 * 60 * 60;

		if (cycleDurationMinutes == 0 || cycleDurationMinutes > 10080)
		{
			sys_err("Festival: duracao de ciclo invalida: %u",
				cycleDurationMinutes);
			return false;
		}

		// Mantem o evento fechado durante a preparacao.
		quest::CQuestManager::instance().RequestSetEventFlag("festival_guild", 0);

		// Registra o evento e prepara o primeiro ciclo.
		auto pmsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_event "
			"SET status = 'CLOSED', "
			"started_at = NOW(), "
			"ends_at = DATE_ADD(NOW(), INTERVAL %u SECOND), "
			"current_cycle = 1, "
			"cycle_duration_seconds = %u, "
			"cycle_started_at = NOW(), "
			"cycle_ends_at = DATE_ADD(NOW(), INTERVAL %u SECOND) "
			"WHERE id = %u AND status = 'CLOSED'",
			EVENT_DURATION_SECONDS,
			cycleDurationMinutes * 60,
			cycleDurationMinutes * 60,
			FESTIVAL_EVENT_ID
		);

		if (!pmsg || pmsg->uiSQLErrno != 0 || !pmsg->Get())
		{
			sys_err("Festival: falha ao registrar inicio do evento");
			return false;
		}

		SQLResult* pResult = pmsg->Get();

		if (pResult->uiAffectedRows != 1)
		{
			sys_err("Festival: evento inexistente ou ja ativo");
			return false;
		}

		// Distribui as missoes do primeiro ciclo.
		if (!DistributeMissionsForAllGuilds(1))
		{
			sys_err("Festival: falha na distribuicao do ciclo 1");
			return false;
		}

		// Somente agora o evento pode ser ativado.
		pmsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_event "
			"SET status = 'ACTIVE' "
			"WHERE id = %u AND status = 'CLOSED' AND current_cycle = 1",
			FESTIVAL_EVENT_ID
		);

		if (!pmsg || pmsg->uiSQLErrno != 0 || !pmsg->Get())
		{
			sys_err("Festival: falha ao ativar evento");
			return false;
		}

		pResult = pmsg->Get();

		if (pResult->uiAffectedRows != 1)
		{
			sys_err("Festival: evento nao foi ativado");
			return false;
		}

		quest::CQuestManager::instance().RequestSetEventFlag(
			"festival_guild", 1);

		sys_log(0,
			"Festival: evento %u iniciado. Ciclo 1, duracao %u minutos",
			FESTIVAL_EVENT_ID,
			cycleDurationMinutes);

		return true;
	}


	bool CloseEvent()
	{
		auto pmsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_event "
			"SET status = 'CLOSED', "
			"ends_at = NOW(), "
			"cycle_ends_at = NOW() "
			"WHERE id = %u",
			FESTIVAL_EVENT_ID
		);
		
		SQLResult* pResult = pmsg->Get();

		if (pResult->uiAffectedRows != 1)
		{
			sys_err("Festival: registro do evento nao encontrado");
			return false;
		}

		if (!pmsg || pmsg->uiSQLErrno != 0 || !pmsg->Get())
		{
			sys_err("Festival: falha ao encerrar evento");
			return false;
		}

		quest::CQuestManager::instance().RequestSetEventFlag(
			"festival_guild", 0);

		sys_log(0, "Festival: evento %u encerrado", FESTIVAL_EVENT_ID);

		return true;
	}

	void OnMetinKilled(LPCHARACTER ch, DWORD metinVnum)
	{
		if (!ch || !ch->IsPC() || metinVnum == 0)
			return;

		if (quest::CQuestManager::instance().GetEventFlag("festival_guild") == 0)
			return;

		CGuild* guild = ch->GetGuild();

		if (!guild)
			return;

		const DWORD playerID = ch->GetPlayerID();
		const DWORD guildID = guild->GetID();

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT mission.id, mission.progress, mission.target_count "
			"FROM srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND template.type = 1 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW() "
			"LIMIT 1",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			metinVnum
		);

		SQLResult* pResult = GetSelectResult(pmsg,"consultar missao de metin");

		if (!pResult)
			return;

		MYSQL_ROW row = mysql_fetch_row(pResult->pSQLResult);

		if (!row || !row[0])
			return;

		sys_log(0,"Festival: jogador %u destruiu metin %u. Missao encontrada: %s",playerID,	metinVnum,row[0]);
		auto updateMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"SET mission.progress = mission.progress + 1 "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.progress + 1 < mission.target_count "
			"AND template.type = 1 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			metinVnum
		);

		if (!updateMsg || updateMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao atualizar progresso da missao");
			return;
		}
		
		auto completeMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "

			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "

			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "

			"INNER JOIN srv1_player.festival_guild_state AS guild_state "
			"ON guild_state.event_id = mission.event_id "
			"AND guild_state.guild_id = mission.guild_id "

			"SET "
			"mission.progress = mission.target_count, "
			"mission.status = 'COMPLETED', "
			"mission.completed_at = NOW(), "
			"mission.points_awarded = 1, "

			"guild_state.total_points = "
			"guild_state.total_points + mission.points, "

			"guild_state.completed_missions = "
			"guild_state.completed_missions + 1 "

			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.points_awarded = 0 "
			"AND mission.progress + 1 >= mission.target_count "
			"AND template.type = 1 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",

			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			metinVnum
		);

		if (!completeMsg || completeMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao concluir missao e contabilizar pontos");
			return;
		}

		if (completeMsg->Get())
		{
			SQLResult* result = completeMsg->Get();

			if (result->uiAffectedRows > 0)
			{
				sys_log(
					0,
					"Festival: missao concluida. Guilda %u recebeu %u pontos",
					guildID,
					result->uiAffectedRows
				);
			}
		}
	}
	
	void OnMonsterKilled(LPCHARACTER ch, DWORD monsterVnum)
	{
		if (!ch || !ch->IsPC() || monsterVnum == 0)
			return;

		if (quest::CQuestManager::instance().GetEventFlag("festival_guild") == 0)
			return;

		CGuild* guild = ch->GetGuild();

		if (!guild)
			return;

		const DWORD playerID = ch->GetPlayerID();
		const DWORD guildID = guild->GetID();

		auto updateMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"SET mission.progress = mission.progress + 1 "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.progress + 1 < mission.target_count "
			"AND template.type = 2 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			monsterVnum
		);

		if (!updateMsg || updateMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao atualizar progresso de monstros");
			return;
		}

		auto completeMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"INNER JOIN srv1_player.festival_guild_state AS guild_state "
			"ON guild_state.event_id = mission.event_id "
			"AND guild_state.guild_id = mission.guild_id "
			"SET "
			"mission.progress = mission.target_count, "
			"mission.status = 'COMPLETED', "
			"mission.completed_at = NOW(), "
			"mission.points_awarded = 1, "
			"guild_state.total_points = "
			"guild_state.total_points + mission.points, "
			"guild_state.completed_missions = "
			"guild_state.completed_missions + 1 "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.points_awarded = 0 "
			"AND mission.progress + 1 >= mission.target_count "
			"AND template.type = 2 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			monsterVnum
		);

		if (!completeMsg || completeMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao concluir missao de monstros");
			return;
		}

		if (completeMsg->Get())
		{
			SQLResult* result = completeMsg->Get();

			if (result->uiAffectedRows > 0)
			{
				sys_log(
					0,
					"Festival: jogador %u concluiu missao de monstros. Guilda %u recebeu pontos",
					playerID,
					guildID
				);
			}
		}
	}
	
	void OnPlayerKilled(LPCHARACTER killer, LPCHARACTER victim, bool isDuel)
	{
		// Validacoes iniciais
		if (!killer || !victim)
			return;

		if (!killer->IsPC() || !victim->IsPC())
			return;

		// Nao contabilizar duelos de arena
		if (isDuel)
			return;

		// Atacante precisa ter guilda
		if (!killer->GetGuild())
			return;

		DWORD killerGuildID = killer->GetGuild()->GetID();
		DWORD victimGuildID = 0;

		// A vitima pode nao possuir guilda
		if (victim->GetGuild())
			victimGuildID = victim->GetGuild()->GetID();

		// Nao permite mortes dentro da mesma guilda
		if (victimGuildID != 0 && killerGuildID == victimGuildID)
			return;

		// Nivel da vitima precisa ser igual ou superior
		if (victim->GetLevel() < killer->GetLevel())
			return;

		// Validar VNUM/IDs
		DWORD playerID = killer->GetPlayerID();
		DWORD victimID = victim->GetPlayerID();

		if (playerID == 0 || victimID == 0 || killerGuildID == 0)
			return;

		// Verificar evento ativo
		if (!quest::CQuestManager::instance().GetEventFlag("festival_guild"))
			return;
		
		auto missionMsg = DBManager::instance().DirectQuery(
			"SELECT mission.id "
			"FROM srv1_player.festival_guild_mission AS mission "
			"JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"WHERE event.id = %u "
			"AND event.status = 'ACTIVE' "
			"AND mission.cycle_id = event.current_cycle "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND template.type = 3 "
			"AND mission.expires_at > NOW() "
			"LIMIT 1",
			FESTIVAL_EVENT_ID,
			killerGuildID,
			playerID
		);

		if (!missionMsg || missionMsg->uiSQLErrno != 0 || !missionMsg->Get())
		{
			sys_err("FESTIVAL PVP: Erro ao verificar missao ativa.");
			return;
		}

		MYSQL_ROW missionRow = mysql_fetch_row(missionMsg->Get()->pSQLResult);

		if (!missionRow)
			return;

		int sameTargetInterval = GetFestivalConfig(
			"pvp_same_target_interval",
			60
		);

		if (sameTargetInterval < 0)
			sameTargetInterval = 0;

		// Consultar quantidade de eliminacoes anteriores
		auto countMsg = DBManager::instance().DirectQuery(
			"SELECT COUNT(*) "
			"FROM srv1_player.festival_pvp_history "
			"WHERE event_id = %u "
			"AND cycle_id = ("
				"SELECT current_cycle "
				"FROM srv1_player.festival_event "
				"WHERE id = %u"
			") "
			"AND killer_id = %u "
			"AND victim_id = %u "
			"AND guild_id = %u",
			FESTIVAL_EVENT_ID,
			FESTIVAL_EVENT_ID,
			playerID,
			victimID,
			killerGuildID
		);

		if (!countMsg || countMsg->uiSQLErrno != 0 || !countMsg->Get())
		{
			sys_err("FESTIVAL PVP: Erro ao consultar quantidade de eliminacoes.");
			return;
		}

		MYSQL_ROW countRow = mysql_fetch_row(countMsg->Get()->pSQLResult);

		if (!countRow || !countRow[0])
		{
			sys_err("FESTIVAL PVP: Resultado da contagem invalido.");
			return;
		}

		DWORD previousKills = 0;
		str_to_number(previousKills, countRow[0]);

		// Calcular intervalo progressivo
		long long waitTime = (long long)sameTargetInterval * (previousKills + 1);

		// Verificar intervalo
		if (waitTime > 0)
		{
			auto checkMsg = DBManager::instance().DirectQuery(
				"SELECT id "
				"FROM srv1_player.festival_pvp_history "
				"WHERE event_id = %u "
				"AND cycle_id = ("
					"SELECT current_cycle "
					"FROM srv1_player.festival_event "
					"WHERE id = %u"
				") "
				"AND killer_id = %u "
				"AND victim_id = %u "
				"AND guild_id = %u "
				"AND killed_at > DATE_SUB(NOW(), INTERVAL %lld SECOND) "
				"LIMIT 1",
				FESTIVAL_EVENT_ID,
				FESTIVAL_EVENT_ID,
				playerID,
				victimID,
				killerGuildID,
				waitTime
			);

			if (!checkMsg || checkMsg->uiSQLErrno != 0 || !checkMsg->Get())
			{
				sys_err("FESTIVAL PVP: Erro ao consultar intervalo.");
				return;
			}

			MYSQL_ROW row = mysql_fetch_row(checkMsg->Get()->pSQLResult);

			if (row)
				return;
		}

		// Registrar eliminacao no historico PvP
		auto historyMsg = DBManager::instance().DirectQuery(
			"INSERT INTO srv1_player.festival_pvp_history "
			"(event_id, cycle_id, killer_id, victim_id, guild_id) "
			"SELECT event.id, event.current_cycle, %u, %u, %u "
			"FROM srv1_player.festival_event AS event "
			"JOIN srv1_player.festival_guild_mission AS mission "
			"ON mission.event_id = event.id "
			"JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"WHERE event.id = %u "
			"AND event.status = 'ACTIVE' "
			"AND mission.cycle_id = event.current_cycle "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND template.type = 3 "
			"AND mission.expires_at > NOW()",
			playerID,
			victimID,
			killerGuildID,
			FESTIVAL_EVENT_ID,
			killerGuildID,
			playerID
		);

		if (!historyMsg || historyMsg->uiSQLErrno != 0 ||
			!historyMsg->Get() ||
			historyMsg->Get()->uiAffectedRows != 1)
		{
			sys_err("FESTIVAL PVP: Erro ao registrar eliminacao ou missao inexistente.");
			return;
		}

		// Incrementar progresso quando ainda nao for a ultima eliminacao
		auto updateMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"SET mission.progress = mission.progress + 1 "
			"WHERE event.id = %u "
			"AND event.status = 'ACTIVE' "
			"AND mission.cycle_id = event.current_cycle "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND template.type = 3 "
			"AND mission.progress + 1 < mission.target_count "
			"AND mission.expires_at > NOW()",
			FESTIVAL_EVENT_ID,
			killerGuildID,
			playerID
		);

		if (!updateMsg || updateMsg->uiSQLErrno != 0)
		{
			sys_err("FESTIVAL PVP: Erro ao atualizar progresso.");
			return;
		}

		// Concluir missao e atribuir pontos
		auto completeMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"JOIN srv1_player.festival_guild_state AS guild "
			"ON guild.event_id = mission.event_id "
			"AND guild.guild_id = mission.guild_id "
			"SET mission.progress = mission.target_count, "
			"mission.status = 'COMPLETED', "
			"mission.completed_at = NOW(), "
			"mission.points_awarded = 1, "
			"guild.total_points = guild.total_points + mission.points, "
			"guild.completed_missions = guild.completed_missions + 1 "
			"WHERE event.id = %u "
			"AND event.status = 'ACTIVE' "
			"AND mission.cycle_id = event.current_cycle "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.points_awarded = 0 "
			"AND mission.progress + 1 >= mission.target_count "
			"AND template.type = 3 "
			"AND mission.expires_at > NOW()",
			FESTIVAL_EVENT_ID,
			killerGuildID,
			playerID
		);

		if (!completeMsg || completeMsg->uiSQLErrno != 0)
		{
			sys_err("FESTIVAL PVP: Erro ao concluir missao.");
			return;
		}

		if (completeMsg->Get()->uiAffectedRows > 0)
		{
			sys_log(0,
				"FESTIVAL PVP: Player %u completou uma missao PvP. Guild %u.",
				playerID,
				killerGuildID
			);
		}
	}
	
	
	void OnItemCollected(LPCHARACTER ch, DWORD itemVnum, DWORD itemCount)
	{
		if (!ch || !ch->IsPC() || itemVnum == 0 || itemCount == 0)
			return;

		if (quest::CQuestManager::instance().GetEventFlag("festival_guild") == 0)
			return;

		CGuild* guild = ch->GetGuild();

		if (!guild)
			return;

		const DWORD playerID = ch->GetPlayerID();
		const DWORD guildID = guild->GetID();

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT mission.id, mission.progress, mission.target_count "
			"FROM srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND template.type = 4 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW() "
			"LIMIT 1",
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			itemVnum
		);

		SQLResult* pResult = GetSelectResult(pmsg, "consultar missao de coleta");

		if (!pResult)
			return;

		MYSQL_ROW row = mysql_fetch_row(pResult->pSQLResult);

		if (!row || !row[0])
			return;

		sys_log(
			0,
			"Festival: jogador %u coletou item %u quantidade %u. Missao encontrada: %s",
			playerID,
			itemVnum,
			itemCount,
			row[0]
		);

		// Atualiza o progresso quando a missao ainda nao sera concluida.
		auto updateMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "
			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "
			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "
			"SET mission.progress = mission.progress + %u "
			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.progress + %u < mission.target_count "
			"AND template.type = 4 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",
			itemCount,
			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			itemCount,
			itemVnum
		);

		if (!updateMsg || updateMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao atualizar progresso da missao de coleta");
			return;
		}

		// Conclui a missao e contabiliza os pontos.
		auto completeMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_player.festival_guild_mission AS mission "

			"INNER JOIN srv1_player.festival_event AS event "
			"ON event.id = mission.event_id "

			"INNER JOIN srv1_player.festival_mission_template AS template "
			"ON template.id = mission.template_id "

			"INNER JOIN srv1_player.festival_guild_state AS guild_state "
			"ON guild_state.event_id = mission.event_id "
			"AND guild_state.guild_id = mission.guild_id "

			"SET "
			"mission.progress = mission.target_count, "
			"mission.status = 'COMPLETED', "
			"mission.completed_at = NOW(), "
			"mission.points_awarded = 1, "

			"guild_state.total_points = "
			"guild_state.total_points + mission.points, "

			"guild_state.completed_missions = "
			"guild_state.completed_missions + 1 "

			"WHERE mission.event_id = %u "
			"AND mission.cycle_id = event.current_cycle "
			"AND event.status = 'ACTIVE' "
			"AND mission.guild_id = %u "
			"AND mission.player_id = %u "
			"AND mission.status = 'IN_PROGRESS' "
			"AND mission.points_awarded = 0 "
			"AND mission.progress + %u >= mission.target_count "
			"AND template.type = 4 "
			"AND (template.target_vnum = 0 "
			"OR template.target_vnum = %u) "
			"AND mission.expires_at > NOW()",

			FESTIVAL_EVENT_ID,
			guildID,
			playerID,
			itemCount,
			itemVnum
		);

		if (!completeMsg || completeMsg->uiSQLErrno != 0)
		{
			sys_err("Festival: erro ao concluir missao de coleta");
			return;
		}

		if (completeMsg->Get())
		{
			SQLResult* result = completeMsg->Get();

			if (result->uiAffectedRows > 0)
			{
				sys_log(
					0,
					"Festival: missao de coleta concluida. Guilda %u recebeu %u pontos",
					guildID,
					result->uiAffectedRows
				);
			}
		}
	}

	int GetFestivalConfig(const char* configKey, int defaultValue)
	{
		auto msg = DBManager::instance().DirectQuery(
			"SELECT config_value "
			"FROM srv1_player.festival_config "
			"WHERE config_key = '%s'",
			configKey
		);

		if (!msg || msg->uiSQLErrno != 0 || !msg->Get())
			return defaultValue;

		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (!row || !row[0])
			return defaultValue;

		int value = defaultValue;
		str_to_number(value, row[0]);

		return value;
	}
}
