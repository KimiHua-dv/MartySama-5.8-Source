import ui, chat, guild, net

FESTIVAL_LIST_ACTIVE = 0
FESTIVAL_LIST_UNAVAILABLE = 1

class Festival(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.missions = []
		self.missionItems = []
		self.TotalPoints = 0
		self.RewardProgress = 0
		self.PendingPoints = 0
		self.ClaimedChests = 0
		self.maxFestivalPoints = 2000
		self.listStatus = FESTIVAL_LIST_ACTIVE
		self.PendingChestCounts = [0, 0, 0, 0, 0]
		
		self.LoadWindow()

	def LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/festival.py")

		self.board = self.GetChild("board")
		self.painel_missoes = self.GetChild("painel_central")
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.mission = self.GetChild("btn_m")
		self.mission.SetEvent(ui.__mem_func__(self.ReloadMissionVisuals))
		self.guildName = self.GetChild("Nome")
		self.guildLevel = self.GetChild("Nivel")
		self.guildPoint = self.GetChild("Pontos")
		self.painel_progresso = self.GetChild("painel_lateral_d_1")
		self.painel_MM = self.GetChild("painel_lateral_d_3")
		self.painel_reconpensa = self.GetChild("painel_central_baixo")
		self.btn_resgate = self.GetChild("btn_resgate")
		self.btn_resgate.SetEvent(ui.__mem_func__(self.RewardChest))
		self.statusMessages = []

		messages = [
			"Festival de Guildas encerrado",
			"ou não está em execução.",
			"Aguarde a próxima edição do evento."
		]

		for index, message in enumerate(messages):
			panelWidth = 372
			textLine = ui.TextLine()
			textLine.SetParent(self.painel_missoes)
			textLine.SetPosition(12, 140 + index * 20)
			textLine.SetPosition(panelWidth / 2, 140 + index * 20)
			textLine.SetHorizontalAlignCenter()
			textLine.SetText(message)
			textLine.Hide()

			self.statusMessages.append(textLine)
		
		self.RefreshGuildInfos()
		self.CreateGuildGauge()
		self.CreateMissionGauge()
		self.CreateRewardGauge()

	def SetFestivalListStatus(self, status):
		self.listStatus = int(status)

		# Limpar missoes antigas ao receber qualquer novo estado.
		self.missions = []

		self.RefreshMissionList()
	
	def RewardChest(self):
		net.SendFestivalClaimPending()
		
	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

	def ClearFestivalMissions(self):
		self.missions = []

		self.RefreshMissionList()

	def ReloadMissionVisuals(self):
		chat.AppendChat(3, "Reload")
		self.RefreshMissionList()
	
	def RefreshGuildInfos(self):
		guild_name = guild.GetGuildName()
		guild_level = guild.GetGuildLevel()
		guild_points = self.TotalPoints
		if not guild_name:
			guild_name = "Sem guilda"
			self.guildName.SetText("Guilda: %s" % guild_name)
			return
		self.guildName.SetText("Guilda: %s" % guild_name)
		self.guildLevel.SetText("Nivel: %s" % guild_level)
		self.guildPoint.SetText("Pontos: %s" % guild_points)
	
	def AppendFestivalMission(self, mission_id, title, description, points, mission_type, target_vnum, target_count, time_limit, status):
		mission = {
			"id": int(mission_id),
			"title": title,
			"description": description,
			"points": int(points),
			"type": int(mission_type),
			"target_vnum": int(target_vnum),
			"target_count": int(target_count),
			"time_limit": int(time_limit),
			"status": int(status)
		}

		self.missions.append(mission)

		self.RefreshMissionList()

	
	
	def AcceptMission(self, mission_id):
		mission_id = int(mission_id)
		if mission_id <= 0:
			return

		net.SendFestivalMissionAccept(mission_id)

	def RefreshMissionList(self):
		# Ocultar os cartões criados anteriormente.
		for item in self.missionItems:
			item.Hide()

		self.missionItems = []
		
		# Ocultar mensagens anteriores.
		for message in self.statusMessages:
			message.Hide()

		# Evento encerrado ou em processamento.
		if self.listStatus == FESTIVAL_LIST_UNAVAILABLE:
			for message in self.statusMessages:
				message.Show()

			return

		# Evento ativo, mas sem missoes.
		if not self.missions:
			return

		panelWidth = self.painel_missoes.GetWidth()

		# A barra "Missões" ocupa o topo do painel.
		startY = 30
		marginX = 8
		rowHeight = 51
		spacing = 5

		maxVisible = 5

		for index, mission in enumerate(self.missions[:maxVisible]):
			card = ui.ThinBoardCircle()
			card.SetParent(self.painel_missoes)
			card.SetPosition(marginX, startY + index * (rowHeight + spacing))
			card.SetSize(panelWidth - marginX * 2, rowHeight)
			card.Show()

			self.missionItems.append(card)
			
			board = ui.ThinBoardCircle()
			board.SetParent(card)
			board.SetSize(42, 42)
			board.SetPosition(5, 5)
			board.Show()
			
			image = ui.ExpandedImageBox()
			image.SetParent(board)
			image.SetPosition(1,1)
			image_size = (40, 40)
			
			tipo = mission["type"]
			if tipo == 1:
				objetivo = "Metins"
				image.LoadImage("bl_teleport/mobs/8003.tga")
			elif tipo == 2:
				objetivo = "Monstros"
				image.LoadImage("bl_teleport/mobs/2414.tga")
			elif tipo == 3:
				objetivo = "PVP"
				image.LoadImage("bl_teleport/mobs/pvp.tga")
			elif tipo == 4:
				objetivo = "Coletar Itens"
				image.LoadImage("icon/auto_1.tga")
			else:
				objetivo = "Desconhecido"
			image_width = image.GetWidth()
			image_height = image.GetHeight()
			if image_width > 0 and image_height > 0:
				escala_x = image_size[0] / float(image_width)
				escala_y = image_size[1] / float(image_height)
				image.SetScale(escala_x, escala_y)
			image.Show()
				
			# Título da missão.
			title = ui.TextLine()
			title.SetParent(card)
			title.SetPosition(62, 4)
			title.SetText(mission["title"])
			title.SetLimitWidth(220)
			title.Show()

			# Descrição da missão.
			description = ui.TextLine()
			description.SetParent(card)
			description.SetPosition(62, 19)
			description.SetText(mission["description"])
			description.SetLimitWidth(220)
			description.Show()

				
						# Pontos e quantidade necessária.
			info = ui.TextLine()
			info.SetParent(card)
			info.SetPosition(62, 34)
			info.SetText(
				"%d pontos  |  %s: %d  |  Tempo: %d min" % (
					mission["points"],
					objetivo,
					mission["target_count"],
					mission["time_limit"] // 60
				)
			)
			info.SetLimitWidth(220)
			info.Show()

			# Botao de aceitar apenas para missoes ainda disponiveis.
			if mission["status"] == 0:
				acceptButton = ui.Button()
				acceptButton.SetParent(card)
				acceptButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
				acceptButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
				acceptButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
				acceptButton.SetEvent(ui.__mem_func__(self.AcceptMission), mission["id"])
				acceptButton.SetPosition(card.GetWidth() - 58, 15)
				acceptButton.SetText("Aceitar")
				acceptButton.Show()
				self.missionItems.append(acceptButton)
			else:
				statusText = ui.TextLine()
				statusText.SetParent(card)
				statusText.SetPosition(card.GetWidth() - 62, 20)
				statusText.SetLimitWidth(58)
				statusText.SetText({1: "Em curso", 2: "Feita", 3: "Falhou"}.get(mission["status"], "Indisp."))
				statusText.Show()
				self.missionItems.append(statusText)

			# Manter referencias aos elementos de UI enquanto estiverem ativos.
			# Inclui o botao ou o estado de cada missao.
			self.missionItems.append(image)
			self.missionItems.append(board)
			self.missionItems.append(title)
			self.missionItems.append(description)
			self.missionItems.append(info)

	def CreateGuildGauge(self):
		self.guildGauge = ui.Gauge()
		self.guildGauge.SetParent(self.painel_progresso)
		self.guildGauge.MakeGauge(100, "blue")
		self.guildGauge.SetPosition(10, 40)
		self.guildGauge.Show()
		
		self.text_gaude = ui.TextLine()
		self.text_gaude.SetParent(self.painel_progresso)
		self.text_gaude.SetPosition(20, 50)
		self.text_gaude.SetText("Pontos : %d/%d" % (self.TotalPoints,self.maxFestivalPoints))
		self.text_gaude.Show()
		
		self.slot = ui.ExpandedImageBox()
		self.slot.SetParent(self.painel_progresso)
		self.slot.SetPosition(110, 25)
		self.slot.LoadImage("festival/slot.tga")
		self.slot.Show()
		self.RefreshGuildProgress()
		
	def RefreshGuildProgress(self):
		self.guildGauge.SetPercentage(self.TotalPoints,self.maxFestivalPoints)
		self.text_gaude.SetText("Pontos : %d/%d" % (self.TotalPoints,self.maxFestivalPoints))
		
	def CreateMissionGauge(self):
		self.MissionGauge = ui.Gauge()
		self.MissionGauge.SetParent(self.painel_MM)
		self.MissionGauge.MakeGauge(130, "purple")
		self.MissionGauge.SetPosition(10, 40)
		self.MissionGauge.SetPercentage(1, 2)
		self.MissionGauge.Show()
		
		self.textMission = ui.TextLine()
		self.textMission.SetParent(self.painel_MM)
		self.textMission.SetPosition(40, 50)
		self.textMission.SetText("Missões : 1/2")
		self.textMission.Show()
		
	def CreateRewardGauge(self):
		self.RewardGauge = ui.Gauge()
		self.RewardGauge.SetParent(self.painel_reconpensa)
		self.RewardGauge.MakeGauge(350, "red")
		self.RewardGauge.SetPosition(10, 40)
		self.RewardGauge.Show()

		# TESTE TEMPORÁRIO: metade da barra
		self.UpdateRewardGauge()
		self.SetMilestone()

	def UpdateGuildState(self,total_points,reward_progress,pending_points,claimed_chests,pending_200,pending_500,pending_1000,pending_1500,pending_2000):
		self.TotalPoints = int(total_points)
		self.RewardProgress = int(reward_progress)
		self.PendingPoints = int(pending_points)
		self.ClaimedChests = int(claimed_chests)

		self.PendingChestCounts = [
			int(pending_200),
			int(pending_500),
			int(pending_1000),
			int(pending_1500),
			int(pending_2000)
		]

		self.RefreshGuildInfos()
		self.RefreshGuildProgress()
		self.UpdateRewardGauge()
		self.UpdateMileStone()
		
	def AddTestPoints(self, amount):
		self.RewardProgress += amount
		self.RewardProgress = min(self.RewardProgress, self.maxFestivalPoints)
		self.UpdateRewardGauge()
		self.UpdateMileStone()
		self.RefreshGuildProgress()
		self.RefreshGuildInfos()
	
	def UpdateRewardGauge(self):
		 self.RewardGauge.SetPercentage(self.TotalPoints,self.maxFestivalPoints)
		
	def UpdateMileStone(self):
		milestones = [
			(200, self.rec_1),
			(500, self.rec_2),
			(1000, self.rec_3),
			(1500, self.rec_4),
			(2000, self.rec_5)
		]

		for index, (points, image) in enumerate(milestones):

			pending = self.PendingChestCounts[index]

			if pending > 0:
				# Existem recompensas aguardando resgate.
				image.LoadImage(
					"festival/rec_%d_1.tga" % (index + 1)
				)

				self.chestStatus[index].SetText(str(pending))

			else:
				# Sem recompensas pendentes: volta ao estado bloqueado.
				image.LoadImage(
					"festival/rec_%d.tga" % (index + 1)
				)

				self.chestStatus[index].SetText("")
			
	def SetMilestone(self):
		self.rec_1 = ui.ExpandedImageBox()
		self.rec_1.SetParent(self.painel_reconpensa)
		self.rec_1.SetPosition(50, 30)
		self.rec_1.LoadImage("festival/rec_1.tga")
		self.rec_1.Show()
		
		self.text = ui.TextLine()
		self.text.SetParent(self.painel_reconpensa)
		self.text.SetPosition(58, 60)
		self.text.SetText("200")
		self.text.Show()
		
		self.rec_2 = ui.ExpandedImageBox()
		self.rec_2.SetParent(self.painel_reconpensa)
		self.rec_2.SetPosition(103, 30)
		self.rec_2.LoadImage("festival/rec_2.tga")
		self.rec_2.Show()
		
		self.textrec_2 = ui.TextLine()
		self.textrec_2.SetParent(self.painel_reconpensa)
		self.textrec_2.SetPosition(111, 60)
		self.textrec_2.SetText("500")
		self.textrec_2.Show()
		
		self.rec_3 = ui.ExpandedImageBox()
		self.rec_3.SetParent(self.painel_reconpensa)
		self.rec_3.SetPosition(185, 30)
		self.rec_3.LoadImage("festival/rec_3.tga")
		self.rec_3.Show()
		
		self.textrec_3 = ui.TextLine()
		self.textrec_3.SetParent(self.painel_reconpensa)
		self.textrec_3.SetPosition(192, 60)
		self.textrec_3.SetText("1000")
		self.textrec_3.Show()
		
		self.rec_4 = ui.ExpandedImageBox()
		self.rec_4.SetParent(self.painel_reconpensa)
		self.rec_4.SetPosition(265, 30)
		self.rec_4.LoadImage("festival/rec_4.tga")
		self.rec_4.Show()
		
		self.textrec_4 = ui.TextLine()
		self.textrec_4.SetParent(self.painel_reconpensa)
		self.textrec_4.SetPosition(272, 60)
		self.textrec_4.SetText("1500")
		self.textrec_4.Show()
		
		self.rec_5 = ui.ExpandedImageBox()
		self.rec_5.SetParent(self.painel_reconpensa)
		self.rec_5.SetPosition(335, 30)
		self.rec_5.LoadImage("festival/rec_5.tga")
		self.rec_5.Show()
		
		self.textrec_5 = ui.TextLine()
		self.textrec_5.SetParent(self.painel_reconpensa)
		self.textrec_5.SetPosition(343, 60)
		self.textrec_5.SetText("2000")
		self.textrec_5.Show()
		
		self.chestStatus = []

		chestPositions = [50, 103, 185, 265, 335]

		for x in chestPositions:
			status = ui.TextLine()
			status.SetParent(self.painel_reconpensa)

			status.SetPosition(x + 25, 49)
			status.SetHorizontalAlignCenter()
			status.SetFontColor(255,0,0)
			status.SetText("")
			status.Show()

			self.chestStatus.append(status)

		self.UpdateMileStone()
		
