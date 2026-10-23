#pragma once
#include <map>
#include <string>
#include "Sound.h"

class SoundManager
{
public:
	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static SoundManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	/// <summary>
	/// サウンドリソースの追加登録
	/// </summary>
	bool AddSound(const std::wstring& key, Sound::TYPE type, const std::wstring& path, float maxVolume = 1.0f);

	/// <summary>
	/// 音の再生
	/// </summary>
	bool Play(const std::wstring& key, Sound::TIMES times, bool isForce = false);	// 2D用
	bool Play(const std::wstring& key, Sound::TIMES times, VECTOR pos, float radius);	// 3D用

	void Stop(const std::wstring& key);	// 音源を停止する
	void AllStop(void);					// 全音源を停止する

	bool Pause(const std::wstring& key, Sound::TIMES times);
	bool Resume(const std::wstring& key, Sound::TIMES times);

	bool CheckMove(const std::wstring& key);

	void ChengeVolume(const std::wstring& key, float per);	// per 0.0～1.0

private:
	// 静的インスタンス
	static SoundManager* instance_;

	std::map<std::wstring, Sound> soundMap_;

	SoundManager(void) = default;
	~SoundManager(void) = default;
};