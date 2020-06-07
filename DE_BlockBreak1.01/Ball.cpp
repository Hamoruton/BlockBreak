#include <Windows.h>
#include <d2d1.h>

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
CBall::CBall(ID2D1RenderTarget* pRenderTarget) 
{
	m_pBrush = NULL;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);

	m_bTL = m_bTR = m_bBL = m_bBR = false;
	m_bStartCollision = false;
	m_bResetStart = false;
}

//================================================================================
// デストラクタ
//================================================================================
CBall::~CBall() 
{
	SAFE_RELEASE(m_pBrush);
}

//================================================================================
// ボールを反射させる外枠を設定する
//================================================================================
void CBall::SetWall(float l, float t, float r, float b) 
{
	m_fL = l;
	m_fR = r;
	m_fT = t;
	m_fB = b;
}

//================================================================================
// ボールの座標と初速度を設定する
//================================================================================
void CBall::Reset(float x, float y, float vx, float vy) 
{
	m_fX = x;
	m_fY = y;
	m_fVX = vx;
	m_fVY = vy;
	m_bTL = m_bTR = m_bBL = m_bBR = false;
}

//================================================================================
// アニメーション
//================================================================================
bool CBall::move() 
{
	m_fX += m_fVX;
	m_fY += m_fVY;

	// スタート時の当たり判定を無効にする
	if (m_fY > 350.0f)
		m_bStartCollision = true;

	return true;
}

//================================================================================
// 描画
//================================================================================
void CBall::draw(ID2D1RenderTarget* pRenderTarget) 
{
	D2D1_ELLIPSE ellipse;
	if (m_pBrush != NULL) 
	{
		ellipse.point.x = m_fX;
		ellipse.point.y = m_fY;
		ellipse.radiusX = 8.0f;
		ellipse.radiusY = 8.0f;
		pRenderTarget->FillEllipse(ellipse, m_pBrush);
	}
}

//================================================================================
// 短形と座標の当たり判定
//================================================================================
static inline bool ptinrect(float x, float y, float l, float t, float r, float b) 
{
	if (x < l)
		return false;
	if (x > r)
		return false;
	if (y < t)
		return false;
	if (y > b)
		return false;
	return true;
}

//================================================================================
// 短形との当たり判定
//================================================================================
bool CBall::Collide(float l, float t, float r, float b) 
{
	bool tl = false, tr = false, bl = false, br = false;
	if (ptinrect(m_fX - 4.0f, m_fY - 4.0f, l, t, r, b))
		tl = true;
	if (ptinrect(m_fX + 4.0f, m_fY - 4.0f, l, t, r, b))
		tr = true;
	if (ptinrect(m_fX - 4.0f, m_fY + 4.0f, l, t, r, b))
		bl = true;
	if (ptinrect(m_fX + 4.0f, m_fY + 4.0f, l, t, r, b))
		br = true;

	// 当たり判定
	m_bTL |= tl;
	m_bTR |= tr;
	m_bBL |= bl;
	m_bBR |= br;

	return (tl | tr | bl | br);
}

//================================================================================
// 反射実行
//================================================================================
void CBall::Reflect() 
{
	float vx = m_fVX, vy = m_fVY;

	// 画面との当たり判定
	if (m_fY - 4 < m_fT)
		m_bTL = m_bTR = true;
	if (m_fX - 4 < m_fL)
		m_bTL = m_bBL = true;
	if (m_fX + 4 > m_fR)
		m_bTR = m_bBR = true;
	if (m_fY + 4 > m_fB) 
	{
		// ゲームリセット
		Reset(50.0f, 50.0f, 4.0f, 1.5f);
		m_bStartCollision = false;
		m_bResetStart = true;
		m_bBL = m_bBR = true;
	}

	// 水平衝突判定
	if (m_bTL && m_bTR && !m_bBL && !m_bBR)
	{
		if (vy < 0)   //  **
			vy = -vy; //  oo
	}
	else if (!m_bTL && !m_bTR && m_bBL && m_bBR) 
	{
		if (vy > 0)   //  oo
			vy = -vy; //  **
	}
	else if (!m_bTL && m_bTR && !m_bBL && m_bBR) 
	{
		if (vx > 0)   //  o*
			vx = -vx; //  o*
	}
	else if (m_bTL && !m_bTR && m_bBL && !m_bBR) 
	{
		if (vx < 0)   //  *o
			vx = -vx; //  *o
	}
	// 鉛直衝突判定
	else if ((!m_bTL && !m_bTR && m_bBL && !m_bBR)
		|| (m_bTL && !m_bTR && m_bBL && m_bBR))
	{
		//  oo  *o
		//  *o  **
		if ((vy - vx) >= 0)
		{
			vx = m_fVY;
			vy = m_fVX;
		}
	}
	else if (
		(!m_bTL && !m_bTR && !m_bBL && m_bBR)
		|| (!m_bTL && m_bTR && m_bBL && m_bBR))
	{
		//  oo  o*
		//  o*  **
		if ((vx + vy) >= 0) 
		{
			vx = -m_fVY;
			vy = -m_fVX;
		}
	}
	else if (
		(!m_bTL && m_bTR && !m_bBL && !m_bBR)
		|| (m_bTL && m_bTR && !m_bBL && m_bBR))
	{
		//  o*  **
		//  oo  o*
		if ((vx - vy) >= 0) 
		{
			vx = m_fVY;
			vy = m_fVX;
		}
	}
	else if (
		(m_bTL && !m_bTR && !m_bBL && !m_bBR)
		|| (m_bTL && m_bTR && m_bBL && !m_bBR))
	{
		//  *o  **
		//  oo  *o
		if ((-vx - vy) >= 0) 
		{
			vx = -m_fVY;
			vy = -m_fVX;
		}
	}
	else if (m_bTL && !m_bTR && !m_bBL && m_bBR) 
	{
		//  *o
		//  o*
		vx = m_fVY;
		vy = m_fVX;
	}
	else if (!m_bTL && m_bTR && m_bBL && !m_bBR) 
	{
		//  o*
		//  *o
		vx = -m_fVY;
		vy = -m_fVX;
	}
	m_bTL = m_bTR = m_bBL = m_bBR = false;
	m_fVX = vx;
	m_fVY = vy;
}