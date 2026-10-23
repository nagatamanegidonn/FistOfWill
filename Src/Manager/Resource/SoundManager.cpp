#include <DxLib.h>
#include "../Application.h"
#include "SoundManager.h"

SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
	instance_->Init();
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

bool SoundManager::AddSound(const std::wstring& key, Sound::TYPE type, const std::wstring& path, float maxVolume)
{
	// 既に同じキーが存在する場合は追加しない
	if (soundMap_.find(key) != soundMap_.end())
	{
		return false;
	}

	Sound res(type, path);
	res.ChengeMaxVolume(maxVolume);
	soundMap_.emplace(key, res);
	return true;
}

void SoundManager::Init(void)
{
	// BGM
	AddSound(L"TITLE_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/033_long_BPM184.mp3", 0.8f);
	AddSound(L"SELECT_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/084_long_BPM80.mp3", 0.8f);
	AddSound(L"BATTLE_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/159_long_BPM130.mp3", 0.8f);
	AddSound(L"TUTORIAL_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/416_long_BPM128.mp3", 0.8f);
	AddSound(L"GAMECLEAR_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/160_BPM172.mp3", 0.8f);
	AddSound(L"GAMEOVER_BGM", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"BGM/350_BPM100.mp3", 0.8f);

	// ダメージ・アクション
	AddSound(L"ATTACK1", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Attack/Attack1.mp3", 1.0f);
	AddSound(L"ATTACK2", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Attack/Attack3.mp3", 1.0f);
	AddSound(L"ATTACK3", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Attack/Attack2.mp3", 1.0f);

	AddSound(L"HitCounter", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Damage/Lobot_Hit.mp3", 1.0f);
	AddSound(L"HitDamageLow", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Damage/HitDamageLow.mp3", 1.0f);

	// カウンタータイミング
	AddSound(L"Counter", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Counter.mp3", 1.0f);

	// コネクトシーン
	AddSound(L"CONECT_START", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Conect/Start.mp3", 0.8f);
	AddSound(L"ADD", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/Conect/Add.mp3", 0.8f);

	// システムSE
	AddSound(L"SELECT", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/System/Select.mp3", 1.0f);
	AddSound(L"ENTER", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/System/Enter.mp3", 1.0f);
	AddSound(L"START", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/System/Start.mp3", 0.8f);
	AddSound(L"GAME_START", Sound::TYPE::SOUND_2D, Application::PATH_SOUND + L"SE/System/GameStart.mp3", 0.6f);
}

void SoundManager::Release(void)
{
	for (auto& p : soundMap_)
	{
		p.second.Release();  // サウンドのリソースを解放
	}
	soundMap_.clear();  // マップをクリア
}

void SoundManager::Destroy(void)
{
	Release();
	delete instance_;
	instance_ = nullptr;
}

// 音の再生（2D）
bool SoundManager::Play(const std::wstring& key, Sound::TIMES times, bool isForce)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		if (!it->second.CheckLoad())
		{
			it->second.Load();
		}
		return it->second.Play(times, isForce);
	}
	return false;
}

// 音の再生（3D）
bool SoundManager::Play(const std::wstring& key, Sound::TIMES times, VECTOR pos, float radius)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		if (!it->second.CheckLoad())
		{
			it->second.Load();
		}
		return it->second.Play(pos, radius, times);
	}
	return false;
}

void SoundManager::Stop(const std::wstring& key)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		it->second.Stop();
	}
}

void SoundManager::AllStop(void)
{
	for (auto& pair : soundMap_)
	{
		pair.second.Stop();
	}
}

bool SoundManager::Pause(const std::wstring& key, Sound::TIMES times)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		if (!it->second.CheckLoad())
		{
			it->second.Load();
		}
		it->second.Pause(times);
		return true;
	}
	return false;
}

bool SoundManager::Resume(const std::wstring& key, Sound::TIMES times)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		if (!it->second.CheckLoad())
		{
			it->second.Load();
		}
		it->second.Resume(times);
		return true;
	}
	return false;
}

bool SoundManager::CheckMove(const std::wstring& key)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		return it->second.CheckMove();
	}
	return false;
}

void SoundManager::ChengeVolume(const std::wstring& key, float per)
{
	auto it = soundMap_.find(key);
	if (it != soundMap_.end())
	{
		it->second.ChengeVolume(per);
	}
}