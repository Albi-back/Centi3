#pragma once

#include <vector>

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Model.h"
#include "Timer.h"
#include "Input.h"
#include "Singleton.h"


class PlayMode
{
public:
	PlayMode(MyD3D& d3d);
	void Update(float dTime);
	void Render(float dTime, DirectX::SpriteBatch& batch);
	float getRadius();
	void movement(float dtime);
	
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
	int counter = 0;
	bool spun = false;
	void Update(float dTime);
	void Render(float dTime, DirectX::SpriteBatch& batch);
	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	float gAngle = 0;
	std::vector<Model> mModels;
	void initMouse();
	enum Modelid { LOGO, TITLE,START,EXIT, TOTAL = 4 };
	void Init();
private:
	void InitMenu();
	

};


/*
Basic wrapper for a game
*/
class Game : public Singleton<Game>
{
public:
	enum class State { START, PLAY };
	static MouseAndKeys input;
	State state = State::START;
	Game(MyD3D& d3d);
	
	void Release();
	LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Update(float dTime);
	void Render(float dTime);
private:
	MyD3D& mD3D;
	DirectX::SpriteBatch* mpSB = nullptr;
	//not much of a game, but this is it
	StartScreen mSMode;
	PlayMode mPMode;

};


