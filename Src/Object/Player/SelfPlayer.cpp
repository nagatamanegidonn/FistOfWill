#include <memory>
#include <fstream>
#include "../Common/InputController.h" // 追加: InputControllerの完全な型定義を取得
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/Game/CollisionManager.h"

#include "../Component/ControlMoveComponent.h"
#include "../Component/GravityComponent.h"
#include "../Component/GoalRotateComponent.h"
#include "../Component/MoveApplyComponent.h"
#include "../Component/DrawAfterimageComponent.h"	// 残像
#include "../Component/DrawTrailComponent.h"		// 

#include "../../Utility/Utility.h"
#include "../../Utility/ModelFrameUtility.h"
#include "../../Utility/CSVDataLoader.h"

#include "../UI/UIBase.h"
#include "../UI/Image/ImageUI.h"
#include "../UI/HP/HpGaugeUI.h"
#include "../UI/Player/PlayerGuideUI.h"

#include "../Attack/AttackData.h"

#include "../OnHit/OnHitBase.h"
#include "../OnHit/PlayerOnHit.h"

#include "../Common/AnimationController.h"
#include "../Common/Collider/Collider.h"
#include "../Common/Collider/Geometry/Geometry.h"
#include "../Common/Collider/Geometry/Line.h"
#include "../Common/Collider/Geometry/Capsule.h"

#include "../../Manager/System/Camera/PlayerCamera.h"

#include "SelfPlayer.h"

#include "../Lib/ImGui/imgui.h"

namespace
{
}

SelfPlayer::SelfPlayer(int _key)
	:
	PlayerBase(_key),
	hipsPos_(VGet(0.0f, 0.0f, 0.0f))
{
	// 外部データ
	LoadCsvAttackData(L"Data/CSV/PlayerAttackData.csv");
}

SelfPlayer::~SelfPlayer(void)
{
}

void SelfPlayer::Init(void)
{
	auto self = std::dynamic_pointer_cast<PlayerBase>(shared_from_this());
	// カメラの設定
	SceneManager::GetInstance().GetCameraBase().lock()->ChangeCamera(std::make_shared<PlayerCamera>(self));

	// コントローラーの登録
	inputController_ = std::make_shared<InputController>(SceneManager::GetInstance().GetControllId());
	// 移動用のコンポーネントを追加
	component_.emplace_back(std::make_shared<ControlMoveComponent>(shared_from_this(), inputController_));
	// 重力用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GravityComponent>(shared_from_this()));
	// 移動適用用のコンポーネントを追加
	component_.emplace_back(std::make_shared<MoveApplyComponent>(shared_from_this()));
	// 目標回転用のコンポーネントを追加
	component_.emplace_back(std::make_shared<GoalRotateComponent>(shared_from_this()));
	// 残像描画用のコンポーネントを追加
	auto con = std::make_shared<DrawAfterimageComponent>(shared_from_this());
	con->SetActive(false);
	component_.emplace_back(std::move(con));
	// 軌跡描画用のコンポーネントを追加
	auto trail = std::make_shared<DrawTrailComponent>(shared_from_this());
	trail->SetActive(false);
	component_.emplace_back(std::move(trail));

	// 基底クラスの初期化
	PlayerBase::Init();

	// アニメーションの初期化
	InitAnimation(L"Data/CSV/PlayerAnimData.csv");

	// アニメーションの初期再生	
	animationController_->Play(L"Idle");

	// コンポーネントの初期化
	for (const auto& component : component_)
	{
		component->Init();
	}

	hipsPos_ = Utility::MV1GetFreamPos(transform_.modelId, L"mixamorig:Hips");

	// 接地判定用の当たり判定
	// 2本の線分で足元を判定する（片方だと引っかかることがあるため）
	std::unique_ptr<Geometry> geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP, LINE_DOWN);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	// 2本目は少し前にずらす
	geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP2, LINE_DOWN2);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	// 2本目は少し後ろにずらす
	geo = std::make_unique<Line>(transform_.pos, transform_.pos, transform_.quaRot
		, LINE_UP3, LINE_DOWN3);
	this->MakeCollider(Collider::TAG::FOOT, std::move(geo), {  });
	// カプセルコライダの作成
	geo = std::make_unique<Capsule>(transform_.pos, transform_.pos, transform_.quaRot
		, VECTOR(0.0f, 130.0f, 0.0f), VECTOR(0.0f, 30.0f + 10.0f, 0.0f), 30.0f);
	this->MakeCollider(Collider::TAG::MACHINE, std::move(geo), { /*Collider::TAG::GROUND,*/Collider::TAG::FOOT });

	// 当たり判定後処理
	onHit_ = std::make_shared<PlayerOnHit>(*this);

	// UI
	// プレイヤーガイドUI
	auto GuideUI = std::make_shared<PlayerGuideUI>(charaStatus_,inputController_);
	GuideUI->Init();
	GuideUI->EnableDraw();
	SceneManager::GetInstance().AddOtherUI(GuideUI);
}

void SelfPlayer::Update(void)
{
	// ヒップの位置を更新
	hipsPos_ = Utility::MV1GetFreamPos(transform_.modelId, L"mixamorig:Hips");

	// ImGuiの操作を行う
	UpdateDebugImGui();

	// コントローラーの更新
	inputController_->Update();

	// 基底クラスの更新
	PlayerBase::Update();

	isGround_ = false;
	isJust_ = false;
}

void SelfPlayer::UpdateDebugImGui(void)
{
	return;

	// ウィンドウタイトル&開始処理
	ImGui::Begin("Player");
	// 大きさ
	ImGui::Text("scale");
	ImGui::InputFloat("SclX", &transform_.scl.x);
	ImGui::InputFloat("SclY", &transform_.scl.y);
	ImGui::InputFloat("SclZ", &transform_.scl.z);
	// 角度
	VECTOR rotDeg = VECTOR();
	rotDeg.x = Utility::Rad2DegF(transform_.rot.x);
	rotDeg.y = Utility::Rad2DegF(transform_.rot.y);
	rotDeg.z = Utility::Rad2DegF(transform_.rot.z);
	ImGui::Text("angle(deg)");
	ImGui::SliderFloat("RotX", &rotDeg.x, 0.0f, 360.0f);
	ImGui::SliderFloat("RotY", &rotDeg.y, 0.0f, 360.0f);
	ImGui::SliderFloat("RotZ", &rotDeg.z, 0.0f, 360.0f);
	transform_.rot.x = Utility::Deg2RadF(rotDeg.x);
	transform_.rot.y = Utility::Deg2RadF(rotDeg.y);
	transform_.rot.z = Utility::Deg2RadF(rotDeg.z);
	// 位置
	ImGui::Text("position");
	// 構造体の先頭ポインタを渡し、xyzと連続したメモリ配置へアクセス
	ImGui::InputFloat3("Pos", &transform_.pos.x);
	// ImGui::Separator();
	ImGui::Text("Collision Info");
	// 終了処理
	ImGui::End();
}

void SelfPlayer::Draw(void)
{
	// 基底クラスの描画
	PlayerBase::Draw();
}

void SelfPlayer::OnHit(const std::weak_ptr<Collider> _hitCol)
{
	onHit_->Update(_hitCol);
}