# blackdragonx61 / Mali
# Adaptado para mapa de teleporte Raiden5

import app
import chat
import localeInfo
import net
import player
import ui
import uitooltip
import wndMgr, item, chr

MAP_IMAGE = "bl_teleport/teleport.tga"
MOB_IMAGE_PATH = "bl_teleport/mobs/"
MAP_ASSET_WIDTH = 450.0
MAP_ASSET_HEIGHT = 600.0
MAP_POINT_WIDTH = 650.0
MAP_POINT_HEIGHT = 872.0
MAP_DISPLAY_WIDTH = 342.0
MAP_DISPLAY_HEIGHT = 456.0
MAP_MARKER_SIZE = 22.0

# Coordenadas da referencia 650x872, em ordem de indice de destino.
TELEPORT_POINTS = (
    (332, 160),
    (133, 288),
    (365, 333),
    (566, 384),
    (145, 731),
    (523, 710),
)

class TeleportItem:
	def __init__(self, main, parent, idx, name, desc_list, image, min_level, price, drop_list, point, mob_list, image_size, board = False):
		if board == True:
			self.board = ui.ThinBoardGold()
			self.board.SetParent(parent)
			self.board.SetSize(200, 75)
			self.board.Show()
			
			self.image = ui.ExpandedImageBox()
			self.image.SetParent(self.board)
			self.image.LoadImage(image)
			image_width = self.image.GetWidth()
			image_height = self.image.GetHeight()
			if image_width > 0 and image_height > 0:
				escala_x = image_size[0] / float(image_width)
				escala_y = image_size[1] / float(image_height)
				self.image.SetScale(escala_x, escala_y)
				
			self.nome = ui.TextLine()
			self.nome.SetParent(self.image)
			self.nome.SetPosition(30, 30)
			self.nome.SetText(name)
			self.nome.SetFontColor(128,128,128)
			self.nome.Show()
			
			if app.__BL_CLIP_MASK__:
				self.board.SetClippingMaskWindow(parent)
		else:
			self.image = ui.ExpandedImageBox()
			self.image.SetParent(parent)
			self.image.LoadImage(image)
			image_width = self.image.GetWidth()
			image_height = self.image.GetHeight()
			if image_width > 0 and image_height > 0:
				escala_x = image_size[0] / float(image_width)
				escala_y = image_size[1] / float(image_height)
				self.image.SetScale(escala_x, escala_y)
				
		self.image.SetEvent(ui.__mem_func__(main.TeleportEventProgress), "mouse_click", self)
		self.image.SetEvent(ui.__mem_func__(main.TeleportEventProgress), "mouse_over_in", self)
		self.image.SetEvent(ui.__mem_func__(main.TeleportEventProgress), "mouse_over_out", self)
		self.image.Show()
			
		
			
		self.idx = idx
		self.name = name
		self.desc_list = desc_list
		self.min_level = min_level
		self.price = price
		self.drop_list = drop_list
		self.point = point
		self.mob_list = mob_list
		

	def __del__(self):
		self.image = None

	def SetPosition(self, x, y):
		if hasattr(self, "board"):
			self.board.SetPosition(x, y)
		else:
			self.image.SetPosition(x, y)

	def SetAlpha(self, val):
		self.image.SetAlpha(val)

class TeleportItemEvent:
	def __init__(self, owner, vnum):
		self.owner = owner
		self.vnum = vnum

	def __call__(self, slotIndex):
		self.owner.OverIn(self.vnum)

