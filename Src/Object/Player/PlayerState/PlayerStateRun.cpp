
#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"
#include "../../Common/InputController.h"

#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateStep.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDamage.h"

namespace
{
	static const std::wstring ANIM_RUN = L"Run";
}

PlayerStateRun::PlayerStateRun(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::RUN;
}

void PlayerStateRun::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
}

void PlayerStateRun::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_RUN);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	const std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());

	if (chara.lock()->GetIsReaction())
	{
		ChangeState(std::make_shared<PlayerStateDamage>(owner_));
		return;
	}

	if (player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::SKILL))
	{
		ChangeState(std::make_shared<PlayerStateAttack>(owner_, 3));
	}
	else if (player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::ATTACK))
	{
		ChangeState(std::make_shared<PlayerStateAttack>(owner_));
	}
	// 移動量が0ならIdleへ
	else if (chara.lock()->GetMovePow().x == 0.0f && chara.lock()->GetMovePow().z == 0.0f)
	{
		ChangeState(std::make_shared<PlayerStateIdle>(owner_));
	}
	else if (!player.lock()->GetIsGround()
		|| player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::JUMP))
	{
		ChangeState(std::make_shared<PlayerStateJump>(owner_));
	}
	else if (player.lock()->GetIsGround()
		&& player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::STEP))
	{
		ChangeState(std::make_shared<PlayerStateStep>(owner_));
	}
}

void PlayerStateRun::Exit()
{
}