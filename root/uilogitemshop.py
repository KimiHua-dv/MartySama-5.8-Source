import ui, uiToolTip, uiscriptLocale, net, chat, localeInfo
import item as item_module

class LogWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()
		self.purchaseInfos = []
		self.infos = {0: [],}
		self.scrollPosition = 0.0
		self.logTop = 40
		self.logBottom = 480
		self.logOffset = 0
		self.logLimit = 50
		self.loadingLogs = False
		self.hasMoreLogs = True
		self.logs = []
		self.searchMode = False
		self.searchText = ""
		
		
	def __LoadWindow(self):
		script = ui.PythonScriptLoader()
		script.LoadScriptFile(self, "uiscript/logwindow.py")

		self.board = self.GetChild("board")
		# self.titulos = self.GetChild("titulos")
		self.scrollBar = ui.ScrollBar()
		
		self.board.SetMouseWheelEvent(self.OnMouseWheel)
		self.scrollBar = ui.ScrollBar()
		
		self.scrollBar.SetParent(self.board)
		self.scrollBar.SetPosition(915, 30)
		self.scrollBar.SetScrollBarSize(480)
		self.scrollBar.SetScrollEvent(self.OnScroll)
		self.scrollBar.Hide()
		
		self.inputThinBoard = ui.ThinBoard()
		self.inputThinBoard.SetParent(self)
		self.inputThinBoard.SetPosition(590, 10)
		self.inputThinBoard.SetSize(150, 20)
		self.inputThinBoard.Show()
		
		self.searchInput = ui.EditLine()
		self.searchInput.SetParent(self)
		self.searchInput.SetPosition(600, 10)
		self.searchInput.SetSize(150, 20)
		self.searchInput.SetMax(30)
		self.searchInput.SetText("")
		self.searchInput.Show()
		
		self.searchButton = ui.Button()
		self.searchButton.SetParent(self)
		self.searchButton.SetPosition(755, 10)
		self.searchButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.searchButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.searchButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.searchButton.SetText("Buscar")
		self.searchButton.SetEvent(ui.__mem_func__(self.SearchLogs))
		self.searchButton.Show()
		
	def OnPressEscapeKey(self):
		self.Hide()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
	def SearchLogs(self):
		search = self.searchInput.GetText().strip()
		self.ClearInfos()
		self.scrollPosition = 0.0
		self.scrollBar.SetPos(0.0)
		self.logOffset = 0
		self.loadingLogs = False
		self.hasMoreLogs = True
		if not search:
			self.searchMode = False
			self.searchText = ""
			for info in self.logs:
				self.CreatePurchaseInfo(info)
			self.UpdateScrollBar()
			return
		self.searchMode = True
		self.searchText = search
		net.SendSearchItemShopLog(search, 0)
				
	def OnScroll(self):
		self.scrollPosition = self.scrollBar.GetPos()
		self.UpdateScrollBar()
		if self.scrollPosition >= 0.80:
			self.LoadMoreLogs()
		self.RefreshVisibleItems()
		
	def UpdateScrollBar(self):
		totalHeight = self.CalculateContentHeight()
		visibleHeight = self.logBottom - self.logTop
		maxScroll = max(0, totalHeight - visibleHeight)
		offset = self.scrollPosition * maxScroll
		for purchase in self.purchaseInfos:
			purchase.SetPosition(0,purchase.originalY - offset)
		if maxScroll <= 0:
			self.scrollBar.Hide()
			self.scrollPosition = 0.0
			return
		self.scrollBar.Show()
			
	def GetTotalRows(self):
		return (len(self.purchaseInfos) + self.columns - 1) // self.columns
		
	def CalculateContentHeight(self):
		total = 0
		for purchase in self.purchaseInfos:
			total += purchase.height + 5
		return total
		
	def CreatePurchaseInfo(self, info):
		data, item, qnt, moeda, moeda_vnum, valor , player= info

		x = 0
		y = 40

		for purchase in self.purchaseInfos:
			y += purchase.height + 5

		purchase = PurchaseInfo()

		purchase.Create(
			self.board,
			data,
			item,
			qnt,
			moeda,
			moeda_vnum,
			valor,
			player,
			x,
			y
		)
		purchase.originalY = y
		self.purchaseInfos.append(purchase)
		self.UpdateScrollBar()
		self.RefreshVisibleItems()
		

	def ClearInfos(self):
		for purchase in self.purchaseInfos:
			purchase.Hide()
			purchase.Destroy()

		self.purchaseInfos = []
		self.infos[0] = []
		
	def Open(self):
		self.ClearInfos()
		self.scrollPosition = 0.0
		self.scrollBar.SetPos(0.0)
		self.Show()
		self.logOffset = 0
		self.loadingLogs = False
		self.hasMoreLogs = True

		net.SendItemShopLog(self.logOffset)
		
	def Close(self):
		self.Hide()

	def LoadMoreLogs(self):
		if self.loadingLogs:
			return
		if not self.hasMoreLogs:
			return
		self.loadingLogs = True
		if self.searchMode:
			net.SendSearchItemShopLog(self.searchText, self.logOffset)
		else:
			net.SendItemShopLog(self.logOffset)
		
	def FinishItemShopLogLoad(self, count):
		self.logOffset += count
		self.loadingLogs = False
		chat.AppendChat(1,"%d " % count) 
		if count < 50:
			self.hasMoreLogs = False
			
	def RefreshVisibleItems(self):
		totalHeight = self.CalculateContentHeight()
		visibleHeight = self.logBottom - self.logTop
		maxScroll = max(0, totalHeight - visibleHeight)
		offset = self.scrollPosition * maxScroll
		for purchase in self.purchaseInfos:
			y = purchase.originalY - offset
			bottom = y + purchase.height
			if bottom <= self.logTop or y >= self.logBottom:
				purchase.Hide()
			elif y < self.logTop:
				purchase.Hide()
			elif y >= self.logBottom:
				purchase.Hide()
			else:
				purchase.SetPosition(0, y)
				purchase.Show()
				
	def OnMouseWheel(self, nLen):
		if not self.board.IsShow():
			return False

		if nLen > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()	

		return True
	
			
	def GetItemName(self, vnum):
		item_module.SelectItem(vnum)
		return item_module.GetItemName()
		
