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
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	void GameStart(void);
	void GameEnd(void);
};

