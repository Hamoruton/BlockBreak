#pragma once
#ifndef __BALL_H_INCLUDED__
#define __BALL_H_INCLUDED__

struct ID2D1RenderTarget;
struct ID2D1SolidColorBrush;

//================================================================================
// �{�[���N���X
//================================================================================
class CBall
{
public:
	CBall(ID2D1RenderTarget* pRenderTarget);
	virtual ~CBall();

	void SetWall(float l, float t, float r, float b);
	void Reset(float x, float y, float vx, float vy);
	bool Collide(float l, float t, float r, float b);
	void Reflect();
	virtual bool move();
	virtual void draw(ID2D1RenderTarget* pRenderTarget);

	bool GetStartCollision() const { return m_bStartCollision; }
	bool GetResetStart() const { return m_bResetStart; }

	void SetResetStart(bool flag) { m_bResetStart = flag; }

protected:
	ID2D1SolidColorBrush*		m_pBrush;

	float	m_fL;		// ���̕�
	float	m_fR;		// �E�̕�
	float	m_fT;		// ��̕�
	float	m_fB;		// ���̕�

	float	m_fX;
	float	m_fY;
	float	m_fVX;
	float	m_fVY;

	// �Փ˔���
	bool	m_bTL;
	bool	m_bTR;
	bool	m_bBL;
	bool	m_bBR;

	bool	m_bStartCollision;
	bool	m_bResetStart;
};

#endif // !__BALL_H_INCLUDED__