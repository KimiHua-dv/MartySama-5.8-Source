#pragma once

#include "GrpImage.h"
#include "GrpIndexBuffer.h"
#include "GrpVertexBufferDynamic.h"
#include "Pool.h"
#include "../UserInterface/Locale_inc.h" //#if defined(__BL_CLIP_MASK__)

class CGraphicImageInstance
{
	public:
		static DWORD Type();
		BOOL IsType(DWORD dwType);

	public:
		CGraphicImageInstance();
		virtual ~CGraphicImageInstance();

		void Destroy();

#if defined(__BL_CLIP_MASK__)
		void Render(RECT* pClipRect = NULL);
#else
		void Render();
#endif

		void SetDiffuseColor(float fr, float fg, float fb, float fa);
		void SetPosition(float fx, float fy);
#ifdef ENABLE_RENDER_TARGET
		void SetScale(float fx, float fy);
#endif
		void SetImagePointer(CGraphicImage* pImage);
		void ReloadImagePointer(CGraphicImage* pImage);
		bool IsEmpty() const;

		int GetWidth();
		int GetHeight();

#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_DYNAMIC_LIGHT)
		void LeftRightReverse();
#endif
		CGraphicTexture * GetTexturePointer();
		const CGraphicTexture &	GetTextureReference() const;
		CGraphicImage * GetGraphicImagePointer();

		bool operator == (const CGraphicImageInstance & rhs) const;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_DYNAMIC_LIGHT)
		D3DXCOLOR GetPixelColor(int x, int y);
		D3DXCOLOR& GetDiffuseColor() { return m_DiffuseColor; };
#endif

	protected:
		void Initialize();

#if defined(__BL_CLIP_MASK__)
		virtual void OnRender(RECT* pClipRect);
#else
		virtual void OnRender();
#endif
		virtual void OnSetImagePointer();

		virtual BOOL OnIsType(DWORD dwType);

	protected:
		D3DXCOLOR m_DiffuseColor;
		D3DXVECTOR2 m_v2Position;
#ifdef ENABLE_RENDER_TARGET
		D3DXVECTOR2 m_v2Scale;
#endif
		CGraphicImage::TRef m_roImage;
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_DYNAMIC_LIGHT)
		bool m_bLeftRightReverse;
#endif

	public:
		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

		static CGraphicImageInstance* New();
		static void Delete(CGraphicImageInstance* pkImgInst);

		static CDynamicPool<CGraphicImageInstance>		ms_kPool;
};

