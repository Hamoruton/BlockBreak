#pragma once
#ifndef __GAMECLEAR_H_INCLUDED__
#define __GAMECLEAR_H_INCLUDED__

#ifndef __IGAMESCENE_H_INCLUDED__
#include "IGameScene.h"
#endif // !__IGAMESCENE_H_INCLUDED__

struct	ID2D1RenderTarget;
struct	ID2D1SolidColorBrush;
struct	IDWriteTextFormat;
struct	ID2D1Bitmap;
class	CSelector;
class	CScoreUI;

enum GameClearPhase
{
	GAMECLEAR_INIT = 0,
	GAMECLEAR_RUN = 1,
	GAMECLEAR_FADE = 2,
	GAMECLEAR_DONE = 3
};

//================================================================================
// ゲームクリアクラス
//================================================================================
class CGameClear
	:public IGameScene
{
public:
	CGameClear(CSelector* pSystem);
	virtual ~CGameClear();

	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget* pRenderTarget) override;

	void ResultScore(ID2D1RenderTarget* pRenderTarget);

protected:
	CSelector*				m_pSystem;
	CScoreUI*				m_pScoreUI;
	GameClearPhase			m_ePhase;
	IDWriteTextFormat*		m_pTextFormat;
	ID2D1Bitmap*			m_pGameClearImage;
	ID2D1Bitmap*			m_pSpaceImage;
	ID2D1SolidColorBrush*	m_pBlack;
	ID2D1SolidColorBrush*	m_pWhite;
	bool					m_bFlag;
	int						m_iTimer;
	int						m_iFadeTimer;
	int						m_iResultScore;
};

#endif // !__GAMECLEAR_H_INCLUDED__