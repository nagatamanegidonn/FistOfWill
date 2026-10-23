#include "PlayerStateDead.h"

#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"
// コンポーネント
#include "../../Component/MoveApplyComponent.h"
#include "../../Component/GoalRotateComponent.h"
#include "../../Component/ComponentBase.h"

#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

//#include "../Manager/General/SceneManager.h"
//#include "../Manager/System/Camera/CameraBase.h"
//#include "../Manager/System/Camera/FinishCamera.h"

namespace
{
	static const std::wstring ANIM_DEAD = L"Dead";
}

PlayerStateDead::PlayerStateDead(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void PlayerStateDead::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->SetIsReaction(false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) { move->SetActive(false); }
		std::shared_ptr<GoalRotateComponent> rota = std::dynamic_pointer_cast<GoalRotateComponent>(component);
		if (rota != nullptr) { rota->SetActive(false); }
	}
}

void PlayerStateDead::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_DEAD, false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());

	if (player.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		// 自身の削除
		player.lock()->Delete();
	}
}

void PlayerStateDead::Exit()
{
}