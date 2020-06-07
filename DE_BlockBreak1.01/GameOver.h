#pragma once
#ifndef __GAMEOVER_H_INCLUDED__
#define __GAMEOVER_H_INCLUDED__

#ifndef __IGAMESCENE_H_INCLUDED__
#include "IGameScene.h"
#endif // !__IGAMESCENE_H_INCLUDED__

struct	ID2D1RenderTarget;
struct	ID2D1SolidColorBrush;
class	CSelector;
struct	ID2D1Bitmap;

enum GameOverPhase
{
	GAMEOVER_INIT = 0,
	GAMEOVER_RUN = 1,
	GAMEOVER_FADE = 2,
	GAMEOVER_DONE = 3
};

//================================================================================
// ゲームオーバークラス
//================================================================================
class CGameOver
	:public IGameScene
{
public:
	CGameOver(CSelector* pSystem);
	virtual ~CGameOver();

	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget* pRenderTarget) override;

protected:
	CSelector*				m_pSystem;
	GameOverPhase			m_ePhase;
	ID2D1Bitmap*			m_pGameOverImage;
	ID2D1Bitmap*			m_pSpaceImage;
	ID2D1SolidColorBrush*	m_pBlack;
	bool					m_bFlag;
	int						m_iTimer;
	int						m_iFadeTimer;
};

#endif // !__GAMEOVER_H_INCLUDED__