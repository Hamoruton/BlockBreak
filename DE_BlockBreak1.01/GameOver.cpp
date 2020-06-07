#include <tchar.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>

#include "Selector.h"
#include "GameOver.h"
#include "TextureLoader.h"

//================================================================================
// コンストラクタ
//================================================================================
CGameOver::CGameOver(CSelector* pSystem) 
{
	ID2D1RenderTarget* pRenderTarget = NULL;
	m_pSystem = pSystem;
	pRenderTarget = pSystem->GetRenderTarget();
	if (pRenderTarget) 
	{
		CTextureLoader::CreateD2D1BitmapFromFile(pRenderTarget, _T("Assets\\Sprites\\gameover.png"), &m_pGameOverImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pRenderTarget, _T("Assets\\Sprites\\spacelogo.png"), &m_pSpaceImage);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pRenderTarget->Release();
		pRenderTarget = NULL;
	}
	m_ePhase = GAMEOVER_INIT;
	m_iTimer = 0;
	m_iFadeTimer = 0;
}

//================================================================================
// デストラクタ
//================================================================================
CGameOver::~CGameOver()
{
}

//================================================================================
// アニメーション
//================================================================================
GameSceneResultCode CGameOver::move() 
{
	switch (m_ePhase) 
	{
	case GAMEOVER_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = GAMEOVER_RUN;
	case GAMEOVER_RUN:
	{
		bool bDone = false;
		++m_iTimer;
		if (m_iTimer > 600) 
		{
			bDone = true;
		}
		else if (GetAsyncKeyState(VK_SPACE)) 
		{
			if (!m_bFlag) 
			{
				bDone = true;
				m_bFlag = true;
			}
		}
		else
		{
			m_bFlag = false;
		}
		if (bDone) 
		{
			m_iFadeTimer = 0;
			m_ePhase = GAMEOVER_FADE;
		}
		break;
	}
	case GAMEOVER_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 60)
			break;
		m_ePhase = GAMEOVER_DONE;
	case GAMEOVER_DONE:
		return GameSceneResultCode::GAMESCENE_END_OK;
	}
	return GameSceneResultCode::GAMESCENE_DEFAULT;
}

//================================================================================
// 描画
//================================================================================
void CGameOver::draw(ID2D1RenderTarget* pRenderTarget) 
{
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;

	// ゲームオーバーロゴ
	screenSize = pRenderTarget->GetSize();
	textureSize = m_pGameOverImage->GetSize();
	rc.left = (screenSize.width - textureSize.width) / 2;
	rc.top = (screenSize.height - textureSize.height) / 2;
	rc.right = rc.left + textureSize.width;
	rc.bottom = rc.top + textureSize.height;
	pRenderTarget->DrawBitmap(m_pGameOverImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

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
	case GAMEOVER_FADE:
	case GAMEOVER_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 60.0f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
	}
}
