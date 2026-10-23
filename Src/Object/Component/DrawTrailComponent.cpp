#include "DrawTrailComponent.h"

#include "../ActorBase.h"
#include "../CharaBase.h"

#include "../Object/Attack/AttackData.h"

#include "../Renderer/Trail/SwordTrailData.h"
#include "../Renderer/Trail/SwordTrailMaterial.h"
#include "../Renderer/Trail/SwordTrailRenderer.h"

#include "../../Utility/Utility.h"
#include "../../Utility/ModelFrameUtility.h"

namespace{
	VECTOR HeadDistance = VGet(5.0f, 0.0f, 0.0f);	// 剣の先端と根元の距離を計算するためのベクトル
	VECTOR TailDistance = VGet(-5.0f, 0.0f, 0.0f);	// 剣の先端と根元の距離を計算するためのベクトル
}

DrawTrailComponent::DrawTrailComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;

	// --- 軌跡システムの初期化 ---
	// シェーダーファイル名はプロジェクト内の実際のファイル名に合わせて変更してください
	// 第2・4引数は Renderer.zip の ModelMaterial 側の設定に合わせています
	trailMaterial_ = std::make_unique<SwordTrailMaterial>(L"SwordTrailVS.cso", 12, L"SwordTrailPS.cso", 4);
	trailRenderer_ = std::make_unique<SwordTrailRenderer>(*trailMaterial_);
	swordTrail_ = std::make_unique<SwordTrailData>(20); // 20フレーム分の履歴
}

DrawTrailComponent::~DrawTrailComponent()
{
}

void DrawTrailComponent::Init()
{
	auto chara = GetChara();
	if (!chara) return;

	if (attackData_.expired())return;

	// 剣の先端と根元の計算
	// transform_ から計算されているこのベクトルをそのまま使います
	VECTOR swordHead = ModelFrameUtility::GetFreamPosFromDistance(chara->GetTransform(), attackData_.lock()->frameName, HeadDistance);
	VECTOR swordTail = ModelFrameUtility::GetFreamPosFromDistance(chara->GetTransform(), attackData_.lock()->frameName, TailDistance);
	
	// 軌跡に座標をセット (DirectX::XMFLOAT3 にキャストして渡す)
	VECTOR head = { swordHead.x, swordHead.y, swordHead.z };
	VECTOR tail = Utility::Lerp(swordHead, swordTail, 1.0f);
	swordTrail_->Reset(head, tail);
}

void DrawTrailComponent::Update()
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	if (attackData_.expired())return;

	if (trailRenderer_ && swordTrail_)
	{
		// 剣の先端と根元の計算
		// transform_ から計算されているこのベクトルをそのまま使います
		VECTOR swordHead = ModelFrameUtility::GetFreamPosFromDistance(chara->GetTransform(), attackData_.lock()->frameName, HeadDistance);
		VECTOR swordTail = ModelFrameUtility::GetFreamPosFromDistance(chara->GetTransform(), attackData_.lock()->frameName, TailDistance);

		// 軌跡に座標をセット (DirectX::XMFLOAT3 にキャストして渡す)
		VECTOR head = { swordHead.x, swordHead.y, swordHead.z };
		VECTOR tail = Utility::Lerp(swordHead, swordTail, 1.0f);
		swordTrail_->SetPos(head, tail);

		// 鮮やかな緑色をデバッグ用にセット
		FLOAT4 debugColor = { 0.0f, 1.0f, 0.0f, 1.0f }; // RGBA
		trailMaterial_->SetConstBufPS(0, debugColor);

		// 軌跡の履歴更新と頂点作成
		swordTrail_->Update();
	}
}

void DrawTrailComponent::PostUpdate(void)
{
}

void DrawTrailComponent::Draw()
{
	if (!IsActive()) return;

	// --- 軌跡の描画 ---
	if (trailRenderer_ && swordTrail_)
	{
		trailRenderer_->Draw(*swordTrail_);
	}
}