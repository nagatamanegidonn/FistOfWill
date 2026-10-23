#pragma once
#include <memory>
#include <unordered_map> // 高速化のため unordered_map に変更
#include <string>
#include "Resource.h"

class ResourceManager
{
public:
	// 明示的にインスタンスを生成
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化（パスの事前登録等を行う）
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースの登録（パス指定）
	void RegisterResource(const std::wstring& key, Resource::TYPE type, const std::wstring& path);
	void RegisterDivImage(const std::wstring& key, const std::wstring& path, int numX, int numY, int sizeX, int sizeY);

	// リソースのロード（文字列キー指定）
	const Resource& Load(const std::wstring& key);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(const std::wstring& key);

private:

	static ResourceManager* instance_;

	// enum SRC から std::wstring に変更
	std::unordered_map<std::wstring, std::unique_ptr<Resource>> resourcesMap_;
	std::unordered_map<std::wstring, Resource> loadedMap_;

	Resource dummy_;

	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(const std::wstring& key);
};