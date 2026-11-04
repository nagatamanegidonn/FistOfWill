#pragma once
#include <deque>
#include <memory>

#include "ComponentBase.h"

class ModelMaterial;
class ModelRenderer;

// RIM描画コンポーネント
class DrawRimLightComponent :
	public ComponentBase
{
public:

	// コンストラクタ
	DrawRimLightComponent(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~DrawRimLightComponent(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void PostUpdate(void) override;
	virtual void DrawAfter(void) override;

protected:

	// モデル用のシェーダを保持する
	std::unique_ptr<ModelRenderer> renderer_;
	std::unique_ptr<ModelMaterial> material_;

};
