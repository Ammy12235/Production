#include "ChangeEffectMover.h"
#include "Core/DEFINE.h"
#include "Core/Process/Fps.h"
#include "Graphics/Renderer/DirectX11/DIRECTWRITE.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

ChangeEffectMover::ChangeEffectMover()
{
	//texId = TEX_FAC.Load("focusHole.png");
}

ChangeEffectMover::~ChangeEffectMover()
{
	//TEX_FAC.Unload(texId);
}

void ChangeEffectMover::setSceneChangeEffect(const eSceneChangeEffect eSceneChangeEffect, float second)
{

	clear();

	fadeInterval = second;

	switch (eSceneChangeEffect)
	{
	case FadeIn:
		isFadeIn = true;
		isFadeOut = false;
		break;
	case FadeOut:
		isFadeOut = true;
		isFadeIn = false;
		break;
	default:
		break;
	}
}

bool ChangeEffectMover::getFadeInOutEnd(const eSceneChangeEffect eSceneChangeEffect, bool* isEnd)const
{
	switch (eSceneChangeEffect)
	{
	case FadeIn:
		if (isEnd != nullptr)*isEnd = isFadeInEnd;
		return isFadeInEnd;
	case FadeOut:
		if (isEnd != nullptr)*isEnd = isFadeOutEnd;
		return isFadeOutEnd;
	default:
		return false;
	}
}

bool ChangeEffectMover::getFadeInOutContinue(const eSceneChangeEffect eSceneChangeEffect, bool* isContinue)const
{
	switch (eSceneChangeEffect)
	{
	case FadeIn:
		if (isContinue != nullptr)*isContinue = isFadeIn;
		return  isFadeIn;
	case FadeOut:
		if (isContinue != nullptr)*isContinue = isFadeOut;
		return   isFadeOut;
	default:
		return false;
	}
}

void ChangeEffectMover::update()
{
	isFadeInEnd = false;
	isFadeOutEnd = false;

	if (fadeInterval == -1)return;

	//フェードアウト処理
	if (isFadeOut)
	{
		fadeTimer += 255 / Define::GameFPS / fadeInterval;
		alpha = fadeTimer / 255;
		if (alpha > 1)
		{
			alpha = 1;
			isFadeOutEnd = true;
			isFadeOut = false;
			fadeTimer = 0.0f;
			fadeInterval = -1;//更新停止
		}
	}

	//フェードイン処理
	if (isFadeIn)
	{
		fadeTimer += 255 / Define::GameFPS / fadeInterval;
		alpha = 1 - fadeTimer / 255;
		if (alpha < 0)
		{
			alpha = 0;
			isFadeInEnd = true;
			isFadeIn = false;
			fadeTimer = 0.0f;
			fadeInterval = -1;//更新停止
		}
	}

}

void ChangeEffectMover::draw()const
{

	DWRITE.DrawD2DBox(0, 0, Define::WIN_W+100, Define::WIN_H+100, D3D.GetColor(255, 255, 255), alpha, 0, 1);
	
	if (Define::Debug)
	{
		WCHAR fade[256];
		swprintf(fade, 256, L"FadeAlpha=%lf", alpha);
		DWRITE.DrawFormatText(fade, 0, 0, 100, 100, D3D.GetColor(255, 255, 255), 1, 1);
	}
}

void ChangeEffectMover::clear()
{
	isFadeOut = false;
	isFadeOutEnd = false;
	isFadeIn = false;
	isFadeInEnd = false;
	fadeTimer = 0.0f;
}
