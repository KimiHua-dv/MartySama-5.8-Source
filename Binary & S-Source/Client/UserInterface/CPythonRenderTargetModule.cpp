#include "StdAfx.h"
#include "PythonApplication.h"
#include "Locale_inc.h"

PyObject* renderTargetSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SelectModel(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetChangeWeapon(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeWeapon(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetChangeArmor(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeArmor(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetChangeHair(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeHair(modelIndex);

	return Py_BuildNone();
}
PyObject* renderTargetChangeAcce(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeAcce(modelIndex);

	return Py_BuildNone();
}

/* #ifdef ENABLE_AURA_SYSTEM
PyObject* renderTargetChangeAura(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeAura(modelIndex);

	return Py_BuildNone();
}
#endif */

#ifdef ENABLE_SHINING_ITEMS
PyObject* renderTargetChangeWeaponShining(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeWeaponShining(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetChangeArmorShining(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeArmorShining(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetChangeSpecialShining(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeSpecialShining(modelIndex);

	return Py_BuildNone();
}
#endif

PyObject* renderTargetChangeEffect(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeEffect();

	return Py_BuildNone();
}

PyObject* renderTargetSetVisibility(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	bool isShow = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetVisibility(isShow);

	return Py_BuildNone();
}

PyObject* renderTargetSetBackground(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	char * szPathName;
	if (!PyTuple_GetString(poArgs, 1, &szPathName))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->CreateBackground(
		szPathName, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject* renderTargetSetZoom(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	bool bZoom = true;
	if (!PyTuple_GetBoolean(poArgs, 1, &bZoom))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetZoom(bZoom);

	return Py_BuildNone();
}

PyObject* renderGetFreeIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iMin;
	if (!PyTuple_GetInteger(poArgs, 0, &iMin))
		return Py_BadArgument();
	int iMax;
	if (!PyTuple_GetInteger(poArgs, 1, &iMax))
		return Py_BadArgument();
	for (int j = iMin; j < iMax; ++j)
	{
		if (!CRenderTargetManager::Instance().GetRenderTarget(j))
			return Py_BuildValue("i", j);
	}
	return Py_BuildValue("i", iMin + 20);
}

PyObject* renderTargetResetModel(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->ResetModel();
	return Py_BuildNone();
}

PyObject* renderTargetSetModelRotation(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float rotation;
	if (!PyTuple_GetFloat(poArgs, 1, &rotation))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target =
		CRenderTargetManager::Instance().GetRenderTarget(iIndex);

	if (target)
		target->SetModelRotation(rotation);

	return Py_BuildNone();
}

PyObject* renderTargetSetArmor(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->ChangeArmor(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject* renderTargetSetWeapon(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->ChangeWeapon(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject* renderTargetSetHair(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();

	bool isValue = false;

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		PyTuple_GetBoolean(poArgs, 2, &isValue);

		if (!isValue)
		{
			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(iItemVnum, &pItemData))
				return Py_BuildNone();
			iItemVnum = pItemData->GetValue(3);
		}

		if (target->IsReady())
			target->GetModel()->SetHair(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject* renderTargetSetAcce(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->SetAcce(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject* renderTargetSetMount(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->MountHorse(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject* renderTargetSetRotation(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	bool bRot;
	if (!PyTuple_GetBoolean(poArgs, 1, &bRot))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetRotation(bRot);
	return Py_BuildNone();
}


PyObject* renderSetModelV3Eye(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetModelV3Eye(x, y, z);
	return Py_BuildNone();
}

PyObject* renderSetModelV3Target(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetModelV3Target(x, y, z);
	return Py_BuildNone();
}

PyObject* renderZoomCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 1, &iDirection))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
		{
			const float fRatio = 1.0f + CPythonApplication::Instance().GetCameraZoomSpeed() * float(iDirection);
			target->Zoom(fRatio);
		}
	}
	return Py_BuildNone();
}

void initRenderTarget() {
	static PyMethodDef s_methods[] =
	{
		{ "SelectModel", renderTargetSelectModel, METH_VARARGS },
		{ "SetVisibility", renderTargetSetVisibility, METH_VARARGS },
		{"SetBackground", renderTargetSetBackground, METH_VARARGS},
		{"SetArmor", renderTargetSetArmor, METH_VARARGS},
		{"SetWeapon", renderTargetSetWeapon, METH_VARARGS},
		{"SetHair", renderTargetSetHair, METH_VARARGS},
		{"SetRotation", renderTargetSetRotation, METH_VARARGS},
		{"SetAcce", renderTargetSetAcce, METH_VARARGS},
		{"SetMount", renderTargetSetMount, METH_VARARGS},
		{ "ChangeArmor", renderTargetChangeArmor, METH_VARARGS },
		{ "ChangeWeapon", renderTargetChangeWeapon, METH_VARARGS },
		{ "ChangeHair", renderTargetChangeHair, METH_VARARGS },
		{ "ChangeAcce", renderTargetChangeAcce, METH_VARARGS },
		{ "ChangeEffect", renderTargetChangeEffect, METH_VARARGS },
		{ "SetZoom", renderTargetSetZoom, METH_VARARGS},

#ifdef ENABLE_SHINING_ITEMS
		{ "ChangeWeaponShining", renderTargetChangeWeaponShining, METH_VARARGS },
		{ "ChangeArmorShining", renderTargetChangeArmorShining, METH_VARARGS },
		{ "ChangeSpecialShining", renderTargetChangeSpecialShining, METH_VARARGS },
#endif
		{"GetFreeIndex", renderGetFreeIndex, METH_VARARGS },
		{"ResetModel", renderTargetResetModel, METH_VARARGS},
		{"SetModelRotation", renderTargetSetModelRotation, METH_VARARGS},
		{"SetModelV3Eye", renderSetModelV3Eye, METH_VARARGS },
		{"SetModelV3Target", renderSetModelV3Target, METH_VARARGS },
		{"Zoom", renderZoomCamera, METH_VARARGS },

		{nullptr, nullptr, 0 },
	};

	PyObject* poModule = Py_InitModule("renderTarget", s_methods);

}