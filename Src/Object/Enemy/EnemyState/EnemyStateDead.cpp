#include "EnemyStateDead.h"

#include "../EnemyBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../Component/MoveApplyComponent.h"

#include "../../Common/AnimationController.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Camera/CameraBase.h"
#include "../Manager/System/Camera/FinishCamera.h"

namespace
{
	static const std::wstring ANIM_DEAD = L"Dead";
}

EnemyStateDead::EnemyStateDead(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void EnemyStateDead::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	// 現在のプレイヤーカメラ（自分自身）のポインタを用意
	auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();

	// フィニッシュカメラを生成（追跡対象の敵と、終わったら戻るカメラを渡す）
	auto finishCam = std::make_shared<FinishCamera>(owner_.lock(), currentCam);
	SceneManager::GetInstance().GetCameraBase().lock()->ChangeCamera(finishCam);

	// コンポーネントの更新
	for (auto& component : owner_.lock()->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) move->SetActive(false);
	}
}

void EnemyStateDead::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_DEAD, false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());

	if (enemy.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		// 自身の削除
		enemy.lock()->Delete();
	}
}

void EnemyStateDead::Exit()
{
}