class Teleport(ui.ScriptWindow):
	TELEPORT_NAME = 0
	TELEPORT_DESC_LIST = 1
	TELEPORT_IMAGE = 2
	TELEPORT_MIN_LEVEL = 3
	TELEPORT_PRICE = 4
	TELEPORT_DROP_LIST = 5
	TELEPORT_MOB_LIST = 6
	
	VISIBLE_ITEM_COUNT = 2
	Y_GAP = 85

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Load()
		self.infos = None
		self.tooltipItem = uitooltip.ItemToolTip()
		self.tooltipItem.Hide()
		self.drop_items = []
		self.panel_detail_lines = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.selected_teleport_idx = -1
		self.teleport_list = []
		self.toolTip = None
		self.map_image = None
		self.map_area = None
		self.go_button = None
		self.close_button = None
		self.panel_hint = None
		self.scroll_bar = None

	def __Load(self):
		self.selected_teleport_idx = -1
		self.teleport_list = []
		self.toolTip = uitooltip.ToolTip()
		self.toolTip.ClearToolTip()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/Teleport.py")

			self.map_area = self.GetChild("map_area")
			self.go_button = self.GetChild("gobutton")
			self.panel_hint = self.GetChild("panel_hint")
			self.panel =self.GetChild("action_panel")
			self.title = self.GetChild("panel_title")
			self.scroll_bar = self.GetChild("scroll_bar")
			
			self.panel_hint.textY = 5
			
		except:
			import exception
			exception.Abort("Teleport.__Load.LoadObject")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.go_button.SetEvent(ui.__mem_func__(self.Go))
			self.scroll_bar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		except:
			import exception
			exception.Abort("Teleport.__Load.BindObject")

		self.map_image = ui.ExpandedImageBox()
		self.map_image.SetParent(self.map_area)
		self.map_image.LoadImage(MAP_IMAGE)
		map_scale_x = MAP_DISPLAY_WIDTH / MAP_ASSET_WIDTH
		map_scale_y = MAP_DISPLAY_HEIGHT / MAP_ASSET_HEIGHT
		self.map_image.SetScale(map_scale_x, map_scale_y)
		map_width = int(MAP_ASSET_WIDTH * map_scale_x)
		map_height = int(MAP_ASSET_HEIGHT * map_scale_y)
		self.map_image.SetPosition((int(MAP_DISPLAY_WIDTH) - map_width) / 2, (int(MAP_DISPLAY_HEIGHT) - map_height) / 2)
		self.map_image.Show()

		teleport_list = net.GetTeleportList()
		point_count = min(len(teleport_list), len(TELEPORT_POINTS))
		for i in xrange(point_count):
			teleport = teleport_list[i]
			point = TELEPORT_POINTS[i]
			self.teleport_list.append(
				TeleportItem(
					self,
					self.map_area,
					i,
					teleport[self.TELEPORT_NAME],
					teleport[self.TELEPORT_DESC_LIST],
					teleport[self.TELEPORT_IMAGE],
					teleport[self.TELEPORT_MIN_LEVEL],
					teleport[self.TELEPORT_PRICE],
					teleport[self.TELEPORT_DROP_LIST],
					point,
					teleport[self.TELEPORT_MOB_LIST],
					(22, 22)
				)
			)
			marker_x = int((int(MAP_DISPLAY_WIDTH) - map_width) / 2 + point[0] / MAP_POINT_WIDTH * MAP_DISPLAY_WIDTH - MAP_MARKER_SIZE / 2)
			marker_y = int((int(MAP_DISPLAY_HEIGHT) - map_height) / 2 + point[1] / MAP_POINT_HEIGHT * MAP_DISPLAY_HEIGHT - MAP_MARKER_SIZE / 2)
			self.teleport_list[-1].SetPosition(marker_x, marker_y)
			
		for i, teleport in enumerate(teleport_list[6:]):
			self.teleport_list.append(
				TeleportItem(
					self,
					self.GetChild("object_area_window"),
					i + 6,
					teleport[self.TELEPORT_NAME],
					teleport[self.TELEPORT_DESC_LIST],
					teleport[self.TELEPORT_IMAGE],
					teleport[self.TELEPORT_MIN_LEVEL],
					teleport[self.TELEPORT_PRICE],
					teleport[self.TELEPORT_DROP_LIST],
					(0,0),
					teleport[self.TELEPORT_MOB_LIST],
					(200,75),
					True
				)
			)
			marker_x = 0
			marker_y = (i // 1) * self.Y_GAP
			self.teleport_list[-1].SetPosition(marker_x, marker_y)
		step = 1.0
		diff = len(self.teleport_list[6:]) - self.VISIBLE_ITEM_COUNT
		if diff > 0:
			step = max(0.007, step / diff)

		self.scroll_bar.SetScrollStep(step)

	def OnScroll(self):
		pos = self.scroll_bar.GetPos() * max(0, len(self.teleport_list[6:]) - self.VISIBLE_ITEM_COUNT) * self.Y_GAP
		
		for i, teleport in enumerate(self.teleport_list[6:]):
			teleport.SetPosition(0, (i // 1) * self.Y_GAP - pos)
	
	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.scroll_bar.OnUp()
		else:
			self.scroll_bar.OnDown()	
		return True
		
	def Open(self):
		self.scroll_bar.SetPos(0.0)
		self.Select(0)
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		if app.__BL_MOUSE_WHEEL_TOP_WINDOW__:
			wndMgr.SetWheelTopWindow(self.hWnd)

	def Close(self):
		self.toolTip.HideToolTip()
		self.Hide()
		if app.__BL_MOUSE_WHEEL_TOP_WINDOW__:
			wndMgr.ClearWheelTopWindow()

	def Go(self):
		if self.selected_teleport_idx == -1:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_MUST_SELECT)
			return

		teleport = self.teleport_list[self.selected_teleport_idx]
		if player.GetStatus(player.LEVEL) < teleport.min_level:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_LEVEL_LOW)
			return
		if player.GetMoney() < teleport.price:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_MONEY_LOW)
			return

		net.Teleport(self.selected_teleport_idx)
		self.Close()

	def Select(self, idx):
		if idx < 0 or idx >= len(self.teleport_list):
			self.selected_teleport_idx = -1
			return
		self.selected_teleport_idx = idx
		for teleport in self.teleport_list:
			teleport.SetAlpha(0.55)
		self.teleport_list[idx].SetAlpha(1.0)

	def info(self):
		if self.infos:
			self.infos.Hide()
		self.infos = ui.ThinBoardGold()
		self.infos.SetParent(self)
		self.infos.SetSize(285, 365)
		self.infos.SetPosition(380, 55)
		self.infos.textY = 5
		self.infos.Show()
	

	def AddMobImages(self, mob_list, parent=None, start_y=None):
		if parent is None:
			parent = self.infos

		if not hasattr(self, "mob_images"):
			self.mob_images = []

		for board, image in self.mob_images:
			image.Hide()
			self.board.Hide()
			self.text.Hide()
		self.mob_images = []
		
		ytext = start_y if start_y is not None else 40
		self.text = ui.TextLine()
		self.text.SetParent(parent)
		self.text.SetPosition(20, ytext)
		self.text.SetText("Mobs Locais:")
		self.text.Show()
		
		x = 20
		y = ytext + 20
		row_height = 0
		
		for filename in mob_list:
			vid = filename
			filename = str(filename)
			if not filename.lower().endswith(".tga"):
				filename += ".tga"
			
			self.board = ui.ThinBoardCircle()
			self.board.SetParent(parent)
			self.board.SetSize(64, 64)
			
			image = ui.ExpandedImageBox()
			image.SetParent(self.board)
			image.LoadImage(MOB_IMAGE_PATH + filename)

			image_width = self.board.GetWidth()
			image_height = self.board.GetHeight()

			if x + image_width > parent.GetWidth() - 10 and x > 20:
				x = 20
				y += row_height + 4
				row_height = 0

			image.SetPosition(2, 2)
			
			self.board.SetPosition(x, y)
			self.board.Show()
			image.Show()
			self.mob_images.append((self.board, image))

			x += image_width + 4
			row_height = max(row_height, image_height)
				
	def AddDropItems(self, drop_list, parent=None):
		if parent is None:
			parent = self.infos

		for slot in self.drop_items:
			slot.Hide()
		self.drop_items = []
		
		ytext = int(getattr(parent, "textY", 0) + 15)
		self.textDrop = ui.TextLine()
		self.textDrop.SetParent(parent)
		self.textDrop.SetPosition(20, ytext)
		self.textDrop.SetText("Drops Locais:")
		self.textDrop.Show()
		
		x = 10
		y = ytext + 20
		row_height = 0

		for vnum in drop_list:
			item.SelectItem(vnum)
			x_item, y_item = item.GetItemSize()
			w = int(x_item * 32)
			h = int(y_item * 32)

			# Se o item não couber, começa uma nova linha.
			if x + w > parent.GetWidth() - 10 and x > 10:
				x = 10
				y += row_height + 4
				row_height = 0

			slot = ui.SlotWindow()
			slot.SetParent(parent)
			slot.SetPosition(x, y)
			slot.SetSize(w, h)
			slot.AppendSlot(0, 0, 0, w, h)
			slot.SetItemSlot(0, vnum, 1)
			slot.SetOverInItemEvent(TeleportItemEvent(self, vnum))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
			slot.Show()
			self.drop_items.append(slot)

			x += w + 4
			row_height = max(row_height, h)
		
		self.line = ui.Line()
		self.line.SetParent(parent)
		self.line.SetColor(0xffffffff)
		self.line.SetPosition(5, y + row_height + 5)
		self.line.SetSize(310, 0)
		self.line.Show()
		# Devolve a posição abaixo da última linha de drops.
		return y + row_height + 8

	def OverIn(self, vnum):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(vnum)

	def OverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def TeleportEventProgress(self, event_type, teleport_item):
		if "mouse_click" == event_type:
			self.title.SetText("Local Escolhido:")
			self.Select(teleport_item.idx)
			self.panel_hint.SetText(teleport_item.name)
			# Esconde as informações do ponto clicado anteriormente
			for line in self.panel_detail_lines:
				line.Hide()
			self.panel_detail_lines = []
			# panel_hint fica em y=42; começa as informações abaixo dele
			self.panel.textY = 62
			for desc in teleport_item.desc_list:
				line = self.panel.AutoAppendTextLine("{}".format(desc), 0xff00ff00)
				self.panel_detail_lines.append(line)
			self.AddDropItems(teleport_item.drop_list, self.panel)
			drop_end_y = self.AddDropItems(teleport_item.drop_list, self.panel)
			self.AddMobImages(teleport_item.mob_list, self.panel, drop_end_y)
			if teleport_item.mob_list == []:
				self.text.Hide()
				self.line.Hide()
			if teleport_item.drop_list == []:
				self.textDrop.Hide()
		elif "mouse_over_in" == event_type:
			self.toolTip.ClearToolTip()
			self.toolTip.toolTipWidth = self.toolTip.TOOL_TIP_WIDTH
			self.toolTip.SetTitle(teleport_item.name)
			for desc in teleport_item.desc_list:
				self.toolTip.AutoAppendTextLine("{}".format(desc), 0xffffffff)
			self.toolTip.AutoAppendTextLine("{}: {}".format(localeInfo.TELEPORT_MIN_LEVEL, teleport_item.min_level), 0xffffffff)
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.ShowToolTip()
		elif "mouse_over_out" == event_type:
			self.toolTip.HideToolTip()


	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.__BL_MOUSE_WHEEL_TOP_WINDOW__:
		def OnMouseWheelButtonUp(self):
			return False

		def OnMouseWheelButtonDown(self):
			return False








