import ui, uiScriptLocale
import mouseModule
import player
import uiToolTip
import localeInfo, renderTarget
import uiInventory, chat, item, net

ITEMSHOP_ADMIN_ADD_SUCCESS = 0
ITEMSHOP_ADMIN_ADD_ITEM_NOT_FOUND = 1
ITEMSHOP_ADMIN_ADD_CURRENCY_NOT_FOUND = 2
ITEMSHOP_ADMIN_ADD_DATABASE_ERROR = 3

class AdmPage(ui.ScriptWindow):
	ITEM_CATEGORIES = [
		(0, "Todos"),
		(item.ITEM_TYPE_WEAPON, "Armas"),
		(item.ITEM_TYPE_ARMOR, "Armaduras"),
		(item.ITEM_TYPE_COSTUME, "Costumes"),
		(item.ITEM_TYPE_DS, "Dragon Soul"),
		(item.ITEM_TYPE_BELT, "Cintos"),
	]
	ITEM_SUBCATEGORIES = {
		item.ITEM_TYPE_ARMOR: [
			(item.ARMOR_BODY, "Corpo"),
			(item.ARMOR_HEAD, "Cabeça"),
			(item.ARMOR_SHIELD, "Escudo"),
			(item.ARMOR_WRIST, "Bracelete"),
			(item.ARMOR_FOOTS, "Sapatos"),
			(item.ARMOR_NECK, "Colar"),
			(item.ARMOR_EAR, "Brinco"),
		],

		item.ITEM_TYPE_WEAPON: [
			(item.WEAPON_SWORD, "Espada"),
			(item.WEAPON_DAGGER, "Adaga"),
			(item.WEAPON_BOW, "Arco"),
			(item.WEAPON_TWO_HANDED, "Duas mãos"),
			(item.WEAPON_BELL, "Sino"),
			(item.WEAPON_FAN, "Leque"),
			(item.WEAPON_ARROW, "Flecha"),
			(item.WEAPON_MOUNT_SPEAR, "Lança de montaria"),
		],
		
		item.ITEM_TYPE_COSTUME: [
			(item.COSTUME_TYPE_BODY, "Traje"),
			(item.COSTUME_TYPE_HAIR, "Cabelo"),
			(item.COSTUME_TYPE_MOUNT, "Montaria"),
			(item.COSTUME_TYPE_ACCE, "Faixa"),
			(item.COSTUME_TYPE_WEAPON, "Arma"),
		],
	}
	ITEM_SUBTYPES = {
		item.ITEM_TYPE_ARMOR: [
			item.ARMOR_BODY,
			item.ARMOR_HEAD,
			item.ARMOR_SHIELD,
			item.ARMOR_WRIST,
			item.ARMOR_FOOTS,
			item.ARMOR_NECK,
			item.ARMOR_EAR,
		],

		item.ITEM_TYPE_WEAPON: [
			item.WEAPON_SWORD,
			item.WEAPON_DAGGER,
			item.WEAPON_BOW,
			item.WEAPON_TWO_HANDED,
			item.WEAPON_BELL,
			item.WEAPON_FAN,
			item.WEAPON_ARROW,
			item.WEAPON_MOUNT_SPEAR,
		],
		item.ITEM_TYPE_COSTUME: [
			item.COSTUME_TYPE_BODY,
			item.COSTUME_TYPE_HAIR,
			item.COSTUME_TYPE_MOUNT,
			item.COSTUME_TYPE_ACCE,
			item.COSTUME_TYPE_WEAPON
		],
	}
	
	ITEM_MOEDA = [
		(0, "cash"),
		(1, "Gold"),
		(2, "Won"),
		(3, "Item"),
	]
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.items = []
		self.filteredItems = []
		self.selectedCategory = 0
		self.selectedSubCategory = 0
		self.selectedItem = None
		self.itemSlot = None

		self.itemPrice = None
		self.MoedaCombo = None

		self.moeda = 0
		self.price = 0
		self.rodar = 0.0
		
		self.textPriceItem = None
		self.PriceItemBackground = None
		self.PriceItem = None

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		
		self.previewItems = []
		
		self.LoadWindow()
		
	def LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/admpage.py")
		
		self.board = self.GetChild("board")
		self.button = self.GetChild("buttonadd")
		self.button01 = self.GetChild("button01")
		self.button02 = self.GetChild("button02")
		self.nextItemButton = self.GetChild("next")
		self.PreviousItemButton = self.GetChild("Previous")
		self.itemSlot = self.GetChild("grid")
		self.itemSearchBackground = self.GetChild("search")
		self.itemSearch = self.GetChild("itemSearch")
		self.itemPriceBackground = self.GetChild("PriceBackground")
		self.itemPrice = self.GetChild("itemPrice")
		self.textPrice = self.GetChild("textPrice")
		self.textsearch = self.GetChild("textsearch")
		self.textMoeda = self.GetChild("textMoeda")
		self.textPriceItem = self.GetChild("textPriceItem")
		self.PriceItemBackground = self.GetChild("PriceItemBackground")
		self.PriceItem = self.GetChild("PriceItem")
		self.SlotItem = self.GetChild("grid_vnum")
		self.adminStatus = self.GetChild("result")
		self.categoria = self.GetChild("category")
		self.subcategoria = self.GetChild("subcategory")
		self.itemtx = self.GetChild("item")
		
		self.categoria.Hide()
		self.subcategoria.Hide()
		self.itemtx.Hide()
		self.adminStatus.Hide()
		self.SlotItem.Hide()
		self.textPriceItem.Hide()
		self.PriceItemBackground.Hide()
		self.PriceItem.Hide()
		self.textMoeda.Hide()
		self.textsearch.Hide()
		self.itemPriceBackground.Hide()
		self.textPrice.Hide()
		self.itemPrice.Hide()
		self.itemSearch.Hide()
		self.itemSearchBackground.Hide()
		self.itemSlot.Hide()
		self.button.Hide()
		self.nextItemButton.Hide()
		self.PreviousItemButton.Hide()
		# Carrega os itens recebidos do item_proto
		self.items = item.GetItemList()

		self.button01.SetEvent(ui.__mem_func__(self.Adicionar))
		self.button02.SetEvent(ui.__mem_func__(self.Provar))
		

	def Adicionar(self):
		# ComboBox de categorias
		self.categoryCombo = DropDownList(self.board, 260, 50)
		for categoryType, categoryName in self.ITEM_CATEGORIES:
			self.categoryCombo.InsertItem(categoryType,categoryName)
		self.categoryCombo.SetEvent(ui.__mem_func__(self.OnCategorySelected))
		self.categoryCombo.Show()

		# ComboBox de subcategorias
		self.subCategoryCombo = DropDownList(self.board,384,50)
		self.subCategoryCombo.SetEvent(ui.__mem_func__(self.OnSubCategorySelected))
		self.subCategoryCombo.Show()
		
		# ComboBox de itens
		self.itemCombo = DropDownList(self.board, 508, 50)
		self.itemCombo.SetEvent(ui.__mem_func__(self.OnItemSelected))
		self.itemCombo.Show()
		
		self.button.SetEvent(ui.__mem_func__(self.AddItem))
		self.categoria.Show()
		self.subcategoria.Show()
		self.itemtx.Show()
		
	def Provar(self):
		# Área que captura o mouse
		self.previewMouse = ui.Button()
		self.previewMouse.SetParent(self.board)
		self.previewMouse.SetPosition(780, 120)
		self.previewMouse.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.previewMouse.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.previewMouse.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.previewMouse.SetText("Rodar")
		self.previewMouse.SetEvent(self.OnPreviewMouseDown)
		self.previewMouse.Show()
		
		# Render do personagem
		self.renderIndex = renderTarget.GetFreeIndex(200, 300)

		self.renderWnd = ui.RenderTarget()
		self.renderWnd.SetParent(self.board)
		self.renderWnd.SetPosition(450, 70)
		self.renderWnd.SetSize(300, 400)
		self.renderWnd.SetRenderTarget(self.renderIndex)
		self.renderWnd.Show()
		renderTarget.SetBackground(self.renderIndex,"d:/ymir work/ui/game/render/preview_back.tga")


		# Estado atual do provador
		self.previewArmor = 0
		self.previewWeapon = 0
		self.previewHair = 0
		self.previewAcce = 0
		self.previewRotating = False
		self.previewRace = 0
		self.previewMount = 0
		
		# ComboBox de categorias
		self.categoryCombo = DropDownList(self.board, 160, 50)

		for categoryType, categoryName in self.ITEM_CATEGORIES:
			self.categoryCombo.InsertItem(categoryType, categoryName)

		self.categoryCombo.SetEvent(ui.__mem_func__(self.OnCategorySelected1))

		self.categoryCombo.Show()


		# ComboBox de subcategorias
		self.subCategoryCombo = DropDownList(self.board,284,50)

		self.subCategoryCombo.SetEvent(ui.__mem_func__(self.OnSubCategorySelected1))

		self.subCategoryCombo.Show()


		# ComboBox de itens
		self.itemCombo = DropDownList(self.board,408,50	)

		self.itemCombo.SetEvent(ui.__mem_func__(self.OnItemSelected1))

		self.itemCombo.Show()

		self.currentItemIndex = 0
		self.ProvarAtualizarPersonagem()

		self.previewVnumText = ui.TextLine()
		self.previewVnumText.SetParent(self.board)
		self.previewVnumText.SetPosition(508, 80)
		self.previewVnumText.SetText("Vnum: -")
		self.previewVnumText.Show()
		self.previewItemText = ui.TextLine()
		self.previewItemText.SetParent(self.board)
		self.previewItemText.SetPosition(508, 100)
		self.previewItemText.SetText("Item: -")
		self.previewItemText.Show()
		
		self.classe = DropDownList(self.board, 200, 70)
		self.classe.InsertItem(0, "Guerreiro")
		self.classe.InsertItem(1, "Ninja F")
		self.classe.InsertItem(2, "Sura")
		self.classe.InsertItem(3, "Shaman F")
		self.classe.InsertItem(4, "Guerreiro F")
		self.classe.InsertItem(5, "Ninja")
		self.classe.InsertItem(6, "Sura F")
		self.classe.InsertItem(7, "Shaman")
		self.classe.SetEvent(ui.__mem_func__(self.ClasseSelected))
		self.classe.Show()
		
		self.categoria.Show()
		self.subcategoria.Show()
		self.itemtx.Show()
		self.nextItemButton.SetEvent(ui.__mem_func__(self.NextPreviewItem))
		self.PreviousItemButton.SetEvent(ui.__mem_func__(self.PreviousPreviewItem))
		self.nextItemButton.Show()
		self.PreviousItemButton.Show()
		
		
		
	def ClasseSelected(self, select):
		select = int(select)

		self.previewRace = select

		self.ProvarAtualizarPersonagem()
		
	def OnPreviewMouseDown(self):
		self.rodar += 90.0

		if self.rodar >= 360.0:
			self.rodar = 0.0
		renderTarget.SetRotation(self.renderIndex, False)
		renderTarget.SetModelRotation(self.renderIndex, self.rodar)
		

	def OnPreviewMouseUp(self):
		self.previewDragging = False
		renderTarget.SetModelRotation(self.renderIndex, 90.0)
		
	def OnSubCategorySelected1(self, subCategory):
		self.selectedSubCategory = int(subCategory)
		self.itemCombo.ClearItem()
		self.previewItems = []
		itemList = item.GetItemList()
		for vnum, name, itemType, itemSubType in itemList:
			if itemType != self.selectedCategory:
				continue
			if itemSubType != self.selectedSubCategory:
				continue
			self.previewItems.append((vnum, name))
			self.itemCombo.InsertItem(vnum,"%s [%d]" % (name, vnum))
		self.CreatSearch()

			
	def OnItemSelected1(self, itemVnum):
		itemVnum = int(itemVnum)

		items = self.filteredItems if self.itemSearch.GetText() else self.previewItems

		for i, itemData in enumerate(items):
			if itemData[0] == itemVnum:
				self.currentItemIndex = i
				break

		category = self.selectedCategory
		subCategory = self.selectedSubCategory

		if category == item.ITEM_TYPE_COSTUME:
			if subCategory == item.COSTUME_TYPE_BODY:
				self.previewArmor = itemVnum
			elif subCategory == item.COSTUME_TYPE_HAIR:
				self.previewHair = itemVnum
			elif subCategory == item.COSTUME_TYPE_WEAPON:
				self.previewWeapon = itemVnum
			elif subCategory == item.COSTUME_TYPE_ACCE:
				self.previewAcce = itemVnum
			elif subCategory == item.COSTUME_TYPE_MOUNT:
				self.previewMount = itemVnum

		elif category == item.ITEM_TYPE_ARMOR:
			if subCategory == item.ARMOR_BODY:
				self.previewArmor = itemVnum
			elif subCategory == item.ARMOR_HEAD:
				self.previewHair = itemVnum
			elif subCategory == item.ARMOR_WEAPON:
				self.previewWeapon = itemVnum

		elif category == item.ITEM_TYPE_WEAPON:
			self.previewWeapon = itemVnum

		self.ProvarAtualizarPersonagem()
		
	def OnCategorySelected1(self, categoryType):

		self.selectedCategory = int(categoryType)
		self.subCategoryCombo.ClearItem()
		self.itemCombo.ClearItem()
		self.selectedSubCategory = -1
		if self.selectedCategory == item.ITEM_TYPE_COSTUME:
			# Aqui entraremos com as subcategorias de costume

			self.subCategoryCombo.InsertItem(item.COSTUME_TYPE_BODY,	"Corpo")

			self.subCategoryCombo.InsertItem(item.COSTUME_TYPE_HAIR,"Cabelo")
			self.subCategoryCombo.InsertItem(item.COSTUME_TYPE_MOUNT, "Montaria")

			if hasattr(item, "COSTUME_ACCE"):
				self.subCategoryCombo.InsertItem(item.COSTUME_TYPE_ACCE,"Acessório")
			if hasattr(item, "COSTUME_WEAPON"):
				self.subCategoryCombo.InsertItem(item.COSTUME_TYPE_WEAPON,"Arma")
		elif self.selectedCategory in self.ITEM_SUBCATEGORIES:

			for subType, subName in self.ITEM_SUBCATEGORIES[
				self.selectedCategory
			]:
				self.subCategoryCombo.InsertItem(
					subType,
					subName
				)
	
	def NextPreviewItem(self):
		items = self.filteredItems if self.itemSearch.GetText() else self.previewItems

		if not items:
			return

		self.currentItemIndex = (self.currentItemIndex + 1) % len(items)

		vnum, name = items[self.currentItemIndex]

		self.previewVnumText.SetText("Vnum: %d" % vnum)
		self.previewItemText.SetText("Item: %s" % name)

		self.OnItemSelected1(vnum)
		
	def PreviousPreviewItem(self):
		if not self.previewItems:
			return
		self.currentItemIndex -= 1
		if self.currentItemIndex < 0:
			self.currentItemIndex = len(self.previewItems) - 1
		vnum, name = self.previewItems[self.currentItemIndex]
		
		self.previewVnumText.SetText("Vnum: %d" % vnum)
		self.previewItemText.SetText("Item: %s" % name)
		
		self.OnItemSelected1(vnum)
		
	def ProvarAtualizarPersonagem(self):

		renderIndex = self.renderIndex
		renderTarget.ResetModel(renderIndex)

		renderTarget.SelectModel(renderIndex, self.previewRace)
		renderTarget.SetVisibility(renderIndex,	True)
		renderTarget.SetWeapon(renderIndex,self.previewWeapon)
		renderTarget.SetHair(renderIndex,self.previewHair)
		renderTarget.SetAcce(renderIndex,self.previewAcce)
		renderTarget.SetArmor(renderIndex,self.previewArmor	)
		renderTarget.SetMount(renderIndex,self.previewMount	)
		renderTarget.SetModelV3Eye(renderIndex,	0.0,-1000.0,600.0)
		renderTarget.Zoom(renderIndex,20)
		 # Desativa a rotação automática
		renderTarget.SetRotation(renderIndex, False)
		
	def Open(self):
		self.Show()
		
	def Close(self):
		self.Hide()
		
	def OnItemSearchUpdate(self):
		searchText = self.itemSearch.GetText().lower()

		self.itemCombo.ClearItem()
		self.filteredItems = []

		for vnum, name in self.previewItems:
			if searchText and searchText not in name.lower():
				continue

			self.filteredItems.append((vnum, name))

		for vnum, name in self.filteredItems:
			self.itemCombo.InsertItem(
				vnum,
				"%s [%d]" % (name, vnum)
			)
		
	def OnCategorySelected(self, categoryType):
		self.selectedCategory = categoryType
		# Limpa subcategoria
		self.subCategoryCombo.ClearItem()
		# Limpa itens
		self.itemCombo.ClearItem()
		self.filteredItems = []
		# TODOS
		if categoryType == 0:
			for itemData in self.items:
				vnum = itemData[0]
				name = itemData[1]
				self.itemCombo.InsertItem(vnum, name)
				self.filteredItems.append((vnum, name))

			return
		# Pega as subcategorias dessa categoria
		subCategories = self.ITEM_SUBCATEGORIES.get(categoryType,[])
		for index, subCategory in enumerate(subCategories):
			subType, subCategoryName = subCategory
			self.subCategoryCombo.InsertItem(subType,subCategoryName)
			
	def OnSubCategorySelected(self, subType):
		self.selectedSubCategory = subType
		self.itemCombo.ClearItem()
		self.filteredItems = []
		for itemData in self.items:
			vnum, name, itemType, itemSubType = itemData
			if itemType != self.selectedCategory:
				continue
			if itemSubType != subType:
				continue
			self.filteredItems.append(itemData)
		for index, itemData in enumerate(self.filteredItems):
			vnum, name, itemType, itemSubType = itemData
			self.itemCombo.InsertItem(index,"%d - %s" % (vnum, name))
		self.CreatSearch()
			
	def OnItemSelected(self, index):
		if index < 0:
			return
		if index >= len(self.filteredItems):
			return
		itemData = self.filteredItems[index]
		vnum, name, itemType, subType = itemData
		self.selectedItem = itemData
		chat.AppendChat(1,"Item selecionado: %d - %s" % (vnum, name))
		self.CreatSlot(vnum)
		self.CreatPrice()
		self.CreatMoeda()
		
	def CreatSlot(self, vnum):
		item.SelectItem(vnum)
		x_item, y_item = item.GetItemSize()
		self.item = vnum
		# self.itemSlot.ArrangeSlot(0,x_item,y_item,32,32,0,0	)
		self.itemSlot.SetItemSlot(0, vnum, 1) 
		self.itemSlot.Show()
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverIn))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		
	def CreatSearch(self):
		self.itemSearchBackground.Show()
		self.itemSearch.SetUpdateEvent(ui.__mem_func__(self.OnItemSearchUpdate))
		self.itemSearch.Show()
		self.textsearch.Show()
		
	def CreatPrice(self):
		self.textPrice.Show()
		self.itemPriceBackground.Show()
		self.itemPrice.Show()
		
	def CreatMoeda(self):
		self.textMoeda.Show()
		self.MoedaCombo = DropDownList(self.board, 110, 135)
		for moedaType, moedaName in self.ITEM_MOEDA:
			self.MoedaCombo.InsertItem(moedaType,moedaName)
		self.MoedaCombo.SetEvent(ui.__mem_func__(self.OnMoedaSelected))
		self.MoedaCombo.Show()
		
	def OnMoedaSelected(self, moedaType):
		self.moeda = moedaType
		self.button.Show()
		if moedaType == 3:
			self.CreatPriceItem()
			self.PriceItem.OnIMEUpdate = ui.__mem_func__(self.OnPriceItemUpdate)
		else:
			self.RemovePriceItem()
			
			
	def RemovePriceItem(self):
		if self.textPriceItem:
			self.textPriceItem.Hide()

		if self.PriceItemBackground:
			self.PriceItemBackground.Hide()

		if self.PriceItem:
			self.PriceItem.Hide()
		
	def OnPriceItemUpdate(self):
		ui.EditLine.OnIMEUpdate(self.PriceItem)

		text = self.PriceItem.GetText().strip()
		if not text:
			self.SlotItem.ClearSlot(0)
			self.SlotItem.Hide()
			return
		if not text.isdigit():
			self.SlotItem.ClearSlot(0)
			self.SlotItem.Hide()
			return
		self.vnum = int(text)
		self.SlotItem.SetItemSlot(0, self.vnum, 1)
		self.SlotItem.Show()
		self.SlotItem.SetOverInItemEvent(ui.__mem_func__(self.OverInMoeda))
		self.SlotItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		
	def CreatPriceItem(self):
		if self.PriceItem:
			self.textPriceItem.Show()
			self.PriceItemBackground.Show()
			self.PriceItem.Show()
			return
		self.textPriceItem.Show()
		self.PriceItemBackground.Show()
		self.PriceItem.Show()
		
	def OverIn(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.item)
			
	def OverInMoeda(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.vnum)
			
	def OverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()
			
	def AddItem(self):
		vnum = self.selectedItem[0]
		count = 1
		price = int(self.itemPrice.GetText())
		currencyType = self.moeda + 1
		category = self.selectedCategory + 1
		if currencyType == 4:
			currencyVnum = int(self.PriceItem.GetText())
		else:
			currencyVnum = 0
		
		net.SendItemShopAdminAddItem(vnum, count, price, 0, category, currencyType, currencyVnum)
		
	def SetAdminStatus(self, text):
		self.adminStatus.SetText(text)
		self.adminStatus.Show()
	
	def ResultAdm(self, result):

		if result == ITEMSHOP_ADMIN_ADD_SUCCESS:
			self.SetAdminStatus("Item adicionado com sucesso.")
			self.ResetAddItem()

		elif result == ITEMSHOP_ADMIN_ADD_ITEM_NOT_FOUND:
			self.SetAdminStatus("O item informado não existe.")

		elif result == ITEMSHOP_ADMIN_ADD_CURRENCY_NOT_FOUND:
			self.SetAdminStatus("A moeda informada não existe.")

		elif result == ITEMSHOP_ADMIN_ADD_DATABASE_ERROR:
			self.SetAdminStatus("Erro ao adicionar o item.")

		elif result == ITEMSHOP_ADMIN_ADD_NO_PERMISSION:
			self.SetAdminStatus("Você não possui permissão.")
			
	def ResetAddItem(self):
		# Item selecionado
		if self.itemSlot:
			self.itemSlot.Hide()

		# Campo de preço
		if self.itemPrice:
			self.itemPrice.Hide()

		# Moeda
		if self.MoedaCombo:
			self.MoedaCombo.Hide()

		# Campo adicional de preço/item
		if self.textPriceItem:
			self.textPriceItem.Hide()

		if self.PriceItemBackground:
			self.PriceItemBackground.Hide()

		if self.PriceItem:
			self.PriceItem.Hide()

		# Remove item selecionado
		self.selectedItem = None

		# Reseta categoria
		self.selectedCategory = None
		self.categoryCombo.SetCurrentItem("Selecione...")

		# Limpa subcategoria
		self.subCategoryCombo.ClearItem()
		self.subCategoryCombo.SetCurrentItem("Selecione...")

		# Limpa item
		self.itemCombo.ClearItem()
		self.itemCombo.SetCurrentItem("Selecione...")

		# Limpa campos
		self.itemPrice.SetText("")
		self.PriceItem.SetText("")

		# Limpa moeda
		self.moeda = None

		# Limpa preview
		self.SlotItem.ClearSlot(0)
		self.SlotItem.Hide()
		
		self.textMoeda.Hide()
		self.textPrice.Hide()
		self.itemPriceBackground.Hide()
		self.button.Hide()
		self.adminStatus.Hide()
		
		self.moeda = 0
		self.price = 0
		self.priceItem = 0
		
