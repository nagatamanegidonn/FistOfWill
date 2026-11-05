
#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "Resource.h"

// コンストラクタ
Resource::Resource(void)
    :
    type_(TYPE::NONE),
    path_(L""),
    handleId_(-1),
    handleIds_(nullptr),
    numX_(-1),
    numY_(-1),
    sizeX_(-1),
    sizeY_(-1),
    isLoaded_(false)
{
}

// コンストラクタ
Resource::Resource(TYPE type, const std::wstring& path)
    :
    type_(type),
    path_(path),
    handleId_(-1),
    handleIds_(nullptr),
    numX_(-1),
    numY_(-1),
    sizeX_(-1),
    sizeY_(-1),
    isLoaded_(false)
{
}

// コンストラクタ（分割画像用）
Resource::Resource(
    TYPE type,
    const std::wstring& path,
    int numX,
    int numY,
    int sizeX,
    int sizeY)
    :
    type_(type),
    path_(path),
    handleId_(-1),
    handleIds_(nullptr),
    numX_(numX),
    numY_(numY),
    sizeX_(sizeX),
    sizeY_(sizeY),
    isLoaded_(false)
{
}

// デストラクタ
Resource::~Resource(void)
{
    Release();
}

// 読み込み
void Resource::Load(void)
{
    // すでに読み込み済みなら何もしない
    if (isLoaded_)
    {
        return;
    }

    switch (type_)
    {
    case TYPE::IMG:
    {
        handleId_ = LoadGraph(path_.c_str());
        isLoaded_ = (handleId_ != -1);
        break;
    }
    case TYPE::IMGS:
    {
        if (numX_ <= 0 || numY_ <= 0)
        {
            break;
        }
        const int count = numX_ * numY_;
        handleIds_ = new int[count];
        // すべて無効なハンドルで初期化
        for (int i = 0; i < count; i++)
        {
            handleIds_[i] = -1;
        }
        int result = LoadDivGraph(
            path_.c_str(),
            count,
            numX_,
            numY_,
            sizeX_,
            sizeY_,
            handleIds_);
        isLoaded_ = (result != -1);
        break;
    }
    case TYPE::MODEL:
    {
        // 元モデルを読み込む
        // 複製モデルはResourceManager::LoadModelDuplicate()で作成する
        handleId_ = MV1LoadModel(path_.c_str());
        isLoaded_ = (handleId_ != -1);
        break;
    }
    case TYPE::EFFEKSEER:
    {
        handleId_ = LoadEffekseerEffect(path_.c_str());
        isLoaded_ = (handleId_ != -1);
        break;
    }
    default:
        break;
    }
}

// 解放
void Resource::Release(void)
{
    // 追加複製モデルを解放
    for (int id : duplicateModelIds_)
    {
        if (id != -1)
        {
            MV1DeleteModel(id);
        }
    }

    duplicateModelIds_.clear();

    switch (type_)
    {
    case TYPE::IMG:
    {
        if (handleId_ != -1)
        {
            DeleteGraph(handleId_);
        }

        break;
    }
    case TYPE::IMGS:
    {
        if (handleIds_ != nullptr)
        {
            const int count = numX_ * numY_;

            for (int i = 0; i < count; i++)
            {
                if (handleIds_[i] != -1)
                {
                    DeleteGraph(handleIds_[i]);
                }
            }

            delete[] handleIds_;
            handleIds_ = nullptr;
        }
        break;
    }
    case TYPE::MODEL:
    {
        // 元モデルを解放
        if (handleId_ != -1)
        {
            MV1DeleteModel(handleId_);
        }
        break;
    }
    case TYPE::EFFEKSEER:
    {
        if (handleId_ != -1)
        {
            DeleteEffekseerEffect(handleId_);
        }
        break;
    }
    default:
        break;
    }

    handleId_ = -1;
    isLoaded_ = false;
}

// 分割画像ハンドルをコピー
void Resource::CopyHandle(int* imgs)
{
    if (handleIds_ == nullptr || imgs == nullptr)
    {
        return;
    }

    const int count = numX_ * numY_;

    for (int i = 0; i < count; i++)
    {
        imgs[i] = handleIds_[i];
    }
}
