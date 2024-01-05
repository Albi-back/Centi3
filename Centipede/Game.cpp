#include "Game.h"
#include "WindowUtils.h"
#include "CommonStates.h"
#include "GeometryBuilder.h"
#include "D3D.h"
#include "Timer.h"
#include "Input.h"
#include "WindowUtils.h"



using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)
{
	m.Initialise(source);
	m.GetScale() = scale;
	m.GetPosition() = pos;
	m.GetRotation() = rot;
}
void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot)
{
	Setup(m, source, Vector3(scale, scale, scale), pos, rot);
}
Game::Game(MyD3D& d3d)
	: mD3D(d3d), mpSB(nullptr), mSMode(d3d), mPMode(d3d), mGMode(d3d)
{
	BuildCube(d3d.GetMeshMgr());
	mpSB = new SpriteBatch(&mD3D.GetDeviceCtx());
	mSMode.Init();
	
	input.Initialise(WinUtil::Get().GetMainWnd(), true, true);
	mGMode.Init();
}


//any memory or resources we made need releasing at the end
void Game::Release()
{
	delete mpSB;
	mpSB = nullptr;
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INPUT:
		input.MessageEvent((HRAWINPUT)lParam);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'e':
		case 'E':
			state = State::PLAY;
			return 0;

		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		case 'm':
		case 'M':
			state = State::GAME_OVER;
			return 0;
		}
		break;
	}

	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

//called over and over, use it to update game logic
void Game::Update(float dTime)
{

	switch (state)
	{
	case State::START:
		mSMode.Update(dTime);
		break;
	case State::PLAY:
		mPMode.Update(dTime);
		break;
	case State::GAME_OVER:
		mPMode.Update(dTime);
		break;
	}
}

//called over and over, use it to render things
void Game::Render(float dTime)
{
	mD3D.BeginRender(Colours::Black);


	CommonStates dxstate(&mD3D.GetDevice());
	mpSB->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &mD3D.GetWrapSampler());

	switch (state)
	{

	case State::START:
		mSMode.Render(dTime, *mpSB);
		break;
	case State::PLAY:
		mPMode.Render(dTime, *mpSB);
		break;
	case State::GAME_OVER:
		mGMode.Render(dTime, *mpSB);
		break;
	}


	mpSB->End();

	mD3D.EndRender();
	input.PostProcess();
}


PlayMode::PlayMode(MyD3D& d3d)
	:mD3D(d3d), mPlayer(d3d),mBullet(d3d)
{
	InitBgnd();
	InitPlayer();
	InitBullet();
}


void PlayMode::Update(float dTime)
{
	Vector2 mousePos = Game::Get().input.GetMousePos(true);



	if (mousePos.x > (WinUtil::Get().GetClientWidth() - getRadius()))   //Makes the ship not leave the screen
	{
		mPlayer.mPos.x = WinUtil::Get().GetClientWidth() - getRadius();
	}
	else if (mousePos.x < getRadius()) //Makes the ship not leave the screen
	{
		mPlayer.mPos.x = getRadius();
	}
	else
	{
		mPlayer.mPos.x = mousePos.x;
	}
	
	

	int i = 0;
	for (auto& s : mBgnd)
	{
		s.Scroll(-0, dTime * (i++) * SCROLL_SPEED);

	}
	
		
	
}

RECTF rect;
bool bullshot= false;
void PlayMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	
	for (auto& s : mBgnd)
		s.Draw(batch);
	mPlayer.Draw(batch);
	if (Game::Get().input.GetMouseButton(MouseAndKeys::LBUTTON))
	{
		bullshot = true;
	}
	if (bullshot)
	{
		PlayMode* shoots = new PlayMode(mD3D);
		shoots->mBullet.Draw(batch);
		shoots->mBullet.mPos.y +=  3* dTime;
		
		
	}
}

float PlayMode::getRadius()
{
	return mPlayer.GetScale().x * (mPlayer.GetTexData().dim.x / 2);
	//Return radius;
}

StartScreen::StartScreen(MyD3D& d3d)
{
	
}

void StartScreen::Update(float dTime)
{
	
	
	if (Game::Get().mModels[Game::Modelid::LOGO].GetRotation().x>-(PI * 2))
	{ 
		spun = false;
		gAngle += dTime * 2.f;
		Game::Get().mModels[Game::Modelid::LOGO].GetRotation().x = -gAngle;
		
	}
	else
	{
		spun = true;
	}
	
	counter++;
	
	


		
	

}
GameOver::GameOver(MyD3D& d3d)
{
}
void GameOver::Update(float dTime)
{
	gAngle += dTime * 2.f;
	Game::Get().mModels[Game::Modelid::OVER].GetRotation().x = -gAngle;
}
void StartScreen::Render(float dTime, DirectX::SpriteBatch& batch)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
	CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(1.47f, 1.47f, 1.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	//d3d.BeginRender(Colours::Black);
	
	if(!spun)
        d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::LOGO]);
	
	else if (spun)
	{		
        d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::TITLE]);
		d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::START]);
		d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::EXIT]);
		d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::TV]);
	}
	
	
		

	
	
	
	
}

void GameOver::Render(float dTime, DirectX::SpriteBatch& batch)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
	CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(1.47f, 1.47f, 1.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::OVER]);
	d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::START]);
	d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::EXIT]);
	d3d.GetFX().Render(Game::Get().mModels[Game::Modelid::CART]);

}

