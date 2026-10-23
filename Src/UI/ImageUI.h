#pragma once
#include <string>
#include "UIBase.h"
#include "../Common/Vector2.h"

class ImageUI : 
    public UIBase
{
public:
    // コンストラクタ / デストラクタ
    ImageUI();
    ImageUI(Vector2 pos, const std::wstring& resourceKey = L"");
    virtual ~ImageUI();

    // UIBaseのオーバーライド
    void Init() override;
    void Update() override;
    void Draw() const override;

    // キーの設定（変更時にハンドルを自動再取得）
    void SetResourceKey(const std::wstring& resourceKey);
    const std::wstring& GetResourceKey() const { return resourceKey_; }

    // 直接ハンドルを指定したい場合（MakeScreenなどで作成した動的画像用）
    void SetHandle(int handle) { handle_ = handle; handleIds_ = nullptr; }

    // --- 分割画像（IMGS）用設定 ---
    void SetFrameIndex(int index) { frameIndex_ = index; }
    int GetFrameIndex() const { return frameIndex_; }

    // --- トランスフォーム & パラメータ ---
    const Vector2& GetPos() const { return pos_; }
    void SetPos(const Vector2& pos) { pos_ = pos; }

    const Vector2& GetPosLocal() const { return posLocal_; }
    void SetPosLocal(const Vector2& posLocal) { posLocal_ = posLocal; }

    float GetScale() const { return scale_; }
    void SetScale(float scale) { scale_ = scale; }

    float GetAngle() const { return angle_; }
    void SetAngle(float angle) { angle_ = angle; }

    bool GetTransFlag() const { return transFlag_; }
    void SetTransFlag(bool transFlag) { transFlag_ = transFlag; }

    bool GetTurnFlag() const { return turnFlag_; }
    void SetTurnFlag(bool turnFlag) { turnFlag_ = turnFlag; }

    // UIBase::m_isFrontDraw のセッター
    void SetIsFrontDraw(bool isFront) { m_isFrontDraw = isFront; }

private:
    // リソースの再読み込み・キャッシュ更新
    void CheckResource();

private:
    Vector2 pos_;             // ワールド座標
    Vector2 posLocal_;        // ローカルオフセット座標
    std::wstring resourceKey_;// ResourceManager用の登録キー

    // 描画用のキャッシュ情報
    int handle_;              // 単一画像用ハンドル (IMG)
    int* handleIds_;          // 分割画像用ハンドル配列ポインタ (IMGS)
    int maxFrames_;           // 分割画像の全コマ数
    int frameIndex_;          // 現在描画するコマ番号

    float scale_;             // 拡大率（デフォルト: 1.0f）
    float angle_;             // 回転角（ラジアン）
    bool transFlag_;          // 透過フラグ（デフォルト: true）
    bool turnFlag_;           // 左右反転フラグ（デフォルト: false）
};