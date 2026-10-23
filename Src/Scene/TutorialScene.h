
#pragma once
#include <memory>
#include <vector>
#include "../Common/Vector2.h"

#include "SceneBase.h"

class PlayerBase;
class StageManager;
class ActorManager;
class SkyDome;

class TutorialScene :
	public SceneBase
{
public:

	// コンストラクタ
	TutorialScene(void);

	// デストラクタ
	~TutorialScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// アクター管理
	std::shared_ptr<ActorManager> actorManager_;
	std::shared_ptr<StageManager> stageManager_;

	// オブジェクトクラス
	std::vector<std::shared_ptr<PlayerBase>> players_;

	// 背景インスタンス	
	std::unique_ptr<SkyDome> skyDome_;// スカイドーム
};

