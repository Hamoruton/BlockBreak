#include <Windows.h>
#include <tchar.h>
#include <cmath>
#include <d2d1.h>
#include <dwrite.h>

#include "Selector.h"
#include "Title.h"
#include "TextureLoader.h"

//================================================================================
// コンストラクタ
//================================================================================
CTitle::CTitle(CSelector* pSystem) 
{
	ID2D1RenderTarget* pTarget;
	m_iFadeTimer = 0;
	m_ePhase = TitlePhase::TITLE_INIT;
	m_pTitleImage = NULL;
	m_pSystem = pSystem;
	m_pBlack = NULL;
	m_pSpaceImage = NULL;
	pTarget = pSystem->GetRenderTarget();
	if (pTarget) 
	{
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("Assets\\Sprites\\titlelogo.png"), &m_pTitleImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("Assets\\Sprites\\spacelogo.png"), &m_pSpaceImage);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}
}

//================================================================================
// デストラクタ
//================================================================================
CTitle::~CTitle() 
{
	SAFE_RELEASE(m_pSpaceImage);
	SAFE_RELEASE(m_pTitleImage);
}

//================================================================================
// アニメーション
//================================================================================
GameSceneResultCode CTitle::move() 
{
	switch (m_ePhase) 
	{
	case TITLE_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = TITLE_RUN;

	case TITLE_RUN:
	{
		bool bDone = false;
		++m_iTimer;
		if (m_iTimer > 600)
			bDone = true;
		else if (GetAsyncKeyState(VK_SPACE))
		{
			if (!m_bFlag)
			{
				bDone = true;
				m_bFlag = true;
			}
		}
		else
			m_bFlag = false;
		if (bDone)
		{
			m_iFadeTimer = 0;
			m_ePhase = TITLE_FADE;
		}
		break;
	}

	case TITLE_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 30)
			break;
		m_ePhase = TITLE_DONE;

	case TITLE_DONE:
		return GameSceneResultCode::GAMESCENE_END_OK;
	}

	return GameSceneResultCode::GAMESCENE_DEFAULT;
}

//================================================================================
// 描画
//================================================================================
void CTitle::draw(ID2D1RenderTarget *pRenderTarget) 
{
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();

	// タイトルロゴ
	textureSize = m_pTitleImage->GetSize();
	rc.left = (screenSize.width - textureSize.width) / 2;
	rc.top = (screenSize.height - textureSize.height) / 2;
	rc.right = rc.left + textureSize.width;
	rc.bottom = rc.top + textureSize.height;
	pRenderTarget->DrawBitmap(m_pTitleImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	// スペースキーロゴ
	textureSize = m_pSpaceImage->GetSize();
	textureSize.width *= 0.5f;
	textureSize.height *= 0.5f;
	rc.left = (screenSize.width - textureSize.width)*0.5f;
	rc.top = (screenSize.height - textureSize.height * 4);
	rc.right = rc.left + textureSize.width;
	rc.bottom = rc.top + textureSize.height;
	pRenderTarget->DrawBitmap(m_pSpaceImage, rc, sinf(m_iTimer / 10) + 1.0f);

	switch (m_ePhase)
	{
	case TITLE_FADE:
	case TITLE_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 30.0f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
	}
}
