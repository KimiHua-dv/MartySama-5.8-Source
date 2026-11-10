#pragma once

#include "Locale.h"
#include "../gamelib/RaceData.h"
#include "../gamelib/ItemData.h"

// Cliente solicita abertura da loja
#pragma pack(push, 1)

typedef struct SPacketGCItemShop
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCItemShop;

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SPacketCGItemShop
{
    BYTE header;
    BYTE subheader;
    BYTE category;
} TPacketCGItemShop;
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

typedef struct SPacketGCItemShopBuyResult
{
    BYTE result;
} TPacketGCItemShopBuyResult;
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
#pragma pack(pop)

#pragma pack(push, 1)

typedef struct SPacketCGItemShopAdminAddItem
{
    BYTE header;
    BYTE subheader;

    DWORD vnum;
    DWORD count;
    DWORD price;

    BYTE discount;
    BYTE category;
    BYTE currencyType;

    DWORD currencyVnum;

} TPacketCGItemShopAdminAddItem;

#pragma pack(pop)