class DropDownList(ui.Window):

	def __init__(self, parent, x, y):
		ui.Window.__init__(self)
		self.AddFlag("float")
		self.SetPickAlways()
		self.SetParent(parent)
		self.SetPosition(x, y)

		self.x = x
		self.y = y
		self.width = 123
		self.height = 20
		
		self.isSelected = False
		self.isOver = False
		self.isListOpened = False

		self.event = None
		self.currentText = ""
		
		# =========================================
		# TEXTO DO COMBOBOX FECHADO
		# =========================================

		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetPosition(5, 2)
		self.textLine.SetText("Selecione...")
		self.textLine.AddFlag("not_pick")
		self.textLine.Show()
		
		self.listBox = ui.DynamicListBox()
		self.listBox.SetParent(self)
		self.listBox.SetPickAlways()
		self.listBox.SetVisibleLineCount(12)
		self.listBox.SetEvent(
			ui.__mem_func__(self.OnSelectItem)
		)

		self.listBox.SetPosition(0, self.height + 5)
		self.listBox.SetSize(self.width, self.listBox.GetVisibleHeight())

		self.listBox.Hide()
		
		self.SetSize(self.width, self.height)
		
		self.CreatBord()
		
	def CreatBord(self):
		self.borderTop = ui.Bar()
		self.borderTop.SetParent(self)
		self.borderTop.SetPosition(0, 0)
		self.borderTop.SetSize(self.width, 1)
		self.borderTop.SetColor(0xFF6F6F6F)
		self.borderTop.AddFlag("not_pick")
		self.borderTop.Show()

		self.borderBottom = ui.Bar()
		self.borderBottom.SetParent(self)
		self.borderBottom.SetPosition(0, self.height - 1)
		self.borderBottom.SetSize(self.width, 1)
		self.borderBottom.SetColor(0xFF6F6F6F)
		self.borderBottom.AddFlag("not_pick")
		self.borderBottom.Show()

		self.borderLeft = ui.Bar()
		self.borderLeft.SetParent(self)
		self.borderLeft.SetPosition(0, 0)
		self.borderLeft.SetSize(1, self.height)
		self.borderLeft.SetColor(0xFF6F6F6F)
		self.borderLeft.AddFlag("not_pick")
		self.borderLeft.Show()

		self.borderRight = ui.Bar()
		self.borderRight.SetParent(self)
		self.borderRight.SetPosition(self.width - 1, 0)
		self.borderRight.SetSize(1, self.height)
		self.borderRight.SetColor(0xFF6F6F6F)
		self.borderRight.AddFlag("not_pick")
		self.borderRight.Show()
		
	def SetSize(self, width, height=20):
		ui.Window.SetSize(self, width, height)

		self.width = width
		self.height = height

		self.listBox.SetPosition(0, self.height + 5)
		self.listBox.SetSize(
			self.width,
			self.listBox.GetVisibleHeight()
		)

	def InsertItem(self, index, name):
		self.listBox.InsertItem(index, name)
		self.AdjustListBox()

	def ClearItem(self):
		self.CloseListBox()
		self.listBox.ClearItem()

	def AdjustListBox(self):
		if self.listBox.GetItemCount() <= 0:
			return

		self.listBox.SetSize(
			self.width,
			self.listBox.GetVisibleHeight()
		)

		self.listBox.LocateItem()

	def SetEvent(self, event):
		self.event = event

	def OpenListBox(self):
		if self.listBox.GetItemCount() <= 0:
			return
		self.isListOpened = True
		self.listBox.Show()
		self.listBox.SetTop()

	def CloseListBox(self):
		self.isListOpened = False
		self.listBox.Hide()

	def IsOpened(self):
		return self.isListOpened

	def OnSelectItem(self, index, name):
		self.CloseListBox()
		self.SetCurrentItem(name)
		if self.event:
			self.event(index)

	def OnMouseLeftButtonDown(self):
		self.isSelected = True

	def OnMouseLeftButtonUp(self):
		self.isSelected = False

		if self.isListOpened:
			self.CloseListBox()
		else:
			if self.listBox.GetItemCount() > 0:
				self.OpenListBox()

	def OnMouseWheel(self, nLen):
		if not self.isListOpened:
			return False

		if nLen > 0:
			self.listBox.OnUp()
			return True

		if nLen < 0:
			self.listBox.OnDown()
			return True

		return False
		
	def SetCurrentItem(self, text):
		self.currentText = text
		self.textLine.SetText(text)

	def GetCurrentItem(self):
		return self.currentText

	def InsertItem(self, index, name):
		self.listBox.InsertItem(index, name)

	def ClearItem(self):
		self.listBox.ClearItem()

		self.SetCurrentItem("Selecione...")