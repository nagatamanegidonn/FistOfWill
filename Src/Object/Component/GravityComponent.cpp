#include "../CharaBase.h"
#include "../../Utility/Utility.h"
#include "../../Manager/General/SceneManager.h"

#include "GravityComponent.h"


GravityComponent::GravityComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	gravityPow_ = VGet(0.0f,0.0f,0.0f);
	actor_ = _actor;
}

GravityComponent::~GravityComponent()
{
}

void GravityComponent::Init()
{
}

void GravityComponent::Update()
{
	auto chara = GetChara();
	if (!chara) return;

	// 重力方向
	VECTOR dirGravity = Utility::DIR_D;

	// 重力の強さ
	float gravityPow = DEFAULT_GRAVITY_POW * SceneManager::GetInstance().GetGameSpeedRate();

	// 重力
	gravityPow_ = VScale(dirGravity, gravityPow);

	// ジャンプ力と重力を足す
	chara->SetJumpPow(VAdd(chara->GetJumpPow(), gravityPow_));

	// 最初は実装しない。地面と突き抜けることを確認する。
	// 内積
	float dot = VDot(dirGravity, VNorm(chara->GetJumpPow()));
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		chara->SetJumpPow(gravityPow_);
	}
}

void GravityComponent::PostUpdate(void)
{
}

void GravityComponent::Draw()
{
}

