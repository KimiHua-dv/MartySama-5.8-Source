#include "stdafx.h"
#include "switchbonus.h"
#include "char.h"
#include "locale_service.h"
#include "packet.h"
#include "desc_client.h"
#include "item_manager.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"

CSwitchBonus::CSwitchBonus()
{
}

CSwitchBonus::~CSwitchBonus()
{
	
}

bool CSwitchBonus::CheckElementsSB(LPCHARACTER ch){
	if (ch->GetExchange() || ch->GetShop() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		return true;
	return false;
}

bool CSwitchBonus::CanMoveIntoSB(LPITEM item)
{
	if(!item){return false;}
	if(item->GetType()==ITEM_WEAPON && item->GetSubType() != WEAPON_ARROW){return true;}
	if(item->GetType()==ITEM_ARMOR){return true;}
	return false;
}

void CSwitchBonus::OpenSB(LPCHARACTER ch)
{
	if (ch->IsSBOpen()){
		return;
	}

	ch->SetSBOpen(true);
	SendSBPackets(ch,SWITCH_RECEIVE_OPEN);

	LPITEM item = ch->GetSwitchBotItem(0);
	if(item){
		SendSBItem(ch,item);
	}
}

void CSwitchBonus::CloseSB(LPCHARACTER ch)
{
	ch->SetSBOpen(false);
}

void CSwitchBonus::ClearSB(LPCHARACTER ch)
{
	SendSBPackets(ch,SWITCH_RECEIVE_CLEAR_INFO);
}


void CSwitchBonus::SendSBItem(LPCHARACTER ch, LPITEM item)
{
	if (!item)
		return;

	ClearSB(ch);

	int iAttributeSet = item->GetAttributeSetIndex();

	// Bônus normais
	for (int b = 0; b < MAX_APPLY_NUM; ++b)
	{
		const TItemAttrTable& r = g_map_itemAttr[b];

		if (r.dwApplyIndex != 0 &&
			r.bMaxLevelBySet[iAttributeSet] > 0)
		{
			SendSBPackets(
				ch,
				SWITCH_RECEIVE_INFO_BONUS_ITEM,
				b,
				r.lValues[0],
				r.lValues[1],
				r.lValues[2],
				r.lValues[3],
				r.lValues[4]);
		}
	}

	// Bônus raros
	for (int b = 0; b < MAX_APPLY_NUM; ++b)
	{
		const TItemAttrTable& r = g_map_itemRare[b];

		if (r.dwApplyIndex != 0 &&
			r.bMaxLevelBySet[iAttributeSet] > 0)
		{
			SendSBPackets(
				ch,
				SWITCH_RECEIVE_INFO_RARE_BONUS_ITEM,
				b,
				r.lValues[0],
				r.lValues[1],
				r.lValues[2],
				r.lValues[3],
				r.lValues[4]);
		}
	}

	TItemTable const* pProto = item->GetProto();

	if (pProto && pProto->sAddonType)
	{
		SendSBPackets(ch, SWITCH_RECEIVE_INFO_EXTRA);
	}
}


void CSwitchBonus::ChangeSB(LPCHARACTER ch,const TPlayerItemAttribute* aAttr)
{
	LPITEM item = ch->GetSwitchBotItem(0);

	if(!item)
		return;

	if (!ch->IsSBOpen()){
		return;
	}

	if (item->CheckBonusSwitchBot(aAttr))
	{
		ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Succefull.");
		return;
	}

	int count_change = ch->CountSpecifyItem(VNUM_CHANGE_ITEM);

	bool change_succefull = false;

	if (count_change > 0)
	{
		int result = item->ChangeAttributeSwtichBot(
			count_change,
			aAttr,
			&change_succefull
		);
		item->UpdatePacket();
		item->Save();

		if(result > 0)
		{
			if(change_succefull)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SWITCHBOT_SUCCEFULL %d",result);
			}else{
				ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Fail.");
			}

			ch->RemoveSpecifyItem(VNUM_CHANGE_ITEM,result);

		}else{

			ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Fail.");
		}
	}else{
		ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Not enough switch items.");
	}
}


void CSwitchBonus::SendSBPackets(
	LPCHARACTER ch,
	BYTE subheader,
	int id_bonus,
	int value_bonus_0,
	int value_bonus_1,
	int value_bonus_2,
	int value_bonus_3,
	int value_bonus_4)
{
	TPacketGCSwitchBotReceive pack;
	pack.bSubType = subheader;

	if (subheader == SWITCH_RECEIVE_INFO_BONUS_ITEM ||
		subheader == SWITCH_RECEIVE_INFO_RARE_BONUS_ITEM)
	{
		pack.id_bonus = id_bonus;
		pack.value_bonus_0 = value_bonus_0;
		pack.value_bonus_1 = value_bonus_1;
		pack.value_bonus_2 = value_bonus_2;
		pack.value_bonus_3 = value_bonus_3;
		pack.value_bonus_4 = value_bonus_4;
	}

	LPDESC d = ch->GetDesc();

	if (NULL == d)
	{
		sys_err("User CSwitchBonus::SendSBPackets (%s)'s DESC is NULL POINT.",
			ch->GetName());
		return;
	}

	d->Packet(&pack, sizeof(pack));
}


void CSwitchBonus::ChangeRareSB(LPCHARACTER ch,const TPlayerItemAttribute* aAttr)
{
	if (!ch || !aAttr)
		return;

	if (!ch->IsSBOpen())
		return;

	LPITEM item = ch->GetSwitchBotItem(0);

	if (!item)
		return;

	if (item->IsExchanging())
		return;

	if (item->GetAttributeSetIndex() == -1)
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"Este item nao pode ter seus atributos alterados."
		);
		return;
	}

	if (item->GetRareAttrCount() <= 0)
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"Este item nao possui bonus raros para alterar."
		);
		return;
	}

	// Se os bônus selecionados já estiverem presentes, não consome itens.
	if (item->CheckRareBonusSwitchBot(aAttr))
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"Os bonus raros selecionados ja estao ativos."
		);
		return;
	}

	const DWORD VNUM_RARE_CHANGE_ITEM = 71052;
	const int count_change =
		ch->CountSpecifyItem(VNUM_RARE_CHANGE_ITEM);

	if (count_change <= 0)
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"Voce nao possui o item de alteracao de bonus raros."
		);
		return;
	}
	
	for (int i = 0; i < ITEM_ATTRIBUTE_RARE_NUM; ++i)
	{
		sys_log(0,
			"SWITCHBOT RARE TARGET[%d]: type=%u value=%d",
			i,
			aAttr[i].bType,
			aAttr[i].sValue
		);
	}

	bool change_succefull = false;

	const int result = item->ChangeRareAttributeSwitchBot(
		count_change,
		aAttr,
		&change_succefull
	);

	if (result <= 0)
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"Nao foi possivel alterar os bonus raros."
		);
		return;
	}

	item->UpdatePacket();
	item->Save();
	
	sys_log(0,
		"SWITCHBOT RARE RESULT: attempts=%d success=%d",
		result,
		change_succefull
	);

	// Consome somente a quantidade de tentativas realizadas.
	ch->RemoveSpecifyItem(VNUM_RARE_CHANGE_ITEM, result);

	if (change_succefull)
	{
		ch->ChatPacket(
			CHAT_TYPE_COMMAND,
			"SWITCHBOT_SUCCEFULL %d",
			result
		);
	}
	else
	{
		ch->ChatPacket(
			CHAT_TYPE_INFO,
			"[SB]Alteracao dos bonus raros nao encontrou os valores desejados."
		);
	}
}