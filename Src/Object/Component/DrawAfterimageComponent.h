#pragma once
#include <deque>
#include <memory>

#include "ComponentBase.h"

class ModelMaterial;
class ModelRenderer;

// 残像描画コンポーネント
class DrawAfterimageComponent :
    public ComponentBase
{
public:

	// コンストラクタ
	DrawAfterimageComponent(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~DrawAfterimageComponent(void);

    virtual void Init(void) override;
    virtual void Update(void) override;
    virtual void PostUpdate(void) override;
    virtual void Draw(void) override;

protected:

	// 過去のポーズを記録する構造体
	struct GhostData
	{
		VECTOR pos;
		Quaternion quaRot;
		std::wstring animType;
		float animStep;
	};

	// クラスのメンバ変数として追加
	std::deque<GhostData> ghostHistory_;
	const int GHOST_MAX = 5;      // 残像の最大数
	const int GHOST_INTERVAL = 5; // 何フレームごとに記録するか
	int ghostFrameCount_ = 0;

	// 毎フレームの更新処理など
	float dissolveRate_ = 10.0f; // 変数として保持しておく

	// 各残像ごとに独立したモデルハンドルを持たせる（最大5個）
	std::vector<int> afterimageModels_;
	// それぞれのモデル用のRendererを保持する
	std::vector<std::unique_ptr<ModelRenderer>> renderers_;

	std::unique_ptr<ModelMaterial> material_;
};
