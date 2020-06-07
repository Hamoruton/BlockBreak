#pragma once
#ifndef __STAGE_H_INCLUDED__
#define __STAGE_H_INCLUDED__

#include "IGameScene.h"

struct	ID2D1RenderTarget;
struct	ID2D1SolidColorBrush;
class	CSelector;
class	CBall;
class	CRacket;
class	CObstacle;
class	CScoreUI;

//================================================================================
// ステージクラス
//================================================================================
class CStage
	:public IGameScene
{
public:
	CStage(CSelector* pSystem);
	virtual ~CStage();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget* pRenderTarget);

protected:
	void CollideBlocksAndTheBall();

	CSelector*				m_pSystem;
	ID2D1SolidColorBrush*	m_pBrush;
	CBall*					m_pBall;
	CRacket*				m_pRacket;
	CObstacle*				m_pObstacle[4];
	CScoreUI*				m_pScoreUI;

	// ブロックデータ
	BYTE*					m_pBlocks;
	// ブロックジオメトリ
	INT						m_iBlockRows;
	INT						m_iBlockCols;
	INT						m_iBlockLeft;
	INT						m_iBlockRight;
	INT						m_iBlockTop;
	INT						m_iBlockBottom;

	// フェードアウト
	ID2D1SolidColorBrush*	m_pBlackBrush;
	int						m_iFadeTimer;
	static const int		kFadeOutDuration = 120;

	static const int		kNumRows = 8;
	static const int		kNumCols = 11;
	static const int		kDefaultLeft = 80;
	static const int		kDefaultRight = 520;
	static const int		kDefaultTop = 64;
	static const int		kDefaultBottom = 344;

	static const int		kToDeleteFlag = 0x80;

	int						m_iScoreTimer;
	int						m_iStartBallTimer;
};

#endif // !__STAGE_H_INCLUDED__