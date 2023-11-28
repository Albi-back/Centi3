#include "Game.h"
#include "WindowUtils.h"
#include "CommonStates.h"
#include "GeometryBuilder.h"
#include "D3D.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;



Game::Game(MyD3D& d3d)
	: mD3D(d3d), mpSB(nullptr), mSMode(d3d), mPMode(d3d)
{

	mpSB = new SpriteBatch(&mD3D.GetDeviceCtx());
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
	case State::PLAY:
		mPMode.Render(dTime, *mpSB);
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
}

void StartScreen::Render(float dTime, DirectX::SpriteBatch& batch)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
	CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);

	d3d.BeginRender(Colours::Blue);
	d3d.GetFX().Render(mLogo);

	d3d.EndRender();
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
void StartScreen::InitLogo()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	mLogo.Initialise(BuildQuad(d3d.GetMeshMgr()));
	mLogo.GetRotation() = Vector3(0, 0, 0);
	mLogo.GetScale() = Vector3(3, 1, 3);
	mLogo.GetPosition() = Vector3(0, -1, 0);
	//change the defatult material inside the mesh
	Material& matQ = mLogo.GetMesh().GetSubMesh(0).material;
	matQ.gfxData.Set(Vector4(1.f, 0.4f, 0.1f, 1), Vector4(0.1f, 1.0f, 0.1f, 1), Vector4(0.9f, 0.8f, 0.8f, 1));
	matQ.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "floor.dds");
	matQ.texture = "floor.dds";
}
void StartScreen::InitMenu()
{

}



