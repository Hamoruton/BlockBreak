#include <Windows.h>
#include <d2d1.h>

#include "Selector.h"
#include "Stage.h"
#include "Ball.h"
#include "Racket.h"
#include "Obstacle.h"
#include "ScoreUI.h"

//================================================================================
// �R���X�g���N�^
//================================================================================
CStage::CStage(CSelector* pSystem) 
{
	ID2D1RenderTarget* pRenderTarget = NULL;
	m_pSystem = pSystem;
	m_pBrush = NULL;
	m_pBall = NULL;

	pRenderTarget = pSystem->GetRenderTarget();
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
	m_pBall = new CBall(pRenderTarget);
	if (m_pBall) 
	{
		m_pBall->SetWall(0, 0, 600.0f, 800.0f);
		m_pBall->Reset(240.0f, 0.0f, 4.0f, 1.5f);
	}

	m_pRacket = new CRacket(pRenderTarget);
	if (m_pRacket) 
	{
		m_pRacket->SetArea(0, 300, 600.0f, 800.0f);
	}

	m_pScoreUI = pSystem->GetClassScoreUI();
	if (m_pScoreUI) 
	{
		m_pScoreUI->SetScore(0);
	}

	m_pObstacle[0] = new CObstacle(pRenderTarget);
	m_pObstacle[1] = new CObstacle(pRenderTarget);
	m_pObstacle[2] = new CObstacle(pRenderTarget);
	m_pObstacle[3] = new CObstacle(pRenderTarget);
	if (m_pObstacle) 
	{
		m_pObstacle[0]->Reset(150.0f, 480.0f, 50.0f, 1);
		m_pObstacle[1]->Reset(150.0f, 480.0f, 50.0f, 2);
		m_pObstacle[2]->Reset(450.0f, 480.0f, 50.0f, 3);
		m_pObstacle[3]->Reset(450.0f, 480.0f, 50.0f, 4);
	}

	pRenderTarget->Release();

	// �u���b�N������
	m_iBlockRows = kNumRows;
	m_iBlockCols = kNumCols;
	m_iBlockLeft = kDefaultLeft;
	m_iBlockRight = kDefaultRight;
	m_iBlockTop = kDefaultTop;
	m_iBlockBottom = kDefaultBottom;

	m_pBlocks = new BYTE[m_iBlockRows*m_iBlockCols];
	for (int row = 0; row < m_iBlockRows; ++row)
	{
		for (int col = 0; col < m_iBlockCols; ++col)
		{
			m_pBlocks[m_iBlockCols*row + col] = 1;
		}
	}

	// �u���b�N�����@
	// 1�s��
	m_pBlocks[0] = m_pBlocks[1] = m_pBlocks[3] = m_pBlocks[4] = m_pBlocks[5] = 0;
	m_pBlocks[6] = m_pBlocks[7] = m_pBlocks[9] = m_pBlocks[10] = 0;
	// 2�s��
	m_pBlocks[11] = m_pBlocks[12] = m_pBlocks[13] = m_pBlocks[15] = m_pBlocks[16] = 0;
	m_pBlocks[17] = m_pBlocks[19] = m_pBlocks[20] = m_pBlocks[21] = 0;
	// 3�s��
	m_pBlocks[22] = m_pBlocks[23] = m_pBlocks[31] = m_pBlocks[32] = 0;
	// 4�s��
	m_pBlocks[33] = m_pBlocks[36] = m_pBlocks[40] = m_pBlocks[43] = 0;
	// 6�s��
	m_pBlocks[56] = m_pBlocks[64] = 0;
	// 7�s��
	m_pBlocks[67] = m_pBlocks[69] = m_pBlocks[70] = m_pBlocks[71] = 0;
	m_pBlocks[72] = m_pBlocks[73] = m_pBlocks[75] = 0;
	// 8�s��
	m_pBlocks[77] = m_pBlocks[78] = m_pBlocks[79] = m_pBlocks[82] = 0;
	m_pBlocks[85] = m_pBlocks[86] = m_pBlocks[87] = 0;

	// �t�F�[�h�A�E�g������
	m_iFadeTimer = 0;
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);

	// �X�R�A�^�C�}�[ 1�b�ȓ��Ƀu���b�N�������ƃX�R�A2�{
	m_iScoreTimer = 0;

	//�{�[���^�C�}�[�@���Z�b�g��2�b�ԃ{�[�����~������
	m_iStartBallTimer = 0;
}

