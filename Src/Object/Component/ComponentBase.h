#pragma once
#include <memory>
#include <map>
#include "../CharaBase.h"

class ActorBase;

// コンポーネント基底クラス
class ComponentBase
{
public:

	// コンポーネントの種類
	enum class TYPE
	{
		CONTROL_MOVE,
		AI_MOVE,
		ATTRCK,
		EFFECT,
		SOUND,
		CAMERA,
		MAX
	};

	// コンストラクタ
	ComponentBase(void) = default;
	// デストラクタ
	virtual ~ComponentBase(void) = default;

	// 初期化処理
	virtual void Init(void) {}
	// 更新処理
	virtual void Update(void) {}
	// 後更新処理
	virtual void PostUpdate(void) {}
	// 描画処理
	virtual void Draw(void) {}

	// 活動状態の設定
	void SetActive(bool isActive) { isActive_ = isActive; }

	// 活動状態の取得
	bool IsActive() const { return isActive_; }

protected:

	// コンポーネントの有効状態
	bool isActive_ = true;

	//コンポーネントで操作するアクターベース
	std::weak_ptr<ActorBase> actor_;

	// CharaBaseを取得する（失敗したらnullptrを返す）
	std::shared_ptr<CharaBase> GetChara()
	{
		auto actor = actor_.lock();
		if (!actor) return nullptr;

		// PlayerBase → CharaBase でもOK（継承関係があるので）
		return std::dynamic_pointer_cast<CharaBase>(actor);
	}
};