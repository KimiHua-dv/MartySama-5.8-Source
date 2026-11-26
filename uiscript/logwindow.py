import uiScriptLocale

window = {
		"name": "LogWindow",
		"x" : 30,
		"y" : 30,
		
		"width" : 980,
		"height" : 550,
		 "children" :
		(
			{
				"name" : "board",
				# "type" : "thinboard",
				"x" : 0,
				"y" : 50,
				"width" : 980,
				"height" : 500,
				"children":
					(
						{
							"name" : "titulos",
							"type" : "thinboard",
							"x" : 5,
							"y" : 5,
							"width" : 910,
							"height" : 35,
							"children":
								(
									{
										"name" : "data",
										"type" : "text",
										"text" : "Data de Compra",
										"x" : 15,
										"y" : 5,
									},
									{
										"name" : "item",
										"type" : "text",
										"text" : "Item",
										"x" : 245,
										"y" : 5,
									},
									{
										"name" : "quantidade",
										"type" : "text",
										"text" : "Quantidade",
										"x" : 425,
										"y" : 5,
									},
									{
										"name" : "moeda",
										"type" : "text",
										"text" : "Moeda",
										"x" : 580,
										"y" : 5,
									},
									{
										"name" : "valor",
										"type" : "text",
										"text" : "Valor",
										"x" : 680,
										"y" : 5,
									},
								),
						},
						# {
							# "name" : "informações",
							# "type" : "thinboard",
							# "x" : 5,
							# "y" : 40,
							# "width" : 785,
							# "height" : 5,
							
						# },
					),
			},
			{
				"name" : "title",
				"type" : "text",
				"x" : 350,
				"y" : 30,
				"text" : "Log de Compras",
				
			},
			
		),
}