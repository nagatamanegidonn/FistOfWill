#pragma once
#include <vector>
#include <string>
#include "../../Application.h"

class LoadingUI;

/// <summary>
/// シングルトン
/// </summary>
class LoadingManager final
{
private:

	//シングルトンの準備
	LoadingManager(void) = default;
	~LoadingManager(void) = default;
	//コピー禁止
	LoadingManager(const LoadingManager&) = delete;
	LoadingManager& operator = (const LoadingManager&) = delete;
	//ムーブ禁止
	LoadingManager(LoadingManager&&) = delete;
	LoadingManager& operator = (LoadingManager&&) = delete;

public:

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static LoadingManager& GetInstance(void);

	//初期化
	void Init(void);

	//更新
	void Update(void);

	//描画
	void Draw(void);

	//終了処理
	void Destroy(void);

	//非同期ロード開始
	void StartLoading(void);

	//非同期ロード終了
	void EndLoading(void);

	//非同期ロード中かどうか
	bool IsLoading(void) const { return isLoading_; };

private:

	// 静的インスタンス
	static LoadingManager* instance_;

	//最低ローディング時間
	static constexpr float LOADING_TIME = 2.0f;

	//フォントサイズ
	static constexpr int FONT_SIZE = 28;

	//ローディング
	static constexpr int COMMA_MAX_NUM = 7;											//「now loading......」のコンマの数
	static constexpr float COMMA_TIME = 0.5f;										//「now loading......」のコンマ数を増やす時間
	static constexpr int LOADING_STRING_POS_X = Application::SCREEN_SIZE_X - 300;	//「now loading......」の座標X
	static constexpr int LOADING_STRING_POS_Y = Application::SCREEN_SIZE_Y - 40;	//「now loading......」の座標Y

	static constexpr int TRAIN_POS_X = 1100;
	static constexpr int TRAIN_POS_Y = 640;
	static constexpr float ANIMATION_SPEED = 0.2f;

	// ローディング経過時間
	float loadingTime_;

	// ローディング中かどうか
	bool isLoading_;

	//ローディング文字列
	//CharacterString loadingString_;

	std::shared_ptr<LoadingUI> testLoadingUI_;
};

