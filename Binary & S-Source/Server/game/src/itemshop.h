#ifndef __INC_ITEMSHOP_H__
#define __INC_ITEMSHOP_H__

#include "char.h"

namespace ItemShop
{
	void SendOpen(LPCHARACTER ch);
	void SendCategoryList(LPCHARACTER ch);
	void SendItemList(LPCHARACTER ch, BYTE category);
	void BuyItem(LPCHARACTER ch, DWORD itemID);
	void SendBuyResult(LPCHARACTER ch, BYTE result);
	void SendHomeOffers(LPCHARACTER ch);
	bool CanAfford(LPCHARACTER ch,DWORD accountID,BYTE currencyType, DWORD currencyVnum,DWORD amount);
	bool ProcessPayment(LPCHARACTER ch,BYTE currencyType,DWORD currencyVnum,DWORD amount);
	bool ProcessCashPayment(LPCHARACTER ch,DWORD amount);
	bool ProcessYangPayment(LPCHARACTER ch,DWORD amount);
	bool ProcessChequePayment(LPCHARACTER ch,DWORD amount);
	bool ProcessItemPayment(LPCHARACTER ch,DWORD currencyVnum,DWORD amount);
	bool SendItemShopLog(LPCHARACTER ch, DWORD offset);
	bool SearchItemShopLog(LPCHARACTER ch,const char* search,DWORD offset);
	bool AdmAddItem(LPCHARACTER ch, const TPacketCGItemShopAdminAddItem& packet);
	void SendAdmAddResult(LPCHARACTER ch, BYTE result);
}

enum EItemShopCurrencyResult
{
    CURRENCY_SUCCESS,
    CURRENCY_INSUFFICIENT,
    CURRENCY_INVALID,
    CURRENCY_ITEM_NOT_FOUND,
    CURRENCY_ERROR
};

enum EItemShopLog
{
    ITEMSHOP_GC_LOG = 6,
    ITEMSHOP_CG_REQUEST_LOG = 7,
	ITEMSHOP_CG_SEARCH_LOG
};

#pragma pack(push, 1)

typedef struct SPacketGCItemShopLogList
{
    WORD count;
} TPacketGCItemShopLogList;

typedef struct SItemShopLog
{
    char  data[20];
    DWORD vnum;
    BYTE quantidade;
    BYTE  currencyType;
	DWORD currencyVnum;
    DWORD currencyCount;
    DWORD accountID;
    char  player_name[25];
} TItemShopLog;

typedef struct SPacketCGItemShopLog
{
    BYTE header;
    BYTE subheader;
    DWORD offset;
} TPacketCGItemShopLog;

typedef struct SPacketCGItemShopSearchLog
{
    BYTE header;
    BYTE subheader;
    DWORD offset;
    char search[64];
} TPacketCGItemShopSearchLog;


enum EItemShopAdminCGSubHeaders
{
    ITEMSHOP_ADMIN_CG_ADD_ITEM = 0,
    ITEMSHOP_ADMIN_CG_REMOVE_ITEM,
    ITEMSHOP_ADMIN_CG_UPDATE_ITEM,
};

enum EItemShopAdminGCSubHeaders
{
    ITEMSHOP_ADMIN_GC_ADD_RESULT = 0,
    ITEMSHOP_ADMIN_GC_REMOVE_RESULT,
    ITEMSHOP_ADMIN_GC_UPDATE_RESULT,
};
#pragma pack(pop)

enum EItemShopAdminAddResult
{
    ITEMSHOP_ADMIN_ADD_SUCCESS = 0,
    ITEMSHOP_ADMIN_ADD_NO_PERMISSION,
    ITEMSHOP_ADMIN_ADD_ITEM_NOT_FOUND,
    ITEMSHOP_ADMIN_ADD_CURRENCY_NOT_FOUND,
    ITEMSHOP_ADMIN_ADD_DATABASE_ERROR,
};

#pragma pack(push, 1)

typedef struct SPacketGCItemShopAdminAddResult
{
    BYTE header;
    BYTE subheader;
    BYTE result;
} TPacketGCItemShopAdminAddResult;

#pragma pack(pop)
enum EItemShopCGSubHeaders
{
    ITEMSHOP_CG_OPEN = 0,
    ITEMSHOP_CG_BUY,
    ITEMSHOP_CG_CLOSE,
};

enum EItemShopGCSubHeaders
{
    ITEMSHOP_GC_OPEN = 0,
    ITEMSHOP_GC_CATEGORY_LIST,
    ITEMSHOP_CG_SELECT_CATEGORY,
    ITEMSHOP_GC_ITEM_LIST,
    ITEMSHOP_GC_BUY_RESULT,
    ITEMSHOP_GC_CLOSE,
};

enum EItemShopBuyResult
{
	ITEMSHOP_BUY_SUCCESS = 0,
	ITEMSHOP_BUY_CASH_INSUFFICIENT = 1,
	ITEM_SHOP_SUBHEADER_GC_INVENTORY_FULL,
};




//ITEM_SHOP#endif
// Cliente solicita abertura da loja
#pragma pack(push, 1)

typedef struct SPacketGCItemShop
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCItemShop;

#pragma pack(pop)

//lista de categorias
typedef struct SPacketGCItemShopCategoryList
{
    WORD count;
} TPacketGCItemShopCategoryList;

#pragma pack(push, 1)
 // lista de itens
typedef struct SPacketGCItemShopItemList
{
    WORD count;
    BYTE category;
} TPacketGCItemShopItemList;
#pragma pack(pop)

// Cliente compra um item
#pragma pack(push, 1)

typedef struct SPacketCGItemShopBuy
{
	BYTE header;
	BYTE subheader;
	DWORD itemID;
} TPacketCGItemShopBuy;

#pragma pack(pop)


// Dados de um item da loja
#pragma pack(push, 1)

typedef struct SItemShopData
{
	DWORD id;
	DWORD vnum;
	DWORD count;
	DWORD price;
	BYTE discount;
	BYTE category;
	BYTE currencyType;
	DWORD currencyVnum;
} TItemShopData;
#pragma pack(pop)

//seleciona gategoria
typedef struct SPacketCGItemShopCategory
{
    BYTE header;
    BYTE subheader;

    BYTE category;
} TPacketCGItemShopCategory;


typedef struct SItemShopCategory
{
    BYTE id;
    char name[32];

} TItemShopCategory;

#pragma pack()

#pragma pack(push, 1)

typedef struct SPacketGCItemShopBuyResult
{
	BYTE result;
} TPacketGCItemShopBuyResult;

#pragma pack(pop)

#pragma pack(push, 1)

typedef struct SPacketGCItemShopAdmin
{
    BYTE header;
    BYTE subheader;
    BYTE result;
} TPacketGCItemShopAdmin;

#pragma pack(pop)
#endif