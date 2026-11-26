import app, ui, chat, guild, net, uitooltip

FESTIVAL_LIST_ACTIVE = 0
FESTIVAL_LIST_UNAVAILABLE = 1


class FestivalMissionItem(ui.ListBoxEx.Item):
	def __init__(self, festival, mission):
		ui.ListBoxEx.Item.__init__(self)
		self.children = []

		self.card = ui.ThinBoardCircle()
		self.card.SetParent(self)
		self.card.SetPosition(0, 0)
		self.card.SetSize(332, 88)
		self.card.Show()

		iconBoard = ui.ThinBoardCircle()
		iconBoard.SetParent(self.card)
		iconBoard.SetSize(42, 42)
		iconBoard.SetPosition(5, 22)
		iconBoard.Show()
		self.children.append(iconBoard)

		image = ui.ExpandedImageBox()
		image.SetParent(iconBoard)
		image.SetPosition(1, 1)
		missionType = mission["type"]
		if missionType == 1:
			image.LoadImage("bl_teleport/mobs/8003.tga")
		elif missionType == 2:
			image.LoadImage("bl_teleport/mobs/2414.tga")
		elif missionType == 3:
			image.LoadImage("bl_teleport/mobs/pvp.tga")
		elif missionType == 4:
			image.LoadImage("icon/auto_1.tga")
		iw, ih = image.GetWidth(), image.GetHeight()
		if iw > 0 and ih > 0:
			image.SetScale(40.0 / iw, 40.0 / ih)
		image.Show()
		self.children.append(image)

		def AddText(y, value, width=195):
			line = ui.TextLine()
			line.SetParent(self.card)
			line.SetPosition(55, y)
			line.SetText(value)
			line.SetLimitWidth(width)
			line.Show()
			self.children.append(line)
			return line

		AddText(5, mission["title"])
		AddText(20, mission["description"])
		AddText(36, "%d pontos | Objetivo: %d | Limite: %d min" % (
			mission["points"], mission["target_count"], mission["time_limit"] // 60
		))

		self.progress = int(mission["progress"])
		self.targetCount = int(mission["target_count"])
		self.hasCountdown = (mission["status"] == 1)
		self.endTime = app.GetTime() + max(0, int(mission["remaining_time"]))

		self.progressTimerText = AddText(52, "")
		self.lastDisplayedSecond = None
		self.UpdateTimer(app.GetTime())

		playerName = mission["player_name"] or ""
		if mission["is_owner"]:
			responsible = "Voce"
		elif playerName:
			responsible = playerName
		else:
			responsible = "Nao atribuida"
		AddText(68, "Responsavel: %s" % responsible)

		statusLabels = {0: "Disponivel", 1: "Em curso", 2: "Concluida", 3: "Falhou"}
		status = ui.TextLine()
		status.SetParent(self.card)
		status.SetPosition(258, 7)
		status.SetHorizontalAlignCenter()
		status.SetLimitWidth(68)
		status.SetText(statusLabels.get(mission["status"], "Indisp."))
		status.Show()
		self.children.append(status)

		if mission["status"] == 0:
			self.acceptButton = ui.Button()
			self.acceptButton.SetParent(self.card)
			self.acceptButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
			self.acceptButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
			self.acceptButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
			self.acceptButton.SetEvent(ui.__mem_func__(festival.AcceptMission), mission["id"])
			self.acceptButton.SetPosition(263, 38)
			self.acceptButton.SetText("Aceitar")
			self.acceptButton.Show()
			self.children.append(self.acceptButton)


	def UpdateTimer(self, currentTime):
		if self.hasCountdown:
			remaining = max(0, int(self.endTime - currentTime))
			if remaining == self.lastDisplayedSecond:
				return
			self.lastDisplayedSecond = remaining
			remainingText = "%02d:%02d" % (remaining // 60, remaining % 60)
		else:
			if self.lastDisplayedSecond == -1:
				return
			self.lastDisplayedSecond = -1
			remainingText = "--:--"

		self.progressTimerText.SetText("Progresso: %d/%d | Restante: %s" % (
			self.progress, self.targetCount, remainingText
		))


class Festival(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.missions = []
		self.missionItems = []

		self.TotalPoints = 0
		self.RewardProgress = 0
		self.PendingPoints = 0
		self.ClaimedChests = 0
		self.AcceptedMissions = 0

		self.maxFestivalPoints = 2000
		self.listStatus = FESTIVAL_LIST_ACTIVE

		self.PendingChestCounts = [0, 0, 0, 0, 0]

		self.LoadWindow()

	def LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/festival.py")

		self.board = self.GetChild("board")
		self.painel_missoes = self.GetChild("painel_central")
		# Reutilizar a barra declarada no uiscript/festival.py.
		self.missionScrollBar = self.GetChild("mission_scrollbar")
		self.missionListBox = ui.ListBoxEx()
		self.missionListBox.SetParent(self.painel_missoes)
		self.missionListBox.SetPosition(8, 28)
		self.missionListBox.SetViewItemCount(3)
		self.missionListBox.SetItemStep(92)
		self.missionListBox.SetItemSize(332, 88)
		self.missionListBox.SetScrollBar(self.missionScrollBar)
		self.missionListBox.Show()
		self.missionScrollBar.Hide()
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
		if hasattr(self, "chestToolTip"):
			self.chestToolTip.HideToolTip()
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
	
	def AppendFestivalMission(self, mission_id, title, description, points,mission_type, target_vnum, target_count,time_limit, status, progress, remaining_time,player_name, is_owner):
		mission = {
			"id": int(mission_id),
			"title": title,
			"description": description,
			"points": int(points),
			"type": int(mission_type),
			"target_vnum": int(target_vnum),
			"target_count": int(target_count),
			"time_limit": int(time_limit),
			"status": int(status),
			"progress": int(progress),
			"remaining_time": int(remaining_time),
			"player_name": player_name,
			"is_owner": int(is_owner)
		}

		self.missions.append(mission)
		chat.AppendChat(3, "[Festival] Missao recebida: %d | total: %d" % (
			mission["id"], len(self.missions)
		))
		self.RefreshMissionList()

	
	
	def AcceptMission(self, mission_id):
		mission_id = int(mission_id)
		if mission_id <= 0:
			return

		net.SendFestivalMissionAccept(mission_id)



	def RefreshMissionList(self):
		for message in self.statusMessages:
			message.Hide()

		self.missionListBox.RemoveAllItems()
		self.missionItems = []

		if self.listStatus == FESTIVAL_LIST_UNAVAILABLE:
			self.missionScrollBar.Hide()
			for message in self.statusMessages:
				message.Show()
			return

		if not self.missions:
			self.missionScrollBar.Hide()
			return

		for mission in self.missions:
			item = FestivalMissionItem(self, mission)
			self.missionListBox.AppendItem(item)
			self.missionItems.append(item)

		if len(self.missions) > 3:
			self.missionScrollBar.SetMiddleBarSize(
				min(1.0, 3.0 / float(len(self.missions)))
			)
			self.missionScrollBar.Show()
		else:
			self.missionScrollBar.Hide()

		self.missionScrollBar.SetPos(0)

	def OnUpdate(self):
		currentTime = app.GetTime()
		for missionItem in self.missionItems:
			missionItem.UpdateTimer(currentTime)

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
		totalPoints = int(self.TotalPoints)

		# Progresso do ciclo atual.
		progress = totalPoints % self.maxFestivalPoints

		# Em um multiplo exato, mantem a barra cheia.
		if totalPoints > 0 and progress == 0:
			progress = self.maxFestivalPoints
		self.guildGauge.SetPercentage(progress,self.maxFestivalPoints)
		self.text_gaude.SetText("Pontos : %d/%d" % (progress,self.maxFestivalPoints))
		
	def CreateMissionGauge(self):
		self.MissionGauge = ui.Gauge()
		self.MissionGauge.SetParent(self.painel_MM)
		self.MissionGauge.MakeGauge(130, "purple")
		self.MissionGauge.SetPosition(10, 40)
		self.MissionGauge.SetPercentage(self.AcceptedMissions,2)
		self.MissionGauge.Show()

		self.textMission = ui.TextLine()
		self.textMission.SetParent(self.painel_MM)
		self.textMission.SetPosition(40, 50)
		self.textMission.SetText("Missões : %d/2" % self.AcceptedMissions)
		self.textMission.Show()
		
	def UpdateMemberMissionGauge(self):
		self.MissionGauge.SetPercentage(self.AcceptedMissions,2)
		self.textMission.SetText("Missões : %d/2" % self.AcceptedMissions)
		
	def CreateRewardGauge(self):
		self.RewardGauge = ui.Gauge()
		self.RewardGauge.SetParent(self.painel_reconpensa)
		self.RewardGauge.MakeGauge(350, "red")
		self.RewardGauge.SetPosition(10, 40)
		self.RewardGauge.Show()

		# TESTE TEMPORÁRIO: metade da barra
		self.UpdateRewardGauge()
		self.SetMilestone()

	def UpdateGuildState(self,total_points,reward_progress,pending_points,claimed_chests,pending_200,pending_500,pending_1000,pending_1500,pending_2000,accepted_missions):
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
		
		self.AcceptedMissions = int(accepted_missions)
		
		self.RefreshGuildInfos()
		self.RefreshGuildProgress()
		self.UpdateRewardGauge()
		self.UpdateMileStone()
		self.UpdateMemberMissionGauge()
		
	def AddTestPoints(self, amount):
		self.RewardProgress += amount
		self.RewardProgress = min(self.RewardProgress, self.maxFestivalPoints)
		self.UpdateRewardGauge()
		self.UpdateMileStone()
		self.RefreshGuildProgress()
		self.RefreshGuildInfos()
	
	def UpdateRewardGauge(self):
		totalPoints = int(self.TotalPoints)

		# Progresso do ciclo atual.
		progress = totalPoints % self.maxFestivalPoints

		# Em um multiplo exato, mantem a barra cheia.
		if totalPoints > 0 and progress == 0:
			progress = self.maxFestivalPoints

		self.RewardGauge.SetPercentage(progress,self.maxFestivalPoints)
		
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
		self.chestToolTip = uitooltip.ToolTip()
		self.chestToolTip.HideToolTip()

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

			status.SetPosition(x + 25, 48)
			status.SetHorizontalAlignCenter()
			status.SetText("")
			status.SetFontColor(1.0, 0.25, 0.25)
			status.SetOutline()
			status.Show()

			self.chestStatus.append(status)
			
		chestImages = [
			self.rec_1,
			self.rec_2,
			self.rec_3,
			self.rec_4,
			self.rec_5
		]

		chestPoints = [200, 500, 1000, 1500, 2000]

		for index, image in enumerate(chestImages):
			image.SAFE_SetStringEvent("MOUSE_OVER_IN",self.ShowChestToolTip,chestPoints[index])
			image.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.HideChestToolTip)

		self.UpdateMileStone()

		self.UpdateMileStone()
		
	def ShowChestToolTip(self, points):
		self.chestToolTip.ClearToolTip()
		self.chestToolTip.toolTipWidth = 250
		self.chestToolTip.SetTitle("Bau de recompensa - %d pontos" % points)
		self.chestToolTip.AppendTextLine("Os numeros indicam a quantidade",0xffffffff)
		self.chestToolTip.AppendTextLine("de baus disponiveis para resgate.",0xffffffff)
		self.chestToolTip.AppendTextLine("Confira as recompensas na aba",0xffffffff)
		self.chestToolTip.AppendTextLine("Recompensas.",0xffffffff)
		self.chestToolTip.ShowToolTip()

	def HideChestToolTip(self):
		self.chestToolTip.HideToolTip()