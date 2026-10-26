#pragma once
#include <memory>
#include "../SceneBase.h"

class PauseScene :
    public SceneBase
{
public:

	// コンストラクタ
	PauseScene(void);
	// デストラクタ
	~PauseScene(void);

	void Update(void) override;
	void Draw(void) override;

private:

	// 元のシーンに戻る際の処理
	void ResetGame(void) const;

	float timeRate_;
};


