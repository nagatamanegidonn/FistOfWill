#include <DxLib.h>
#include <cmath>
#include <cstring>

#include "LoadingUI.h"

#include "../Application.h"

LoadingUI::LoadingUI()
    :
    m_angle_(0.0f)
{
}

void LoadingUI::Init()
{
    // UIBaseの初期化
    UIBase::Init();

    m_angle_ = 0.0f;

    // 最前面に描画
    SetFrontDraw(true);
}

void LoadingUI::Update()
{
    // アニメーション更新
    // 60FPSを基準にする
    m_angle_ += ROTATION_SPEED / 60.0f;

    constexpr float PI = 3.14159265f;
    constexpr float TWO_PI = PI * 2.0f;

    if (m_angle_ >= TWO_PI)
    {
        m_angle_ -= TWO_PI;
    }
}

void LoadingUI::Draw() const
{
    constexpr float PI = 3.14159265f;
    constexpr float TWO_PI = PI * 2.0f;

    // 画面中央
    const float centerX = Application::SCREEN_SIZE_X * 0.5f;
    const float centerY = Application::SCREEN_SIZE_Y * 0.5f;

    for (int i = 0; i < DOT_COUNT; i++)
    {
        float rad =
            m_angle_ + TWO_PI * i / DOT_COUNT;

        int x = static_cast<int>(
            centerX + std::cos(rad) * ORBIT_RADIUS);

        int y = static_cast<int>(
            centerY + std::sin(rad) * ORBIT_RADIUS);

        // 順番に明るさを変える
        int brightness = 60 + i * 25;

        if (brightness > 255)
        {
            brightness = 255;
        }

        unsigned int color = GetColor(
            brightness / 3,
            brightness / 2,
            brightness
        );

        DrawCircle(x, y, 5, color, TRUE);
    }

    // ローディング文字
#if defined(UNICODE) || defined(_UNICODE)
    const TCHAR* text = _T("LOADING...");
#else
    const char* text = "LOADING...";
#endif

    int textWidth = GetDrawStringWidth(
        text,
        static_cast<int>(_tcslen(text))
    );

    DrawString(
        static_cast<int>(centerX - textWidth * 0.5f),
        static_cast<int>(centerY + 55.0f),
        text,
        GetColor(255, 255, 255)
    );
}