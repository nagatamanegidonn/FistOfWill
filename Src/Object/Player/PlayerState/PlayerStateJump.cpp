
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
	static const std::wstring ANIM_JUMP = L"Jump";
}

PlayerStateJump::PlayerStateJump(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	stepJump_ = 0.0f;
	m_State = STATE::JUMP;
}

void PlayerStateJump::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_JUMP, false);
}

void PlayerStateJump::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());
	
	// ジャンプの入力受付時間をヘラス
	stepJump_ += SceneManager::GetInstance().GetDeltaTime();

	// ジャンプ方向
	VECTOR jumpPow = player.lock()->GetJumpPow();
	float jumpSpped = 10.0f * SceneManager::GetInstance().GetGameSpeedRate();

	// 重力方向
	VECTOR dirGravity = Utility::DIR_D;

	if (player.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		ChangeState(std::make_shared<PlayerStateFall>(owner_));
	}

	if (stepJump_ < 0.5)
	{
		//jumpPow = VScale(VScale(dirGravity, -1.0f), 10.0f);
		player.lock()->AddJumpPow(VScale(jumpPow, -1.0f));					// 元のジャンプ力を打ち消す
		player.lock()->AddJumpPow(VScale(VScale(dirGravity, -1.0f), jumpSpped));// ジャンプ力を加える
	}
	else
	{
		ChangeState(std::make_shared<PlayerStateFall>(owner_));
	}

}

void PlayerStateJump::Exit()
{
}