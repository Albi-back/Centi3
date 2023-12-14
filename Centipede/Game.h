#pragma once

#include <vector>

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Model.h"
#include "Timer.h"
#include "Input.h"
#include "Singleton.h"



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
	
	
	
	void Init();
private:
	
	

};
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
	void InitBullet();
};

class GameOver
{
public:
	GameOver(MyD3D& d3d);
	int counter = 0;
	void Update(float dTime);
	void Render(float dTime, DirectX::SpriteBatch& batch);
	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	float gAngle = 0;
	
	void Init();
private:
	


};


/*
Basic wrapper for a game
*/
class Game : public Singleton<Game>
{
public:
	enum class State { START, PLAY, GAME_OVER };
	MouseAndKeys input;
	State state = State::START;
	Game(MyD3D& d3d);
	
	void Release();
	LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Update(float dTime);
	void Render(float dTime);
	std::vector<Model> mModels;
	enum Modelid { LOGO, TITLE, START, EXIT, OVER, TOTAL = 5 };
private:
	MyD3D& mD3D;
	DirectX::SpriteBatch* mpSB = nullptr;
	//not much of a game, but this is it
	StartScreen mSMode;
	PlayMode mPMode;
	GameOver mGMode;

};


