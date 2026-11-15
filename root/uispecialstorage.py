import ui
import player
import mouseModule
import ime
import net
import app
import snd
import item
import chat
import uiScriptLocale
import uiCommon
import uiPrivateShopBuilder
import uiPickMoney
import localeInfo
import constInfo
import interfacemodule

INVANTORY_ADET = 5

class SpecialStorageWindow(ui.ScriptWindow):
	UPGRADE_TYPE = 0
	BOOK_TYPE = 1
	STONE_TYPE = 2
	CHEST_TYPE = 3
	COSTUME_TYPE = 4
	remaining_time = 0

	SLOT_WINDOW_TYPE = {
		UPGRADE_TYPE		:	{"window" : player.UPGRADE_INVENTORY,		"slot" : player.SLOT_TYPE_UPGRADE_INVENTORY},
		BOOK_TYPE			:	{"window" : player.BOOK_INVENTORY,			"slot" : player.SLOT_TYPE_BOOK_INVENTORY},
		STONE_TYPE			:	{"window" : player.STONE_INVENTORY,			"slot" : player.SLOT_TYPE_STONE_INVENTORY},
		CHEST_TYPE			:	{"window" : player.CHEST_INVENTORY,			"slot" : player.SLOT_TYPE_CHEST_INVENTORY},
		COSTUME_TYPE		:	{"window" : player.COSTUME_INVENTORY,		"slot" : player.SLOT_TYPE_COSTUME_INVENTORY}
	}

	WINDOW_NAMES = {
		UPGRADE_TYPE		:	localeInfo.SPECIAL_INVENTORY_UPGRADE,
		BOOK_TYPE			:	localeInfo.SPECIAL_INVENTORY_BOOK,
		STONE_TYPE			:	localeInfo.SPECIAL_INVENTORY_STONE,
		CHEST_TYPE			:	localeInfo.SPECIAL_INVENTORY_CHEST,
		COSTUME_TYPE		:	localeInfo.SPECIAL_INVENTORY_COSTUME
	}

	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.questionDialog = None
		self.tooltipItem = None
		self.dlgSplitItems = None

		self.wndItemDelete = None
		self.interface = None
		self.sellingSlotNumber = -1
		self.isLoaded = 0
		self.inventoryPageIndex = 0
		self.categoryPageIndex = 0
		self.SetWindowName("SpecialStorageWindow")
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/SpecialStorageWindow.py")
		except:
			import exception
			exception.Abort("SpecialStorageWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.titleName = self.GetChild("TitleName")

			self.inventoryTab = []
			self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_04"))

			self.categoryTab = []
			self.categoryTab.append(self.GetChild("Category_Tab_01"))
			self.categoryTab.append(self.GetChild("Category_Tab_02"))
			self.categoryTab.append(self.GetChild("Category_Tab_03"))
			self.categoryTab.append(self.GetChild("Category_Tab_04"))
			self.categoryTab.append(self.GetChild("Category_Tab_05")) #costume

		except:
			import exception
			exception.Abort("SpecialStorageWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))

		## Grade button
		self.inventoryTab[0].SetEvent(lambda arg=0: self.SetInventoryPage(arg))
		self.inventoryTab[1].SetEvent(lambda arg=1: self.SetInventoryPage(arg))
		self.inventoryTab[2].SetEvent(lambda arg=2: self.SetInventoryPage(arg))
		self.inventoryTab[3].SetEvent(lambda arg=3: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		self.categoryTab[0].SetEvent(lambda arg=0: self.SetCategoryPage(arg))
		self.categoryTab[1].SetEvent(lambda arg=1: self.SetCategoryPage(arg))
		self.categoryTab[2].SetEvent(lambda arg=2: self.SetCategoryPage(arg))
		self.categoryTab[3].SetEvent(lambda arg=3: self.SetCategoryPage(arg))
		self.categoryTab[4].SetEvent(lambda arg=4: self.SetCategoryPage(arg)) #costume
		self.categoryTab[0].Down()

		## Etc
		self.wndItem = wndItem

		self.wndPopupDialog = uiCommon.PopupDialog()

		self.dlgSplitItems = uiPickMoney.PickMoneyDialog()
		self.dlgSplitItems.LoadDialog()
		self.dlgSplitItems.Hide()

		if app.ENABLE_HIGHLIGHT_NEW_ITEM:
			self.listHighlightedUpdateSlot = []
			self.listHighlightedBookSlot = []
			self.listHighlightedStoneSlot = []
			self.listHighlightedChestSlot = []
			self.listHighlightedCostumeSlot = []

		self.SetInventoryPage(0)
		self.SetCategoryPage(0)
		self.RefreshItemSlot()
		self.RefreshBagSlotWindow()

	@ui.WindowDestroy
	def Destroy(self):
		self.ClearDictionary()
		self.tooltipItem = None
		self.wndItem = 0
		self.questionDialog = None
		self.dlgSplitItems.Destroy()
		self.dlgSplitItems = None
		self.inventoryTab = []
		self.categoryTab = []
		self.titleName = None
		self.interface = None
		self.editButton = None

	if app.ENABLE_HIGHLIGHT_NEW_ITEM:
		def HighlightSlotUpdate(self, slot):
			if not slot in self.listHighlightedUpdateSlot:
				self.listHighlightedUpdateSlot.append(slot)

		def HighlightSlotBook(self, slot):
			if not slot in self.listHighlightedBookSlot:
				self.listHighlightedBookSlot.append(slot)

		def HighlightSlotStone(self, slot):
			if not slot in self.listHighlightedStoneSlot:
				self.listHighlightedStoneSlot.append(slot)

		def HighlightSlotChest(self, slot):
			if not slot in self.listHighlightedChestSlot:
				self.listHighlightedChestSlot.append(slot)

		def HighlightSlotCostume(self, slot):
			if not slot in self.listHighlightedCostumeSlot:
				self.listHighlightedCostumeSlot.append(slot)

	def BindInterfaceClass(self, interface):
		self.interface = interface

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

	def Close(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		if self.dlgSplitItems:
			self.dlgSplitItems.Close()
		self.Hide()

	def SetInventoryPage(self, page):
		self.inventoryTab[self.inventoryPageIndex].SetUp()
		self.inventoryPageIndex = page
		self.inventoryTab[self.inventoryPageIndex].Down()

		self.RefreshBagSlotWindow()
		self.RefreshBagSlotInventoryWindow(page+1)

	def SetCategoryPage(self, page):
		self.categoryTab[self.categoryPageIndex].SetUp()
		self.categoryPageIndex = page
		self.categoryTab[self.categoryPageIndex].Down()

		self.titleName.SetText(self.WINDOW_NAMES[self.categoryPageIndex])
		self.RefreshBagSlotWindow()

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def RefreshItemSlot(self):
		self.RefreshBagSlotWindow()

	def RefreshStatus(self):
		self.RefreshItemSlot()

	def __InventoryLocalSlotPosToGlobalSlotPos(self, localSlotPos):
		return self.inventoryPageIndex * player.SPECIAL_PAGE_SIZE + localSlotPos

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	def GetInventoryWindow(self):
		return self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"]
		
	def GetInventoryType(self):
		return self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["slot"]

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=None):
			if not self.interface:
				return
			if not self.wndItem:
				return
			onTopWnd = self.interface.GetOnTopWindow()
			if localIndex:
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
				slotType = self.GetInventoryWindow()
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)


				elif app.ENABLE_ACCE_COSTUME_SYSTEM and onTopWnd == player.ON_TOP_WND_ACCE_RESTORE:
					if ():
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				return

			for i in xrange(player.SPECIAL_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				slotType = self.GetInventoryWindow()
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotType, slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

	def RefreshBagSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVnum=self.wndItem.SetItemSlot

		for i in xrange(player.SPECIAL_PAGE_SIZE*INVANTORY_ADET):
			self.wndItem.EnableSlot(i)
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)
			itemVnum = getItemVNum(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)

			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			setItemVnum(i, itemVnum, itemCount)

			if app.ENABLE_HIGHLIGHT_NEW_ITEM:
				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.UPGRADE_INVENTORY:
					if slotNumber in self.listHighlightedUpdateSlot:
						self.wndItem.ActivateSlot(i)

					if slotNumber in self.listHighlightedBookSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedStoneSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedChestSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedCostumeSlot:
						self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.BOOK_INVENTORY:
					if slotNumber in self.listHighlightedBookSlot:
						self.wndItem.ActivateSlot(i)

					if slotNumber in self.listHighlightedUpdateSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedStoneSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedChestSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedCostumeSlot:
						self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.STONE_INVENTORY:
					if slotNumber in self.listHighlightedStoneSlot:
						self.wndItem.ActivateSlot(i)

					if slotNumber in self.listHighlightedUpdateSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedBookSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedChestSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedCostumeSlot:
						self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.CHEST_INVENTORY:
					if slotNumber in self.listHighlightedChestSlot:
						self.wndItem.ActivateSlot(i)

					if slotNumber in self.listHighlightedUpdateSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedBookSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedStoneSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedCostumeSlot:
						self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.COSTUME_INVENTORY:
					if slotNumber in self.listHighlightedCostumeSlot:
						self.wndItem.ActivateSlot(i)

					if slotNumber in self.listHighlightedUpdateSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedBookSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedStoneSlot:
						self.wndItem.DeactivateSlot(i)

					if slotNumber in self.listHighlightedChestSlot:
						self.wndItem.DeactivateSlot(i)

		self.wndItem.RefreshSlot()

	def RefreshBagSlotInventoryWindow(self, page):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVnum=self.wndItem.SetItemSlot

		for i in xrange(player.SPECIAL_PAGE_SIZE*INVANTORY_ADET):
			self.wndItem.EnableSlot(i)
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)
			itemVnum = getItemVNum(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)

			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			setItemVnum(i, itemVnum, itemCount)

			if app.ENABLE_HIGHLIGHT_NEW_ITEM:
				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.UPGRADE_INVENTORY:
					if player.SPECIAL_PAGE_SIZE*(page-1) <= slotNumber and\
						player.SPECIAL_PAGE_SIZE*page >= slotNumber:
						if slotNumber in self.listHighlightedUpdateSlot:
							self.wndItem.ActivateSlot(i)
						else:
							self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.BOOK_INVENTORY:
					if player.SPECIAL_PAGE_SIZE*(page-1) <= slotNumber and\
						player.SPECIAL_PAGE_SIZE*page >= slotNumber:
						if slotNumber in self.listHighlightedBookSlot:
							self.wndItem.ActivateSlot(i)
						else:
							self.wndItem.DeactivateSlot(i)

				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.STONE_INVENTORY:
					if player.SPECIAL_PAGE_SIZE*(page-1) <= slotNumber and\
						player.SPECIAL_PAGE_SIZE*page >= slotNumber:
						if slotNumber in self.listHighlightedStoneSlot:
							self.wndItem.ActivateSlot(i)
						else:
							self.wndItem.DeactivateSlot(i)
			
				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.CHEST_INVENTORY:
					if player.SPECIAL_PAGE_SIZE*(page-1) <= slotNumber and\
						player.SPECIAL_PAGE_SIZE*page >= slotNumber:
						if slotNumber in self.listHighlightedChestSlot:
							self.wndItem.ActivateSlot(i)
						else:
							self.wndItem.DeactivateSlot(i)
			
				if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] == player.COSTUME_INVENTORY:
					if player.SPECIAL_PAGE_SIZE*(page-1) <= slotNumber and\
						player.SPECIAL_PAGE_SIZE*page >= slotNumber:
						if slotNumber in self.listHighlightedCostumeSlot:
							self.wndItem.ActivateSlot(i)
						else:
							self.wndItem.DeactivateSlot(i)
			
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)

		self.wndItem.RefreshSlot()

		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"])

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()
			
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overSlotPosGlobal = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(False)

		if app.ENABLE_HIGHLIGHT_NEW_ITEM:
			if overSlotPosGlobal in self.listHighlightedUpdateSlot:
				self.listHighlightedUpdateSlot.remove(overSlotPosGlobal)
				self.wndItem.DeactivateSlot(overSlotPos)

			if overSlotPosGlobal in self.listHighlightedBookSlot:
				self.listHighlightedBookSlot.remove(overSlotPosGlobal)
				self.wndItem.DeactivateSlot(overSlotPos)

			if overSlotPosGlobal in self.listHighlightedStoneSlot:
				self.listHighlightedStoneSlot.remove(overSlotPosGlobal)
				self.wndItem.DeactivateSlot(overSlotPos)

			if overSlotPosGlobal in self.listHighlightedChestSlot:
				self.listHighlightedChestSlot.remove(overSlotPosGlobal)
				self.wndItem.DeactivateSlot(overSlotPos)

			if overSlotPosGlobal in self.listHighlightedCostumeSlot:
				self.listHighlightedCostumeSlot.remove(overSlotPosGlobal)
				self.wndItem.DeactivateSlot(overSlotPos)

		self.ShowToolTip(overSlotPosGlobal)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgSplitItems.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["slot"], itemSlotIndex, selectedItemVNum, count)

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return
		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
		selectedItemVNum = player.GetItemIndex(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()
			attachedItemCount  = mouseModule.mouseController.GetAttachedItemCount()

			if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["slot"] == attachedSlotType:
				if attachedItemVID == selectedItemVNum:
					if item.GetItemType() == item.ITEM_TYPE_METIN:
						net.SendItemUseToItemPacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"],attachedSlotPos,self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"],itemSlotIndex)
					self.__DropSrcItemToDestItemInInventory(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], attachedSlotPos, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex, attachedItemCount)
				else:
					net.SendItemUseToItemPacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], attachedSlotPos, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)

			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()

			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)

				if itemCount > 1:
					self.dlgSplitItems.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgSplitItems.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgSplitItems.Open(itemCount)
					self.dlgSplitItems.itemGlobalSlotIndex = itemSlotIndex
			else:
				itemCount = player.GetItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotIndex)
				mouseModule.mouseController.AttachObject(self, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["slot"], itemSlotIndex, selectedItemVNum, itemCount)

				self.wndItem.SetUseMode(False)
				snd.PlaySound("sound/ui/pick.wav")

	def __SellItem(self, itemSlotPos):#, soruSorma=False):
		self.sellingSlotNumber = itemSlotPos
		itemIndex = player.GetItemIndex(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotPos)
		itemCount = player.GetItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotPos)

		item.SelectItem(itemIndex)

		if item.IsAntiFlag(item.ANTIFLAG_SELL):
			popup = uiCommon.PopupDialog()
			popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup
			return

