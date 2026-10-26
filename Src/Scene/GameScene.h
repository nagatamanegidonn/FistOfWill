#pragma once
#include <memory>
#include <vector>
#include "../Common/Vector2.h"

#include "SceneBase.h"

class PlayerBase;
class StageManager;
class ActorManager;
class SkyDome;
class Grid;
class Timer;
//class PixelMaterial;
//class PixelRenderer;

class GameScene : 
	public SceneBase
{
public:

	// ゲームプレイヤー数
	static constexpr int GAME_PLAYER_NUM = 2;

	// プレイヤーダウンカウント限界数
	static constexpr int MAX_DOWN_COUNT = 2;

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void DownCountPuls(void);
	const int GetDownCount(void)const { return downCnt_; }

private:

	// タイマー
	std::unique_ptr<Timer> timer_;

	// アクター管理
	std::shared_ptr<ActorManager> actorManager_;
	std::shared_ptr<StageManager> stageManager_;

	// オブジェクトクラス
	std::vector<std::shared_ptr<PlayerBase>> players_;

	// 背景インスタンス	
	std::unique_ptr<SkyDome> skyDome_;// スカイドーム
	std::unique_ptr<Grid> grid_;

	// ゲーム開始待機時間
	float stepCountDown_;
	
	// ゲームシーン用変数
	int downCnt_;		// ダウンした回数

};

