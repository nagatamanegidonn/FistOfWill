#pragma once
#include <memory>
#include <DxLib.h>

#include "../Lib/nlohmann/json.hpp"

// 長いのでnamespaceの省略
using json = nlohmann::json;

class GameManager
{

public:

	// ゲーム結果
	enum class GAME_RESULT
	{
		NONE,
		GAME_CLEAR,
		GAME_OVER,
		TIME_OVER,
		DRAW
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static GameManager& GetInstance(void);

	void Init(void);
	void GameInit(void);
	void Destroy(void);

	// ゲーム結果の取得
	GAME_RESULT GetGameResult(void);

	// ゲーム結果の設定
	void SetGameResult(GAME_RESULT result);

	const int GetControllId(void)const { return ControllerId_; }
	const int GetWeponId(void)const { return weponId_; }
	const void SetWeponId(int weponId) { weponId_ = weponId; }

	// シーンを画像保存
	const void CaptureMainScreen(void);
	const void DrawCapturedScreen(int _x, int _y) const;

	// 接続モードについて
	const bool IsHost(void)const { return IsHost_; }
	void SetHost(bool value);

	// ステージパスの取得
	const std::wstring& GetStagePath(void)const { return stagePath_; }
	void SetStagePath(const std::wstring& path) { stagePath_ = path; }

	// エネミーパスの取得
	const std::wstring& GetEnemyPath(void)const { return enemyPath_; }
	void SetEnemyPath(const std::wstring& path) { enemyPath_ = path; }

private:

	// 静的インスタンス
	static GameManager* instance_;
	
	// コンストラクタ
	GameManager(void);

	// コピーの禁止
	GameManager(const GameManager& _copy) = delete;
	GameManager& operator= (const GameManager& _copy) = delete;

	// デストラクタ
	~GameManager(void) = default;

	//新規////////////////////////////

	// ステージパス
	std::wstring stagePath_;
	std::wstring enemyPath_;

	//////////////////////////////		

	// 描画スクリーン
	int mainScreen_;
	int capturedScreenGraph_ = -1; // 複製した画像のハンドル

	// ゲーム結果
	GAME_RESULT gameResult_;

	// プレイヤー設定
	int charId_;
	int weponId_;

	// 接続モード
	bool IsHost_;

	// コントローラーID
	int ControllerId_;

};