import ui
import chat
import uiToolTip
import uiScriptLocale
import net
import item
import uiCommon, uilogitemshop, uishop, player, uiitemshopadm, app

class ItemShop(ui.ScriptWindow):
    
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.container = None
		self.log = None
		self.adm = None
		self.npcShop = None
		self.isImplementor = False
		
		self.banner = None
		self.highlights = []
		self.promotions = []
		self.news = []
		
		self.__LoadWindow()
		self.LoadBanner()
		self.LoadDestaques()
		self.LoadNovidades()
		self.LoadPromocoes()
		self.CreateLogWindow()
		self.CreatAdmPage()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/itemshop.py")
		
		self.board = self.GetChild("board")
		self.title = self.GetChild("title")
		self.categoryParent = self.GetChild("categoryPanel")
		self.contentPanel = self.GetChild("contentPanel")
		
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.savedItemShopCategories = []
		self.NPC_CATEGORIES = {
				11: [],
				12: [],
				13: [],
			}
		self.homeCategories = {
			"Destaque": [],

			"promocao": []
		}
		self.shopCategories = {
				2: [],
				3: [],
				4: [],
				5: [],
				6: [],
				7: [],
				# 11: [],
				# 12: [],
			}

		self.gm = net.IsItemShopAdmin()
		self.CreateContainer()
		self.CreateCategoryPanel()
		self.LoadHome()
		
		self.itemShopButton = TabButton()
		self.itemShopButton.SetParent(self.board)
		self.itemShopButton.SetPosition(30, 600)
		self.itemShopButton.SetText("ItemShop")
		self.itemShopButton.SetEvent(
			lambda: self.SelectTab("itemshop")
		)
		self.itemShopButton.Show()
		
		self.logButton = TabButton()
		self.logButton.SetParent(self.board)
		self.logButton.SetPosition(125, 600)
		self.logButton.SetText("Log")
		self.logButton.SetEvent(
			lambda: self.SelectTab("log")
		)
		self.logButton.Show()
		
		self.AdmButton = TabButton()
		self.AdmButton.SetParent(self.board)
		self.AdmButton.SetPosition(220, 600)
		self.AdmButton.SetText("Painel ADM")
		self.AdmButton.SetEvent(
			lambda: self.SelectTab("adm")
		)
		self.AdmButton.Hide()
		
		self.NpcButton = TabButton()
		self.NpcButton.SetParent(self.board)
		if self.gm:
			self.NpcButton.SetPosition(305, 600)
		else:
			self.NpcButton.SetPosition(220, 600)
		self.NpcButton.SetText("Loja NPC")
		self.NpcButton.SetEvent(
			lambda: self.SelectTab("npc")
		)
		self.NpcButton.Show()
		
	
	def IsGm(self, isImplementor):
		chat.AppendChat(chat.CHAT_TYPE_INFO, "IsGm() FOI CHAMADA")
		chat.AppendChat(chat.CHAT_TYPE_INFO, "VALOR: %s" % str(isImplementor))

		self.isImplementor = bool(isImplementor)

		if self.isImplementor:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "MOSTRANDO ADM")
			self.AdmButton.Show()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "OCULTANDO ADM")
			self.AdmButton.Hide()
		
	def AppendInfo(self, data, item, qnt, moeda, moeda_vnum, valor, player):
		infoData = (data, item, qnt, moeda, moeda_vnum, valor, player)
		self.log.infos[0].append(infoData)
		self.log.CreatePurchaseInfo(infoData)
		self.log.logs.append(infoData)
		
	def FinishLog(self, count):
		self.log.FinishItemShopLogLoad(count)
		
	def ResultAdm(self, result):
		self.adm.ResultAdm(result)
		
	def SelectTab(self, tab):
		if tab == "adm" and not self.isImplementor:
			return
			
		self.itemShopButton.SetSelected(tab == "itemshop")
		self.logButton.SetSelected(tab == "log")
		self.AdmButton.SetSelected(tab == "adm")
		self.NpcButton.SetSelected(tab == "npc")

		if tab == "itemshop":
			self.ShowItemShop()

		elif tab == "log":
			self.ShowPurchaseLog()
			
		elif tab == "adm":
			self.ShowAdm()
		elif tab == "npc":
			self.ShowNpc()
			
			
				
	def ShowNpc(self):
		self.HidePages()
		
		self.categoryParent.Show()
		self.categoryPanel.Show()
		self.title.SetText("Loja NPC")
		
		# Força o painel a desenhar APENAS as categorias da NPC (11 e 12)
		self.RefreshNpcCategories()
		
		# Limpa o grid de itens central para não misturar com os do ItemShop anterior
		self.container.Clear()
		self.container.Show()
		
		# Executa a primeira categoria do dicionário de forma segura usando a lógica de list keys
		if self.NPC_CATEGORIES:
			firstNpcCategory = list(self.NPC_CATEGORIES.keys())[0]
			self.OnCategorySelected(firstNpcCategory)

			
	def ShowItemShop(self):
		self.HidePages()
		
		# Garante que os parents visuais estejam ativos
		self.categoryParent.Show()
		self.categoryPanel.Show()
		
		# Força o painel a desenhar APENAS as categorias comerciais salvas
		self.RefreshItemShopCategories()
		
		# Carrega os elementos da Home
		self.LoadHome()
		net.SendItemShopCategory(1)

		
	def CreateLogWindow(self):
		self.log = uilogitemshop.LogWindow()
		self.log.SetParent(self.board)
		self.log.Hide()
		
	def CreatAdmPage(self):
		self.adm = uiitemshopadm.AdmPage()
		self.adm.SetParent(self.board)
		self.adm.Hide()
		
	def ShowPurchaseLog(self):
		chat.AppendChat(1, "ShowPurchaseLog")
		self.HidePages()
		self.categoryPanel.Hide()
		self.categoryParent.Hide()
		if not self.log:
			self.log = uilogitemshop.LogWindow()
			self.log.SetParent(self.board)
		self.title.SetText("Log de Compra de Itens")
		self.log.Open()
		
	def ShowAdm(self):
		self.HidePages()
		self.categoryPanel.Hide()
		self.categoryParent.Hide()
		self.title.SetText("Painel Administrativo")
		self.adm.Open()
		
		
	def LoadItemsPromocao(self, items):
		self.LoadPromocoes(items)

		self.categoryButtons = []
			
	def AppendItem(self, itemID,vnum,itemCount,price,discount,category,currencyType,currencyVnum):
		itemData = ( itemID,vnum,itemCount,price,discount,currencyType,currencyVnum)
		if category == 0:
			self.homeCategories["promocao"].append(itemData)

			self.LoadItemsPromocao(
				self.homeCategories["promocao"]
			)
			return

		if category == 1:
			self.homeCategories["Destaque"].append(itemData)

			self.LoadItemsDestaque(
				self.homeCategories["Destaque"]
			)
			return

		if category not in self.shopCategories and not self.NpcButton.selected:
			chat.AppendChat(
				3,
				"ITEMSHOP: categoria desconhecida: %d" % category
			)
			return
			
		if self.NpcButton.selected:
			self.NPC_CATEGORIES[category].append(itemData)

			self.container.LoadItems(
				self.NPC_CATEGORIES[category]
			)
		else:
			
			self.shopCategories[category].append(itemData)

			self.container.LoadItems(
				self.shopCategories[category]
			)

		self.LoadItens()
			
	def OpenHome(self):
		self.LoadHome()
		net.SendItemShopCategory(1)
		
		
	def LoadHome(self):

		loader = ui.PythonScriptLoader()
		loader.LoadScriptFile(self.contentPanel, "uiscript/itemshop_home.py")

		self.banner = self.contentPanel.GetChild("banner")
		self.destaques = self.contentPanel.GetChild("destaques")
		self.promocoes = self.contentPanel.GetChild("promocoes")
		self.novidades = self.contentPanel.GetChild("novidades")
		self.title.SetText("Loja de Itens")
		self.HidePages()

		self.banner.Show()
		self.destaques.Show()
		self.promocoes.Show()
		self.novidades.Show()

		self.LoadDestaques()
		self.LoadPromocoes()
		self.categoryPanel.Show()
		self.categoryParent.Show()
		
	def ShowHome(self):

		self.HidePages()
		
		self.categoryParent.Show()
		self.categoryPanel.Show()
		self.banner.Show()
		self.destaques.Show()
		self.promocoes.Show()
		self.novidades.Show()

		for item in self.highlights:
			item.Show()

		for item in self.promotions:
			item.Show()
		
	def LoadItens(self):
		self.HidePages()
		self.container.Show()
		self.container.ShowItems()
		self.container.firstVisibleRow = 0
		self.scrollBar.SetPos(0)
		self.UpdateScrollBar()
		
	def CreateCategoryPanel(self):

		self.categoryPanel = CategoryPanel()

		self.categoryPanel.SetParent(self.categoryParent)
		self.categoryPanel.SetPosition(0,0)
		self.categoryPanel.SetSize(120, 500)
		
		self.categoryPanel.SetCategorySelectEvent(ui.__mem_func__(self.OnCategorySelected))

		self.categoryPanel.Show()
		
	def OnCategorySelected(self, categoryID):
		self.currentCategory = categoryID
		if categoryID == 1:
			self.ShowHome()
			net.SendItemShopCategory(1)
			return
			
		self.HidePages()
		self.categoryParent.Show()
		self.categoryPanel.Show()
		
		# Limpa o container e exibe a estrutura para receber os itens da categoria clicada
		self.container.Clear()
		self.container.Show()
		self.ClearCategoryItems(categoryID)
		
		# Reutiliza o pacote de rede existente para pedir ao servidor os itens desta categoria específica
		net.SendItemShopCategory(categoryID)
		

		
	def CreateContainer(self):

		self.container = ShopContainer()
		self.container.SetParent(self.contentPanel)
		self.container.SetPosition(0,0)
		self.container.SetSize(740,500)
		self.container.Show()
		
		self.container.SetMouseWheelEvent(self.OnMouseWheel)
		self.scrollBar = ui.ScrollBar()

		self.scrollBar.SetParent(self.board)
		self.scrollBar.SetPosition(950, 60)
		self.scrollBar.SetScrollBarSize(500)
		self.scrollBar.SetScrollEvent(self.OnScroll)
		self.scrollBar.Hide()
		
	def HidePages(self):
		self.banner.Hide()
		self.destaques.Hide()
		self.promocoes.Hide()
		self.novidades.Hide()
		
		if self.log:
			self.log.Hide()
		if self.adm:
			self.adm.Hide()
		self.container.HideItems()

		if self.scrollBar:
			self.scrollBar.Hide()

		for item in self.highlights:
			item.Hide()

		for item in self.promotions:
			item.Hide()
				
	
	def UpdateScrollBar(self):

		totalRows = self.container.GetTotalRows()

		if totalRows <= self.container.visibleRows:
			self.scrollBar.Hide()
			return

		self.scrollBar.Show()

		maxRow = totalRows - self.container.visibleRows

		self.scrollBar.SetPos(float(self.container.firstVisibleRow) / maxRow)

		self.container.RefreshVisibleItems()
		
	def Close(self):
		self.Hide()
	
	def Open(self):
		self.Show()
		self.SelectTab("itemshop")
		
	def OnScroll(self):
		totalRows = self.container.GetTotalRows()

		maxRow = max(0, totalRows - self.container.visibleRows)

		row = int(self.scrollBar.GetPos() * maxRow)

		self.container.firstVisibleRow = row

		self.container.RefreshVisibleItems()
		
	def LoadBanner(self):
		self.banner.Show()


	def LoadDestaques(self):
		for item in self.highlights:
			item.Hide()
		self.highlights = []
		self.destaques.Show()


	def LoadPromocoes(self, items=None):
		if items is None:
			items = self.homeCategories["promocao"]

		for item in self.promotions:
			item.Hide()

		self.promotions = []

		self.promocoes.Show()

		self.LoadItemsHome(
			self.promocoes,
			items,
			self.promotions
		)

	def LoadNovidades(self):

		chat.AppendChat(
			1,
			"Carregando novidades"
		)
	
	def LoadItemsHome(self, parent, items, storage, columns=1):
		for index, (itemID,vnum,itemCount,price,discount,currencyType,currencyVnum) in enumerate(items):
			coluna = index % columns
			linha = index // columns
			x = 15 + coluna * 200
			y = 20 + linha * 120
			item = ShopItem()
			item.Creat_destaque(parent,itemID,vnum,itemCount,price,discount,currencyType,currencyVnum,x,y)
			storage.append(item)
			
	def LoadItemsDestaque(self, items):
		self.LoadDestaques()

		self.LoadItemsHome(
			self.destaques,
			items,
			self.highlights)
		
	def OnPressEscapeKey(self):
		self.Hide()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
	def OnMouseWheel(self, nLen):
		if not self.container.IsShow():
			return False

		if nLen > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()	

		return True
		
	def AddCategory(self, categoryID, categoryName):
		# Se pertencer à Loja NPC (procurando nas chaves do dicionário), ignora o fluxo padrão
		if categoryID in self.NPC_CATEGORIES:
			return

		# Guarda no cache local para podermos reconstruir quando o jogador alternar as abas
		# Evitamos duplicados verificando se o ID já foi salvo
		if not any(cat[0] == categoryID for cat in self.savedItemShopCategories):
			self.savedItemShopCategories.append((categoryID, categoryName))

		# Desenha o botão normalmente na tela
		self.categoryPanel.AddCategory(categoryID, categoryName)
		
	def RefreshItemShopCategories(self):
		# Limpa os botões antigos usando o método Clear do seu CategoryPanel
		self.categoryPanel.Clear()
		
		# Reconstrói os botões a partir do cache salvo na memória do cliente
		for categoryID, categoryName in self.savedItemShopCategories:
			self.categoryPanel.AddCategory(categoryID, categoryName)

	def RefreshNpcCategories(self):
		# Limpa os botões antigos
		self.categoryPanel.Clear()
		
		# Mapeamento estático de nomes para as suas chaves 11 e 12 da NPC
		npcNames = {
			11: "Armas",
			12: "Armaduras",
			13: "Acessórios"
		}
		
		# Varre o dicionário da NPC pegando as chaves (11, 12)
		for categoryID in self.NPC_CATEGORIES.keys():
			categoryName = npcNames.get(categoryID, "NPC Cat %d" % categoryID)
			self.categoryPanel.AddCategory(categoryID, categoryName)

	def ClearCategoryItems(self, category):
		if category == 0:
			self.homeCategories["promocao"] = []
			return
		if category == 1:
			self.homeCategories["Destaque"] = []
			return
		if category in self.shopCategories:
			self.shopCategories[category] = []
		if category in self.NPC_CATEGORIES:
			self.NPC_CATEGORIES[category] = []
		
	def ClearCategories(self):
		self.categoryPanel.ClearCategories()
	
	def OnBuyResult(self, result):
		if result == 1:
			self.ShowBuyMessage("Cash insuficiente.")
		elif result == 2:
			self.ShowBuyMessage("Inventario Cheio")
	
	def ShowBuyMessage(self, text):
		self.dialog = uiCommon.PopupDialog()
		self.dialog.SetText(text)
		self.dialog.SetAcceptEvent(
			ui.__mem_func__(self.CloseDialog)
		)
		self.dialog.Open()
		
	def CloseDialog(self):
		self.inputDialog = None
		return True
		
