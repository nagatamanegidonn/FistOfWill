#include <DxLib.h>

#include <thread>
#include <atomic>

#include "Application.h"

std::atomic<bool> running(true);

void backgroundTask() {
	while (running) {
		// バックグラウンドでの処理
		Sleep(100); // 処理の間隔を調整
	}
}

// テスト用のシェアードポインタ
std::shared_ptr<int> testSha_;
static void TestShared(std::shared_ptr<int> a)
{
	int cnt2 = testSha_.use_count();
	std::shared_ptr<int> b = a;

	// シェアードポインタの参照カウント
	int cnt3 = testSha_.use_count();
}

// 高性能GPU（NVIDIA / AMD）を優先的に使用させるためのエクスポート設定
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// WinMain関数
//---------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::thread bgThread(backgroundTask); // バックグラウンドタスクをスレッドで実行

	// インスタンスの生成
	Application::CreateInstance();

	// インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsError())
	{
		return -1;
	}

	// ゲームループ起動
	instance.Run();

	running = false; // バックグラウンドスレッドを停止
	bgThread.join(); // スレッドの終了を待機

	// 解放処理
	instance.Destroy();

	return 0;

}

