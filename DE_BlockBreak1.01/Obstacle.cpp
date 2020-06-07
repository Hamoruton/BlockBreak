#include <d2d1.h>
#include <cmath>

#include "Obstacle.h"
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
CObstacle::CObstacle(ID2D1RenderTarget* pRenderTarget) 
{
	m_pBrush = NULL;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);

	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fRadius = 80.0f;
	m_fTheta = 0.0f;
}

//================================================================================
// デストラクタ
//================================================================================
CObstacle::~CObstacle() 
{
	SAFE_RELEASE(m_pBrush);
}

//================================================================================
// 円運動時の中心座標と幅を設定する
//================================================================================
void CObstacle::Reset(float cx, float cy, float width, int MoveState)
{
	m_fWidth = width;
	m_fCircleCenterX = cx;
	m_fCircleCenterY = cy;
	m_iMoveState = MoveState;
}

//================================================================================
// アニメーション
//================================================================================
void CObstacle::move()
{
	++m_fTheta;
	switch (m_iMoveState) 
	{
	case 1:
		m_fX = m_fCircleCenterX + cosf(m_fTheta / 60)*m_fRadius*1.0f;
		m_fY = m_fCircleCenterY + sinf(m_fTheta / 60)*m_fRadius*1.0f;
		break;

	case 2:
		m_fX = m_fCircleCenterX + cosf(m_fTheta / 60)*m_fRadius*-1.0f;
		m_fY = m_fCircleCenterY + sinf(m_fTheta / 60)*m_fRadius*-1.0f;
		break;

	case 3:
		m_fX = m_fCircleCenterX + sinf(m_fTheta / 60)*m_fRadius*1.0f;
		m_fY = m_fCircleCenterY + cosf(m_fTheta / 60)*m_fRadius*1.0f;
		break;

	case 4:
		m_fX = m_fCircleCenterX + sinf(m_fTheta / 60)*m_fRadius*-1.0f;
		m_fY = m_fCircleCenterY + cosf(m_fTheta / 60)*m_fRadius*-1.0f;
		break;

	default:
		m_fX = m_fCircleCenterX + cosf(m_fTheta / 60)*m_fRadius*1.0f;
		m_fY = m_fCircleCenterY + sinf(m_fTheta / 60)*m_fRadius*1.0f;
		break;
	}
}

//================================================================================
// 描画
//================================================================================
void CObstacle::draw(ID2D1RenderTarget* pRenderTarget) 
{
	D2D1_RECT_F rc;
	rc.left = m_fX - m_fWidth * 0.5f;
	rc.right = rc.left + m_fWidth;
	rc.top = m_fY;
	rc.bottom = m_fY + 770 * 0.03f;
	pRenderTarget->FillRectangle(rc, m_pBrush);
}

//================================================================================
// ボールとの当たり判定
//================================================================================
bool CObstacle::Collide(CBall* pBall) 
{
	float l, t, r, b;
	l = m_fX - m_fWidth * 0.5f;
	r = l + m_fWidth;
	t = m_fY;
	b = m_fY + 770 * 0.03f;

	return pBall->Collide(l, t, r, b);
}