#pragma once
#include "../ActorBase.h"

class ModelMaterial;
class ModelRenderer;
class Transform;

class StageObject :
    public ActorBase
{
public:

	// コンストラクタ
	StageObject(void);
	StageObject(Transform _transform);
	StageObject(Transform _transform,
		std::unique_ptr<ModelMaterial> material);

	// デストラクタ
	~StageObject(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	virtual void OnHit(const std::weak_ptr<Collider> _hitCol) override {};

private:

	// シェーダ
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;
};

