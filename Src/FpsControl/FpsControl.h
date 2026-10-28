#pragma once
#include <vector>
#include <chrono>
#include <string>

//フレームレート
static constexpr float FRAME_RATE(1000 / 60);

class FpsControl
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fixedFps"></param>
	FpsControl(const int fixedFps);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FpsControl();

	/// <summary>
	/// フレーム制御
	/// </summary>
	void Wait();

	/// <summary>
	/// デバッグ用FPS表示
	/// </summary>
	void Draw();

	/// <summary>
	/// FPSを返す
	/// </summary>
	/// <returns></returns>
	const float GetFPS() const { return fps_; }

private:

	// 最大FPS
	const int MAX_FPS = 1200;

	// 平均FPS計算に使用するフレーム数
	const int AVG_FPS_COUNT = 60;

	// 平均FPSの右上描画位置調整
	const int MARGIN = 20;

	// 描画フォーマット
	const std::wstring TEXT_FORMAT = L"FPS : %.2f";

	// 指定された固定フレームレート
	const int fixedFps_;

	// 1フレームの理想時間(秒)
	const double idealFrameTime_;

	// 計測用FPS
	float fps_;

	// 平均FPS計測用)(秒単位)
	std::vector<double> timeList_;

	// 前フレームの時間
	std::chrono::high_resolution_clock::time_point prevTime_;
};