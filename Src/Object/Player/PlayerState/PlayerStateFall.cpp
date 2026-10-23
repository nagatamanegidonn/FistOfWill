
#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../../Manager/General/SceneManager.h"
#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"
#include "../../Common/InputController.h"

#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateFall.h"
#include "../../../Utility/Utility.h"

namespace
{
	static const std::wstring ANIM_FALL = L"Fall";
}

PlayerStateFall::PlayerStateFall(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	stepJump_ = 0.0f;
	gravityPow_ = VECTOR();
	m_State = STATE::FAll;
}

void PlayerStateFall::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_FALL);
}

void PlayerStateFall::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}



	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());

	// アニメーションの切り替え
	chara.lock()->GetAnimController().lock()->Play(ANIM_FALL);

	if (chara.lock()->GetIsGround())
	{
		//ChangeState(std::make_shared<PlayerStateWalk>(owner_));
		ChangeState(std::make_shared<PlayerStateRun>(owner_));
	}


	// 重力方向
	VECTOR dirGravity = Utility::DIR_D;
	// 加速度的に落下していく
	gravityPow_ = VAdd(gravityPow_, VScale(dirGravity, SceneManager::GetInstance().GetDeltaTime() * 9.8f));
	player.lock()->AddJumpPow(gravityPow_);
}

void PlayerStateFall::Exit()
{
}