class PurchaseInfo(ui.ScriptWindow):
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.height = 0
		
	def Create(self, parent, data, item, qnt, moeda, moeda_vnum, valor, player, x, y):
		self.itemName = self.GetItemName(item)
		item_module.SelectItem(item)
		_, item_height = item_module.GetItemSize()
		self.height = 35 + ((item_height - 1) * 32)
		self.item = item
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(300, 100)
		self.Show()

		self.CreatBoard(item)
		self.CreatPlayerName(player)
		self.CreatData(data)
		self.CreatItem(item, qnt)
		self.CreatValor(valor)
		self.CreatMoeda(moeda, moeda_vnum)
		self.CreatQnt(qnt)
		

		
	def CreatBoard(self, item):
		self.boardinfo = ui.ThinBoard()
		self.boardinfo.SetParent(self)
		self.boardinfo.SetPosition(5,0)
		self.boardinfo.SetSize(910,self.GetHeight() + 5)
		self.boardinfo.Show()
		
	def GetItemName(self, vnum):
		item_module.SelectItem(vnum)
		return item_module.GetItemName()
		
	def CreatData(self, data):
		self.dataText = ui.TextLine()
		self.dataText.SetParent(self)
		self.dataText.SetPosition(30,5)
		self.dataText.SetText(data)
		self.dataText.Show()
		
	def CreatItem(self, item, qnt):
		item_module.SelectItem(item)
		x_item, y_item = item_module.GetItemSize()
		w = x_item * 32
		h = y_item * 32
		self.itemText = ui.TextLine()
		self.itemText.SetParent(self)
		self.itemText.SetPosition(250,5)
		self.itemText.SetText(self.itemName)
		self.itemText.Show()
		self.slot = ui.SlotWindow()
		self.slot.SetParent(self)
		self.slot.SetPosition(200,2)
		self.slot.SetSize(w,h)
		for i in range(y_item):
			self.slot.AppendSlot(i,0,i * 32,32,32)
		self.slot.SetItemSlot(0, item, qnt)
		self.slot.SetOverInItemEvent(ui.__mem_func__(self.OverIn))
		self.slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		self.slot.Show()
		self.slot.SetTop()
		
	def CreatQnt(self, qnt):
		self.qntText = ui.TextLine()
		self.qntText.SetParent(self)
		self.qntText.SetPosition(440,5)
		self.qntText.SetText(str(qnt))
		self.qntText.Show()
		
	def CreatMoeda(self, moeda, moeda_vnum):
		self.moedaText = ui.TextLine()
		self.moedaText.SetParent(self)
		self.moedaText.SetPosition(595,5)
		if moeda == 1:
			self.moedaText.SetFontColor(14, 33, 237)
			self.moedaText.SetText("Cash")
		elif moeda == 2:
			self.moedaText.SetFontColor(14, 33, 37)
			self.moedaText.SetText("Gold")
		elif moeda == 3:
			self.moedaText.SetFontColor(214, 33, 137)
			self.moedaText.SetText("Won")
		else:
			self.moedaText.SetFontColor(214, 133, 137)
			self.moedaText.SetText("%s" % self.GetItemName(moeda_vnum))
		self.moedaText.Show()
		
	def CreatValor(self, valor):
		self.valorText = ui.TextLine()
		self.valorText.SetParent(self)
		self.valorText.SetPosition(695,5)
		self.valorText.SetText(str(valor))
		self.valorText.Show()
		
	def CreatPlayerName(self, player):
		self.PlayerLabel = ui.TextLine()
		self.PlayerLabel.SetParent(self)
		self.PlayerLabel.SetPosition(30, 20)
		self.PlayerLabel.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.PlayerLabel.SetFontColor(58, 219, 97)
		self.PlayerLabel.SetText("Player:")
		self.PlayerLabel.Show()
		

		width, height = self.PlayerLabel.GetTextSize()

		self.PlayerName = ui.TextLine()
		self.PlayerName.SetParent(self)
		self.PlayerName.SetPosition(30 + width + 5, 20)
		self.PlayerName.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.PlayerName.SetFontColor(14, 33, 237)
		self.PlayerName.SetText(str(player))
		self.PlayerName.Show()
		
		
	def OverIn(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.item)
			
	def OverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def GetHeight(self):
		return self.height
		