#include "DrawOutlineComponent.h"

#include "../ActorBase.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/Camera/CameraBase.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

DrawOutlineComponent::DrawOutlineComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
}

DrawOutlineComponent::~DrawOutlineComponent()
{
}

void DrawOutlineComponent::Init()
{
	auto actor = actor_.lock();
	if (!actor) return;

	// シェーダの設定
	/*material_ = std::make_unique<ModelMaterial>(
		"OutLineVS.cso", 1,
		"OutLinePS.cso", 2);
	material_->AddConstBufVS({ 2.0f,1.0f,1.0f,1.0f });

	const VECTOR light = GetLightDirection();
	material_->AddConstBufPS({ light.x,light.y,light.z,1.0f });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->AddConstBufPS({ cmeraPos.x,cmeraPos.y,cmeraPos.z,1.0f });
	renderer_ = std::make_unique<ModelRenderer>(actor->GetTransform().modelId, *material_);*/
}

void DrawOutlineComponent::Update()
{
	if (!IsActive()) return;

}

void DrawOutlineComponent::PostUpdate(void)
{
	if (!IsActive()) return;


	/*const VECTOR light = GetLightDirection();
	material_->SetConstBufPS(0, { light.x,light.y,light.z,SceneManager::GetInstance().GetTotalGameTime() });
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	material_->SetConstBufPS(1, { cmeraPos.x,cmeraPos.y,cmeraPos.z,0.0f });*/

}

void DrawOutlineComponent::Draw()
{
	if (!IsActive()) return;

	//アウトライン描画	
	//renderer_->Draw();
}