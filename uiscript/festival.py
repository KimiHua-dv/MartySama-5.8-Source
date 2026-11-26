
window = {
	"name" : "Festival",
	
	"x": 0, "y" : 0,
	"width": 700, "height" : 550,
	"style" : ("movable", "float",),
	"align" : "center",
	"children":
	(
		{
			"name": "board",
			"type" : "board_with_titlebar",
			"x": 0, "y" : 0,
			"width": 700, "height" : 550,
			"align" : "center",
			"title" : "Painel Festival de Guildas",
			"children" :
			(
				{
					"name": "painel_lateral_e",
					"type" : "thinboard_circle",
					"x": 10, "y": 30,
					"width": 150, "height": 505,
					"children":
					(
						{
							"name": "btn_m",
							"type": "button",
							"x": 10, "y": 15,
							"text": "Missões",
							"default_image" : "festival/bt_01.tga",
							"over_image" : "festival/bt_02.tga",
							"down_image" : "festival/bt_03.tga",
						},
						{
							"name": "btn_r",
							"type": "button",
							"x": 10, "y": 65,
							"text": "Recompensas",
							"default_image" : "festival/bt_01.tga",
							"over_image" : "festival/bt_02.tga",
							"down_image" : "festival/bt_03.tga",
						},
						{
							"name": "btn_re",
							"type": "button",
							"x": 10, "y": 115,
							"text": "Regras",
							"default_image" : "festival/bt_01.tga",
							"over_image" : "festival/bt_02.tga",
							"down_image" : "festival/bt_03.tga",
						},
						{
							"name": "btn_c",
							"type": "button",
							"x": 10, "y": 165,
							"text": "Classificação",
							"default_image" : "festival/bt_01.tga",
							"over_image" : "festival/bt_02.tga",
							"down_image" : "festival/bt_03.tga",
						},
						{
							"name": "btn_p",
							"type": "button",
							"x": 10, "y": 215,
							"text": "Participação",
							"default_image" : "festival/bt_01.tga",
							"over_image" : "festival/bt_02.tga",
							"down_image" : "festival/bt_03.tga",
						},
					),
				},
				{
					"name": "painel_lateral_d_cima",
					"type" : "thinboard_circle",
					"x": 535, "y": 30,
					"width": 150, "height": 70,
					"children":
					(
						{
							"name" : "TitleBar","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 140,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Guilda", "align":"center" },
							),
						},
						{"name":"Nome", "type":"text", "x":10, "y":25, "text":"Nome: "},
						{"name":"Nivel", "type":"text", "x":10, "y":37, "text":"Nivel:"},
						{"name":"Pontos", "type":"text", "x":10, "y":49, "text":"Pontos:"},
					),
				},
				{
					"name": "painel_lateral_d_1",
					"type" : "thinboard_circle",
					"x": 535, "y": 102,
					"width": 150, "height": 70,
					"children":
					(
						{
							"name" : "TitleBar","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 140,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Progresso", "align":"center" },
							),
						},
					),
				},
				{
					"name": "painel_lateral_d_2",
					"type" : "thinboard_circle",
					"x": 535, "y": 174,
					"width": 150, "height": 120,
					"children":
					(
						{
							"name" : "TitleBar","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 140,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Renovar Missões", "align":"center" },
							),
						},
						{"name": "info", "type": "text", "x" : 8, "y": 23, "text": "Ao terminar um ciclo,"},
						{"name": "info1", "type": "text", "x" : 8, "y": 33, "text": "as missões serão renovadas."},
						{"name": "info2", "type": "text", "x" : 8, "y": 43, "text": "O Lider da Guilda poderá"},
						{"name": "info3", "type": "text", "x" : 8, "y": 53, "text": "acelerar essa renovação!"},
						{"name": "info4", "type": "text", "x" : 8, "y": 63, "text": "Todas as missões precisam"},
						{"name": "info5", "type": "text", "x" : 8, "y": 73, "text": "estar concluidas."},
						{"name": "custo", "type": "text", "x" : 25, "y": 93, "text": "Custo: 300.000"},
						{"name": "renovar", "type": "button", "x": 95, "y": 90, 
						 "text": "Ativar",
						"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
						"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
						"down_image" : "d:/ymir work/ui/public/small_button_03.sub"},
					),
				},
				{
					"name": "painel_lateral_d_3",
					"type" : "thinboard_circle",
					"x": 535, "y": 296,
					"width": 150, "height": 70,
					"children":
					(
						{
							"name" : "TitleBar","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 140,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Missões por Membros ", "align":"center" },
							),
						},
					),
				},
				{
					"name": "painel_lateral_d_4",
					"type" : "thinboard_circle",
					"x": 535, "y": 368,
					"width": 150, "height": 168,
					"children":
					(
						{
							"name" : "TitleBar","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 140,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Informações:", "align":"center" },
							),
						},
					),
				},
				{
					"name": "painel_central_cima",
					"type" : "expanded_image",
					"image" : "festival/central.tga",
					"x": 162, "y": 30,
					# "width": 372, "height": 70,
				},
				{
					"name": "painel_central",
					"type" : "thinboard_circle",
					"x": 162, "y": 102,
					"width": 372, "height": 320,
					"children":
					(
						{
							"name" : "Missoes","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 360,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Missões", "align":"center" },

							),
						},
						{
							"name": "mission_scrollbar",
							"type": "scrollbar",
							"x": 350,
							"y": 26,
							"size": 280,
						},
					),
				},
				{
					"name": "painel_central_baixo",
					"type" : "thinboard_circle",
					"x": 162, "y": 425,
					"width": 372, "height": 110,
					"children":
					(
						{
							"name" : "Recompensas","type" : "horizontalbar","style" : ("attach",),"x" : 6,"y" : 6,"width" : 160,
							"children" :
							(
								{ "name":"TitleName", "type":"text", "x":15, "y":0, "text":"Recompensas do Festival", "align":"center" },
							),
						},
						{
							"name": "btn_resgate",
							"type": "button",
							"x": 155, "y": 80,
							"text": "Resgatar",
							"default_image" : "festival/bt_01_1.tga",
							"over_image" : "festival/bt_02_1.tga",
							"down_image" : "festival/bt_03_1.tga",
						},
					),
				},
			),
		},
	),
}