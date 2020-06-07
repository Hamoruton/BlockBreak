#pragma once
#ifndef __OBSTACLE_H_INCLUDED__
#define __OBSTACLE_H_INCLUDED__

struct	ID2D1RenderTarget;
struct	ID2D1SolidColorBrush;
class	CBall;

//================================================================================
// è·äQï®ÉNÉâÉX
//================================================================================
class CObstacle
{
public:
	CObstacle(ID2D1RenderTarget* pRenderTarget);
	virtual ~CObstacle();
	void move();
	void draw(ID2D1RenderTarget* pRenderTarget);
	void Reset(float cx, float cy, float width, int MoveState);
	bool Collide(CBall* pBall);
	
protected:
	ID2D1SolidColorBrush*	m_pBrush;
	float					m_fX;
	float					m_fY;
	float					m_fWidth;
	float					m_fRadius;
	float					m_fCircleCenterX;
	float					m_fCircleCenterY;
	float					m_fTheta;
	int 					m_iMoveState;
};

#endif // !__OBSTACLE_H_INCLUDED__