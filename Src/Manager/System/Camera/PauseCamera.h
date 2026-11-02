#pragma once
#include "CameraBase.h"

class PauseCamera :
    public CameraBase,
    public std::enable_shared_from_this<PauseCamera>
{
public:

    // 元のカメラと、操作対象（プレイヤーなど）の参照を受け取る
    // コンストラクタ
    PauseCamera(std::shared_ptr<CameraBase> returnCamera);
    // デストラクタ
    ~PauseCamera(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Exit(void) override;

    // 外部から明示的にポーズ解除を指示したい場合の関数
    void Resume(void);

private:

    std::shared_ptr<CameraBase> returnCam_; // ポーズ解除後に戻すカメラ

    float radius_;          // 注視点からの距離
    int prevMouseX_;        // 前フレームのマウスX位置
    int prevMouseY_;        // 前フレームのマウスY位置
    bool isDragging_;       // ドラッグ中かどうか
};