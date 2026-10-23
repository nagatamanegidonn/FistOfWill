
#include <memory>
#include "../Object/CharaStatus.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Manager/System/Game/CollisionManager.h"

#include "../Component/ControlMoveComponent.h"
#include "../Component/GravityComponent.h"
#include "../Component/GoalRotateComponent.h"
#include "../Component/MoveApplyComponent.h"

#include "../../Utility/Utility.h"
#include "../../Utility/ModelFrameUtility.h"

#include "../OnHit/OnHitBase.h"
#include "../OnHit/EnemyOnHit.h"

#include "../Common/Collider/Collider.h"
#include "../Common/Collider/Geometry/Geometry.h"
#include "../Common/Collider/Geometry/Line.h"
#include "../Common/Collider/Geometry/Capsule.h"

#include "../../UI/HP/EnemyGaugeUI.h"
#include "BossEnemy.h"

namespace
{
	constexpr int MAX_HP = 500;
	constexpr float ATTRCK_RANGE = 200.0f;
}

BossEnemy::BossEnemy(int _key)
	:
	EnemyBase(_key),
	hipsPos_(VGet(0.0f, 0.0f, 0.0f))
{
}

BossEnemy::~BossEnemy(void)
{
}

void BossEnemy::Init(void)
{
	// コントローラーの登録
	// 重力用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GravityComponent>(shared_from_this()));
	// 移動適用用のコンポーネントを追加
	component_.emplace_back(std::make_shared<MoveApplyComponent>(shared_from_this()));
	// 目標回転用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GoalRotateComponent>(shared_from_this()));

	// コンポーネントの初期化
	for (const auto& component : component_)
	{
		component->Init();
	}

	// 攻撃データの読み込み
	LoadCsvAttackData(L"Data/CSV/BossEnemyAttackData.csv");

	// 体力の設定
	charaStatus_->SetMaxHP(MAX_HP);
	charaStatus_->SetNowHP(MAX_HP);
	
	// モデルの基本設定
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"Enemy"));
	transform_.Update();

	// 基底クラスの初期化
	EnemyBase::Init();

	// アニメーションの初期化
	InitAnimation(L"Data/CSV/BossEnemyAnimData.csv");

	hipsPos_ = Utility::MV1GetFreamPos(transform_.modelId, L"mixamorig:Hips");
	attackRange_ = ATTRCK_RANGE;

	// 判定の登録
	// 接地判定用の当たり判定
	// 2本の線分で足元を判定する（片方だと引っかかることがあるため）
	std::unique_ptr<Geometry> geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP, LINE_DOWN);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	// 2本目は少し前にずらす
	geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP2, LINE_DOWN2);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	// 3本目は少し後ろにずらす
	geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP3, LINE_DOWN3);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });

	// カプセルコライダの作成
	geo = std::make_unique<Capsule>(transform_.pos, transform_.pos, transform_.quaRot
		, VECTOR(0.0f, 60.0f + 100.0f, 0.0f), VECTOR(0.0f, 60.0f + 10.0f, 0.0f), 60.0f);
	this->MakeCollider(Collider::TAG::MACHINE, std::move(geo), { Collider::TAG::FOOT });

	// 当たり判定後処理
	onHit_ = std::make_shared<EnemyOnHit>(*this);

	// HPゲージUI
	auto HP_UI = std::make_shared<EnemyGaugeUI>(charaStatus_, GetTransform().pos);
	HP_UI->Init();
	HP_UI->EnableDraw();
	SceneManager::GetInstance().AddOtherUI(HP_UI);
}

void BossEnemy::Update(void)
{
	hipsPos_ = Utility::MV1GetFreamPos(transform_.modelId, L"mixamorig:Hips");

	// 基底クラスの更新
	EnemyBase::Update();

	isGround_ = false;
}

void BossEnemy::Draw(void)
{
	// 基底クラスの描画
	EnemyBase::Draw();
}

void BossEnemy::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	onHit_->Update(_hitCol);
}