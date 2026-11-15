import ui, chat, grp, player, chr, app, localeInfo

SELECT_COLOR = grp.GenerateColor(0.7, 0.7, 0.7, 1.0)
COLOR_BG = grp.GenerateColor(1.0,1.0,1.0, 0.5)
TOOLTIP_COLOR = grp.GenerateColor(0.0, 1.0, 0.0, 1.0)
class MetinQueue(ui.BoardWithTitleBar):

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		
		self.childrenList = []
		self.textY = 0
		self.lastQueue = []
		
		self.SetSize(200, 300)
		self.SetCenterPosition()
		self.SetTitleName("Metin Queue")
		self.background()
		self.SetCloseEvent(self.Close)
		self.AddFlag("movable")
		self.Show()
		self.refreshTime = 0.0
		self.SetUpdateEvent(self.OnUpdate)
		self.texto = ui.TextLine()
		self.texto.SetParent(self)
		self.texto.SetFontName(localeInfo.UI_DEF_FONT)
		self.texto.SetText("Ativar Automatico")
		self.texto.SetPosition(65, 190)
		self.texto.Show()
		self.box()
		self.info()
		
	def background(self):
		self.bg = ui.ThinBoardGold()
		self.bg.SetParent(self)
		self.bg.SetPosition(15, 35)
		self.bg.SetSize(170, 150)
		self.bg.Show()

		self.AppendMetinQueue()
	
	def info(self):
		self.infos = ui.TextLine()
		self.infos.SetParent(self)
		self.infos.SetPosition(20, 230)
		self.infos.SetMultiLine()
		self.infos.SetLimitWidth(165)
		self.infos.SetText("Clique em outra Metin Pra Adicionar a Fila. Modo Automatico requer um item especial.")
		self.infos.SetFontColor(0.0, 1.0, 0.0)
		self.infos.Show()
		
	def box(self):
		self.is_active = False 

		# --- CONFIGURAÇÃO DO BOTÃO ---
		self.autoAttackToggle = ui.Button() # Corrigido para ui.Button()
		self.autoAttackToggle.SetParent(self)
		self.autoAttackToggle.SetPosition(60, 205)

		self.autoAttackToggle.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.autoAttackToggle.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.autoAttackToggle.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")

		self.autoAttackToggle.SetText("Desativado")
		self.autoAttackToggle.SetEvent(ui.__mem_func__(self.OnAutoAttackOff))
		self.autoAttackToggle.Show()
		
		self.infoButton = ui.Button()
		self.infoButton.SetParent(self)
		self.infoButton.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		self.infoButton.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
		self.infoButton.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		self.infoButton.SetPosition(40, 190)
		self.infoButton.Show()
		self.infoButton.SetToolTipText("Ativa o ataque automático por proximidade.", 0 , 30, TOOLTIP_COLOR)
		
		
		# self.tooltip = ui.TextToolTip("Ativa o ataque automático por proximidade.")
		# self.tooltip.SetText("Ativa o ataque automático por proximidade.")
		# self.tooltip.Hide()
		
		# self.autoAttackToggle.SetOverEvent(ui.__mem_func__(self.ShowTooltip))
		# self.autoAttackToggle.SetOverOutEvent(ui.__mem_func__(self.HideTooltip))
		
	def ShowTooltip(self):
		self.tooltip.Show()

	def HideTooltip(self):
		self.tooltip.Hide()
		
	def OnAutoAttackOff(self):
		# Inverte o estado booleano (True vira False, False vira True)
		self.is_active = not self.is_active
		if self.is_active:
			vnum = 72724
			if player.GetItemCountByVnum(vnum) > 0:
				self.autoAttackToggle.Down() 
				self.autoAttackToggle.SetText("ATIVADO")
				chat.AppendChat(chat.CHAT_TYPE_INFO, "[Sistema] Função ativada!")
				player.StartMetinProximity(1000.0)
			else:
				chat.AppendChat(3, "O jogador não possui o item.")
				self.autoAttackToggle.SetUp() 
				self.autoAttackToggle.SetText("Desativado")
				chat.AppendChat(3, "[Sistema] Função desativada!")
				self.is_active = not self.is_active
		else:
			# Estado: DESATIVADO
			# Retorna o botão ao estado normal "solto"
			self.autoAttackToggle.SetUp() 
			self.autoAttackToggle.SetText("Desativado")
			chat.AppendChat(chat.CHAT_TYPE_INFO, "[Sistema] Função desativada!")
			player.StopMetinProximity()
			# (Opcional) Restaurar imagem padrão
			# self.ToggleButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		
		
	def AppendMetinQueue(self):
		lista = player.GetMetinQueue()
		for pos, vid in enumerate(lista, 1):
			nome = chr.GetNameByVID(vid)
			if not nome:
				nome = "Metin desconhecida"
			self.bg.AutoAppendTextLine( "%d. %s" % (pos, nome))
		
	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()
		
	def OnUpdate(self):
		if not self.IsShow():
			return
		now = app.GetGlobalTime()
		if now < self.refreshTime:
			return
		self.refreshTime = now + 0.2
		self.Refresh()
		
	def Refresh(self):
		if not self.IsShow():
			return
		lista = player.GetMetinQueue()
		for i, vid in enumerate(lista):
			if i >= len(self.bg.childrenList):
				textLine = self.bg.AutoAppendTextLine("")
			else:
				textLine = self.bg.childrenList[i]
			nome = chr.GetNameByVID(vid)
			textLine.SetText("%d. %s" % (i + 1, nome))
			textLine.Show()

		for i in range(len(lista), len(self.bg.childrenList)):
			self.bg.childrenList[i].Hide()
			
	def ClearTextLines(self):
		for textLine in self.childrenList:
			textLine.Hide()

		self.childrenList = []
		self.textY = 0