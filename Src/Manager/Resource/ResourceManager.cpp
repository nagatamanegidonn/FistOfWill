#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::RegisterResource(const std::wstring& key, Resource::TYPE type, const std::wstring& path)
{
	resourcesMap_[key] = std::make_unique<Resource>(type, path);
}

void ResourceManager::RegisterDivImage(const std::wstring& key, const std::wstring& path, int numX, int numY, int sizeX, int sizeY)
{
	resourcesMap_[key] = std::make_unique<Resource>(Resource::TYPE::IMGS, path, numX, numY, sizeX, sizeY);
}

void ResourceManager::Init(void)
{
	static std::wstring PATH_IMG = Application::PATH_IMAGE;
	static std::wstring PATH_MDL = Application::PATH_MODEL;
	static std::wstring PATH_EFF = Application::PATH_EFFECT;

	// enum の代わりに文字列識別子（または相対パスそのもの）をキーとして登録
	RegisterResource(L"GameClearLogo", Resource::TYPE::IMG, PATH_IMG + L"GameClearLogo.png");
	RegisterResource(L"GameClear", Resource::TYPE::IMG, PATH_IMG + L"GameClear.png");
	RegisterResource(L"GameOverLogo", Resource::TYPE::IMG, PATH_IMG + L"GameOverLogo.png");
	RegisterResource(L"GameOver", Resource::TYPE::IMG, PATH_IMG + L"GameOver.png");

	RegisterResource(L"FootSmoke", Resource::TYPE::EFFEKSEER, PATH_EFF + L"Smoke/Smoke.efkefc");

	// プレイヤー
	RegisterResource(L"PlayerNight", Resource::TYPE::MODEL, PATH_MDL + L"Player2/Player2.mv1");
	// 敵
	RegisterResource(L"Enemy", Resource::TYPE::MODEL, PATH_MDL + L"Enemy/Zombie Run.mv1");
	RegisterResource(L"Boss", Resource::TYPE::MODEL, PATH_MDL + L"Boss/Boss.mv1");

	// スカイドーム
	RegisterResource(L"SkyDorm", Resource::TYPE::MODEL, PATH_MDL + L"SkyDome/SkyDome.mv1");

	// ステージモデル
	// 床モデル
	RegisterResource(L"Plane", Resource::TYPE::MODEL, PATH_MDL + L"Stage/Plane.mv1");
	// 壁モデル
	RegisterResource(L"Wall", Resource::TYPE::MODEL, PATH_MDL + L"Stage/Wall.mv1");
	// 壁モデル
	RegisterResource(L"WallStraight", Resource::TYPE::MODEL, PATH_MDL + L"Stage/WallStraight.mv1");
	// 坂モデル
	RegisterResource(L"Slope", Resource::TYPE::MODEL, PATH_MDL + L"Stage/Slope.mv1");

	// ステージテクスチャ
	RegisterResource(L"PlaneTex", Resource::TYPE::IMG, PATH_MDL + L"Stage/PlaneMain.jpg");
	RegisterResource(L"WallTex", Resource::TYPE::IMG, PATH_MDL + L"Stage/Wall.jpg");

	// エフェクト
	// ヒットエフェクト
	RegisterResource(L"HitEffect", Resource::TYPE::EFFEKSEER, PATH_EFF + L"Sample/Simple_SpawnMethod1.efkefc");
	// ヒットエフェクト
	RegisterResource(L"LaserEffect", Resource::TYPE::EFFEKSEER, PATH_EFF + L"Sample/Laser01.efkefc");;
	// ヒットエフェクト
	RegisterResource(L"RingEffect", Resource::TYPE::EFFEKSEER, PATH_EFF + L"Sample/Simple_Ring_Shape2.efkefc");
	// 閃光エフェクト
	RegisterResource(L"FlashEffect", Resource::TYPE::EFFEKSEER, PATH_EFF + L"Flash/Flash.efkefc");

	// ガイドUI
	RegisterResource(L"Button_A", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0004.png");
	RegisterResource(L"Button_B", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0005.png");
	RegisterResource(L"Button_X", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0006.png");
	RegisterResource(L"Button_Y", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0007.png");

	RegisterResource(L"L_click", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0111.png");
	RegisterResource(L"R_click", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0112.png");
	RegisterResource(L"Key_E", Resource::TYPE::IMG, PATH_IMG + L"Guide/tile_0359.png");

	RegisterResource(L"Attack", Resource::TYPE::IMG, PATH_IMG + L"Guide/Attack.png");
	RegisterResource(L"Skill", Resource::TYPE::IMG, PATH_IMG + L"Guide/Skill.png");
	RegisterResource(L"Step", Resource::TYPE::IMG, PATH_IMG + L"Guide/Step.png");
	RegisterResource(L"Fream", Resource::TYPE::IMG, PATH_IMG + L"Guide/Fream.png");

}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}
	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	resourcesMap_.clear();
	delete instance_;
	instance_ = nullptr;
}

const Resource& ResourceManager::Load(const std::wstring& key)
{
	Resource& res = _Load(key);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(const std::wstring& key)
{
	Resource& res = _Load(key);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(const std::wstring& key)
{
	// ロード済みチェック
	auto lIt = loadedMap_.find(key);
	if (lIt != loadedMap_.end())
	{
		return lIt->second;
	}

	// 事前登録チェック
	auto rIt = resourcesMap_.find(key);
	if (rIt != resourcesMap_.end())
	{
		rIt->second->Load();
		// コピーをロード済みマップへ格納
		loadedMap_.emplace(key, *rIt->second);
		return *rIt->second;
	}

	// 未登録の場合（AssetManagerのようにファイルパスとして直接読み出す場合の拡張）

	return dummy_;
}