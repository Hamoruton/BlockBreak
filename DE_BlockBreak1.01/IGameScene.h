#pragma once
#ifndef __IGAMESCENE_H_INCLUDED__
#define __IGAMESCENE_H_INCLUDED__

enum GameSceneResultCode;
struct ID2D1RenderTarget;

class IGameScene
{
public:
	// èÉêàâºëzä÷êî
	virtual ~IGameScene() = 0;	
	virtual GameSceneResultCode	move() = 0;		//GAME_DEFAULT = 0
	virtual void draw(ID2D1RenderTarget* pRenderTarget) = 0;
};

#endif // !__IGAMESCENE_H_INCLUDED__