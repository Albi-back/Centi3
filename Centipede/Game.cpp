#include "Game.h"
#include "WindowUtils.h"
#include "CommonStates.h"
#include "GeometryBuilder.h"
#include "D3D.h"
#include "Timer.h"


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
	: mD3D(d3d), mpSB(nullptr), mSMode(d3d), mPMode(d3d)
{
	BuildCube(d3d.GetMeshMgr());
	mpSB = new SpriteBatch(&mD3D.GetDeviceCtx());
	mSMode.Init();
}


//any memory or resources we made need releasing at the end
void Game::Release()
{
	delete mpSB;
	mpSB = nullptr;
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
	}


	mpSB->End();

	mD3D.EndRender();
}


PlayMode::PlayMode(MyD3D& d3d)
	:mD3D(d3d), mPlayer(d3d)
{
	InitBgnd();
	InitPlayer();
}


void PlayMode::Update(float dTime)
{
	int i = 0;
	for (auto& s : mBgnd)
		s.Scroll(-0, dTime * (i++) * SCROLL_SPEED);
}
RECTF rect;
void PlayMode::Render(float dTime, DirectX::SpriteBatch& batch)
{

	for (auto& s : mBgnd)
		s.Draw(batch);
	mPlayer.Draw(batch);
}

StartScreen::StartScreen(MyD3D& d3d)
{
	
}

void StartScreen::Update(float dTime)
{
	
	
	if (mModels[Modelid::LOGO].GetRotation().x>-(PI*2))
	{ 
		spun = false;
		gAngle += dTime * 2.f;
		mModels[Modelid::LOGO].GetRotation().x = -gAngle;
		
	}
	else
	{
		spun = true;
	}
	
	counter++;
		

		
	

}

void StartScreen::Render(float dTime, DirectX::SpriteBatch& batch)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
	CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	//d3d.BeginRender(Colours::Black);
	
	if(!spun)
        d3d.GetFX().Render(mModels[Modelid{LOGO}]);
	
	else if (spun)
		d3d.GetFX().Render(mModels[Modelid{TITLE}]);
	
	
	
	//Model cube;
	//cube.Initialise(d3d.GetMeshMgr().GetMesh("Cube"));
	//d3d.GetFX().Render(cube);
	//cube.GetRotation() = Vector3(2, 4, 7);
	//cube.GetPosition() = Vector3(0, 0, 10);
	//d3d.EndRender();
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
void StartScreen::Init()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	Mesh& logoMesh = d3d.GetMeshMgr().GetMesh("Cube");
	Mesh& cb = d3d.GetMeshMgr().CreateMesh("logo");
	cb.CreateFrom("data/logo.fbx", d3d);
	mModels.push_back(Model());
	Setup(mModels[Modelid{LOGO}], cb, 0.25f, Vector3(-0.65, -0.f, 0), Vector3(0, 0, 0));
	
	Mesh& TT = d3d.GetMeshMgr().CreateMesh("TITLE");
	TT.CreateFrom("data/title.fbx", d3d);
	mModels.push_back(Model());
	Setup(mModels[Modelid{TITLE}], TT, 0.25f, Vector3(-0.5, 1.8f, 0), Vector3(0, 0, 0));
}
void StartScreen::InitMenu()
{

}



