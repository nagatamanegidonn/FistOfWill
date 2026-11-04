#pragma once
#include <deque>
#include <memory>

#include "ComponentBase.h"

class ModelMaterial;
class ModelRenderer;

// アウトライン描画コンポーネント
class DrawOutlineComponent :
	public ComponentBase
{
public:

	// コンストラクタ
	DrawOutlineComponent(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~DrawOutlineComponent(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void PostUpdate(void) override;
	virtual void DrawBefore(void) override;

protected:

	// モデル用のシェーダを保持する
	std::unique_ptr<ModelRenderer> renderer_;
	std::unique_ptr<ModelMaterial> material_;

};