class ShopItem(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		
		self.itemID = 0
		self.vnum = 0
		self.price = 0
		self.itemCount = 0
		self.currencyType = 0
		self.currencyVnum = 0
		self.priceText = None
		
		self.board = None
		self.slot = None
		self.itemName = ""
		self.nameText = None
		self.buyButton = None
		
		self.oldPriceText = None
		self.discount = 0
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.questionDialog = None
		
		
	def Creat(self,parent,itemID,vnum,itemCount,price,discount,currencyType,currencyVnum,x,y):
		self.itemID = itemID
		self.vnum = vnum
		self.itemCount = itemCount
		self.price = price
		self.discount = discount
		self.currencyType = currencyType
		self.currencyVnum = currencyVnum

		self.itemName = self.GetItemName(vnum)

		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(180, 120)
		self.Show()

		self.CreatBoard()
		self.CreatSlot(vnum, itemCount)
		self.CreatName(self.itemName)
		self.CreatPrice(price)
		self.CreatButton()

		self.slot.SetOverInItemEvent(ui.__mem_func__(self.OverIn))
		self.slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		
	def GetCurrencyName(self):
		if self.currencyType == 1:
			return "Cash"

		if self.currencyType == 2:
			return "Gold"

		if self.currencyType == 3:
			return "Cheque"

		if self.currencyType == 4:
			return self.GetItemName(self.currencyVnum)

		return "Moeda"
		
	def Creat_destaque(self,parent,itemID,vnum,itemCount,price,discount,currencyType,currencyVnum,x,y):
		self.itemID = itemID
		self.vnum = vnum
		self.price = price
		self.discount = discount
		self.itemName = self.GetItemName(vnum)
		self.currencyType = currencyType
		self.currencyVnum = currencyVnum

		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(180, 120)
		self.Show()

		self.CreatBoard()
		self.CreatSlot(vnum, itemCount)
		self.CreatName(self.itemName)
		self.CreatPrice(price)

		if self.discount:
			self.CreateDiscount()

		self.CreatButton()

		self.slot.SetOverInItemEvent(ui.__mem_func__(self.OverIn))
		self.slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
	
	def GetItemName(self, vnum):
		item.SelectItem(vnum)
		return item.GetItemName()
		
	def CreatBoard(self):

		self.board = ui.Board()
		self.board.SetParent(self)
		self.board.SetPosition(0,0)
		# self.board.LoadImage("img/teste.tga")
		self.board.SetSize(180,120)
		self.board.Show()
		
		
		
		
	def CreatSlot(self, vnum, itemCount):
		self.slot = ui.SlotWindow()
		self.slot.SetParent(self)
		self.slot.SetPosition(10,10)
		self.slot.SetSize(40,40)
		
		self.slot.AppendSlot(0,0,0,32,32)
		self.slot.SetItemSlot(0,vnum, itemCount)
		self.slot.Show()
		
	def CreatName(self, name):
		
		self.nameText = ui.TextLine()
		self.nameText.SetParent(self)
		self.nameText.SetPosition(50,10)
		self.nameText.SetText(name)
		self.nameText.Show()
		
	def CreatPrice(self, price):
		self.priceText = ui.TextLine()
		self.priceText.SetParent(self)
		self.priceText.SetPosition(50, 35)

		currencyName = self.GetCurrencyName()

		if self.discount > 0:
			finalPrice = self.GetFinalPrice()
			self.priceText.SetText(
				"%d %s (-%d%%)" %
				(
					finalPrice,
					currencyName,
					self.discount
				)
			)
		else:
			self.priceText.SetText(
				"%d %s" %
				(
					price,
					currencyName
				)
			)
		self.priceText.Show()
		 
	def CreatButton(self):
		
		self.buyButton = ui.Button()
		self.buyButton.SetParent(self)
		
		
		self.buyButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
		self.buyButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
		self.buyButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
		self.buyButton.SetEvent(self.BuyItem)
		self.buyButton.SetPosition(50,80)
		self.buyButton.SetText("Comprar")
		self.buyButton.Show()
		
	def Destroy(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

		self.Hide()
		
	def BuyItem(self):
		if self.itemID <= 0:
			return

		if self.questionDialog:
			return

		finalPrice = self.GetFinalPrice()

		self.questionDialog = uiCommon.QuestionDialog()

		self.questionDialog.SetText(
			"Deseja comprar %s por %d Moedas?" %
			(self.itemName, finalPrice)
		)

		self.questionDialog.SetAcceptEvent(
			ui.__mem_func__(self.AcceptBuyItem)
		)

		self.questionDialog.SetCancelEvent(
			ui.__mem_func__(self.CancelBuyItem)
		)

		self.questionDialog.Open()
		
	def AcceptBuyItem(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None

		net.SendItemShopBuy(self.itemID)
	
	def CancelBuyItem(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		
	def OverIn(self):
		self.board.SetAlpha(0.4)
		self.board.SetPosition(5,5)
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.vnum)


	def OverOut(self):
		self.board.SetAlpha(1.0)
		self.board.SetPosition(0,0)
		if self.tooltipItem:
			self.tooltipItem.Hide()
		
		
	def GetFinalPrice(self):

		if self.discount > 0:
			return self.price - ((self.price * self.discount) / 100)

		return self.price
		
	def CreateDiscount(self):

		if self.discount <= 0:
			return

		self.oldPriceText = ui.TextLine()
		self.oldPriceText.SetParent(self)
		self.oldPriceText.SetPosition(60,25)
		self.oldPriceText.SetText(
			"Preço: %d" % self.price
		)
		self.oldPriceText.Show()

		
class ShopContainer(ui.Window):
	
	def __init__(self):
		ui.Window.__init__(self)
		self.shopItems = []
		self.AddFlag("not_pick")
		self.Show()
		
		self.columns = 4
		self.visibleRows = 3
		self.firstVisibleRow = 0
		
	def AddItem(self, item):
		self.shopItems.append(item)
		
	def Clear(self):
		for item in self.shopItems:
			item.Hide()
			# item.Destroy()
		self.shopItems = []
	
	def LoadItems(self, items):
		self.Clear()
		for index, (itemID,vnum,itemCount,price,discount,currencyType,currencyVnum) in enumerate(items):
			item = ShopItem()
			item.Creat(self,itemID,vnum,itemCount,price,discount,currencyType,currencyVnum,0,0)
			self.AddItem(item)
		self.RefreshVisibleItems()
	
	def RefreshVisibleItems(self):

		for index, item in enumerate(self.shopItems):

			row = index // self.columns
			column = index % self.columns

			if row < self.firstVisibleRow:
				item.Hide()
				continue

			if row >= self.firstVisibleRow + self.visibleRows:
				item.Hide()
				continue

			x = 0 + column * 200
			y = 20 + (row - self.firstVisibleRow) * 150

			item.SetPosition(x, y)
			item.Show()
		
	def GetTotalRows(self):
		return (len(self.shopItems) + self.columns - 1) // self.columns
		
	def ShowItems(self):
		self.Show()

		for item in self.shopItems:
			item.Show()

	def HideItems(self):
		self.Hide()
		for item in self.shopItems:
			item.Hide()
			
class CategoryPanel(ui.ScriptWindow):
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.categories = {}
		self.categoryButtons = []
		self.categorySelectEvent = None

	def AddCategory(self, categoryID, categoryName):
		button = ui.Button()

		button.SetParent(self)

		button.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		button.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		button.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")

		index = len(self.categoryButtons)

		button.SetPosition(
			20,
			35 + index * 38
		)

		button.SetText(categoryName)

		button.SetEvent(
			lambda arg=categoryID: self.SelectCategory(arg)
		)

		button.Show()

		self.categoryButtons.append(button)
		
	def SelectCategory(self, categoryID):
		if self.categorySelectEvent:
			self.categorySelectEvent(categoryID)
		
	def Clear(self):
		for button in self.categoryButtons:
			button.Hide()

		self.categoryButtons = []
		self.categories = {}

	def OnCategoryClick(self, categoryID):
		# aqui não precisamos decidir como a loja funciona
		# apenas avisamos qual categoria foi escolhida
		if self.categorySelectEvent:
			self.categorySelectEvent(categoryID)
			
	def SetCategorySelectEvent(self, event):
		self.categorySelectEvent = event
		
	def ClearCategories(self):
		for button in self.categoryButtons:
			button.Hide()
			button.Destroy()
			
		self.categoryButtons = []
		self.categories = {}
	
class TabButton(ui.Button):

	def __init__(self):
		ui.Button.__init__(self)

		self.selected = False

		self.SetUpVisual(
			"d:/ymir work/ui/public/large_button_01.sub"
		)
		self.SetOverVisual(
			"d:/ymir work/ui/public/large_button_02.sub"
		)
		self.SetDownVisual(
			"d:/ymir work/ui/public/large_button_03.sub"
		)

	def SetSelected(self, selected):
		self.selected = selected

		if selected:
			self.SetUpVisual(
				"d:/ymir work/ui/public/large_button_03.sub"
			)
			self.SetOverVisual(
				"d:/ymir work/ui/public/large_button_03.sub"
			)
			self.SetDownVisual(
				"d:/ymir work/ui/public/large_button_03.sub"
			)

		else:
			self.SetUpVisual(
				"d:/ymir work/ui/public/large_button_01.sub"
			)
			self.SetOverVisual(
				"d:/ymir work/ui/public/large_button_02.sub"
			)
			self.SetDownVisual(
				"d:/ymir work/ui/public/large_button_03.sub"
			)
			