#pragma once
#include <string>
#include <vector>
#include <map>

class Resource
{

public:
	
	// リソースタイプ
	enum class TYPE
	{
		NONE,
		IMG,
		IMGS,
		MODEL,
		EFFEKSEER
	};

    // コンストラクタ
    Resource(void);
    Resource(TYPE type, const std::wstring& path);
    Resource(TYPE type, const std::wstring& path,
        int numX, int numY, int sizeX, int sizeY);

    // デストラクタ
    ~Resource(void);

    // コピー禁止（ハンドルの二重所有を防ぐ）
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    // 読み込み
    void Load(void);

    // 解放
    void Release(void);

    // ロード済みか確認
    bool IsLoaded(void) const { return isLoaded_; }

    // 複数画像ハンドルを別配列にコピー
    void CopyHandle(int* imgs);

    // リソースタイプ
    TYPE type_;

    // リソースの読み込み先
    std::wstring path_;

    // 画像とモデルのハンドルID
    int handleId_;

    // IMGS::LoadDivGraph用
    int* handleIds_;
    int numX_;
    int numY_;
    int sizeX_;
    int sizeY_;

    // モデル複製用
    // LoadModelDuplicate()で生成したハンドルのみを保存
    std::vector<int> duplicateModelIds_;

private:

    // ロード済みフラグ
    bool isLoaded_;
};
