
#pragma once
#include <memory>
#include <map>
#include "../ActorBase.h"

class ActorManager;

class StageManager :
	public std::enable_shared_from_this<StageManager>
{
public:

	// コンストラクタ
	StageManager(std::shared_ptr<ActorManager>_actorMng);
	// デストラクタ
	~StageManager();

	// 登録
	void Entry(std::shared_ptr<ActorBase> _actor);
	// 解除
	void Exit(std::shared_ptr<ActorBase> _actor);
	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	// 終了処理
	void End();

	// コンテニュー用
	void Restart();

	// JSON読み込み用の関数を追加
	void LoadStageJson(const std::wstring& jsonText);

private:

	// ステージオブジェクト生成関数
	// 個別生成用のヘルパー関数（引数を構造体化・柔軟化）
	void CreatePlane(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl);
	void CreateWall(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl);
	void CreateWallStraight(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl);
	void CreateSlope(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl);

	// アクター
	std::weak_ptr<ActorManager> actorMng_;
};