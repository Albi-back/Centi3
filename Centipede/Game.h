#pragma once

#include <vector>

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Model.h"


class PlayMode
{
public:
	PlayMode(MyD3D& d3d);
	void Update(float dTime);
	void Render(float dTime, DirectX::SpriteBatch& batch);

private:
	const float SCROLL_SPEED = 10.f;
	static const int BGND_LAYERS = 4;
	MyD3D& mD3D;
	Sprite mPlayer;
	RECTF mPlayArea;
	std::vector<Sprite> mBgnd; //paralax layers

	void InitBgnd();
	void InitPlayer();
	void Render1(float dTime, DirectX::SpriteBatch& batch);
};

class StartScreen
{
public:
	StartScreen(MyD3D& d3d);

	void Update(float dTime);
	void Render(float dTime, DirectX::SpriteBatch& batch);
	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	
	std::vector<Model> mModels;
	enum Modelid { LOGO,LOGO2, TOTAL = 2 };
private:
	void InitMenu();
	void InitLogo();

};


/*
Basic wrapper for a game
*/
class Game
{
public:
	enum class State { START, PLAY };

	State state = State::START;
	Game(MyD3D& d3d);

	void Release();
	void Update(float dTime);
	void Render(float dTime);
private:
	MyD3D& mD3D;
	DirectX::SpriteBatch* mpSB = nullptr;
	//not much of a game, but this is it
	StartScreen mSMode;
	PlayMode mPMode;

};


