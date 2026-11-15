import uiScriptLocale

WINDOW_WIDTH = 980
WINDOW_HEIGHT = 570
MAP_AREA_WIDTH = 342
MAP_AREA_HEIGHT = 456

window = {
    "name" : "TeleportSystem",
    "style" : ("movable", "float",),
    "x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
    "y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
    "width" : WINDOW_WIDTH,
    "height" : WINDOW_HEIGHT,
    "children" :
    (
        {
            "name" : "board",
            "type" : "board_with_titlebar",
            "style" : ("attach",),
            "x" : 0,
            "y" : 0,
            "width" : WINDOW_WIDTH,
            "height" : WINDOW_HEIGHT,
            "title" : uiScriptLocale.TELEPORT_TITLE,
            "children" :
            (
                {
                    "name" : "map_area",
                    "type" : "window",
                    "style" : ("attach", "ltr",),
                    "x" : 20,
                    "y" : 52,
                    "width" : MAP_AREA_WIDTH,
                    "height" : MAP_AREA_HEIGHT,
                },
				{
                    "name" : "object_area_window",
                    "type" : "window",
                    "style" : ("attach", "ltr",),
                    "x" : 720,
                    "y" : 52,
                    "width" : 200,
                    "height" : MAP_AREA_HEIGHT,
                },
				{
					"name" : "scroll_bar",
					"type" : "scrollbar",

					"x" : WINDOW_WIDTH - 30,
					"y" : 40,
					"size" : WINDOW_HEIGHT - 75,
				},

                {
                    "name" : "action_panel",
                    "type" : "thinboard_gold",
                    "style" : ("attach", "ltr",),
                    "x" : 380,
                    "y" : 52,
                    "width" : 320,
                    "height" : MAP_AREA_HEIGHT,
                    "children" :
                    (
                        {
                            "name" : "panel_title",
                            "type" : "text",
                            "x" : 0,
                            "y" : 12,
                            "text" : uiScriptLocale.TELEPORT_CHOOSE,
                            "horizontal_align" : "center",
                            "text_horizontal_align" : "center",
                        },
                        {
                            "name" : "panel_hint",
                            "type" : "text",
                            "x" : 12,
                            "y" : 42,
                            "text" : "Clique em um ponto do mapa",
                        },
                    ),
                },
                {
                    "name" : "gobutton",
                    "type" : "button",
                    "x" : 450,
                    "y" : 520,
                    "text" : uiScriptLocale.TELEPORT_GO,
                    "default_image" : "d:/ymir work/ui/public/XLarge_Button_01.sub",
                    "over_image" : "d:/ymir work/ui/public/XLarge_Button_02.sub",
                    "down_image" : "d:/ymir work/ui/public/XLarge_Button_03.sub",
                },
            ),
        },
    ),
}


