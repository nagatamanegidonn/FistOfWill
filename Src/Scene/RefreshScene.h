#pragma once
#include "SceneBase.h"


class RefreshScene : 
	public SceneBase
{
public:

	// コンストラクタ
	RefreshScene(void);

	// デストラクタ
	~RefreshScene(void);

	void Init(void) override;
	void Release(void) override;

private:

	// 更新関数
	void NormalUpdate(void) override;
	void NormalDraw(void) override;

	void GameStart(void);
	void GameEnd(void);
};

