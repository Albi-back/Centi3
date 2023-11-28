#pragma once
#include "SpriteBatch.h"
#include "D3D.h"


class Sprite;

/*
Animate by flicking through a series of sub-rectangle in an texture atlas
*/
class Animate {
private:
	int mStart = 0, mStop = 0, mCurrent = 0; //start, stop and current frame of animation
	float mRateSec = 0;		//how fast to play back
	float mElapsedSec = 0;	//how long the current frame has been on screen
	bool mLoop = false;		//loop at the end ?
	bool mPlay = false;		//should we be playing right now
	Sprite& mSpr;			//the parent sprite

public:
	Animate(Sprite& spr)
		:mSpr(spr)
	{}
	/*
	start	- frameID starting at zero
	stop	- final frameID
	rate	- playback speed
	loop	- loop at the end?
	*/
	void Init(int _start, int _stop, float _rate, bool _loop);
	//choose the frame
	void Update(float _elapsedSec);
	//start and stop
	void Play(bool go) {
		mPlay = go;
	}
	Animate& operator=(const Animate& rhs);
};

/*
Wrap up sprite rendering and movement
*/
class Sprite
{
private:
	ID3D11ShaderResourceView *mpTex;
	MyD3D& mD3D;
	RECTF mTexRect;
	DirectX::SimpleMath::Vector2 scale;
	const TexCache::Data *mpTexData;
	Animate mAnim;

public:
	DirectX::SimpleMath::Vector2 mPos;
	DirectX::SimpleMath::Vector2 mVel;
	float depth;
	DirectX::SimpleMath::Vector4 colour;
	float rotation;
	DirectX::SimpleMath::Vector2 origin;

	Sprite(MyD3D& d3d)
		:mPos(0, 0), mVel(0, 0),
		depth(0), mTexRect{ 0,0,0,0 }, colour(1, 1, 1, 1),
		rotation(0), scale(1, 1), origin(0, 0), mpTex(nullptr),
		mD3D(d3d), mAnim(*this)
	{}
	Sprite(const Sprite& rhs)
		:mD3D(rhs.mD3D), mAnim(*this)
	{
		(*this) = rhs;
	}
	Sprite& operator=(const Sprite& rhs);
	
	//sprite is drawn using batch parameter
	void Draw(DirectX::SpriteBatch& batch);
	//change texture, optional rectf can isolate part of the texture
	void SetTex(ID3D11ShaderResourceView& tex, const RECTF& texRect = RECTF{ 0,0,0,0 });
	//change which part later
	void SetTexRect(const RECTF& texRect);
	void Scroll(float x, float y);

	void SetFrame(int id);
	Animate& GetAnim() {
		return mAnim;
	}


	//getters
	const TexCache::Data& GetTexData() const {
		assert(mpTexData);
		return *mpTexData;
	}
	ID3D11ShaderResourceView& GetTex() {
		assert(mpTex);
		return *mpTex;
	}
	void SetScale(const DirectX::SimpleMath::Vector2& s) {
		scale = s;
	}
	const DirectX::SimpleMath::Vector2& GetScale() const {
		return scale;
	}
	DirectX::SimpleMath::Vector2 GetScreenSize() const {
		assert(mpTexData);
		return scale * mpTexData->dim;
	}
};


