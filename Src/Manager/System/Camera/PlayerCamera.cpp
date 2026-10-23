
#include <memory>
#include <EffekseerForDXLib.h>

#include "PlayerCamera.h"
#include "../Application.h"
#include "../../../Utility/Utility.h"
#include "../../../Object/Common/Collider/Geometry/Geometry.h"
#include "../../../Object/Common/Collider/Geometry/Line.h"
#include "../../../Object/Common/Collider/Geometry/Model.h"
#include "../../../Object/Player/PlayerBase.h"
#include "../../../Object/Common/InputController.h"
#include "NormalCamera.h"

// ローカル座標でのカメラ引き算ベクトル（参考コードの構造を反映）
// 注視点からカメラがどれだけ離れるか（後方：-cameraRadius_、上方：少し高めに設定）
#define LOCAL_C_POS(radius) (VGet(0.0f, 0.0f, -(radius)))

PlayerCamera::PlayerCamera(std::shared_ptr<PlayerBase> player)
	:
	CameraBase(),
	player_(player),
	cameraRadius_(300.0f),
	//cameraRadius_(500.0f),
	lerpTime_(1.0f)
{
	rayGeo_ = nullptr;
}

void PlayerCamera::Init()
{
	// すでにデータがある場合は初期化しない
	if (!IsNeedInit())
	{
		// 次の状態（m_nextCamera）だけは念のため nullptr にクリアしておく
		ChangeCamera(nullptr);
		SetNeedInit(true);
		return;
	}


	// 次の状態を自分の状態を入れる
	ChangeCamera(nullptr);

	// 注視点
	targetPos_ = Utility::VECTOR_ZERO;

	// カメラの上方向
	cameraUp_ = Utility::DIR_U;

	// 初期角度 (ラジアン)
	angles_.x = Utility::Deg2RadF(30.0f); // 縦回転（仰角）
	angles_.y = Utility::Deg2RadF(0.0f);  // 横回転（旋回角）
	angles_.z = 0.0f;

	// 注視点からの距離
	cameraRadius_ = 500.0f;

	// 初期位置を角度と距離から計算
	pos_.x = targetPos_.x + cameraRadius_ * cosf(angles_.x) * sinf(angles_.y);
	pos_.y = targetPos_.y + cameraRadius_ * sinf(angles_.x);
	pos_.z = targetPos_.z - cameraRadius_ * cosf(angles_.x) * cosf(angles_.y);

	rot_ = Quaternion();

	// 初回の位置計算
	UpdateFollow();

	// Lineクラスの仕様に合わせた初期生成
	// 基準位置を targetPos_、基準回転を rot_ にします。
	// ローカル点1は原点(0,0,0)、ローカル点2はカメラの最大距離分後ろ(0, 0, -500)
	std::unique_ptr<Line> geo = std::make_unique<Line>(
		targetPos_,               // _pos: 親（注視点）の座標
		targetPos_,               // _movedPos: 移動後もとりあえず同値
		rot_,                     // _rot: 親（カメラ）の回転
		Utility::VECTOR_ZERO,     // _localPosPoint1: 開始点（プレイヤーの頭）
		LOCAL_C_POS(targetRadius_)// _localPosPoint2: 終了点（最大距離離れたカメラ候補地）
	);
	rayGeo_ = geo.get(); // 後で更新するために保持しておく

	// コライダーの登録
	this->MakeCollider(Collider::TAG::RAY, std::move(geo), { });
}

void PlayerCamera::Update()
{
	// プレイヤーが存在しなければNormalカメラに切り替える
	if (player_.expired() || rayGeo_ == nullptr)
	{
		ChangeCamera(std::make_shared<NormalCamera>());
		return;
	}

	SetCameraNearFar(cameraNear_, cameraFar_);

	float deltaX = 0.0f;
	float deltaY = 0.0f;

	// --- 1. スティック入力によるカメラ回転処理（デッドゾーン追加） ---
	if (!player_.lock()->GetInputController().expired())
	{
		const float ROT_SPEED_X = 0.03f;
		const float ROT_SPEED_Y = 0.02f;
		const float DEAD_ZONE = 150.0f; // 1000中の150（15%）以下は遊びとして無視する

		float rawStickX = player_.lock()->GetInputController().lock()->GetRStickX();
		float rawStickY = player_.lock()->GetInputController().lock()->GetRStickY();

		// 入力値がデッドゾーンを超えている場合のみ加算
		if (std::abs(rawStickX) > DEAD_ZONE)
		{
			deltaX += (rawStickX / 1000.0f) * ROT_SPEED_X;
		}
		if (std::abs(rawStickY) > DEAD_ZONE)
		{
			deltaY += (rawStickY / 1000.0f) * ROT_SPEED_Y;
		}
	}

	// --- 2. マウス入力によるカメラ回転処理（画面中心固定方式の修正版） ---
	const float MOUSE_SENSITIVITY_X = 0.002f;
	const float MOUSE_SENSITIVITY_Y = 0.002f;

	// 画面解像度は Application から直接取得する（ GetGraphSize は使用しない ）
	int centerX = Application::SCREEN_SIZE_X / 2;
	int centerY = Application::SCREEN_SIZE_Y / 2;

	int mouseX = 0, mouseY = 0;
	DxLib::GetMousePoint(&mouseX, &mouseY);

	int diffX = mouseX - centerX;
	int diffY = mouseY - centerY;

	// 1ピクセル以上の移動があった時のみ処理（微小な誤差による自動回転を防止）
	if (std::abs(diffX) > 1 || std::abs(diffY) > 1)
	{
		deltaX += static_cast<float>(diffX) * MOUSE_SENSITIVITY_X;
		deltaY += static_cast<float>(diffY) * MOUSE_SENSITIVITY_Y;

		// 差分を加算した後にカーソルを中心へ戻す
		DxLib::SetMousePoint(centerX, centerY);
	}

	// --- 3. 角度の更新とクランプ ---
	angles_.y += deltaX; // 左右回転
	angles_.x += deltaY; // 上下回転

	// 上下の回転角度制限
	const float LIMIT_UP = Utility::Deg2RadF(80.0f);
	const float LIMIT_DOWN = Utility::Deg2RadF(-10.0f);
	if (angles_.x > LIMIT_UP)   angles_.x = LIMIT_UP;
	if (angles_.x < LIMIT_DOWN) angles_.x = LIMIT_DOWN;

	// プレイヤー追従位置の更新
	UpdateFollow();

	// カメラ半径の補間処理
	if (lerpTime_ < 1.0f) lerpTime_ += 0.1f;
	cameraRadius_ = std::lerp(cameraRadius_, targetRadius_, lerpTime_);

	// コライダー線分の同期
	if (rayGeo_)
	{
		Line* line = dynamic_cast<Line*>(rayGeo_);
		if (line)
		{
			line->SetLocalPosPoints(Utility::VECTOR_ZERO, LOCAL_C_POS(targetRadius_));
		}
	}

#ifdef _DEBUG
	UpdateDebugImGui();
#endif 

	Effekseer_Sync3DSetting();
}
void PlayerCamera::Draw()
{
	
}

