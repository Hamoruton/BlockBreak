#include <Windows.h>
#include <d2d1.h>

#include "Racket.h"
#include "Ball.h"

// マクロ宣言
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

//================================================================================
// コンストラクタ
//================================================================================
CRacket::CRacket(ID2D1RenderTarget* pRenderTarget) 
{
	m_pBrush = NULL;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
}

//================================================================================
// デストラクタ
//================================================================================
CRacket::~CRacket()
{
	SAFE_RELEASE(m_pBrush);
}

//================================================================================
// 画面内の使用範囲を設定する
//================================================================================
void CRacket::SetArea(float l, float t, float r, float b) 
{
	m_fWidth = (r - l) * 0.15f;
	m_fLeft = l + m_fWidth * 0.5f;
	m_fRight = r - m_fWidth * 0.5f;
	m_fTop = t;
	m_fBottom = b;
	m_fX = (l + r) * 0.5f;
	m_fY = (b - t) * 0.8f;
	m_fY += t;
}

//================================================================================
// アニメーション
//================================================================================
void CRacket::move() 
{
	if (GetAsyncKeyState(VK_LEFT))
		m_fX -= 3.0f;
	if(GetAsyncKeyState(VK_RIGHT))
		m_fX += 3.0f;
	if (m_fX < m_fLeft)
		m_fX = m_fLeft;
	if (m_fX > m_fRight)
		m_fX = m_fRight;
}

//================================================================================
// 描画
//================================================================================
void CRacket::draw(ID2D1RenderTarget* pRenderTarget) 
{
	D2D1_RECT_F rc;
	rc.left = m_fX - m_fWidth * 0.5f;
	rc.right = rc.left + m_fWidth;
	rc.top = m_fY;
	rc.bottom = m_fY + (m_fBottom - m_fTop) * 0.03f;
	pRenderTarget->FillRectangle(rc, m_pBrush);
}

//================================================================================
// ボールとの衝突判定
//================================================================================
bool CRacket::Collide(CBall* pBall) 
{
	float l, t, r, b;
	l = m_fX - m_fWidth * 0.5f;
	r = l + m_fWidth;
	t = m_fY;
	b = m_fY + (m_fBottom - m_fTop) * 0.03f;

	return pBall->Collide(l, t, r, b);
}