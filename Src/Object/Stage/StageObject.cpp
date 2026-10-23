
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Manager/System/Camera/CameraBase.h"
#include "../Common/Transform.h"
#include "../Utility/ShadowMap.h"

#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

#include "StageObject.h"
#include "Destination.h"

StageObject::StageObject(void)
{
}

StageObject::StageObject(Transform _transform)
{
	transform_ = _transform;
}

StageObject::StageObject(Transform _transform, std::unique_ptr<ModelMaterial> material)
{
	transform_ = _transform;
	material_ = std::move(material);
}

StageObject::~StageObject(void)
{
}

void StageObject::Init(void)
{
	const VECTOR cmeraDirF = VNorm(VSub(transform_.pos, SceneManager::GetInstance().GetCameraBase().lock()->GetPos()));
	const VECTOR cmeraDirB = VNorm(VSub(SceneManager::GetInstance().GetCameraBase().lock()->GetPos(), transform_.pos));
	const VECTOR cmeraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	VECTOR light = GetLightDirection();
	const float mTime = SceneManager::GetInstance().GetTotalGameTime();

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);

	return;
	// ポストエフェクト用(モノトーン)
	material_ = std::make_unique<ModelMaterial>(
		L"StdModelVS.cso", 0,
		L"StdModelPS.cso", 2);
	material_->AddConstBufPS({ 200.0f,200.0f,1.0f,1.0f });
	material_->AddConstBufPS({ light.x,light.y,light.z,1.0f });
	material_->AddConstBufPS({ 0.2f,0.2f,0.2f,1.0f });//環境光を追加

	material_->SetTextureBuf(0, ResourceManager::GetInstance().Load(L"PlaneTex").handleId_);
	material_->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);

}

void StageObject::Update(void)
{
	VECTOR light = GetLightDirection();
	material_->SetConstBufPS(1, { light.x,light.y,light.z,1.0f });

	transform_.Update();
}

void StageObject::Draw(void)
{
	int shadowTexHandle = SceneManager::GetInstance().GetShadowMap()->GetSyadowHandle();
	material_->SetTextureBuf(1, shadowTexHandle); // スロット1番にセット

	// ───【ここから追加】DXライブラリから自動生成された正確なシャドウマップ行列を取得 ───
	MATRIX lightVP;
	GetShadowMapViewProjectionMatrix(shadowTexHandle, &lightVP);

	// 行列を転置して FLOAT4 に格納
	FLOAT4 row0 = { lightVP.m[0][0], lightVP.m[1][0], lightVP.m[2][0], lightVP.m[3][0] };
	FLOAT4 row1 = { lightVP.m[0][1], lightVP.m[1][1], lightVP.m[2][1], lightVP.m[3][1] };
	FLOAT4 row2 = { lightVP.m[0][2], lightVP.m[1][2], lightVP.m[2][2], lightVP.m[3][2] };
	FLOAT4 row3 = { lightVP.m[0][3], lightVP.m[1][3], lightVP.m[2][3], lightVP.m[3][3] };

	// ground_に影の描画をさせるためにVSび行列を設定
	// Planet（ground_）の定数バッファに正確な行列を転送（インデックス0~3）
	material_->SetConstBufVS(0, row0);
	material_->SetConstBufVS(1, row1);
	material_->SetConstBufVS(2, row2);
	material_->SetConstBufVS(3, row3);
	// ────────────────────────────────────────────────────────────────────────

	renderer_->Draw();
}

