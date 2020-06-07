#pragma once
#ifndef __TITLE_H_INCLUDED__
#define __TITLE_H_INCLUDED__

#ifndef __IGAMESCENE_H_INCLUDED__
#include "IGameScene.h"
#endif // !__IGAMESCENE_H_INCLUDED__

enum TitlePhase
{
	TITLE_INIT = 0,
	TITLE_RUN = 1,
	TITLE_FADE = 2,
	TITLE_DONE = 3
};

class	CSelector;
struct	ID2D1Bitmap;
struct	ID2D1SolidColorBrush;

//================================================================================
// タイトルクラス
//================================================================================
class CTitle
	:public IGameScene
{
public:
	CTitle(CSelector* pSystem);
	virtual ~CTitle();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget* pRenderTarget) override;

protected:
	CSelector*					m_pSystem;
	TitlePhase					m_ePhase;
	ID2D1Bitmap*				m_pTitleImage;
	ID2D1Bitmap*				m_pSpaceImage;
	ID2D1SolidColorBrush*		m_pBlack;
	bool						m_bFlag;
	int							m_iTimer;
	int							m_iFadeTimer;
};

#endif // !__TITLE_H_INCLUDED__

