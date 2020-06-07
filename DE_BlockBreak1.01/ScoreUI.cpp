#include <d2d1.h>
#include <dwrite.h>
#include <tchar.h>

#include "Selector.h"
#include "ScoreUI.h"

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
CScoreUI::CScoreUI(ID2D1RenderTarget* pRenderTarget) 
{
	// Direct Write ������
	{
		HRESULT hr;
		IDWriteFactory *pFactory;

		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pFactory),
			reinterpret_cast<IUnknown **>(&pFactory)
		);

		hr = pFactory->CreateTextFormat(
			(WCHAR*)("consolas"),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20,
			L"", //locale
			&m_pTextFormat
		);
		SAFE_RELEASE(pFactory);
	}

	// �u���V
	m_pScoreBrush = NULL;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pScoreBrush);
	m_pLifeBrush = NULL;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pLifeBrush);

	// �����X�R�A
	m_iScore = 0;

	// �ő僉�C�t��
	m_iLife = 3;

	// ���U���g�X�R�A
	m_iResultScore = 0;

	// �Q�[���N���A���ǂ���
	m_bGameClear = false;
}

//================================================================================
// �f�X�g���N�^
//================================================================================
CScoreUI::~CScoreUI() 
{
	SAFE_RELEASE(m_pScoreBrush);
}

//================================================================================
// �X�R�A���Z�b�g
//================================================================================
void CScoreUI::SetScore(int score) 
{
	m_iScore = score;
}

//================================================================================
// �X�R�A���擾
//================================================================================
int CScoreUI::GetScore() const
{
	return m_iScore;
}

//================================================================================
// �X�R�A�����Z
//================================================================================
void CScoreUI::AddScore(int value) 
{
	m_iScore += value;
	if (m_iScore > 9999)
		m_iScore = 9999;
}

//================================================================================
// ���U���g�X�R�A���擾
//================================================================================
int CScoreUI::GetResultScore(int score, int life)
{
	m_iResultScore = life * 500 + score;
	return m_iResultScore;
}

//================================================================================
// ���C�t���Z�b�g
//================================================================================
void CScoreUI::SetLife(int life) 
{
	m_iLife += life;
}

//================================================================================
// ���C�t���擾
//================================================================================
int CScoreUI::GetLife() const
{
	return m_iLife;
}

//================================================================================
// �X�R�A���Z�b�g
//================================================================================
void CScoreUI::ScoreReset() 
{
	m_iScore = 0;
	m_iResultScore = 0;
	m_iLife = 3;
}

//================================================================================
// �A�j���[�V����
//================================================================================
void CScoreUI::move() 
{
}

//================================================================================
// �`��
//================================================================================
void CScoreUI::draw(ID2D1RenderTarget* pRenderTarget) 
{
	// �X�R�A�E�X�R�A�e�L�X�g
	TCHAR  score[256];
	TCHAR  score_text[256];
	D2D1_SIZE_F size;
	size = pRenderTarget->GetSize();

	_stprintf_s(score, _countof(score), _T("%04d"), m_iScore);
	_stprintf_s(score_text, _countof(score), _T("SCORE"));

	D2D1_RECT_F    rc1, rc2, rc3;
	
	rc1.left = 550;
	rc1.right = size.width;
	rc1.top = 0;
	rc1.bottom = size.height;
	
	rc2.left = 430;
	rc2.right = size.width;
	rc2.top = 0;
	rc2.bottom = size.height;

	if (m_pScoreBrush)
	{
		pRenderTarget->DrawText(score, (DWORD)_tcslen(score), m_pTextFormat, &rc1, m_pScoreBrush);
		pRenderTarget->DrawText(score_text, (DWORD)_tcslen(score_text), m_pTextFormat, &rc2, m_pScoreBrush);
	}

	// ���C�t
	TCHAR  life_text[256];
	_stprintf_s(life_text, _countof(life_text), _T("LIFE"));

	rc3.left = 430;
	rc3.right = size.width;
	rc3.top = 30;
	rc3.bottom = size.height;

	if (m_pLifeBrush) 
	{
		pRenderTarget->DrawText(life_text, (DWORD)_tcslen(life_text), m_pTextFormat, &rc3, m_pLifeBrush);
	}

	D2D1_ELLIPSE ellipse[3];
	for (int i = 0; i < m_iLife; i++)
	{
		if (m_pLifeBrush != NULL)
		{
			ellipse[i].point.x = 585 - i * 15;
			ellipse[i].point.y = 45;
			ellipse[i].radiusX = 6.0f;
			ellipse[i].radiusY = 6.0f;
			pRenderTarget->FillEllipse(ellipse[i], m_pLifeBrush);
		}
	}
}