void PlayerCamera::Exit()
{

}

void PlayerCamera::UpdateFollow()
{
	if (player_.expired()) return;

	// プレイヤーの現在位置を取得
	VECTOR playerPos = player_.lock()->GetTransform().pos;

	// 基底クラスのクォータニオン合成を参考にした回転の作成
	Quaternion gRot = Quaternion();
	rotOutX_ = gRot.Mult(Quaternion::AngleAxis(angles_.y, Utility::AXIS_Y));
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, Utility::AXIS_X));

	// 注視点（targetPos_）の決定
	// プレイヤーの足元（0,0,0）を見つめるとカメラが低くなりすぎるため、
	// プレイヤーの胸~頭の高さ（Y座標を少し上にオフセット）をターゲットにします。
	const float PLAYER_HEAD_HEIGHT = 120.0f;
	targetPos_ = playerPos;
	targetPos_.y += PLAYER_HEAD_HEIGHT;

	// カメラ位置（pos_）の決定
	// 合成したクォータニオンの方向へ、カメラの距離（cameraRadius_）分だけ後ろに引っ張る
	VECTOR localCamPos = LOCAL_C_POS(cameraRadius_);
	VECTOR rotatedCamPos = rot_.PosAxis(localCamPos); // クォータニオンによるベクトル回転

	// 注視点から、回転したオフセットベクトルを足すことでカメラの座標が確定
	pos_ = VAdd(targetPos_, rotatedCamPos);
	// カメラの上方向ベクトルを更新
	cameraUp_ = gRot.GetUp();
}

// カメラとステージの衝突処理
void PlayerCamera::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	auto hitColLock = _hitCol.lock();
	if (!hitColLock) return;

	// ステージ（地面や障害物）に当たった場合
	if (hitColLock->GetTag() == Collider::TAG::GROUND
		|| hitColLock->GetTag() == Collider::TAG::NORMAL_OBJECT)
	{
		// 衝突相手の古来だ情報を取り出す
		const auto& groundPreCol = _hitCol.lock()->GetHitCollider();
		// キャストを試みる
		Model* model = dynamic_cast<Model*>(&_hitCol.lock()->GetGeometry());

		if (model)
		{
			Line* line = dynamic_cast<Line*>(&groundPreCol.lock()->GetGeometry());
			if (line)
			{
				// 地面との衝突
				auto& hit = line->GetHitInfo();

				// 2. 注視点（プレイヤーの頭）から衝突点までの距離を計算
				VECTOR toHitVec = VSub(hit.HitPosition, targetPos_);
				float hitDistance = VSize(toHitVec);

				// 3. カメラが壁を突き抜けないよう、少しだけ手前（20.0f）に引き戻す
				float newRadius = hitDistance - 20.0f;
				if (newRadius < 10.0f) newRadius = 10.0f; // 限界の最小距離

				// 4. 現在の距離よりも短くなる場合のみ更新
				if (newRadius < cameraRadius_)
				{
					cameraRadius_ = newRadius;
				}
			}
		}
		else
		{
			// 1. 衝突相手のコライダーから、交点（壁の表面の座標）を取得
			VECTOR hitPoint = hitColLock->GetGeometry().GetHitResult().point;

			// 2. 注視点（プレイヤーの頭）から衝突点までの距離を計算
			VECTOR toHitVec = VSub(hitPoint, targetPos_);
			float hitDistance = VSize(toHitVec);

			// 3. カメラが壁を突き抜けないよう、少しだけ手前（20.0f）に引き戻す
			float newRadius = hitDistance - 20.0f;
			if (newRadius < 10.0f) newRadius = 10.0f; // 限界の最小距離

			// 4. 現在の距離よりも短くなる場合のみ更新
			if (newRadius < cameraRadius_)
			{
				cameraRadius_ = newRadius;
				lerpTime_ = 0.0f;
			}
		}
	}
}