#		if soruSorma:
#			net.SendItemSellPacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], itemSlotPos, itemCount)
#			snd.PlaySound("sound/ui/money.wav")
#			return
	
		itemPrice = item.GetISellItemPrice()

		item.GetItemName(itemIndex)
		itemName = item.GetItemName()

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.count = itemCount

	def SellItem(self):
		net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"])
		snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()

		self.questionDialog = None

	def __OnClosePopupDialog(self):
		self.pop = None

	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			if player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")
			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)
			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				net.SendSafeboxCheckoutPacket(attachedSlotPos, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], selectedSlotPos)
			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], selectedSlotPos)
			elif player.RESERVED_WINDOW != attachedInvenType or\
				player.UPGRADE_INVENTORY == attachedInvenType or\
				player.BOOK_INVENTORY == attachedInvenType or\
				player.STONE_INVENTORY == attachedInvenType or\
				player.CHEST_INVENTORY == attachedInvenType or\
				player.COSTUME_INVENTORY == attachedInvenType:

				itemCount = player.GetItemCount(attachedInvenType, attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()

				self.__SendMoveItemPacket(attachedInvenType, attachedSlotPos, self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], selectedSlotPos, attachedCount)

			mouseModule.mouseController.DeattachObject()

	def UseItemSlot(self, slotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		windowIndex = self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"]
		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

	
#		if app.IsPressed(app.DIK_LCONTROL) and app.IsPressed(app.DIK_X):
#			self.__SellItem(slotIndex, True)
#			return
#
#		elif app.IsPressed(app.DIK_LCONTROL):
#			itemIndex = player.GetItemIndex(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotIndex)
#			item.SelectItem(itemIndex)
#			if (item.GetItemType() == item.ITEM_TYPE_GIFTBOX) or (item.GetItemType() == item.ITEM_TYPE_USE and item.GetItemSubType() == item.USE_SPECIAL):
#				net.SendInstantChestOpenPacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotIndex)
#				return
#
#		# if app.IsPressed(app.DIK_LCONTROL):
#			# if item.GetItemType() == item.ITEM_TYPE_GIFTBOX:
#				# for i in xrange(700):
#					# net.SendItemUsePacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotIndex)
#
		
		windowIndex = self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"]
		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		itemIndex = player.GetItemIndex(windowIndex, slotIndex)
		item.SelectItem(itemIndex)

		if item.GetItemType() == item.ITEM_TYPE_GIFTBOX:
			if app.ENABLE_SHOW_CHEST_DROP:
				if self.interface and self.interface.dlgChestDrop:
					if self.interface.dlgChestDrop.IsShow():
						return

					self.interface.dlgChestDrop.Open(slotIndex, windowIndex)
					net.SendChestDropInfo(slotIndex)

					mouseModule.mouseController.DeattachObject()
					self.OverOutItem()
					return

		net.SendItemUsePacket(windowIndex, slotIndex)

		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()


	def __SendMoveItemPacket(self, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, srcItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] != player.UPGRADE_INVENTORY and\
			self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] != player.BOOK_INVENTORY and\
			self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] != player.STONE_INVENTORY and\
			self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] != player.CHEST_INVENTORY and\
			self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"] != player.COSTUME_INVENTORY:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YOU_CANNOT_DO_THIS)
			return

		net.SendItemMovePacket(srcSlotWindow , srcSlotPos, dstSlotWindow, dstSlotPos, srcItemCount)

	def __DropSrcItemToDestItemInInventory(self, veri_1, veri_2, veri_3, veri_4, veri_5):
		if veri_2 == veri_4:
			return

		net.SendItemMovePacket(veri_1, veri_2, veri_3, veri_4, veri_5)
