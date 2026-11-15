#author: .syserr {GOD - metin2.forum} | DISCORD: .syserr

#static
import ui, localeInfo, constInfo, uiCommon

#dynamic
import grp, renderTarget, app, player, skill, net

SKILL_GRADE = 3

SKILL_COUNT = 5

class Window(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()
	def Destroy(self):
		self.__children = {}
	def __LoadWindow(self):
		self.Destroy()
		
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		self.SetTitleName("[TEST] Skill Training")
		self.SetCloseEvent(ui.__mem_func__(self.Close))

		(width, height) = (500, 400)
		
		global SKILL_COUNT
		SKILL_COUNT = 5 if player.GetJob() <= 1 else 6

		for j in xrange(2):
			groupBg = CreateWindow(ui.ThinBoardCircle(), self, (8 if j == 0 else width - (100+8), 30), "", "", (100, height - 38))
			groupBg.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.4))
			groupBg.SetMouseLeftButtonUpEvent(lambda arg="", j=j: self.__Refresh(arg, j))
			self.__children["groupBg"+str(j)] = groupBg

			groupName = CreateWindow(ui.TextLine(), groupBg, (0, 0), "Test")
			groupName.SetPosition((groupBg.GetWidth()/2) - (groupName.GetTextSize()[0]/2), 20)
			self.__children["groupName"+str(j)] = groupName

			skillBg = CreateWindow(ui.ThinBoardCircle(), groupBg, (0, 0), "", "", (45, (SKILL_COUNT * 36) + 20))
			skillBg.SetPosition((groupBg.GetWidth()/2) - (skillBg.GetWidth() / 2), (groupBg.GetHeight()/2) - (skillBg.GetHeight() / 2))
			skillBg.SetMouseLeftButtonUpEvent(lambda arg="", j=j: self.__Refresh(arg, j))
			self.__children["skillBg"+str(j)] = skillBg

			slotWindow = CreateWindow(ui.SlotWindow(), skillBg, ((skillBg.GetWidth()/2) - 16, 15), "", "", (32, SKILL_COUNT * 36))
			for i in xrange(SKILL_COUNT):
				slotWindow.AppendSlot(i, 0, i * 36, 32, 32)
			if j == 0:
				slotWindow.SetOverInItemEvent(ui.__mem_func__(self.__OverInFirst))
				slotWindow.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectSkillFirst))
				slotWindow.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectSkillFirst))
				slotWindow.SetUseSlotEvent(ui.__mem_func__(self.__SelectSkillFirst))
			else:
				slotWindow.SetOverInItemEvent(ui.__mem_func__(self.__OverInSecond))
				slotWindow.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectSkillSecond))
				slotWindow.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectSkillSecond))
				slotWindow.SetUseSlotEvent(ui.__mem_func__(self.__SelectSkillSecond))
			slotWindow.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))
			
			self.__children["slotWindow"+str(j)] = slotWindow

		self.SetSize(width, height)
		self.SetCenterPosition()

		renderIndex = renderTarget.GetFreeIndex(100, 200)

		renderWnd = CreateWindow(ui.RenderTarget(), self, (110, 30), "", "", (self.GetWidth() - (16 + 200 + 4), self.GetHeight() - (38)))
		renderWnd.SetRenderTarget(renderIndex)
		self.__children["renderWnd"] = renderWnd

		renderTarget.SetBackground(renderIndex, "d:/ymir work/ui/game/render/preview_back.tga")

		self.__children["renderIndex"] = renderIndex

		selectBtn = CreateWindow(ui.Button(), self, (0, 0))
		selectBtn.SetUpVisual("d:/ymir work/ui/public/Middle_Button_01.sub")
		selectBtn.SetOverVisual("d:/ymir work/ui/public/Middle_Button_02.sub")
		selectBtn.SetDownVisual("d:/ymir work/ui/public/Middle_Button_03.sub")
		selectBtn.SetPosition((width/2) - (selectBtn.GetWidth()/2), height - 50)
		selectBtn.SetText("Select")
		self.__children["selectBtn"] = selectBtn

	def __Refresh(self, emptyArg, idx, skillIdx = 0):
		if self.__children.has_key("idx"):
			if  self.__children["idx"] == idx and skillIdx == 0:
				return
		self.__children["idx"] = idx
		
		renderIndex = self.__children["renderIndex"]

		playerRace = player.GetRace()
		playerJob = player.GetJob()

		weaponList = [469, [1349, 2379], 479, [5349, 7379]]
		weaponIdx = 0
		if playerJob == 1:#ninja {syserrmetin2.forum}
			weaponIdx = weaponList[playerJob][idx]
		elif playerJob == 3:#shaman
			weaponIdx = weaponList[playerJob][app.GetRandom(0, 1)]
		else:
			weaponIdx = weaponList[playerJob]

		renderTarget.ResetModel(renderIndex)
		renderTarget.SelectModel(renderIndex, playerRace)
		renderTarget.SetVisibility(renderIndex, True)
		renderTarget.SetWeapon(renderIndex, weaponIdx)
		renderTarget.SetHair(renderIndex, 1)
		renderTarget.SetAcce(renderIndex, 0)
		if playerJob == 0:
			renderTarget.SetArmor(renderIndex, 11209)
		elif playerJob == 1:
			renderTarget.SetArmor(renderIndex, 11409)
		elif playerJob == 1:
			renderTarget.SetArmor(renderIndex, 11609)
		elif playerJob == 1:
			renderTarget.SetArmor(renderIndex, 11809)
			
		renderTarget.SetModelV3Eye(renderIndex, 0.0, -1000.0, 600.0)
		renderTarget.Zoom(renderIndex, 20)

		SKILL_GROUP_NAME_DICT = {
			0	: { 1 : localeInfo.SKILL_GROUP_WARRIOR_1,	2 : localeInfo.SKILL_GROUP_WARRIOR_2, },
			1	: { 1 : localeInfo.SKILL_GROUP_ASSASSIN_1,	2 : localeInfo.SKILL_GROUP_ASSASSIN_2, },
			2	: { 1 : localeInfo.SKILL_GROUP_SURA_1,		2 : localeInfo.SKILL_GROUP_SURA_2, },
			3	: { 1 : localeInfo.SKILL_GROUP_SHAMAN_1,	2 : localeInfo.SKILL_GROUP_SHAMAN_2, },
		}
		skillStart = 1 + (playerJob * 30)
		global SKILL_COUNT
		for j in xrange(2):
			self.__children["groupName"+str(j)].SetPackedFontColor(0xFF00FF21 if j == idx else 0xFFFF0000)
			self.__children["groupName"+str(j)].SetText(SKILL_GROUP_NAME_DICT[playerJob][j + 1])
			self.__children["groupBg"+str(j)].SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 1.0 if j == idx else 0.5))
			self.__children["skillBg"+str(j)].SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 1.0 if j == idx else 0.5))
			for i in xrange(SKILL_COUNT):
				self.__children["slotWindow"+str(j)].SetSkillSlotNew(i, skillStart + i + (15 if j == 1 else 0), SKILL_GRADE, 40)
				if idx == j:
					self.__children["slotWindow"+str(j)].EnableSlot(i)
					self.__children["slotWindow"+str(j)].SetCoverButton(i)
				else:
					self.__children["slotWindow"+str(j)].DisableSlot(i)
			self.__children["slotWindow"+str(j)].RefreshSlot()

		self.__children["selectBtn"].SAFE_SetEvent(self.__Click, SKILL_GROUP_NAME_DICT[playerJob][idx + 1], idx + 1)

		if skillIdx:
			renderTarget.UseSkill(renderIndex, skillIdx) #syserr metin2.forum


	def __AcceptQuestionDialog(self):
		questionDialog = self.__children["questionDialog"] if self.__children.has_key("questionDialog") else None
		if questionDialog:
			net.SendChatPacket("/skill_training do "+str(questionDialog.idx))
			self.__CloseQuestionDialog()
			self.Close()

	def __CloseQuestionDialog(self):
		questionDialog = self.__children["questionDialog"] if self.__children.has_key("questionDialog") else None
		if questionDialog:
			questionDialog.Close()
			questionDialog.Destroy()
			del self.__children["questionDialog"]

	def __Click(self, name, idx):
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText("Você quer selecionar %s?" % name)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__AcceptQuestionDialog))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
		questionDialog.Open()
		questionDialog.idx = idx
		self.__children["questionDialog"] = questionDialog

	def __OverIn(self, slotIdx, isSec):
		skillIdx = 1 + (player.GetJob() * 30) + slotIdx + (15 if isSec else 0)
		interface = constInfo.GetInterfaceInstance()
		if interface:
			tooltipSkill = interface.tooltipSkill
			if tooltipSkill:
				tooltipSkill.ClearToolTip()
				tooltipSkill.AutoAppendNewTextLineResize(skill.GetSkillName(skillIdx, SKILL_GRADE))
				tooltipSkill.AppendSpace(5)
				try:
					tooltipSkill.AppendDescription(skill.GetSkillDescription(skillIdx), 25)
				except:
					pass
				tooltipSkill.AppendSkillConditionData(skillIdx)
				tooltipSkill.ShowToolTip()
	def __OverOut(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipSkill:
				interface.tooltipSkill.HideToolTip()
	def __OverInSecond(self, slotIdx):
		self.__OverIn(slotIdx, True)
	def __OverInFirst(self, slotIdx):
		self.__OverIn(slotIdx, False)
	def __SelectSkill(self, slotIdx, isSec):
		skillIdx = 1 + (player.GetJob() * 30) + slotIdx + (15 if isSec else 0)
		self.__Refresh("", self.__children["idx"], skillIdx)
	def __SelectSkillFirst(self, slotIdx):
		if self.__children["idx"] == 1:
			return
		self.__SelectSkill(slotIdx, False)
	def __SelectSkillSecond(self, slotIdx):
		if self.__children["idx"] == 0:
			return
		self.__SelectSkill(slotIdx, True)
	def OnUpdate(self):
		if self.__children.has_key("waitTime"):
			if self.__children["waitTime"] < app.GetTime():
				self.__Refresh("", self.__children["idx"])
	def Open(self):
		self.__Refresh("",0)
		self.Show()
		self.SetTop()
	def Close(self):
		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	window.SetPosition(*windowPos)
	if windowSize != (-1, -1):
		window.SetSize(*windowSize)
	if windowPositionRule:
		splitList = windowPositionRule.split(":")
		if len(splitList) == 2:
			(type, mode) = (splitList[0], splitList[1])
			if type == "horizontal":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetHorizontalAlignCenter()
					elif mode == "right":
						window.SetHorizontalAlignRight()
					elif mode == "left":
						window.SetHorizontalAlignLeft()
				else:
					if mode == "center":
						window.SetWindowHorizontalAlignCenter()
					elif mode == "right":
						window.SetWindowHorizontalAlignRight()
					elif mode == "left":
						window.SetWindowHorizontalAlignLeft()
			elif type == "vertical":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetVerticalAlignCenter()
					elif mode == "top":
						window.SetVerticalAlignTop()
					elif mode == "bottom":
						window.SetVerticalAlignBottom()
				else:
					if mode == "top":
						window.SetWindowVerticalAlignTop()
					elif mode == "center":
						window.SetWindowVerticalAlignCenter()
					elif mode == "bottom":
						window.SetWindowVerticalAlignBottom()
	if windowArgument:
		if isinstance(window, ui.TextLine):
			if windowFontName != -1:
				window.SetFontName(windowFontName)
			window.SetText(windowArgument)
		elif isinstance(window, ui.NumberLine):
			window.SetNumber(windowArgument)
		elif isinstance(window, ui.ExpandedImageBox) or isinstance(window, ui.ImageBox):
			window.LoadImage(windowArgument if windowArgument.find("gr2") == -1 else "icon/item/27995.tga")
	window.Show()
	return window
