#include "DrawOutlineComponent.h"

#include "../ActorBase.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/Camera/CameraBase.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

namespace
{
	const float OUTLINE_WIDTH = 0.5f;
}

DrawOutlineComponent::DrawOutlineComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
}

DrawOutlineComponent::~DrawOutlineComponent(void)
{
}

void DrawOutlineComponent::Init(void)
{
	auto actor = actor_.lock();
	if (!actor) return;

	// シェーダの設定
	material_ = std::make_unique<ModelMaterial>(
		L"OutLineVS.cso", 1,
		L"OutLinePS.cso", 2);
	material_->AddConstBufVS({ OUTLINE_WIDTH,1.0f,1.0f,1.0f });

	const VECTOR light = GetLightDirection();
	material_->AddConstBufPS({ light.x,light.y,light.z,1.0f });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->AddConstBufPS({ cmeraPos.x,cmeraPos.y,cmeraPos.z,1.0f });
	renderer_ = std::make_unique<ModelRenderer>(actor->GetTransform().modelId, *material_);
}

void DrawOutlineComponent::Update(void)
{
	if (!IsActive()) return;
}

void DrawOutlineComponent::PostUpdate(void)
{
	if (!IsActive()) return;

	const VECTOR light = GetLightDirection();
	material_->SetConstBufPS(0, { light.x,light.y,light.z,SceneManager::GetInstance().GetTotalGameTime() });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->SetConstBufPS(1, { cmeraPos.x,cmeraPos.y,cmeraPos.z,0.0f });
}

void DrawOutlineComponent::DrawBefore(void)
{
	if (!IsActive()) return;

	// モデル描画のZBufferを無効にする
	MV1SetWriteZBuffer(actor_.lock()->GetTransform().modelId, false);

	// アウトライン描画	
	renderer_->Draw();

	// モデル描画のZBufferを有効にする
	MV1SetWriteZBuffer(actor_.lock()->GetTransform().modelId, true);
}