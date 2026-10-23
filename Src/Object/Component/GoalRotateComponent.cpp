#include "../CharaBase.h"

#include "../../Utility/Utility.h"
#include "../../Manager/General/SceneManager.h"

#include "GoalRotateComponent.h"

GoalRotateComponent::GoalRotateComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
	rotateDir_ = Utility::VECTOR_ZERO;
}

GoalRotateComponent::~GoalRotateComponent()
{
}

void GoalRotateComponent::Init()
{
}

void GoalRotateComponent::Update()
{
}

void GoalRotateComponent::PostUpdate(void)
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	// キャラクターから現在の移動量（ベクトル）を取得
	VECTOR currentMovePow = chara->GetMovePow();

	// 移動量に変更があったとき（前回の移動量と異なるとき）に、回転を更新する
	if (!Utility::EqualsVZero(chara->GetMovePow()))
	{
		// 前回の移動方向と異なる場合のみ更新（無駄な計算を防ぐ）
		if (!Utility::Equals(currentMovePow, rotateDir_))
		{
			rotateDir_ = currentMovePow;
		}
	}
	else
	{
		// 移動量がゼロ（静止状態）なら、最後に保存した方向を維持したままここで処理を抜ける
		return;
	}

	// 移動量がゼロ（静止状態）なら、回転は更新しない
	if (Utility::EqualsVZero(rotateDir_)) return;

	// すでにカメラの回転が乗った movePow から、直接XZ平面上の角度（ラジアン）を計算する
	float rotRad = Utility::CreateRad(rotateDir_);

	// 最終回転を設定（カメラのY回転を考慮する関数）
	SetGoalRotate(rotRad);

	chara->Rotate();
}

void GoalRotateComponent::Draw()
{
}

void GoalRotateComponent::SetGoalRotate(double rotRad)
{
	auto chara = GetChara();
	if (!chara) return;

	Quaternion axis = Quaternion::AngleAxis(rotRad, Utility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, chara->GetGoalQuaRot());

	// しきい値
	if (angleDiff > 0.1)
	{
		chara->SetStepRotTime(TIME_ROT);
	}
	chara->SetGoalQuaRot(axis);
}