//================================================================================
// �f�X�g���N�^
//================================================================================
CStage::~CStage() 
{
	SAFE_DELETE(m_pBall);
	SAFE_RELEASE(m_pBrush);
	SAFE_DELETE(m_pRacket);
	for (int i = 0; i < 4; i++) 
	{
		SAFE_DELETE(m_pObstacle[i]);
	}
}

//================================================================================
// �u���b�N�ƃ{�[���Ƃ̓����蔻��
//================================================================================
void CStage::CollideBlocksAndTheBall() 
{
	INT areaL = m_iBlockLeft;
	INT areaT = m_iBlockTop;
	INT areaW = m_iBlockRight - m_iBlockLeft;
	INT areaH = m_iBlockBottom - m_iBlockTop;
	INT cols = m_iBlockCols;
	INT rows = m_iBlockRows;
	float left, top, right, bottom;
	for (int row = 0; row < m_iBlockRows; ++row) 
	{
		for (int col = 0; col < cols; ++col) 
		{
			if (m_pBlocks[m_iBlockCols*row + col] != 0) 
			{
				left = col * areaW / cols + areaL;
				top = row * areaH / rows + areaT;
				right = (col + 1) * areaW / cols + areaL;
				bottom = (row + 1) * areaH / rows + areaT;
				if (m_pBall->Collide(left, top, right, bottom)) 
				{
					// �_���a:m_pBlocks[]��1���Z�b�g
					m_pBlocks[m_iBlockCols*row + col] |= kToDeleteFlag;
				}
			}
		}
	}
	int num_blocks = 0;
	for (int row = 0; row < m_iBlockRows; ++row) 
	{
		for (int col = 0; col < cols; ++col) 
		{
			// �_����:�_���a��1���Z�b�g�����ꍇ(�Փ�)m_pBlocks[]��0����
			if (0 != (m_pBlocks[m_iBlockCols*row + col] & kToDeleteFlag)) 
			{
				m_pBlocks[m_iBlockCols*row + col] = 0;
				if (m_pScoreUI) 
				{
					// �����̗����̃u���b�N�@�X�R�A10���Z
					if (row < m_iBlockRows / 2) 
					{
						// 1�b�ȓ��Ƀu���b�N�������ƃX�R�A2�{
						if (m_iScoreTimer / 60 <= 0)
							m_pScoreUI->AddScore(20);
						else
							m_pScoreUI->AddScore(10);
					}
					else 
					{
						if (m_iScoreTimer / 60 <= 0)
							m_pScoreUI->AddScore(10);
						else
							m_pScoreUI->AddScore(5);
					}

					// �X�R�A�^�C�}�[������
					m_iScoreTimer = 0;
				}
			}
			else if (m_pBlocks[m_iBlockCols*row + col]) 
			{
				++num_blocks;
			}
		}
	}

	// �Q�[���N���A
	if (num_blocks == 0 && m_iFadeTimer == 0) 
	{
		m_iFadeTimer = kFadeOutDuration;
		m_pScoreUI->SetGameClear(true);
	}

	// �Q�[���I�[�o�[
	if (m_pScoreUI->GetLife() < 0 && m_iFadeTimer == 0)
	{
		m_iFadeTimer = kFadeOutDuration;
		m_pScoreUI->SetGameClear(false);
	}
}


