#pragma once
#include "../UIBase.h"
#include "../Common/Vector2.h"
#include <memory>
#include <DxLib.h> // VECTORを使うため

class CharaStatus;

class EnemyGaugeUI :
    public UIBase
{
public:
    // 引数に敵のステータスと、追従する3D座標へのポインタ（または取得元）を取る
    EnemyGaugeUI(std::shared_ptr<CharaStatus> status, const VECTOR& targetWorldPos);
    ~EnemyGaugeUI(void);

    void Update(void) override;
    void Draw(void) const override;

    // 頭上オフセットの調整
    void SetOffset(float offsetY) { m_offsetY = offsetY; }

private:
    std::weak_ptr<CharaStatus> m_charaStatus;
    const VECTOR& m_targetWorldPos; // 敵の3Dワールド座標（参照）

    VECTOR m_screenPos;            // 変換された画面上の座標
    float m_offsetY;                // 頭上に浮かせるための高さオフセット

    float m_value;
    float m_maxValue;

    Vector2 m_size;                 // ゲージのサイズ（幅、高さ）
};