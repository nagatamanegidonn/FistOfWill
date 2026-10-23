
#include "../../Application.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Utility/Utility.h"

#include "../Common/AnimationController.h"
#include "../Common/SoundController.h"
// コンポーネント
#include "../Component/ComponentBase.h"
// 状態
#include "../State/StateBase.h"
#include "EnemyState/EnemyStateIdle.h"

#include "../Player/PlayerBase.h"
#include "../CharaStatus.h"
#include "EnemyBase.h"

namespace
{
	// 視野角
	constexpr float FOV_DEG = 120.0f;
	// 警戒状態なら索敵範囲を広げる
	constexpr float kAlertedDisRate = 2.0f;
	// 初期攻撃クールタイム
	constexpr float kFirstCoolTime = 5.0f;
}

EnemyBase::EnemyBase(int key)
	:
	CharaBase(),
	attackCoolTime_(kFirstCoolTime),
	canAttack_(true),
	attackRange_(100.0f)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
	// 初期化処理
	playerRotY_ = Quaternion(transform_.rot);

	// アニメーションの設定
	animationController_ = std::make_shared<AnimationController>(transform_.modelId);

	// プレイヤー状態の初期化
	auto self = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
	state_ = std::make_shared<EnemyStateIdle>(self);
	state_->Init();
}

void EnemyBase::Update(void)
{
	// 攻撃クールタイムの更新
	if (attackCoolTime_ > 0.0f)attackCoolTime_ -= SceneManager::GetInstance().GetDeltaTime();

	// ターゲット更新
	UpdateTarget();

	// 状態の更新
	state_->Update();
	// 状態の変化判定
	if (state_->GetNextState().lock() != nullptr && state_ != state_->GetNextState().lock()) {
		// 状態を変化する
		state_ = state_->GetNextState().lock();
		state_->Init();
	}

	// コンポーネントの後処理
	for (const auto& component : component_)
	{
		component->PostUpdate();
	}

	// 重力方向に沿って回転させる
	UpdateRotate();

	transform_.Update();

	// アニメーション再生
	animationController_->Update();
}

void EnemyBase::Draw(void)
{
	// モデルの描画
	MV1DrawModel(transform_.modelId);
#ifdef _DEBUG
	// 視野描画
	DrawFOV(FOV_DEG, charaStatus_->GetSearchRange(), 10, 0xff0000);
#endif // _DEBUG
}

bool EnemyBase::IsEnableAttack() const
{
	// 攻撃可能な条件を満たした場合true
	return attackCoolTime_ <= 0.0f && canAttack_;
}

void EnemyBase::UpdateTarget()
{
	// すでにロックオン中のターゲットが健在なら維持
	if (auto currentTarget = target_.lock())
	{
		// 必要ならここで「離れすぎたら解除」などのロジックを入れる
		return;
	}

	// マネージャーが調べてくれた「一番近いプレイヤー」をチェック
	if (auto closest = closestTarget_.lock())
	{
		// 自身の位置と一番近いプレイヤーの距離を計算
		VECTOR myPos = transform_.pos;
		VECTOR playerPos = closest->GetTransform().pos;
		float distance = VSize(VSub(playerPos, myPos));

		// ターゲットを見つける
		if (distance <= charaStatus_->GetSearchRange())
		{
			// 敵の個性：自分自身の視野に入っているかだけを判断する！
			if (IsTargetInFOV(playerPos, FOV_DEG))
			{
				target_ = closest; // 条件をすべて満たしたらロックオン！
			}
		}
	}
}

// ターゲットが視野内にいるか
bool EnemyBase::IsTargetInFOV(const VECTOR& followPos, float fovDeg)
{
	VECTOR toTarget = VSub(followPos, transform_.pos);
	toTarget.y = 0.0f; // 水平方向だけで判定
	toTarget = VNorm(toTarget);

	// 中心方向の取得
	VECTOR forward = transform_.GetForward();

	// ボーン（フレーム）の番号を取得
	int frmNo = MV1SearchFrame(transform_.modelId, L"mixamorig:Spine");

	// リストにある「MV1GetFrameLocalWorldMatrix」を使用して行列を取得
	// (ローカル座標からワールド座標への変換行列 ＝ ワールド空間でのボーンの姿勢)
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(transform_.modelId, frmNo);

	// 行列の3行目（Z軸）から前方向のベクトルを抽出
	VECTOR boneForward;
	boneForward.x = -frameMat.m[2][0];
	boneForward.y = -frameMat.m[2][1];
	boneForward.z = -frameMat.m[2][2];

	// ベクトルの長さを1に正規化（純粋な向きのデータにする）
	boneForward = VNorm(boneForward);
	forward = boneForward;

	float dot = VDot(forward, toTarget); // コサイン角
	float angleRad = acosf(std::clamp(dot, -1.0f, 1.0f)); // 安定化
	float angleDeg = Utility::Rad2DegF(angleRad);

	// 視野角の半分以内なら true
	return angleDeg <= (fovDeg * 0.5f);
}

// 視野を描画
void EnemyBase::DrawFOV(float fovDeg, float radius, int rayCount, unsigned int color)
{
	// 原点
	VECTOR origin = transform_.pos;
	origin.y += 30;

	VECTOR forward = transform_.GetForward();
	forward.y = 0.0f;
	forward = VNorm(forward);

	// ボーン（フレーム）の番号を取得
	int frmNo = MV1SearchFrame(transform_.modelId, L"mixamorig:Spine");

	// リストにある「MV1GetFrameLocalWorldMatrix」を使用して行列を取得
	// (ローカル座標からワールド座標への変換行列 ＝ ワールド空間でのボーンの姿勢)
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(transform_.modelId, frmNo);

	// 行列の3行目（Z軸）から前方向のベクトルを抽出
	VECTOR boneForward;
	boneForward.x = -frameMat.m[2][0];
	boneForward.y = -frameMat.m[2][1];
	boneForward.z = -frameMat.m[2][2];

	// ベクトルの長さを1に正規化（純粋な向きのデータにする）
	boneForward = VNorm(boneForward);
	forward = boneForward;

	float halfFov = fovDeg * 0.5f;
	float angleStep = fovDeg / (rayCount - 1);

	for (int i = 0; i < rayCount; ++i)
	{
		float angle = -halfFov + angleStep * i;
		float rad = Utility::Deg2RadF(angle);

		// Y軸回転の方向ベクトルを作成
		VECTOR dir = {
			forward.x * cosf(rad) - forward.z * sinf(rad),
			0.0f,
			forward.x * sinf(rad) + forward.z * cosf(rad)
		};

		VECTOR end = VAdd(origin, VScale(dir, radius));
		end.y += 30;

		DrawLine3D(origin, end, color);
	}
}
