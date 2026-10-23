#pragma once
#include <memory>

class SceneUI;
class UIBase;

class SceneBase
{
public:

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void);

	// 初期化処理
	virtual void Init(void);

	// 更新ステップ
	virtual void Update(void);

	// 描画処理
	virtual void Draw(void);

	// 解放処理
	virtual void Release(void);

	// PopSceneで再開させる処理
	virtual void OnPopScene(void);

	// UIの設定
	void AddOtherUI(std::shared_ptr<UIBase> ui);

protected:

	//UI
	std::shared_ptr<SceneUI> SceneUI_;

};
