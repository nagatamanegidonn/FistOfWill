#include "../CharaBase.h"
#include "../../Utility/Utility.h"

#include "MoveApplyComponent.h"

MoveApplyComponent::MoveApplyComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
}

MoveApplyComponent::~MoveApplyComponent()
{
}

void MoveApplyComponent::Init()
{
}

void MoveApplyComponent::Update()
{
}

void MoveApplyComponent::PostUpdate(void)
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	// 現在座標を過去の座標に保存
	chara->SetPrePos(chara->GetTransform().pos);

	// 現在座標を起点に移動後座標を決める
	// 移動量の取得
	const VECTOR& totalMovePow = VAdd(chara->GetMovePow(), chara->GetJumpPow());

	//　移動
	{
		// 現在の位置を取得
		VECTOR pos = chara->GetTransform().pos;

		// 移動量を加算して位置を更新
		pos = VAdd(pos, totalMovePow);

		// 更新した位置を設定
		//chara->SetPosition(pos);
		chara->AddMove(totalMovePow);

#ifdef _DEBUG
		// 落下して地面を突き抜けた場合のリセット
		if (chara->GetTransform().pos.y <= -1000.0f)
		{
			chara->SetPosition({ 0.0f, -30.0f, 0.0f });
		}
#endif // DEBUG
	}
}

void MoveApplyComponent::Draw()
{
}

