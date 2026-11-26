import uiScriptLocale

window = {
	"name" : "ItemShop",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),
	"align" : "center",
	"width" : 980,
	"height" : 650,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 980,
			"height" : 650,

			"title" : "Loja de Itens",

			"children" :
			(
				{
					"name" : "title",

					"type" : "text",

					"x" : 20,
					"y" : 40,

					"text" : "Bem vindo à Loja de Itens",
				},

				{
					"name" : "categoryPanel",

					"type" : "thinboard",

					"x" : 20,
					"y" : 80,

					"width" : 120,
					"height" : 500,
				},

				{
					"name" : "contentPanel",

					"type" : "window",

					"x" : 220,
					"y" : 80,

					"width" : 800,
					"height" : 500,
				},
				
				
			),
		},
	),
}