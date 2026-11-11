#pragma once
#include <memory>
#include <functional>

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

	// 非同期読み込み開始
	virtual void AsyncPreLoad(void);

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

	// 読み込み完了判定
	virtual bool IsLoad(void) const;

protected:

	//更新処理管理
	std::function<void(void)> updataFunc_;
	std::function<void(void)> drawFunc_;

	//更新関数
	virtual void LoadingUpdate(void);
	virtual void NormalUpdate(void);

	//描画関数
	virtual void LoadingDraw(void);
	virtual void NormalDraw(void);


	//UI
	std::shared_ptr<SceneUI> SceneUI_;

};
