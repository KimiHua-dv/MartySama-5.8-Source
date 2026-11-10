#include "stdafx.h"

#include "char.h"
#include "desc.h"
#include "db.h"
#include "itemshop.h"
#include "item_manager.h"
#include "item.h"


#include <vector>
namespace ItemShop
{
	void SendOpen(LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		TPacketGCItemShop packet;

		packet.header = HEADER_GC_ITEMSHOP;
		packet.size = sizeof(TPacketGCItemShop);
		packet.subheader = ITEMSHOP_GC_OPEN;

		d->Packet(&packet, sizeof(packet));
	}

	void SendCategoryList(LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT id, name "
			"FROM item_shop.categories "
			"WHERE visible = 1 "
			"ORDER BY sort_order ASC"
		);

		if (!pmsg)
		{
			sys_err("ItemShop::SendCategoryList - DirectQuery retornou NULL");
			return;
		}

		std::vector<TItemShopCategory> categories;

		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pmsg->Get()->pSQLResult)))
		{
			TItemShopCategory category;

			memset(&category, 0, sizeof(category));

			str_to_number(category.id, row[0]);

			if (row[1])
			{
				strncpy(
					category.name,
					row[1],
					sizeof(category.name) - 1
				);
			}

			categories.push_back(category);
		}

		TPacketGCItemShopCategoryList payload;

		payload.count = static_cast<WORD>(categories.size());

		size_t totalSize =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopCategoryList)
			+ categories.size() * sizeof(TItemShopCategory);

		if (totalSize > 0xFFFF)
		{
			sys_err(
				"ItemShop::SendCategoryList - pacote grande demais: %u",
				(unsigned int)totalSize
			);
			return;
		}

		TPacketGCItemShop header;

		header.header = HEADER_GC_ITEMSHOP;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = ITEMSHOP_GC_CATEGORY_LIST;

		std::vector<char> buffer(totalSize);

		size_t offset = 0;

		memcpy(&buffer[offset], &header, sizeof(header));
		offset += sizeof(header);

		memcpy(&buffer[offset], &payload, sizeof(payload));
		offset += sizeof(payload);

		for (size_t i = 0; i < categories.size(); ++i)
		{
			memcpy(
				&buffer[offset],
				&categories[i],
				sizeof(TItemShopCategory)
			);

			offset += sizeof(TItemShopCategory);
		}

		if (offset != totalSize)
		{
			sys_err(
				"ItemShop::SendCategoryList - montagem incorreta (%u/%u)",
				(unsigned int)offset,
				(unsigned int)totalSize
			);
			return;
		}

		d->Packet(buffer.data(), buffer.size());
	}
	
	
	static const BYTE ITEMSHOP_HOME_OFFERS_CATEGORY = 0;
	
	void SendItemList(LPCHARACTER ch, BYTE category)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		std::unique_ptr<SQLMsg> pmsg;

		if (category == 1)
		{
			// Home: itens mais vendidos de todas as categorias.
			pmsg = DBManager::instance().DirectQuery(
				"SELECT "
					"id, "
					"category_id, "
					"vnum, "
					"`count`, "
					"currency_type, "
					"currency_vnum, "
					"currency_count, "
					"discount, "
					"buy_limit "
				"FROM item_shop.items "
				"WHERE visible = 1 "
				"ORDER BY purchase_count DESC, sort_order ASC, id ASC "
				"LIMIT 3"
			);
		}
		else
		{
			// Categorias normais.
			pmsg = DBManager::instance().DirectQuery(
				"SELECT "
					"id, "
					"category_id, "
					"vnum, "
					"`count`, "
					"currency_type, "
					"currency_vnum, "
					"currency_count, "
					"discount, "
					"buy_limit "
				"FROM item_shop.items "
				"WHERE category_id = %u "
					"AND visible = 1 "
				"ORDER BY sort_order ASC",
				category
			);
		}

		if (!pmsg)
		{
			sys_err(
				"ItemShop::SendItemList - DirectQuery retornou NULL category=%u",
				category
			);
			return;
		}

		std::vector<TItemShopData> items;

		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pmsg->Get()->pSQLResult)))
		{
			TItemShopData item;

			memset(&item, 0, sizeof(item));

			str_to_number(item.id, row[0]);
			str_to_number(item.vnum, row[2]);
			str_to_number(item.count, row[3]);

			// Nesta versão, o client usa currency_count como preço.
			str_to_number(item.currencyType, row[4]);
			str_to_number(item.currencyVnum, row[5]);
			str_to_number(item.price, row[6]);
			str_to_number(item.discount, row[7]);

			item.category = category;

			items.push_back(item);

		}

		TPacketGCItemShopItemList payload;

		payload.count = static_cast<WORD>(items.size());
		payload.category = category;

		size_t totalSize =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopItemList)
			+ items.size() * sizeof(TItemShopData);

		if (totalSize > 0xFFFF)
		{
			sys_err(
				"ItemShop::SendItemList - pacote grande demais: %u",
				(unsigned int)totalSize
			);
			return;
		}

		TPacketGCItemShop header;

		header.header = HEADER_GC_ITEMSHOP;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = ITEMSHOP_GC_ITEM_LIST;

		std::vector<char> buffer(totalSize);

		size_t offset = 0;

		memcpy(&buffer[offset], &header, sizeof(header));
		offset += sizeof(header);

		memcpy(&buffer[offset], &payload, sizeof(payload));
		offset += sizeof(payload);

		for (size_t i = 0; i < items.size(); ++i)
		{
			memcpy(
				&buffer[offset],
				&items[i],
				sizeof(TItemShopData)
			);

			offset += sizeof(TItemShopData);
		}

		if (offset != totalSize)
		{
			sys_err(
				"ItemShop::SendItemList - montagem incorreta (%u/%u)",
				(unsigned int)offset,
				(unsigned int)totalSize
			);
			return;
		}

		d->Packet(buffer.data(), buffer.size());
		if (category == 1)
		{
			SendHomeOffers(ch);
		}
	}

	void BuyItem(LPCHARACTER ch, DWORD itemID)
	{
		if (!ch || itemID == 0)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		DWORD accountID = d->GetAccountTable().id;

		if (accountID == 0)
			return;

		auto pItemMsg = DBManager::instance().DirectQuery(
			"SELECT "
				"vnum, "
				"`count`, "
				"currency_type, "
				"currency_vnum, "
				"currency_count, "
				"discount "
			"FROM item_shop.items "
			"WHERE id = %u "
				"AND visible = 1",
			itemID
		);

		if (!pItemMsg || pItemMsg->Get()->uiNumRows != 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Este item nao esta disponivel.");
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pItemMsg->Get()->pSQLResult);

		if (!row)
			return;

		DWORD vnum = 0;
		DWORD itemCount = 0;
		DWORD currencyType = 0;
		DWORD currencyVnum = 0;
		DWORD currencyCount = 0;
		DWORD discount = 0;

		str_to_number(vnum, row[0]);
		str_to_number(itemCount, row[1]);
		str_to_number(currencyType, row[2]);
		str_to_number(currencyVnum, row[3]);
		str_to_number(currencyCount, row[4]);
		str_to_number(discount, row[5]);

		if (vnum == 0 || itemCount == 0 || itemCount > 255)
		{
			ch->ChatPacket(
				CHAT_TYPE_INFO,
				"Item invalido."
			);
			return;
		}

		if (currencyCount == 0 || discount > 100)
		{
			ch->ChatPacket(
				CHAT_TYPE_INFO,
				"Preco invalido."
			);
			return;
		}

		// Calcula o preço final.
		DWORD finalPrice = currencyCount;

		if (discount > 0)
		{
			unsigned long long discountValue =
				(static_cast<unsigned long long>(currencyCount) * discount) / 100;

			finalPrice = currencyCount - static_cast<DWORD>(discountValue);
		}

		if (finalPrice == 0)
		{
			ch->ChatPacket(
				CHAT_TYPE_INFO,
				"Preco invalido."
			);
			return;
		}


		// ============================================================
		// 1. Cria o item antes de qualquer débito.
		// ============================================================

		LPITEM item = ITEM_MANAGER::instance().CreateItem(
			vnum,
			static_cast<BYTE>(itemCount)
		);

		if (!item)
		{
			sys_err(
				"ItemShop::BuyItem - nao foi possivel criar item "
				"itemID=%u vnum=%u count=%u",
				itemID,
				vnum,
				itemCount
			);

			// ItemShop::SendBuyResult(
				// ch,
				// ITEM_SHOP_SUBHEADER_GC_SOLD_OUT
			// );

			return;
		}


		// ============================================================
		// 2. Verifica espaço no inventário.
		// ============================================================

		int iEmptyPos = ch->GetEmptyInventoryEx(item);

		if (iEmptyPos < 0)
		{
			sys_log(
				1,
				"ItemShop::BuyItem - inventario cheio: %s size=%d "
				"itemID=%u vnum=%u",
				ch->GetName(),
				item->GetSize(),
				itemID,
				vnum
			);

			M2_DESTROY_ITEM(item);

			ItemShop::SendBuyResult(
				ch,
				ITEM_SHOP_SUBHEADER_GC_INVENTORY_FULL
			);

			return;
		}
		
		if (!ItemShop::CanAfford(ch,accountID,static_cast<BYTE>(currencyType),currencyVnum,finalPrice))
		{
			// Por enquanto vamos apenas tratar como saldo insuficiente.
			// Depois refinamos o resultado para cada moeda.
			ItemShop::SendBuyResult(
				ch,
				ITEMSHOP_BUY_CASH_INSUFFICIENT
			);

			M2_DESTROY_ITEM(item);

			return;
		}
		
		if (!ProcessPayment(ch,currencyType,currencyVnum,finalPrice))
		{
			M2_DESTROY_ITEM(item);

			ItemShop::SendBuyResult(ch,CURRENCY_INSUFFICIENT);

			return;
		}		


		// ============================================================
		// 3. Entrega exatamente o item que foi criado e validado.
		// ============================================================

		item->AddToCharacter(
			ch,
			TItemPos(
				item->GetWindowInventoryEx(),
				iEmptyPos
			)
		);


		// ============================================================
		// 4. Atualiza estatística de compra.
		// ============================================================

		auto pCountMsg = DBManager::instance().DirectQuery(
			"UPDATE item_shop.items "
			"SET purchase_count = purchase_count + 1 "
			"WHERE id = %u",
			itemID
		);
		
		auto pLogMsg = DBManager::instance().DirectQuery(
			"INSERT INTO item_shop.log_itens "
			"(data, item, moeda, moeda_vnum, valor, player_name, account_id) "
			"VALUES (NOW(), %d, %d, %d, %d, '%s', %d)",
			vnum,
			currencyType,
			currencyVnum,
			finalPrice,
			ch->GetName(),
			accountID
		);

		if (!pCountMsg || pCountMsg->Get()->uiAffectedRows != 1)
		{
			sys_err(
				"ItemShop::BuyItem - nao foi possivel incrementar "
				"purchase_count itemID=%u",
				itemID
			);
		}


		// ============================================================
		// 5. Compra concluída.
		// ============================================================

		ItemShop::SendBuyResult(ch,ITEMSHOP_BUY_SUCCESS);

		ch->ChatPacket(CHAT_TYPE_INFO,"Compra realizada com sucesso.");

		sys_log(
			1,
			"ItemShop::BuyItem - sucesso accountID=%u itemID=%u "
			"vnum=%u count=%u price=%u",
			accountID,
			itemID,
			vnum,
			itemCount,
			finalPrice
		);
	}
	
	void SendBuyResult(LPCHARACTER ch, BYTE result)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		TPacketGCItemShop header;
		header.header = HEADER_GC_ITEMSHOP;
		header.size =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopBuyResult);
		header.subheader = ITEMSHOP_GC_BUY_RESULT;

		TPacketGCItemShopBuyResult payload;
		payload.result = result;

		d->Packet(&header, sizeof(header));
		d->Packet(&payload, sizeof(payload));
	}
	
	void SendHomeOffers(LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPDESC d = ch->GetDesc();

		if (!d)
			return;

		auto pmsg = DBManager::instance().DirectQuery(
			"SELECT "
				"id, "
				"category_id, "
				"vnum, "
				"`count`, "
				"currency_type, "
				"currency_vnum, "
				"currency_count, "
				"discount, "
				"buy_limit "
			"FROM item_shop.items "
			"WHERE visible = 1 "
				"AND discount > 0 "
			"ORDER BY discount DESC, purchase_count DESC, sort_order ASC, id ASC "
			"LIMIT 3"
		);

		if (!pmsg)
			return;

		std::vector<TItemShopData> items;

		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pmsg->Get()->pSQLResult)))
		{
			TItemShopData item;

			memset(&item, 0, sizeof(item));

			str_to_number(item.id, row[0]);
			str_to_number(item.vnum, row[2]);
			str_to_number(item.count, row[3]);
			str_to_number(item.price, row[6]);
			str_to_number(item.discount, row[7]);

			// 0 identifica o painel Ofertas da Semana no client.
			item.category = ITEMSHOP_HOME_OFFERS_CATEGORY;

			items.push_back(item);
		}

		TPacketGCItemShopItemList payload;

		payload.count = static_cast<WORD>(items.size());
		payload.category = ITEMSHOP_HOME_OFFERS_CATEGORY;

		size_t totalSize =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopItemList)
			+ items.size() * sizeof(TItemShopData);

		if (totalSize > 0xFFFF)
			return;

		TPacketGCItemShop header;

		header.header = HEADER_GC_ITEMSHOP;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = ITEMSHOP_GC_ITEM_LIST;

		std::vector<char> buffer(totalSize);

		size_t offset = 0;

		memcpy(&buffer[offset], &header, sizeof(header));
		offset += sizeof(header);

		memcpy(&buffer[offset], &payload, sizeof(payload));
		offset += sizeof(payload);

		for (size_t i = 0; i < items.size(); ++i)
		{
			memcpy(
				&buffer[offset],
				&items[i],
				sizeof(TItemShopData)
			);

			offset += sizeof(TItemShopData);
		}

		if (offset != totalSize)
			return;


		d->Packet(buffer.data(), buffer.size());
	}
	
	
	bool CanAfford(LPCHARACTER ch,DWORD accountID,BYTE currencyType, DWORD currencyVnum,DWORD amount)
	{
		if (!ch || accountID == 0 || amount == 0)
			return false;

		switch (currencyType)
		{
			case 1: // Cash
			{
				auto pMsg = DBManager::instance().DirectQuery(
					"SELECT cash "
					"FROM srv1_account.account "
					"WHERE id = %u",
					accountID
				);

				if (!pMsg || pMsg->Get()->uiNumRows != 1)
					return false;

				MYSQL_ROW row = mysql_fetch_row(
					pMsg->Get()->pSQLResult
				);

				if (!row || !row[0])
					return false;

				DWORD cash = 0;
				str_to_number(cash, row[0]);

				return cash >= amount;
			}

			case 2: // Yang
			{
				return ch->GetGold() >= amount;
			}

			case 3: // Cheque
			{
				return ch->GetCheque() >= amount;
			}

			case 4: // Item-moeda
			{
				if (currencyVnum == 0)
					return false;

				DWORD total = 0;

				for (WORD cell = 0;
					 cell < INVENTORY_MAX_NUM;
					 ++cell)
				{
					LPITEM item = ch->GetInventoryItem(cell);

					if (!item)
						continue;

					if (item->GetVnum() != currencyVnum)
						continue;

					total += item->GetCount();

					if (total >= amount)
						return true;
				}

				return false;
			}

			default:
				return false;
		}
	}
	
	bool ProcessPayment(LPCHARACTER ch,BYTE currencyType,DWORD currencyVnum,DWORD amount)
	{
		switch (currencyType)
		{
			case 1:
				return ProcessCashPayment(ch, amount);

			case 2:
				return ProcessYangPayment(ch, amount);

			case 3:
				return ProcessChequePayment(ch, amount);

			case 4:
				return ProcessItemPayment(ch, currencyVnum, amount);

			default:
			sys_err(
				"ItemShop::ProcessPayment - moeda invalida type=%u vnum=%u amount=%u",
				currencyType,
				currencyVnum,
				amount
			);
				return false;
		}
	}
	
	bool ProcessCashPayment(LPCHARACTER ch,DWORD amount)
	{
		if (!ch || amount == 0)
			return false;

		LPDESC desc = ch->GetDesc();

		if (!desc)
			return false;

		DWORD accountID = desc->GetAccountTable().id;

		if (accountID == 0)
			return false;

		auto pDebitMsg = DBManager::instance().DirectQuery(
			"UPDATE srv1_account.account "
			"SET cash = cash - %u "
			"WHERE id = %u "
			"AND cash >= %u",
			amount,
			accountID,
			amount
		);

		if (!pDebitMsg || pDebitMsg->Get()->uiAffectedRows != 1)
			return false;

		return true;
	}
	bool ProcessYangPayment(LPCHARACTER ch,DWORD amount)
	{
		if (!ch || amount == 0)
			return false;
		if (ch->GetGold() < amount)
			return false;
		ch->PointChange(POINT_GOLD,-static_cast<long long>(amount),false);
		return true;
	}
	bool ProcessChequePayment(LPCHARACTER ch,DWORD amount)
	{
		if (!ch || amount == 0)
			return false;
		if (ch->GetCheque() < amount)
			return false;
		ch->PointChange(POINT_CHEQUE,-static_cast<long long>(amount),false);
		return true;
	}
	
	bool ProcessItemPayment(LPCHARACTER ch,DWORD currencyVnum,DWORD amount)
	{
		if (!ch || currencyVnum == 0 || amount == 0)
			return false;
		if (ch->CountSpecifyItem(currencyVnum) < amount)
			return false;
		ch->RemoveSpecifyItem(currencyVnum, amount);
		return true;
	}
	
	bool SendItemShopLog(LPCHARACTER ch, DWORD offset)
	{
		if (!ch)
			return false;

		LPDESC d = ch->GetDesc();

		if (!d)
			return false;
			
		DWORD accountID = d->GetAccountTable().id;
		std::unique_ptr<SQLMsg> pmsg_log;
		if (ch->GetGMLevel() >= GM_LOW_WIZARD)
		{
			pmsg_log = DBManager::instance().DirectQuery(
				"SELECT data, item, quantidade, moeda, moeda_vnum, valor, player_name, account_id "
				"FROM item_shop.log_itens "
				"ORDER BY data ASC "
				"LIMIT 50 OFFSET %u",
				offset
			);
		}
		else
		{
			pmsg_log = DBManager::instance().DirectQuery(
				"SELECT data, item, quantidade, moeda, moeda_vnum, valor, player_name, account_id "
				"FROM item_shop.log_itens "
				"WHERE account_id = %d "
				"ORDER BY data ASC "
				"LIMIT 50 OFFSET %u",
				accountID,
				offset
			);
		}
		
		if (!pmsg_log)
		{
			sys_err("ItemShop::SendCSendItemShopLog - DirectQuery retornou NULL");
			return false;
		}
		
		sys_err(
			"ItemShop::SendItemShopLog - rows=%u",
			pmsg_log->Get()->uiNumRows
		);

		std::vector<TItemShopLog> logs;
		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pmsg_log->Get()->pSQLResult)))
		{
			TItemShopLog log;
			memset(&log, 0, sizeof(log));
			strlcpy(log.data, row[0], sizeof(log.data));
			str_to_number(log.vnum, row[1]);
			str_to_number(log.quantidade, row[2]);
			str_to_number(log.currencyType, row[3]);
			str_to_number(log.currencyVnum, row[4]);
			str_to_number(log.currencyCount, row[5]);
			strlcpy(log.player_name, row[6], sizeof(log.player_name));
			str_to_number(log.accountID, row[7]);
			
			sys_err(
				"LOG DB: offset=%u data=%s vnum=%u qnt=%u moeda=%u moeda_vnum=%u valor=%u",
				offset,
				log.data,
				log.vnum,
				log.quantidade,
				log.currencyType,
				log.currencyVnum,
				log.currencyCount
			);
			logs.push_back(log);
		}
		
		TPacketGCItemShopLogList payload;
		payload.count = static_cast<WORD>(logs.size());
		size_t totalSize =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopLogList)
			+ logs.size() * sizeof(TItemShopLog);
			
		if (totalSize > 0xFFFF)
		{
			sys_err(
				"ItemShop::SendLogList - pacote grande demais: %u",
				(unsigned int)totalSize
			);
			return false;
		}
		TPacketGCItemShop header;
		header.header = HEADER_GC_ITEMSHOP;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = ITEMSHOP_GC_LOG;
		std::vector<char> buffer(totalSize);
		size_t bufferOffset = 0;
		memcpy(&buffer[bufferOffset], &header, sizeof(header));
		bufferOffset += sizeof(header);
		memcpy(&buffer[bufferOffset], &payload, sizeof(payload));
		bufferOffset += sizeof(payload);
		for (size_t i = 0; i < logs.size(); ++i)
		{
			memcpy(
				&buffer[bufferOffset],
				&logs[i],
				sizeof(TItemShopLog)
			);
			bufferOffset += sizeof(TItemShopLog);
		}
		if (bufferOffset != totalSize)
		{
			sys_err(
				"ItemShop::SendLogList - montagem incorreta (%u/%u)",
				(unsigned int)bufferOffset,
				(unsigned int)totalSize
			);
			return false;
		}
		sys_err(
			"ItemShop::SendItemShopLog - "
			"header=%u size=%u payload=%u logs=%u itemSize=%u total=%u",
			header.header,
			header.size,
			(unsigned int)sizeof(TPacketGCItemShopLogList),
			(unsigned int)logs.size(),
			(unsigned int)sizeof(TItemShopLog),
			(unsigned int)buffer.size()
		);
		d->Packet(buffer.data(), buffer.size());
		return true;
	}
	
	bool SearchItemShopLog(LPCHARACTER ch, const char* search, DWORD offset)
	{
		if (!ch || !search || !*search)
			return false;

		LPDESC d = ch->GetDesc();

		if (!d)
			return false;

		// A busca será inicialmente exclusiva para GM.
		if (ch->GetGMLevel() < GM_LOW_WIZARD)
			return false;

		std::unique_ptr<SQLMsg> pmsg_log;

		bool isNumber = true;

		for (const char* p = search; *p; ++p)
		{
			if (!isdigit(static_cast<unsigned char>(*p)))
			{
				isNumber = false;
				break;
			}
		}

		if (isNumber)
		{
			DWORD vnum = 0;
			str_to_number(vnum, search);

			pmsg_log = DBManager::instance().DirectQuery(
				"SELECT data, item, quantidade, moeda, moeda_vnum, valor, player_name, account_id "
				"FROM item_shop.log_itens "
				"WHERE item = %u "
				"ORDER BY data DESC "
				"LIMIT 50 OFFSET %u",
				vnum,
				offset
			);
		}
		else
		{
			pmsg_log = DBManager::instance().DirectQuery(
				"SELECT data, item, quantidade, moeda, moeda_vnum, valor, player_name, account_id "
				"FROM item_shop.log_itens "
				"WHERE player_name LIKE '%%%s%%' "
				"ORDER BY data DESC "
				"LIMIT 50 OFFSET %u",
				search,
				offset
			);
		}

		if (!pmsg_log)
		{
			sys_err(
				"ItemShop::SearchItemShopLog - DirectQuery retornou NULL"
			);
			return false;
		}


		std::vector<TItemShopLog> logs;

		MYSQL_ROW row = NULL;

		while ((row = mysql_fetch_row(pmsg_log->Get()->pSQLResult)))
		{
			TItemShopLog log;
			memset(&log, 0, sizeof(log));

			strlcpy(log.data, row[0], sizeof(log.data));
			str_to_number(log.vnum, row[1]);
			str_to_number(log.quantidade, row[2]);
			str_to_number(log.currencyType, row[3]);
			str_to_number(log.currencyVnum, row[4]);
			str_to_number(log.currencyCount, row[5]);
			strlcpy(log.player_name, row[6], sizeof(log.player_name));
			str_to_number(log.accountID, row[7]);

			logs.push_back(log);
		}

		TPacketGCItemShopLogList payload;
		payload.count = static_cast<WORD>(logs.size());

		size_t totalSize =
			sizeof(TPacketGCItemShop)
			+ sizeof(TPacketGCItemShopLogList)
			+ logs.size() * sizeof(TItemShopLog);

		if (totalSize > 0xFFFF)
		{
			sys_err(
				"ItemShop::SearchItemShopLog - pacote grande demais: %u",
				static_cast<unsigned int>(totalSize)
			);

			return false;
		}

		TPacketGCItemShop header;

		header.header = HEADER_GC_ITEMSHOP;
		header.size = static_cast<WORD>(totalSize);
		header.subheader = ITEMSHOP_GC_LOG;

		std::vector<char> buffer(totalSize);

		size_t bufferOffset = 0;

		memcpy(
			&buffer[bufferOffset],
			&header,
			sizeof(header)
		);

		bufferOffset += sizeof(header);

		memcpy(
			&buffer[bufferOffset],
			&payload,
			sizeof(payload)
		);

		bufferOffset += sizeof(payload);

		for (size_t i = 0; i < logs.size(); ++i)
		{
			memcpy(
				&buffer[bufferOffset],
				&logs[i],
				sizeof(TItemShopLog)
			);

			bufferOffset += sizeof(TItemShopLog);
		}

		if (bufferOffset != totalSize)
		{
			sys_err(
				"ItemShop::SearchItemShopLog - "
				"montagem incorreta (%u/%u)",
				static_cast<unsigned int>(bufferOffset),
				static_cast<unsigned int>(totalSize)
			);

			return false;
		}

		d->Packet(buffer.data(), buffer.size());
		
		
		return true;
	}
	
	bool AdmAddItem(LPCHARACTER ch, const TPacketCGItemShopAdminAddItem& packet)
	{
		if (!ch)
			return false;

		if (ch->GetGMLevel() < GM_LOW_WIZARD)
		{
			SendAdmAddResult(
				ch,
				ITEMSHOP_ADMIN_ADD_NO_PERMISSION
			);

			return false;
		}

		DWORD vnum = packet.vnum;
		DWORD vnumPrice = packet.currencyVnum;

		CItem* item = ITEM_MANAGER::instance().CreateItem(vnum);

		if (!item)
		{
			sys_err(
				"ITEMSHOP ADM ADD: erro item não existe vnum=%u",
				vnum
			);

			SendAdmAddResult(
				ch,
				ITEMSHOP_ADMIN_ADD_ITEM_NOT_FOUND
			);

			return false;
		}

		if (packet.currencyType == 4)
		{
			CItem* itemPrice =
				ITEM_MANAGER::instance().CreateItem(vnumPrice);

			if (!itemPrice)
			{
				sys_err(
					"ITEMSHOP ADM ADD: erro item_moeda não existe vnum=%u",
					vnumPrice
				);

				M2_DESTROY_ITEM(item);
				
				SendAdmAddResult(
					ch,
					ITEMSHOP_ADMIN_ADD_CURRENCY_NOT_FOUND
				);

				return false;
			}

			M2_DESTROY_ITEM(itemPrice);
		}

		auto pAddMsg = DBManager::instance().DirectQuery(
			"INSERT INTO item_shop.items "
			"(category_id, vnum, count, currency_type, currency_count, currency_vnum, discount) "
			"VALUES (%d, %d, %d, %d, %d, %d, %d)",
			packet.category,
			packet.vnum,
			packet.count,
			packet.currencyType,
			packet.price,
			packet.currencyVnum,
			packet.discount
		);

		M2_DESTROY_ITEM(item);

		if (!pAddMsg)
		{
			sys_err(
				"ITEMSHOP ADM ADD: erro ao inserir item. vnum=%u",
				packet.vnum
			);

			SendAdmAddResult(
				ch,
				ITEMSHOP_ADMIN_ADD_DATABASE_ERROR
			);

			return false;
		}

		SendAdmAddResult(
			ch,
			ITEMSHOP_ADMIN_ADD_SUCCESS
		);

		return true;
	}
	
	void SendAdmAddResult(LPCHARACTER ch, BYTE result)
	{
		if (!ch)
			return;

		TPacketGCItemShopAdminAddResult packet;

		packet.header = HEADER_GC_ITEMSHOP_ADMIN;
		packet.subheader = ITEMSHOP_ADMIN_GC_ADD_RESULT;
		packet.result = result;

		ch->GetDesc()->Packet(
			&packet,
			sizeof(packet)
		);
	}
}