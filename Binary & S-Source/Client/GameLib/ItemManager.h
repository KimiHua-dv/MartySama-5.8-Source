#pragma once

#include "ItemData.h"
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
#include <map>
#endif

class CItemManager : public CSingleton<CItemManager>
{
	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		enum EItemScaleCol
		{
			ITEMSCALE_COL_VNUM,
			ITEMSCALE_COL_JOB,
			ITEMSCALE_COL_SEX,
			ITEMSCALE_COL_SCALE_X,
			ITEMSCALE_COL_SCALE_Y,
			ITEMSCALE_COL_SCALE_Z,
			ITEMSCALE_COL_PARTICLE_SCALE,
		};
#endif

	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;

	public:
		CItemManager();
		virtual ~CItemManager();

		void			Destroy();

		BOOL			SelectItemData(DWORD dwIndex);
		CItemData *		GetSelectedItemDataPointer();

		BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		bool			GetItemByName(const char* c_szInput, CItemData** ppItemData);
		void			GetItemListByName(const char* c_szInput, std::vector<std::pair<DWORD, std::string>>& r_vec_itemName);
#endif

		/////
		bool			LoadItemDesc(const char* c_szFileName);
		bool			LoadItemList(const char* c_szFileName);
		bool			LoadItemTable(const char* c_szFileName);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool			LoadItemScale(const char* szItemScale);
#endif
		const TItemMap& GetItemMap() const;

		CItemData *		MakeItemData(DWORD dwIndex);

	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*>  m_vec_ItemRange;
		CItemData * m_pSelectedItemData;
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		std::multimap<std::string, CItemData*>	m_NameItemMap;
#endif
};

