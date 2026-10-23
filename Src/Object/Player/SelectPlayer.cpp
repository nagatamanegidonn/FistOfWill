
#include <memory>
#include <fstream>
#include "../Common/InputController.h"
#include "../Common/AnimationController.h"

#include "../../Manager/General/SceneManager.h"

#include "../Component/ComponentBase.h"

#include "../State/CharacterStateNone.h"

#include "../OnHit/OnHitBase.h"
#include "../OnHit/PlayerOnHit.h"

#include "../Common/Collider/Collider.h"

#include "../../Application.h"

#include "SelectPlayer.h"

SelectPlayer::SelectPlayer(void)
	:
	PlayerBase(-1)
{
}

SelectPlayer::~SelectPlayer(void)
{
}

void SelectPlayer::Init(void)
{
	// コントローラーの登録
	inputController_ = std::make_shared<InputController>(SceneManager::GetInstance().GetControllId());

	// 基底クラスの初期化
	PlayerBase::Init();

	// コンポーネントの初期化
	for (const auto& component : component_)
	{
		component->Init();
	}

	// 当たり判定後処理
	onHit_ = std::make_shared<PlayerOnHit>(*this);

	std::wstring path = Application::PATH_MODEL + L"Player2/";
	animationController_->Set(L"Sit", path + L"Sitting.mv1", 30.0f);
	animationController_->Play(L"Sit");
	animationController_->Update();

	// プレイヤー状態の初期化
	auto self = std::dynamic_pointer_cast<PlayerBase>(shared_from_this());
	state_ = std::make_shared<CharacterStateNone>(self);
	state_->Init();

	transform_.pos = VECTOR(0.0f, -50.0f, 0.0f);
	transform_.quaRot =
		Quaternion::Euler({ 0.0f, Utility::Deg2RadF(-145.0f), 0.0f });

	transform_.Update();
}

void SelectPlayer::Update(void)
{
	// コントローラーの更新
	inputController_->Update();

	// 基底クラスの更新
	PlayerBase::Update();

	isGround_ = false;
	isJust_ = false;
}

void SelectPlayer::Draw(void)
{
	// 基底クラスの描画
	PlayerBase::Draw();
}

void SelectPlayer::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	// 衝突時の処理を呼び出す
	//onHit_->Update(_hitCol);
}

void SelectPlayer::UpdateRotate(void)
{
}
