#pragma once
#include <vector>
#include <string>

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

	//インスタンスを取得
	static LoadingManager& GetInstance(void)
	{
		static LoadingManager instance;
		return instance;
	}

	//初期化
	void Init(void);

	//更新
	void Update(void);

	//描画
	void Draw(void);

	//終了処理
	void End(void);

	//非同期ロード開始
	void StartLoading(void);

	//非同期ロード終了
	void EndLoading(void);

	//非同期ロード中かどうか
	bool IsLoading(void) const { return m_isLoading; };

private:

	//ロード中かどうか
	bool m_isLoading = false;

	//非同期ロード数
	int m_totalLoadNum = 0;

	//ロード時間カウント
	int m_loadCountFrame = 0;
	int m_afterLoadFrame = 0;

	//Loadingの.を一定間隔で出すための変数
	int m_loadingPointCountFrame = 0;
};

