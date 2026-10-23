#include <DxLib.h>
#include "../CharaBase.h"
#include "../Common/InputController.h"
#include "../../Utility/Utility.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/Camera/PlayerCamera.h"
#include "ControlMoveComponent.h"

ControlMoveComponent::ControlMoveComponent(std::weak_ptr<ActorBase> _actor, std::shared_ptr<InputController> _input)
	:
	ComponentBase()
{
	actor_ = _actor;
	input_ = _input;
}

ControlMoveComponent::~ControlMoveComponent()
{
}

void ControlMoveComponent::Init()
{
}

void ControlMoveComponent::Update()
{
	auto chara = GetChara();
	if (!chara) return;

	VECTOR dir = Utility::VECTOR_ZERO;

	if (input_.lock()->IsNew(InputController::KEY::FORWARD)) { dir = VAdd(dir, Utility::DIR_F); }
	if (input_.lock()->IsNew(InputController::KEY::LEFT)) { dir = VAdd(dir, Utility::DIR_L); }
	if (input_.lock()->IsNew(InputController::KEY::BACK)) { dir = VAdd(dir, Utility::DIR_B); }
	if (input_.lock()->IsNew(InputController::KEY::RIGHT)) { dir = VAdd(dir, Utility::DIR_R); }

	//移動方向の向き設定
	float rotRad = Utility::CreateRad(dir);

	VECTOR movePow = Utility::VECTOR_ZERO;
	// ダッシュキーの入力に応じてダッシュ状態を設定
	input_.lock()->IsNew(InputController::KEY::DASH) ? chara->SetDash(true) : chara->SetDash(false);

	//float speed = chara->IsDash() ? 20.0f : 10.0f;
	float speed = SPEED;
	speed *= SceneManager::GetInstance().GetGameSpeedRate();

	// 移動方向が入力されている場合
	if (!Utility::EqualsVZero(dir))
	{
		// 斜め移動でも速度が変わらないように、方向ベクトルを一度正規化（長さを1に）する
		dir = VNorm(dir);

		// シーンマネージャーからカメラを取得
		auto cameraPtr = SceneManager::GetInstance().GetCameraBase().lock();
		if (cameraPtr)
		{
			// カメラの現在の angles_ を取得 (GetAngles() が存在する場合)
			VECTOR camAngles = cameraPtr->GetAngles();

			// Y軸（左右）の回転角から、その場でクォータニオンを生成する
			Quaternion cameraRot = Quaternion::AngleAxis(camAngles.y, Utility::AXIS_Y);

			// 入力された方向（dir）をカメラの向きに合わせて変換
			dir = cameraRot.PosAxis(dir);
		}

		// 最終的な移動量を計算
		movePow = VScale(dir, speed);

		// プレイヤーの見た目の向き（キャラクターの回転）も移動方向に合わせる処理
		float rotRad = atan2f(dir.x, dir.z);
	}
	//移動量を設定
	chara->SetMovePow(movePow);
}

void ControlMoveComponent::PostUpdate(void)
{
}

void ControlMoveComponent::Draw()
{
}

void ControlMoveComponent::Move()
{
}
	
void ControlMoveComponent::Lerp()
{
}

void ControlMoveComponent::HitMove()
{
}
