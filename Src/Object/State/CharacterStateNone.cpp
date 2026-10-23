#include "CharacterStateNone.h"

#include "../CharaBase.h"
#include "../ActorBase.h"

#include "../Component/ComponentBase.h"

#include "../State/StateBase.h"


CharacterStateNone::CharacterStateNone(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void CharacterStateNone::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
}

void CharacterStateNone::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}
}

void CharacterStateNone::Exit()
{
}