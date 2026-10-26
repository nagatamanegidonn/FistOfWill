#pragma once
#include <vector>
#include <memory>

class EnemyManager;
class ActorBase;
class PlayerBase;

class ActorManager : 
	public std::enable_shared_from_this<ActorManager>
{
public:

	// ※UIManager::GetInstance() と同じように、どこからでも呼べる窓口
	static ActorManager& Get() { return *s_activeInstance; }

	// コンストラクタ
	ActorManager();
	// デストラクタ
	~ActorManager();

	// 登録
	void Entry(std::shared_ptr<ActorBase> _actor);
	// 解除
	void Exit(std::shared_ptr<ActorBase> _actor);

	// 初期化
	void Init();

	// スタート処理
	void Start();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了処理
	void End();

	// コンテニュー用
	void Restart();

	// アクターの生存確認
	void CheckAlive();

	// アクターの中からプレイヤーをまとめて獲得
	std::vector<std::shared_ptr<PlayerBase>> GetPlayers(void);

	// ゲームオーバー判定
	bool IsGameover()const { return isGameover_; };

	// 敵の全滅判定
	bool IsAllDeadEnemys();

	// 外部データを基にしたアクターの生成
	void CreateActorJson(const std::wstring jsonText);

private:

	// 現在動いているインスタンスを指す静的なポインタ
	static ActorManager* s_activeInstance;

	//ゲームオーバー
	bool isGameover_;

	//プレイヤー(プレイヤーも複数想定ならPlayerManager)
	std::vector<std::weak_ptr<PlayerBase>> players_;
	// 敵マネージャー
	std::shared_ptr<EnemyManager> enemyManager_;
	// アクター
	std::vector<std::shared_ptr<ActorBase>> actors_;
	// ループ中の追加を安全に行うための予約用配列
	std::vector<std::shared_ptr<ActorBase>> addActors_;

	// 敵にプレイヤーをターゲットさせる
	void PlayerEnemyTarget(void);

};

