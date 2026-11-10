#pragma once


#ifndef __INC_PACKET_ITEMSHOP_H__
#define __INC_PACKET_ITEMSHOP_H__


enum EItemShopHeaders
{
    HEADER_CG_ITEMSHOP = 200,
    HEADER_GC_ITEMSHOP = 201,
    HEADER_CG_ITEMSHOP_ADMIN = 202,
    HEADER_GC_ITEMSHOP_ADMIN = 203,
    HEADER_GC_ITEMSHOP_ADMIN_PERMISSION = 204,
};

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

#pragma pack(push, 1)

typedef struct SPacketGCItemShopAdmin
{
    BYTE header;
    BYTE subheader;
    BYTE result;
} TPacketGCItemShopAdmin;

typedef struct SPacketGCItemShopAdminPermission
{
    BYTE header;
    BYTE isImplementor;
} TPacketGCItemShopAdminPermission;
#pragma pack(pop)
#endif