#include <Windows.h>
#include <d2d1.h>

#include "Ball.h"

// �}�N���錾
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

//================================================================================
// �R���X�g���N�^
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
// �f�X�g���N�^
//================================================================================
CBall::~CBall() 
{
	SAFE_RELEASE(m_pBrush);
}

//================================================================================
// �{�[���𔽎˂�����O�g��ݒ肷��
//================================================================================
void CBall::SetWall(float l, float t, float r, float b) 
{
	m_fL = l;
	m_fR = r;
	m_fT = t;
	m_fB = b;
}

//================================================================================
// �{�[���̍��W�Ə����x��ݒ肷��
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
// �A�j���[�V����
//================================================================================
bool CBall::move() 
{
	m_fX += m_fVX;
	m_fY += m_fVY;

	// �X�^�[�g���̓����蔻��𖳌��ɂ���
	if (m_fY > 350.0f)
		m_bStartCollision = true;

	return true;
}

//================================================================================
// �`��
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
// �Z�`�ƍ��W�̓����蔻��
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
// �Z�`�Ƃ̓����蔻��
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

	// �����蔻��
	m_bTL |= tl;
	m_bTR |= tr;
	m_bBL |= bl;
	m_bBR |= br;

	return (tl | tr | bl | br);
}

//================================================================================
// ���ˎ��s
//================================================================================
void CBall::Reflect() 
{
	float vx = m_fVX, vy = m_fVY;

	// ��ʂƂ̓����蔻��
	if (m_fY - 4 < m_fT)
		m_bTL = m_bTR = true;
	if (m_fX - 4 < m_fL)
		m_bTL = m_bBL = true;
	if (m_fX + 4 > m_fR)
		m_bTR = m_bBR = true;
	if (m_fY + 4 > m_fB) 
	{
		// �Q�[�����Z�b�g
		Reset(50.0f, 50.0f, 4.0f, 1.5f);
		m_bStartCollision = false;
		m_bResetStart = true;
		m_bBL = m_bBR = true;
	}

	// �����Փ˔���
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
	// �����Փ˔���
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