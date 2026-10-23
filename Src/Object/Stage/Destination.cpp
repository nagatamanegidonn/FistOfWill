#include "Destination.h"

#include "../Common/Collider/Geometry/Sphere.h"
#include "../Common/AnimationController.h"
#include "../../Manager/System/Game/TutorialManager.h"

Destination::Destination(std::shared_ptr<ActorBase> owner)
	:
	owner_(owner),
	isGoal_(false)
{
}

Destination::~Destination(void)
{
}

void Destination::Init(void)
{
	// モデルの基本設定
	transform_.scl = Utility::VECTOR_ONE;
	// 初期座標
	transform_.pos = { 0.0f,-250,700.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, Utility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	// 当たり判定（Sphere）を自身に作成
	std::unique_ptr<Geometry> geo = std::make_unique<Sphere>(transform_.pos, transform_.pos, 100.0f);
	MakeCollider(Collider::TAG::DESTINATION, std::move(geo), {});

	isGoal_ = false;
}

void Destination::Update(void)
{
}

void Destination::Draw(void)
{
}

void Destination::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	// 当たった相手のコライダを取得
	auto hitCol = _hitCol.lock();
	if (!hitCol) return;

	if(&hitCol->GetOwner() == owner_.lock().get())
	{
		// 持ち主と当たった場合は何もしない
		return;
	}

	// 当たった相手のタグを取得
	auto hitTag = hitCol->GetTag();
	// プレイヤーが当たった場合の処理
	if (hitTag == Collider::TAG::MACHINE)
	{
		// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
		DeleteColliderAtTag(Collider::TAG::DESTINATION);

		isGoal_ = true;

		// Tutorialを進行させる
		TutorialManager::GetInstance().StartNextTutorial(true);
	}
}