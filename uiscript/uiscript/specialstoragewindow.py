import uiScriptLocale
import localeInfo
import app

ENVANTER_ICON = "d:/ymir work/ui/special_inventory/"

window = {
	"name" : "SpecialStorageWindow",

	"x" : SCREEN_WIDTH - 400,
	"y" : SCREEN_HEIGHT / 3,

	"style" : ("movable", "float",),

	"width" : 184,
	"height" : 328+32+30+5,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 184,
			"height" : 328+32+30+5,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 169,
					"color" : "gray",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":84, "y":4, "text": "Toplu Env", "text_horizontal_align":"center" },
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 12,
					"y" : 34,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},
				################---INVENTORY---################
				{
					"name" : "Inventory_Tab_01",
					"type" : "radio_button",

					"x" : 16,
					"y" : 295+32,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,

					"children" :
					(
						{
							"name" : "Inventory_Tab_01_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "I",
						},
					),
				},
				{
					"name" : "Inventory_Tab_02",
					"type" : "radio_button",

					"x" : 16 + 39,
					"y" : 295+32,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,

					"children" :
					(
						{
							"name" : "Inventory_Tab_02_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "II",
						},
					),
				},
				{
					"name" : "Inventory_Tab_03",
					"type" : "radio_button",

					"x" : 16 + 39 + 39,
					"y" : 295+32,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,

					"children" :
					(
						{
							"name" : "Inventory_Tab_03_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "III",
						},
					),
				},
				{
					"name" : "Inventory_Tab_04",
					"type" : "radio_button",

					"x" : 16 + 39 + 39 + 39,
					"y" : 295+32,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,

					"children" :
					(
						{
							"name" : "Inventory_Tab_04_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "IV",
						},
					),
				},
				################---INVENTORY---################
				{
					"name" : "Category_Tab_01",
					"type" : "radio_button",

					"x" : -4+14,
					"y" : 295+32+24,

					"default_image"	: ENVANTER_ICON+"special_upp_00.png",
					"over_image"	: ENVANTER_ICON+"special_upp_01.png",
					"down_image"	: ENVANTER_ICON+"special_upp_02.png",

					"tooltip_text" : localeInfo.SPECIAL_INVENTORY_UPGRADE,
				},
				{
					"name" : "Category_Tab_02",
					"type" : "radio_button",

					"x" : -4+16+38-7,
					"y" : 295+32+24,

					"default_image"	: ENVANTER_ICON+"special_book_00.png",
					"over_image"	: ENVANTER_ICON+"special_book_01.png",
					"down_image"	: ENVANTER_ICON+"special_book_02.png",

					"tooltip_text" : localeInfo.SPECIAL_INVENTORY_BOOK,
				},
				{
					"name" : "Category_Tab_03",
					"type" : "radio_button",

					"x" : -4+14+52+30-7*2-2,
					"y" : 295+32+24,

					"default_image"	: ENVANTER_ICON+"special_stone_00.png",
					"over_image"	: ENVANTER_ICON+"special_stone_01.png",
					"down_image"	: ENVANTER_ICON+"special_stone_02.png",

					"tooltip_text" : localeInfo.SPECIAL_INVENTORY_STONE,
				},
				{
					"name" : "Category_Tab_04",
					"type" : "radio_button",

					"x" : -4+13+52+52+22-7*3-5,
					"y" : 295+32+24,

					"default_image"	: ENVANTER_ICON+"special_chest_00.png",
					"over_image"	: ENVANTER_ICON+"special_chest_01.png",
					"down_image"	: ENVANTER_ICON+"special_chest_02.png",

					"tooltip_text" : localeInfo.SPECIAL_INVENTORY_CHEST,
				},
				{
					"name" : "Category_Tab_05",
					"type" : "radio_button",

					"x" : -4+13+52+52+27,
					"y" : 295+32+24,

					"default_image"	: ENVANTER_ICON+"special_costume_00.png",
					"over_image"	: ENVANTER_ICON+"special_costume_01.png",
					"down_image"	: ENVANTER_ICON+"special_costume_02.png",

					"tooltip_text" : localeInfo.SPECIAL_INVENTORY_COSTUME,
				},
			),
		},
	),
}
