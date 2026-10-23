#include "EnemyGaugeUI.h"
#include "../Object/CharaStatus.h"
#include <algorithm>
#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Camera/CameraBase.h"

EnemyGaugeUI::EnemyGaugeUI(std::shared_ptr<CharaStatus> status, const VECTOR& targetWorldPos)
    : UIBase()
    , m_charaStatus(status)
    , m_targetWorldPos(targetWorldPos)
    , m_offsetY(0.0f) // 敵の足元からどれくらい上に表示するか
    , m_value(100.0f)
    , m_maxValue(100.0f)
    , m_size(Vector2(60, 8)) // 頭上用なので少し小さめ
{
}

EnemyGaugeUI::~EnemyGaugeUI()
{
}

void EnemyGaugeUI::Update()
{
    // ステータスが破棄されていたら自身を削除対象にする
    if (m_charaStatus.expired())
    {
        Delete();
        return;
    }

    auto status = m_charaStatus.lock();
    m_value = status->GetNowHP();
    m_maxValue = status->GetMaxHP();
}

void EnemyGaugeUI::Draw() const
{
    if (!m_isDraw) return;
    if (m_value <= 0.0f) return;
    if (m_maxValue <= 0.0f) return;

    // 敵のワールド座標
    VECTOR centerPos = m_targetWorldPos;
    centerPos.y += m_offsetY;

    // ワールド座標をスクリーン座標に変換
    VECTOR screenPos = ConvWorldPosToScreenPos(centerPos);

    // カメラ取得
    auto camera = SceneManager::GetInstance().GetCameraBase().lock();

    if (!camera)
    {
        return;
    }

    // カメラの位置と注視点から前方向を求める
    VECTOR cameraForward = VNorm(
        VSub(camera->GetTargetPos(), camera->GetPos())
    );

    // カメラから敵への方向
    VECTOR toEnemy = VSub(centerPos, camera->GetPos());

    // カメラ前方への距離
    float forwardDistance = VDot(toEnemy, cameraForward);

    // カメラの後ろ側にいる敵は描画しない
    if (forwardDistance <= 0.0f)
    {
        return;
    }

    // HP割合
    float rate = std::clamp(m_value / m_maxValue, 0.0f, 1.0f);

    // ゲージの画面上のサイズ
    int halfW = static_cast<int>(m_size.x * 0.5f);
    int halfH = static_cast<int>(m_size.y * 0.5f);

    int x = static_cast<int>(screenPos.x);
    int y = static_cast<int>(screenPos.y);

    // 背景（黒）
    DrawBox(
        x - halfW,
        y - halfH,
        x + halfW,
        y + halfH,
        GetColor(30, 30, 30),
        TRUE
    );

    // HPゲージ（赤）
    int gaugeWidth = static_cast<int>(m_size.x * rate);

    if (gaugeWidth > 0)
    {
        DrawBox(
            x - halfW,
            y - halfH,
            x - halfW + gaugeWidth,
            y + halfH,
            GetColor(255, 0, 0),
            TRUE
        );
    }

    // 白枠
    DrawBox(
        x - halfW,
        y - halfH,
        x + halfW,
        y + halfH,
        GetColor(255, 255, 255),
        FALSE
    );
}