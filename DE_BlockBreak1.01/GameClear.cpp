#include <tchar.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>

#include "Selector.h"
#include "GameClear.h"
#include "ScoreUI.h"
#include "TextureLoader.h"

//================================================================================
// コンストラクタ
//================================================================================
CGameClear::CGameClear(CSelector* pSystem) 
{
	ID2D1RenderTarget* pRenderTarget = NULL;
	m_pSystem = pSystem;
	pRenderTarget = pSystem->GetRenderTarget();
	if (pRenderTarget) 
	{
		CTextureLoader::CreateD2D1BitmapFromFile(pRenderTarget, _T("Assets\\Sprites\\gameclear.png"), &m_pGameClearImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pRenderTarget, _T("Assets\\Sprites\\spacelogo.png"), &m_pSpaceImage);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhite);
		pRenderTarget->Release();
		pRenderTarget = NULL;
	}

	// Direct Write 初期化
	{
		HRESULT hr;
		IDWriteFactory *pFactory;

		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pFactory),
			reinterpret_cast<IUnknown **>(&pFactory)
		);

		hr = pFactory->CreateTextFormat(
			(WCHAR*)("consolas"),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20,
			L"", //locale
			&m_pTextFormat
		);
		SAFE_RELEASE(pFactory);
	}

	m_ePhase = GAMECLEAR_INIT;
	m_iTimer = 0;
	m_iFadeTimer;

	m_pScoreUI = m_pSystem->GetClassScoreUI();
	m_iResultScore = m_pScoreUI->GetResultScore(m_pScoreUI->GetScore(), m_pScoreUI->GetLife());
}

//================================================================================
// デストラクタ
//================================================================================
CGameClear::~CGameClear() 
{
	SAFE_RELEASE(m_pWhite);
	SAFE_RELEASE(m_pBlack);
}

//================================================================================
// アニメーション
//================================================================================
GameSceneResultCode CGameClear::move() 
{
	switch (m_ePhase)
	{
	case GAMECLEAR_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = GAMECLEAR_RUN;
	case GAMECLEAR_RUN:
	{
		bool bDone = false;
		++m_iTimer;
		if (m_iTimer > 600) {
			bDone = true;
		}
		else if (GetAsyncKeyState(VK_SPACE)) {
			if (!m_bFlag) {
				bDone = true;
				m_bFlag = true;
			}
		}
		else {
			m_bFlag = false;
		}
		if (bDone) {
			m_iFadeTimer = 0;
			m_ePhase = GAMECLEAR_FADE;
		}
		break;
	}
	case GAMECLEAR_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 60)
			break;
		m_ePhase = GAMECLEAR_DONE;
	case GAMECLEAR_DONE:
		return GameSceneResultCode::GAMESCENE_END_OK;
	}
	return GameSceneResultCode::GAMESCENE_DEFAULT;
}

//================================================================================
// 描画
//================================================================================
void CGameClear::draw(ID2D1RenderTarget* pRenderTarget) 
{
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;

	// ゲームクリアロゴ
	screenSize = pRenderTarget->GetSize();
	textureSize = m_pGameClearImage->GetSize();
	rc.left = (screenSize.width - textureSize.width) / 2;
	rc.top = (screenSize.height - textureSize.height) / 2;
	rc.right = rc.left + textureSize.width;
	rc.bottom = rc.top + textureSize.height;
	pRenderTarget->DrawBitmap(m_pGameClearImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	// スペースキーロゴ
	textureSize = m_pSpaceImage->GetSize();
	textureSize.width *= 0.5f;
	textureSize.height *= 0.5f;
	rc.left = (screenSize.width - textureSize.width)*0.5f;
	rc.top = (screenSize.height - textureSize.height * 4);
	rc.right = rc.left + textureSize.width;
	rc.bottom = rc.top + textureSize.height;
	pRenderTarget->DrawBitmap(m_pSpaceImage, rc, sinf(m_iTimer / 10) + 1.0f);

	// リザルトスコア
	ResultScore(pRenderTarget);

	switch (m_ePhase) 
	{
	case GAMECLEAR_FADE:
	case GAMECLEAR_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 60.0f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
	}
}

//================================================================================
// リザルトスコア
//================================================================================
void CGameClear::ResultScore(ID2D1RenderTarget* pRenderTarget) 
{
	// スコア・スコアテキスト
	TCHAR  score[256];
	TCHAR  score_text[256];
	D2D1_SIZE_F size;
	size = pRenderTarget->GetSize();

	_stprintf_s(score, _countof(score), _T("%04d"), m_pScoreUI->GetScore());
	_stprintf_s(score_text, _countof(score), _T("SCORE"));

	D2D1_RECT_F    rc1, rc2, rc3, rc4;

	rc1.left = 330;
	rc1.right = size.width;
	rc1.top = 330;
	rc1.bottom = size.height;

	rc2.left = 210;
	rc2.right = size.width;
	rc2.top = 330;
	rc2.bottom = size.height;

	if (m_pWhite)
	{
		pRenderTarget->DrawText(score, (DWORD)_tcslen(score), m_pTextFormat, &rc1, m_pWhite);
		pRenderTarget->DrawText(score_text, (DWORD)_tcslen(score_text), m_pTextFormat, &rc2, m_pWhite);
	}

	// ライフ・ライフテキスト
	TCHAR  life[256];
	TCHAR  life_text[256];
	_stprintf_s(life, _countof(life), _T("%04d"), m_pScoreUI->GetLife());
	_stprintf_s(life_text, _countof(life_text), _T("LIFE"));

	rc3.left = 330;
	rc3.right = size.width;
	rc3.top = 360;
	rc3.bottom = size.height;

	rc4.left = 210;
	rc4.right = size.width;
	rc4.top = 360;
	rc4.bottom = size.height;

	if (m_pWhite)
	{
		pRenderTarget->DrawText(life, (DWORD)_tcslen(life), m_pTextFormat, &rc3, m_pWhite);
		pRenderTarget->DrawText(life_text, (DWORD)_tcslen(life_text), m_pTextFormat, &rc4, m_pWhite);
	}

	// 四角形の枠
	D2D1_RECT_F    rc5;

	rc5.left = 145;
	rc5.right = 435;
	rc5.top = 300;
	rc5.bottom = 500;

	pRenderTarget->DrawRectangle(rc5, m_pWhite, 1);

	// リザルト・リザルトテキスト
	TCHAR  result[256];
	TCHAR  result_text[256];
	_stprintf_s(result, _countof(result), _T("%04d"), m_iResultScore);
	_stprintf_s(result_text, _countof(result_text), _T("RESULT"));

	D2D1_RECT_F    rc6, rc7;

	rc6.left = 330;
	rc6.right = size.width;
	rc6.top = 430;
	rc6.bottom = size.height;

	rc7.left = 210;
	rc7.right = size.width;
	rc7.top = 430;
	rc7.bottom = size.height;

	if (m_pWhite)
	{
		pRenderTarget->DrawText(result, (DWORD)_tcslen(result), m_pTextFormat, &rc6, m_pWhite);
		pRenderTarget->DrawText(result_text, (DWORD)_tcslen(result_text), m_pTextFormat, &rc7, m_pWhite);
	}
}