#include "CommonStates.h"


#include "Sprite.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Animate::Init(int _start, int _stop, float _rate, bool _loop)
{
	mElapsedSec = 0;
	mStart = _start;
	mStop = _stop;
	mRateSec = _rate;
	mLoop = _loop;
	mSpr.SetFrame(mStart);
	mCurrent = mStart;
}

void Animate::Update(float _elapsedSec)
{
	if (!mPlay)
		return;
	mElapsedSec += _elapsedSec;
	if (mElapsedSec > (1.f / mRateSec))
	{
		mElapsedSec = 0;
		mCurrent++;
		if (mCurrent > mStop)
		{
			if (mLoop)
				mCurrent = mStart;
			else
				mCurrent = mStop;
		}
		mSpr.SetFrame(mCurrent);
	}
}

Animate& Animate::operator=(const Animate& rhs)
{
	//needed because Animate has a reference to its sprite which cannot be copied
	mStart = rhs.mStart;
	mStop = rhs.mStop;
	mCurrent = rhs.mCurrent;
	mRateSec = rhs.mRateSec;
	mElapsedSec = rhs.mElapsedSec;
	mLoop = rhs.mLoop;
	mPlay = rhs.mPlay;
	return *this;
}


Sprite& Sprite::operator=(const Sprite& rhs) {
	//needed because it has a reference to d3d which cannot be copied
	mPos = rhs.mPos;
	mVel = rhs.mVel;
	depth = rhs.depth;
	mTexRect = rhs.mTexRect;
	colour = rhs.colour;
	rotation = rhs.rotation;
	scale = rhs.scale;
	origin = rhs.origin;
	mpTex = rhs.mpTex;
	mpTexData = rhs.mpTexData;
	mAnim = rhs.mAnim;
	return *this;
}
void Sprite::Draw(SpriteBatch& batch)
{
	batch.Draw(mpTex, mPos, &(RECT)mTexRect, colour, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, depth);
}
void Sprite::SetTex(ID3D11ShaderResourceView& tex, const RECTF& texRect)
{
	mpTex = &tex;
	mTexRect = texRect;
	mpTexData = &mD3D.GetCache().Get(mpTex);
	
	if (mTexRect.left == mTexRect.right && mTexRect.top == mTexRect.bottom)
	{
		SetTexRect(RECTF{ 0,0,mpTexData->dim.x,mpTexData->dim.y });
	}
}
void Sprite::SetTexRect(const RECTF& texRect) {
	mTexRect = texRect;
}
void Sprite::Scroll(float x, float y) {
	mTexRect.left += x;
	mTexRect.right += x;
	mTexRect.top += y;
	mTexRect.bottom += y;
}

void Sprite::SetFrame(int id) 
{
	const TexCache::Data& data = mD3D.GetCache().Get(mpTex);
	SetTexRect(data.frames.at(id));
}