void PlayMode::InitBgnd()
{
	//a sprite for each layer
	assert(mBgnd.empty());
	mBgnd.insert(mBgnd.begin(), BGND_LAYERS, Sprite(mD3D));



	pair<string, string> files[BGND_LAYERS]{
		{ "bgnd0","backgroundlayers/background_1.dds" },
		{ "bgnd1","backgroundlayers/background_2.dds" },
		{ "bgnd2","backgroundlayers/background_3.dds" },
		{ "bgnd3","backgroundlayers/background_4.dds" },


	};
	int i = 0;
	for (auto& f : files)
	{
		//set each texture layer
		ID3D11ShaderResourceView* p = mD3D.GetCache().LoadTexture(&mD3D.GetDevice(), f.second, f.first);
		if (!p)
			assert(false);
		mBgnd[i++].SetTex(*p);
	}


}
void PlayMode::InitPlayer()
{
	ID3D11ShaderResourceView* p = mD3D.GetCache().LoadTexture(&mD3D.GetDevice(), "spaceship.dds");
	mPlayer.SetTex(*p);
	mPlayer.SetScale(Vector2(3.f, 3.f));

	mPlayer.origin = mPlayer.GetTexData().dim / 2.f;
	//mPlayer.rotation = PI / 2.f;
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mPlayArea.left = mPlayer.GetScreenSize().x * 0.6f;
	mPlayArea.top = mPlayer.GetScreenSize().y * 0.6f;
	mPlayArea.right = w - mPlayArea.left;
	mPlayArea.bottom = h * 0.75f;
	mPlayer.mPos = Vector2(mPlayArea.left + mPlayer.GetScreenSize().x * 2.f, (mPlayArea.bottom - mPlayArea.top) * 1.3f);
}
void PlayMode::InitBullet()
{
	ID3D11ShaderResourceView* p = mD3D.GetCache().LoadTexture(&mD3D.GetDevice(), "bullet.dds");
	mBullet.SetTex(*p);
	mBullet.SetScale(Vector2(2.f, 2.f));

	mBullet.origin = mBullet.GetTexData().dim / 2.f;
	//mPlayer.rotation = PI / 2.f;
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mPlayArea.left = mBullet.GetScreenSize().x * 0.6f;
	mPlayArea.top = mBullet.GetScreenSize().y * 0.6f;
	mPlayArea.right = w - mPlayArea.left;
	mPlayArea.bottom = h * 0.75f;
	mBullet.mPos = mPlayer.mPos;//Vector2(mPlayArea.left + mBullet.GetScreenSize().x * 2.f, (mPlayArea.bottom - mPlayArea.top) * 1.3f);
	
}
/*void PlayMode::InitBullet()
{
	ID3D11ShaderResourceView* b = mD3D.GetCache().LoadTexture(&mD3D.GetDevice(), "bullet.dds");
	mPlayer.SetTex(*b);
	mPlayer.SetScale(Vector2(3.f, 3.f));
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mPlayArea.left = mPlayer.GetScreenSize().x * 0.6f;
	mPlayArea.top = mPlayer.GetScreenSize().y * 0.6f;
	mPlayArea.right = w - mPlayArea.left;
	mPlayArea.bottom = h * 0.75f;
	mPlayer.mPos = Vector2(mPlayArea.left + mPlayer.GetScreenSize().x * 2.f, (mPlayArea.bottom - mPlayArea.top) * 1.3f);
}*/
void StartScreen::Init()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	Mesh& logoMesh = d3d.GetMeshMgr().GetMesh("Cube");
	Mesh& cb = d3d.GetMeshMgr().CreateMesh("logo");
	cb.CreateFrom("data/logo.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::LOGO], cb, 0.25f, Vector3(-0.65, -0.f, 0), Vector3(0, 0, 0));
	
	Mesh& TT = d3d.GetMeshMgr().CreateMesh("TITLE");
	TT.CreateFrom("data/title.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::TITLE], TT, 0.25f, Vector3(-0.5, 1.8f, 0), Vector3(0.25, 0, 0));

	Mesh& sT = d3d.GetMeshMgr().CreateMesh("start");
	sT.CreateFrom("data/Start.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::START], sT, 0.25f, Vector3(-0.45, 0.5, 0), Vector3(0.25, 0, 0));

	Mesh& ET = d3d.GetMeshMgr().CreateMesh("exit");
	ET.CreateFrom("data/exit.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::EXIT], ET, 0.25f, Vector3(-0.45, -0.5, 0), Vector3(0.25, 0, 0));

	Mesh& dT = d3d.GetMeshMgr().CreateMesh("tv");
	dT.CreateFrom("data/TV.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::TV], dT, 1.25f, Vector3(0, -0.5, 1), Vector3(0, 90, 0));


}

void GameOver::Init()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	
	Mesh& cb = d3d.GetMeshMgr().CreateMesh("gameover");
	cb.CreateFrom("data/game_over.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::OVER], cb, 0.25f, Vector3(-0.5, 1, 0), Vector3(0, 0,0));

	Mesh& sp = d3d.GetMeshMgr().CreateMesh("3dspc");
	sp.CreateFrom("data/Space_invader.fbx", d3d);
	Game::Get().mModels.push_back(Model());
	Setup(Game::Get().mModels[Game::Modelid::CART], sp, 0.025f, Vector3(-0.5, -2, 2), Vector3(0, 90, 0));

}