//================================================================================
// �A�j���[�V����
//================================================================================
GameSceneResultCode CStage::move() 
{
	if (m_pBall) 
	{
		// �{�[���A�j���[�V����
		if (!m_pBall->GetResetStart()) 
		{
			m_pBall->move();
		}
		else
		{
			m_iStartBallTimer++;
			if (m_iStartBallTimer / 60 > 2) 
			{
				m_pBall->SetResetStart(false);
				m_pScoreUI->SetLife(-1);
				m_iStartBallTimer = 0;
			}
		}
		// ���P�b�g�A�j���[�V����
		if (m_pRacket) 
		{
			m_pRacket->move();
		}

		// ��Q���A�j���[�V����
		if (m_pObstacle) 
		{
			m_pObstacle[0]->move();
			m_pObstacle[1]->move();
			m_pObstacle[2]->move();
			m_pObstacle[3]->move();
		}
		//�Փ˔���
		if (m_pBall->GetStartCollision())
		{
			if (m_pRacket)
			{
				m_pRacket->Collide(m_pBall);
			}
			if (m_pObstacle)
			{
				m_pObstacle[0]->Collide(m_pBall);	
				m_pObstacle[1]->Collide(m_pBall);
				m_pObstacle[2]->Collide(m_pBall);
				m_pObstacle[3]->Collide(m_pBall);
			}
			CollideBlocksAndTheBall();
		}
		m_pBall->Reflect();
	}
	// �X�R�A�A�j���[�V����
	if (m_pScoreUI) 
	{
		m_pScoreUI->move();
		m_iScoreTimer++;
	}
	//�t�F�[�h�A�E�g
	if (m_iFadeTimer > 0) 
	{
		--m_iFadeTimer;
		if (m_iFadeTimer == 0) 
		{
			return GameSceneResultCode::GAMESCENE_END_OK;
		}
	}
		
	return GameSceneResultCode::GAMESCENE_DEFAULT;
}

//================================================================================
// �`��
//================================================================================
void CStage::draw(ID2D1RenderTarget* pRenderTarget) 
{
	// �u���b�N�̕`��
	D2D1_RECT_F    rect;
	INT areaL = m_iBlockLeft;
	INT areaT = m_iBlockTop;
	INT areaW = m_iBlockRight - m_iBlockLeft;  
	INT areaH = m_iBlockBottom - m_iBlockTop; 
	INT cols = m_iBlockCols;
	INT rows = m_iBlockRows;
	for (int row = 0; row < m_iBlockRows; ++row) 
	{
		for (int col = 0; col < cols; ++col) 
		{
			rect.left = col * areaW / cols + areaL;
			rect.top = row * areaH / rows + areaT;
			rect.right = (col + 1) * areaW / cols + areaL;
			rect.bottom = (row + 1) * areaH / rows + areaT;
			rect.right -= 2;
			rect.bottom -= 2;
			if (m_pBlocks[m_iBlockCols*row + col] != 0)
				pRenderTarget->FillRectangle(rect, m_pBrush);
		}
	}
	// �{�[���̕`��
	if (m_pBall)
		m_pBall->draw(pRenderTarget);

	// ���P�b�g�̕`��
	if (m_pRacket)
		m_pRacket->draw(pRenderTarget);

	// ��Q���̕`��
	if (m_pObstacle) 
	{
		m_pObstacle[0]->draw(pRenderTarget);
		m_pObstacle[1]->draw(pRenderTarget);
		m_pObstacle[2]->draw(pRenderTarget);
		m_pObstacle[3]->draw(pRenderTarget);
	}

	if (m_pScoreUI)
		m_pScoreUI->draw(pRenderTarget);

	// �t�F�[�h�A�E�g
	if (m_iFadeTimer > 0) 
	{
		float opacity = kFadeOutDuration - m_iFadeTimer;
		opacity /= kFadeOutDuration;
		rect.left = 0;
		rect.top = 0;
		rect.right = 720;
		rect.bottom = 960;
		m_pBlackBrush->SetOpacity(opacity);
		pRenderTarget->FillRectangle(rect, m_pBlackBrush);
	}
}