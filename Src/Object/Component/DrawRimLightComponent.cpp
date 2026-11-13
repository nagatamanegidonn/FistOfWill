#include "DrawRimLightComponent.h"

#include "../ActorBase.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/Camera/CameraBase.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

namespace
{
	const float RIM_POW = 3.0f;	// リムライトの強さ(小さいほど強くなる)
	const FLOAT4 RIM_COLOR = { 1.0f,0.0f,0.0f,1.0f };
}

DrawRimLightComponent::DrawRimLightComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
}

DrawRimLightComponent::~DrawRimLightComponent(void)
{
}

void DrawRimLightComponent::Init(void)
{
	auto actor = actor_.lock();
	if (!actor) return;

	// シェーダの設定
	material_ = std::make_unique<ModelMaterial>(
		L"RimLightVS.cso", 0,
		L"RimLightPS.cso", 3);

	const VECTOR light = GetLightDirection();
	material_->AddConstBufPS({ light.x,light.y,light.z,1.0f });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->AddConstBufPS({ cmeraPos.x,cmeraPos.y,cmeraPos.z,RIM_POW });
	// 環境光を追加
	material_->AddConstBufPS(RIM_COLOR);
	renderer_ = std::make_unique<ModelRenderer>(actor->GetTransform().modelId, *material_);
}

void DrawRimLightComponent::Update(void)
{
	if (!IsActive()) return;
}

void DrawRimLightComponent::PostUpdate(void)
{
	if (!IsActive()) return;

	const VECTOR light = GetLightDirection();
	material_->SetConstBufPS(0, { light.x,light.y,light.z,SceneManager::GetInstance().GetTotalGameTime() });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->SetConstBufPS(1, { cmeraPos.x,cmeraPos.y,cmeraPos.z,RIM_POW });
}

void DrawRimLightComponent::DrawAfter(void)
{
	if (!IsActive()) return;

	// 描画	
	renderer_->Draw();
}