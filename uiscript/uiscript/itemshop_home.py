import uiScriptLocale

window = {
    "name" : "ItemShopHome",

    "x" : 160,
    "y" : 50,

    "width" : 800,
    "height" : 500,

    "children" :
    (
        {
            "name" : "banner",
            "type" : "expanded_image",
			"align" : "center",
            "x" : 10,
            "y" : 0,
			"image" : "d:/ymir work/ui/itemshop/banner.tga",
            "width" : 780,
            "height" : 140,
        },

        {
            "name" : "destaques",
            "type" : "thinboard",

            "x" : 10,
            "y" : 150,

            "width" : 210,
            "height" : 130*3,
			
			"children":
				(
					{
						"name" : "Texto_destaque",
						"type" : "text",
						"x" : 30, "y" : 5,
						"text" : "Mais Vendidos",
					},
				),
        },

        {
            "name" : "promocoes",
            "type" : "thinboard",

            "x" : 250,
            "y" : 150,
			
			"width" : 210,
            "height" : 130*3,
			
            "children":
				(
					{
						"name" : "Texto_promocoes",
						"type" : "text",
						"x" : 30, "y" : 5,
						"text" : "Ofertas da Semana",
					},
				),
        },

        {
            "name" : "novidades",
            "type" : "thinboard",

            "x" : 480,
            "y" : 150,

            "width" : 310,
            "height" : 130*3,
			"children":
				(
					{
						"name" : "Texto_Novidades",
						"type" : "text",
						"x" : 30, "y" : 5,
						"text" : "Novidades",
					},
				),
        },
    ),
}