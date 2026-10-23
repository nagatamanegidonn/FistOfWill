
#include <memory>

#include "../Object/Stage/Destination.h"
#include "../Object/CharaStatus.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Manager/System/Game/CollisionManager.h"
#include "../../Manager/General/ActorManager.h"

#include "../Component/ControlMoveComponent.h"
#include "../Component/GravityComponent.h"
#include "../Component/GoalRotateComponent.h"
#include "../Component/MoveApplyComponent.h"

#include "../../Utility/Utility.h"
#include "../../Utility/ModelFrameUtility.h"

#include "../OnHit/OnHitBase.h"
#include "../OnHit/EnemyOnHit.h"
#include "../OnHit/TutorialEnemyOnHit.h"

#include "../Common/Collider/Collider.h"
#include "../Common/Collider/Geometry/Geometry.h"
#include "../Common/Collider/Geometry/Line.h"
#include "../Common/Collider/Geometry/Capsule.h"
// 状態
#include "../State/StateBase.h"
#include "TutorialState/TutorialStateIdle.h"
#include "TutorialEnemy.h"

TutorialEnemy::TutorialEnemy(int _key)
	:
	EnemyBase(_key)
{
}

TutorialEnemy::~TutorialEnemy(void)
{
}

void TutorialEnemy::Init(void)
{
	// コントローラーの登録
	// 重力用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GravityComponent>(shared_from_this()));
	// 目標回転用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GoalRotateComponent>(shared_from_this()));

	// コンポーネントの初期化
	for (const auto& component : component_)
	{
		component->Init();
	}

	// 攻撃データの読み込み
	LoadCsvAttackData(L"Data/CSV/TutorialEnemyAttackData .csv");

	// 体力の設定
	charaStatus_->SetMaxHP(10000);
	charaStatus_->SetNowHP(10000);

	charaStatus_->SetArmor(CharaStatus::Armor::Middle);

	// モデルの基本設定
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"Enemy"));
	transform_.Update();

	// 基底クラスの初期化
	EnemyBase::Init();

	// アニメーションの初期化
	InitAnimation(L"Data/CSV/BossEnemyAnimData.csv");

	// プレイヤー状態の初期化
	auto self = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
	state_ = std::make_shared<TutorialStateIdle>(self);
	state_->Init();

	// 攻撃可能距離の設定
	attackRange_ = 200.0f;

	// 判定の登録
	// 接地判定用の当たり判定
	std::unique_ptr<Geometry> geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP, LINE_DOWN);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	
	// カプセルコライダの作成
	geo = std::make_unique<Capsule>(transform_.pos, transform_.pos, transform_.quaRot
		, VECTOR(0.0f, 60.0f + 100.0f, 0.0f), VECTOR(0.0f, 60.0f * 2.0f + 10.0f, 0.0f), 60.0f * 2.0f);
	this->MakeCollider(Collider::TAG::MACHINE, std::move(geo), { Collider::TAG::FOOT });

	// 当たり判定後処理
	onHit_ = std::make_shared<TutorialEnemyOnHit>(*this);

	// チュートリアルの目的地を設定
	destination_ = std::make_shared<Destination>(shared_from_this());
	ActorManager::Get().Entry(destination_);
}

void TutorialEnemy::Update(void)
{
	// 基底クラスの更新
	EnemyBase::Update();

	if(!destination_->IsGoal())
	{
		attackCoolTime_ = 1.0f;
	}

	isGround_ = false;
}

void TutorialEnemy::Draw(void)
{
	// 基底クラスの描画
	EnemyBase::Draw();
}

void TutorialEnemy::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	onHit_->Update(_hitCol);
}