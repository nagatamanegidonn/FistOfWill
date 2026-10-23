#include "ImageUI.h"
#include <DxLib.h>
#include "../Manager/Resource/ResourceManager.h" // プロジェクトのResourceManager

ImageUI::ImageUI()
    : UIBase()
    , pos_(Vector2(0.0f, 0.0f))
    , posLocal_(Vector2(0.0f, 0.0f))
    , resourceKey_(L"")
    , handle_(-1)
    , handleIds_(nullptr)
    , maxFrames_(0)
    , frameIndex_(0)
    , scale_(1.0f)
    , angle_(0.0f)
    , transFlag_(true)
    , turnFlag_(false)
{
}

ImageUI::ImageUI(Vector2 pos, const std::wstring& resourceKey)
    : UIBase()
    , pos_(pos)
    , posLocal_(Vector2(0.0f, 0.0f))
    , resourceKey_(resourceKey)
    , handle_(-1)
    , handleIds_(nullptr)
    , maxFrames_(0)
    , frameIndex_(0)
    , scale_(1.0f)
    , angle_(0.0f)
    , transFlag_(true)
    , turnFlag_(false)
{
}

ImageUI::~ImageUI()
{
}

void ImageUI::Init()
{
    // UIManagerへ登録
    UIBase::Init();

    // 初期化時にリソースからハンドルを取得しておく
    CheckResource();
}

void ImageUI::SetResourceKey(const std::wstring& resourceKey)
{
    resourceKey_ = resourceKey;
    CheckResource();
}

void ImageUI::CheckResource()
{
    if (resourceKey_.empty())
    {
        handle_ = -1;
        handleIds_ = nullptr;
        maxFrames_ = 0;
        return;
    }

    // ResourceManagerからリソースを取得
    const auto& res = ResourceManager::GetInstance().Load(resourceKey_);

    if (res.type_ == Resource::TYPE::IMG)
    {
        handle_ = res.handleId_;
        handleIds_ = nullptr;
        maxFrames_ = 0;
    }
    else if (res.type_ == Resource::TYPE::IMGS)
    {
        handle_ = -1;
        handleIds_ = res.handleIds_;
        maxFrames_ = res.numX_ * res.numY_;
    }
    else
    {
        handle_ = -1;
        handleIds_ = nullptr;
        maxFrames_ = 0;
    }
}

void ImageUI::Update()
{
    // アニメーション処理やUI独自の動的更新が必要な場合はここに記述
}

void ImageUI::Draw() const
{
    if (!m_isDraw) return;

    // 描画するハンドルを特定
    int drawHandle = -1;

    if (handleIds_ != nullptr)
    {
        // 分割画像（IMGS）の場合
        if (frameIndex_ >= 0 && frameIndex_ < maxFrames_)
        {
            drawHandle = handleIds_[frameIndex_];
        }
    }
    else
    {
        // 単一画像（IMG）の場合
        drawHandle = handle_;
    }

    // ハンドルが無効な場合は描画しない
    if (drawHandle < 0) return;

    // ワールド座標 + ローカル座標
    Vector2 drawPos = { pos_.x + posLocal_.x, pos_.y + posLocal_.y };

    // 画像の中心を軸に拡大回転描画
    DrawRotaGraph(
        static_cast<int>(drawPos.x),
        static_cast<int>(drawPos.y),
        static_cast<double>(scale_),
        static_cast<double>(angle_),
        drawHandle,
        transFlag_ ? TRUE : FALSE,
        turnFlag_ ? TRUE : FALSE
    );
}