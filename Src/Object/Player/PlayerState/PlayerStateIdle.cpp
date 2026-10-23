
#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../CharaStatus.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"
#include "../../Common/InputController.h"

#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateStep.h"
#include "PlayerStateFall.h"
#include "PlayerStateDead.h"
#include "PlayerStateDamage.h"

namespace
{
	static const std::wstring ANIM_IDLE = L"Idle";
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void PlayerStateIdle::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
}

void PlayerStateIdle::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_IDLE);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());

	if(player.lock()->GetInputController().lock()->IsNew(InputController::KEY::DASH))
	{
		player.lock()->SetDash(true);
	}
	if (chara.lock()->GetIsReaction())
	{
		ChangeState(std::make_shared<PlayerStateDamage>(owner_));
		return;
	}

	if((chara.lock()->GetMovePow().x != 0.0f || chara.lock()->GetMovePow().z != 0.0f)
		/*&& chara.lock()->IsDash()*/)
	{
		ChangeState(std::make_shared<PlayerStateRun>(owner_));
	}
	else if (player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::ATTACK))
	{
		ChangeState(std::make_shared<PlayerStateAttack>(owner_));
	}
	else if (player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::SKILL))
	{
		ChangeState(std::make_shared<PlayerStateAttack>(owner_, 3));
	}
	else if (!player.lock()->GetIsGround())
	{
		ChangeState(std::make_shared<PlayerStateFall>(owner_));
	}
	else if (player.lock()->GetIsGround()
		&& player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::STEP))
	{
		ChangeState(std::make_shared<PlayerStateStep>(owner_));
	}
	else if (chara.lock()->GetCharaStatus()->GetNowHP() <= 0)
	{
		ChangeState(std::make_shared<PlayerStateDead>(owner_));
	}
}

void PlayerStateIdle::Exit()
{
}