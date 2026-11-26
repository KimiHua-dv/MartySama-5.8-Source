import uiScriptLocale

window = {

		"name" : "AdmPage",
		"x" : 30,
		"y" : 30,
		"width" : 980,
		"height" : 550,
		"children":
			(
				{
					"name" : "board",
					"x" : 0,
					"y" : 50,
					"type" : "thinboard",
					"width" : 910,
					"height" : 500,
					"children":
					(
						{
							"name" : "buttonadd",
							"type" : "button",
							"x" : 500,
							"y": 195,
							"text" : "Adiconar Item",
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						{
							"name" : "button01",
							"type" : "button",
							"x" : 10,
							"y": 20,
							"text" : "Adiconar Item",
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						{
							"name" : "button02",
							"type" : "button",
							"x" : 10,
							"y": 45,
							"text" : "Provador",
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						{
							"name" : "next",
							"type" : "button",
							"x" : 450,
							"y": 475,
							"text" : "Proximo",
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						{
							"name" : "Previous",
							"type" : "button",
							"x" : 620,
							"y": 475,
							"text" : "Anterior",
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						{
							"name" : "grid",
							"type" : "grid_table",
							"x" : 150,
							"y" : 185,

							"start_index" : 0,
							"x_count" : 1,    # 1 coluna de largura
							"y_count" : 3,    # 3 linhas de altura (suporta itens de tamanho 1, 2 ou 3)

							"x_step" : 32,
							"y_step" : 32,

							"image" : "d:/ymir work/ui/public/slot_base.sub",
						},
						{
							"name" : "grid_vnum",
							"type" : "grid_table",
							"x" : 250,
							"y" : 160,

							"start_index" : 0,
							"x_count" : 1,    # 1 coluna de largura
							"y_count" : 1,    # 3 linhas de altura (suporta itens de tamanho 1, 2 ou 3)

							"x_step" : 32,
							"y_step" : 32,

							"image" : "d:/ymir work/ui/public/slot_base.sub",
						},
						{
							"name" : "textPrice",
							"type" : "text",
							"text" : "Preço de Venda:",
							"x" : 10,
							"y" : 110,
						},
						{
							"name" : "textMoeda",
							"type" : "text",
							"text" : "Moeda de Venda:",
							"x" : 10,
							"y" : 135,
						},
						{
							"name" : "textPriceItem",
							"type" : "text",
							"text" : "Vnum do Item:",
							"x" : 10,
							"y" : 160,
						},
						{
							"name" : "textsearch",
							"type" : "text",
							"text" : "Digite para procurar:",
							"x" : 642,
							"y" : 30,
						},
						{
							"name" : "category",
							"type" : "text",
							"text" : "Categoria:",
							"x" : 270,
							"y" : 30,
						},
						{
							"name" : "subcategory",
							"type" : "text",
							"text" : "Sub-Categoria:",
							"x" : 425,
							"y" : 30,
						},
						{
							"name" : "item",
							"type" : "text",
							"text" : "Item:",
							"x" : 605,
							"y" : 30,
						},
						{
							"name" : "result",
							"type" : "text",
							"text" : "",
							"x" : 480,
							"y" : 205,
						},
						{
							"name" : "search",
							"type" : "image",
							"x" : 642,
							"y": 50,
							"image" : "d:/ymir work/ui/itemshop/input.sub",
							"children" :
							(
								# O campo de digitação real entra como filho da imagem
								{
									"name" : "itemSearch",
									"type" : "editline",

									"x" : 3,
									"y" : 3,

									"width" : 120,   # Largura máxima da área de digitação
									"height" : 18,   # Altura máxima da área de digitação

									"input_limit" : 30, # Limite máximo de caracteres permitidos
									"secret_flag" : 0,  # Se for 1, oculta o texto com asteriscos **** (útil para senhas)
									"only_number" : 0,  # Se for 1, o jogador só consegue digitar números (0-9)
								},
							),
						},
						{
							"name" : "PriceBackground",
							"type" : "image",
							"x" : 110,
							"y": 110,
							"image" : "d:/ymir work/ui/itemshop/input.sub",
							"children" :
							(
								# O campo de digitação real entra como filho da imagem
								{
									"name" : "itemPrice",
									"type" : "editline",

									"x" : 3,
									"y" : 3,

									"width" : 120,   # Largura máxima da área de digitação
									"height" : 18,   # Altura máxima da área de digitação

									"input_limit" : 30, # Limite máximo de caracteres permitidos
									"secret_flag" : 0,  # Se for 1, oculta o texto com asteriscos **** (útil para senhas)
									"only_number" : 1,  # Se for 1, o jogador só consegue digitar números (0-9)
								},
							),
						},
						{
							"name" : "PriceItemBackground",
							"type" : "image",
							"x" : 110,
							"y": 160,
							"image" : "d:/ymir work/ui/itemshop/input.sub",
							"children" :
							(
								# O campo de digitação real entra como filho da imagem
								{
									"name" : "PriceItem",
									"type" : "editline",

									"x" : 3,
									"y" : 3,

									"width" : 120,   # Largura máxima da área de digitação
									"height" : 18,   # Altura máxima da área de digitação

									"input_limit" : 30, # Limite máximo de caracteres permitidos
									"secret_flag" : 0,  # Se for 1, oculta o texto com asteriscos **** (útil para senhas)
									"only_number" : 1,  # Se for 1, o jogador só consegue digitar números (0-9)
								},
							),
						},
						
					),
				},
			),
}