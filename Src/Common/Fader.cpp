#include "Fader.h"

#include <DxLib.h>

#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"

#include "../Manager/Resource/ResourceManager.h"
#include "../Manager/General/SceneManager.h"
#include "../Application.h"

Fader::Fader(void)
	:
	state_(NET_STATE::NONE),
	alpha_(0.0f),
	isPreEnd_(true),
	isEnd_(true)
{
}

// デストラクタ
Fader::~Fader(void)
{
}

Fader::NET_STATE Fader::GetState(void) const
{
	return state_;
}

bool Fader::IsEnd(void) const
{
	return isEnd_;
}

void Fader::SetFade(NET_STATE state)
{
	state_ = state;

	if (state_ != NET_STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Init(void)
{
	state_ = NET_STATE::FADE_STOP;
	alpha_ = 0;
	isPreEnd_ = true;
	isEnd_ = true;

	// マテリアルの生成
	material_ = std::make_unique<PixelMaterial>(L"Fade.cso", 1);

	// テクスチャの設定
	fadeImage_ = ResourceManager::GetInstance().Load(L"FadeImage").handleId_;
	material_->AddTextureBuf(fadeImage_);

	// バッファーの設定
	material_->AddConstBuf(FLOAT4{ 0.0f, 0.0f,0.0f, 0.0f });

	// レンダラーの生成
	renderer_ = std::make_unique<PixelRenderer>(*material_);

	// メッシュ生成
	renderer_->MakeSquereVertex(Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2)
		, Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y));
}

void Fader::Update(void)
{
	if (isEnd_)
	{
		return;
	}

	switch (state_)
	{
	case NET_STATE::NONE:
		return;
	case NET_STATE::FADE_OUT:

		alpha_ += SPEED_ALPHA;

		if (alpha_ >= MAX_ALPHA)
		{
			alpha_ = MAX_ALPHA;
			isEnd_ = true;
		}
		break;
	case NET_STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA;

		if (alpha_ <= 0.0f)
		{
			alpha_ = 0.0f;
			isEnd_ = true;
		}
		break;
	case NET_STATE::FADE_STOP:
		break;
	}
}

void Fader::Draw(void)
{
	if (state_ == NET_STATE::NONE)
	{
		return;
	}

	float rate = alpha_ / MAX_ALPHA;

	material_->SetConstBuf(0, FLOAT4{ 0.0f, 0.0f, 0.0f, rate });

	// 描画処理
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	renderer_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
