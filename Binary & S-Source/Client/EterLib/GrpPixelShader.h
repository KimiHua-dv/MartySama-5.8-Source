#pragma once

#include "GrpBase.h"

class CPixelShader : public CGraphicBase
{
	public:
		CPixelShader();
		virtual ~CPixelShader();

		void Destroy();
		bool CreateFromDiskFile(const char* c_szFileName);

		void Set();

	protected:
		void Initialize();

	protected:
#if defined(DIRECTX9)
		LPDIRECT3DPIXELSHADER9 m_handle;
#else
		DWORD m_handle;
#endif
};

