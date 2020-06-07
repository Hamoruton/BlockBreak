#pragma once
#ifndef __SELECTOR_H_INCLUDED__
#define __SELECTOR_H_INCLUDED__

// ゲーム画面のフェーズ状態
enum GamePhase
{
	GAMEPHASE_INIT = 0,
	GAMEPHASE_RESET = 1,
	GAMEPHASE_TITLE = 0x010,
	GAMEPHASE_GAME = 0x100,
	GAMEPHASE_GAMEOVER = 0x200,
	GAMEPHASE_GAMECLEAR = 0x300
};

// Sceneの終わり方定数　<返値>IGameScene::move()
enum GameSceneResultCode
{
	GAMESCENE_DEFAULT = 0,
	GAMESCENE_END_OK = 1,
	GAMESCENE_END_TIMEOUT = 2,
	GAMESCENE_END_FAILURE = 3,
};

class	IGameScene;				// シーン管理クラス
class	CScoreUI;				// ゲームスコアクラス
struct  ID2D1RenderTarget;
struct	IDWriteTextFormat;
struct	ID2D1SolidColorBrush;

//================================================================================
// セレクタークラス
//================================================================================
class CSelector
{
public:
	CSelector(ID2D1RenderTarget* pRenderTarget);
	virtual ~CSelector(void);

	void Anim();
	void Draw(ID2D1RenderTarget* pRenderTarget);

	ID2D1RenderTarget* GetRenderTarget();
	IDWriteTextFormat* GetSystemTextFormat();

	CScoreUI* GetClassScoreUI() const { return m_pScoreUI; }

protected:
	ID2D1RenderTarget*		m_pRenderTarget;
	IDWriteTextFormat*		m_pTextFormat;
	ID2D1SolidColorBrush*	m_pWhiteBrush;
	int						m_iCount;
	int						m_iWait;
	int						m_iCurrent;

	IGameScene*				m_pScene;		//シーン
	GamePhase				m_eGamePhase;	//ゲーム状態
	CScoreUI*				m_pScoreUI;		//ゲームスコア
};

// マクロ宣言
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

#ifndef __IGAMESCENE_H__
#include "IGameScene.h"
#endif

#endif // !__SELECTOR_H_INCLUDED__
