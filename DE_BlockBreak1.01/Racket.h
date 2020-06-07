#pragma once
#ifndef __RACKET_H_INCLUDED__
#define __RACKET_H_INCLUDED__

struct	ID2D1RenderTarget;
struct	ID2D1SolidColorBrush;
class	CBall;

//================================================================================
// ラケットクラス
//================================================================================
class CRacket
{
public:
	CRacket(ID2D1RenderTarget* pRenderTarget);
	virtual ~CRacket();
	void move();
	void draw(ID2D1RenderTarget* pRenderTarget);
	void SetArea(float l, float t, float r, float b);
	bool Collide(CBall* pBall);

protected:
	ID2D1SolidColorBrush*	m_pBrush;
	float					m_fX;
	float					m_fY;
	float					m_fWidth;
	float					m_fLeft;
	float					m_fTop;
	float					m_fBottom;
	float					m_fRight;
};

#endif // !__RACKET_H_INCLUDED__