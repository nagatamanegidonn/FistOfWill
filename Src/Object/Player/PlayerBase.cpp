#include "../Common/AnimationController.h"
#include "../Common/InputController.h"
#include "../Common/SoundController.h"

#include "../Component/ComponentBase.h"

#include "../State/StateBase.h"
#include "PlayerState/PlayerStateIdle.h"
#include "PlayerState/PlayerStateRun.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Utility/Utility.h"

#include "../../Application.h"

#include "../CharaStatus.h"
#include "PlayerBase.h"


namespace {

}

PlayerBase::PlayerBase(int _key) 
	:
	CharaBase()
{
	inputController_ = nullptr;
}
PlayerBase::~PlayerBase(void)
{
}

void PlayerBase::Init(void)
{
	// モデルの基本設定
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"PlayerNight"));
	transform_.scl = Utility::VECTOR_ONE;
	// 初期座標
	transform_.pos = { 0.0f,-250,0.0f };
	transform_.quaRot = Quaternion();
		//Quaternion::Euler({ Utility::Deg2RadF(45.0f),0.0f, 0.0f });
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, Utility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	// アニメーションの設定
	animationController_ = std::make_shared<AnimationController>(transform_.modelId);

	// プレイヤー状態の初期化
	auto self = std::dynamic_pointer_cast<PlayerBase>(shared_from_this());
	state_ = std::make_shared<PlayerStateIdle>(self);
	state_->Init();
}

void PlayerBase::Update(void)
{
	// 追従対象の設定
	UpdateTarget();

	// 重力による移動量
	//CalcGravityPow();

	// 状態の更新
	state_->Update();
	// 状態の変化判定
	if (state_->GetNextState().lock() != nullptr && state_ != state_->GetNextState().lock())
	{
		//状態を変化する
		state_ = state_->GetNextState().lock();
		state_->Init();
	}

	// コンポーネントの後処理
	for (const auto& component : component_)
	{
		component->PostUpdate();
	}

	// 重力方向に沿って回転させる
	UpdateRotate();

	transform_.Update();
	// アニメーション再生
	animationController_->Update();
}

void PlayerBase::Draw(void)
{
	for (const auto& component : component_)
	{
		component->Draw();
	}

	MV1DrawModel(transform_.modelId);
}
