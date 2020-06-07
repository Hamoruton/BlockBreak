#include <tchar.h>
#include <d2d1.h>
#include <dwrite.h>

#include "Selector.h"
#include "Title.h"
#include "Stage.h"
#include "GameOver.h"
#include "GameClear.h"
#include "ScoreUI.h"
#include "TextureLoader.h"

#pragma comment(lib,"dwrite.lib")

//================================================================================
// コンストラクタ
//================================================================================
CSelector::CSelector(ID2D1RenderTarget* pRenderTarget) 
{
	m_pRenderTarget = pRenderTarget;
	m_pRenderTarget->AddRef();

	m_iCount = 0;
	m_eGamePhase = GamePhase::GAMEPHASE_INIT;
	m_pScene = NULL;
	m_pWhiteBrush = NULL;

	m_pScoreUI = new CScoreUI(pRenderTarget);

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

	// 塗りつぶし用ブラシ
	if (FAILED(m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&m_pWhiteBrush
	))) {
		SAFE_RELEASE(m_pWhiteBrush);
	}
}

//================================================================================
// デストラク
//================================================================================
CSelector::~CSelector(void)
{
	SAFE_DELETE(m_pScoreUI);
	SAFE_DELETE(m_pScene);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTextFormat);
	CTextureLoader::Destroy();
}

//================================================================================
// アニメーション
//================================================================================
void CSelector::Anim()
{
	GameSceneResultCode rc = GAMESCENE_DEFAULT;

	// 遷移状態
	switch (m_eGamePhase) 
	{
	// 立ち上げ
	case GAMEPHASE_INIT:			
		m_eGamePhase = GAMEPHASE_RESET;

	// ソフトリセット
	case GAMEPHASE_RESET:
		SAFE_DELETE(m_pScene);		
		m_pScene = new CTitle(this);
		m_eGamePhase = GAMEPHASE_TITLE;

		// スコアリセット
		m_pScoreUI->ScoreReset();

	// タイトル
	case GAMEPHASE_TITLE:			
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;
		SAFE_DELETE(m_pScene);
		m_pScene = new CStage(this);
		m_eGamePhase = GAMEPHASE_GAME;

	// ゲーム本編
	case GAMEPHASE_GAME:			
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;
		if (m_pScoreUI->GetGameClear()) 
		{
			SAFE_DELETE(m_pScene);
			m_pScene = new CGameClear(this);
			m_eGamePhase = GAMEPHASE_GAMECLEAR;
		}
		else 
		{
			SAFE_DELETE(m_pScene);
			m_pScene = new CGameOver(this);
			m_eGamePhase = GAMEPHASE_GAMEOVER;
		}

	// ゲームクリア
	case GAMEPHASE_GAMECLEAR:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;
		m_eGamePhase = GAMEPHASE_RESET;

	// ゲームオーバー
	case GAMEPHASE_GAMEOVER:		
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;
		m_eGamePhase = GAMEPHASE_RESET;
	}
	m_iCount++;
}

//================================================================================
// 描画
//================================================================================
void CSelector::Draw(ID2D1RenderTarget* pRenderTarget) 
{
	TCHAR  str[256];

	// シーン描画
	if (m_pScene != NULL)
		m_pScene->draw(pRenderTarget);
	D2D1_SIZE_F size;
	size = pRenderTarget->GetSize();

	_stprintf_s(str, _countof(str), _T("%08d"), m_iCount);
	D2D1_RECT_F    rc;
	rc.left = 0;
	rc.right = size.width;
	rc.top = 0;
	rc.bottom = size.height;
	if (m_pWhiteBrush)
	{
		//pRenderTarget->DrawText(str, (DWORD)_tcslen(str), m_pTextFormat, &rc, m_pWhiteBrush);
	}
}

//================================================================================
// ゲッター　<返値>RenderTarget
//================================================================================
ID2D1RenderTarget *CSelector::GetRenderTarget() 
{
	m_pRenderTarget->AddRef();
	return m_pRenderTarget;
}

//================================================================================
// ゲッター　<返値>IDWriteTextFormat
//================================================================================
IDWriteTextFormat *CSelector::GetSystemTextFormat() 
{
	m_pTextFormat->AddRef();
	return m_pTextFormat;
}