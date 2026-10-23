#pragma once
#include <chrono>
#include <memory>
#include <DxLib.h>

#include "../Lib/nlohmann/json.hpp"

// 長いのでnamespaceの省略
using json = nlohmann::json;

class Fader;
class SceneBase;
class CameraBase;
class ShadowMap;
class UIBase;

class SceneManager
{

public:

	static constexpr float DEFAULT_FPS = 60.0f;

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		SELECT,
		TUTORIAL,
		GAME,
		RSULT,
	};

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
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);
	void Destroy(void);

	// シーン遷移命令
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void) const;

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	// ゲームの総時間の取得・設定・進行
	float GetTotalGameTime(void);
	void SetTotalGameTime(float time);
	void ForwardGameTime(void);

	void SetGameSpeedRate(float rate = 1.0f) { gameSppedRate_ = rate; }
	float GetGameSpeedRate(void) const { return gameSppedRate_; }

	// ゲーム結果の取得
	GAME_RESULT GetGameResult(void);

	// ゲーム結果の設定
	void SetGameResult(GAME_RESULT result);

	// カメラの取得
	std::weak_ptr<CameraBase> GetCameraBase(void) const { return Camera_; }

	// シャドウマップの取得
	ShadowMap* GetShadowMap(void) const { return shadowMap_.get(); }

	//スクリーンの取得
	const int& GetMainScreen(void) const;

	const int GetControllId(void)const { return ControllerId_; }

	/// <summary>
	/// シーンを新しく「積む」
	/// </summary>
	/// <param name="scene"></param>
	void PushScene(std::shared_ptr<SceneBase> _scene);

	/// <summary>
	/// 最後に追加したシーンを削除する
	/// </summary>
	void PopScene();

	// UIの設定
	void AddOtherUI(std::shared_ptr<UIBase> ui);

private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;
	
	// 描画スクリーン
	int mainScreen_;

	// フェード
	std::unique_ptr<Fader> fader_;

	// 各種シーン
	std::list<std::shared_ptr<SceneBase>> scene_;

	// カメラ
	std::shared_ptr<CameraBase> Camera_;

	// シャドウマップ
	std::unique_ptr<ShadowMap> shadowMap_;

	// シーン遷移中判定
	bool isSceneChanging_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;
	float gameSppedRate_;

	// ゲームの総時間
	float totalGameTime_;

	// ゲーム結果
	GAME_RESULT gameResult_;

	// プレイヤー設定
	int ControllerId_;

	// コンストラクタ
	SceneManager(void);

	// デストラクタ
	~SceneManager(void) = default;

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード処理
	void Fade(void);

	void Load(void);
	void Save(bool isUse) const;
};