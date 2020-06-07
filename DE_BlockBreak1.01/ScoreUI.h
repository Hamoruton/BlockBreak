#pragma once
#ifndef __SCOREUI_H_INCLUDED__
#define __SCOREUI_H_INCLUDED__

struct	ID2D1RenderTarget;
struct ID2D1SolidColorBrush;
struct	IDWriteTextFormat;
class	CSelector;

//================================================================================
// スコアクラス
//================================================================================
class CScoreUI
{
public:
	CScoreUI(ID2D1RenderTarget* pRenderTarget);
	virtual	~CScoreUI();

	void move();
	void draw(ID2D1RenderTarget* pRenderTarget);

	void SetScore(int score);
	int  GetScore() const;
	void AddScore(int value);
	int GetResultScore(int score, int life);

	void SetLife(int life);
	int  GetLife() const;

	void ScoreReset();

	bool GetGameClear() const { return m_bGameClear; }
	void SetGameClear(bool flag) { m_bGameClear = flag; }

protected:
	ID2D1SolidColorBrush*		m_pScoreBrush;
	ID2D1SolidColorBrush*		m_pLifeBrush;
	IDWriteTextFormat*			m_pTextFormat;
	int							m_iScore;
	int							m_iResultScore;
	int							m_iLife;
	bool						m_bGameClear;
};

#endif // !__SCOREUI_H_INCLUDED__