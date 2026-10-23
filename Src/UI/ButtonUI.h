#pragma once
#include <string>
#include <functional>

#include "UIBase.h"
#include "../Common/Vector2.h"

class ButtonUI :
    public UIBase
{
public:

    // コンストラクタ
    ButtonUI(void);
    ButtonUI(Vector2 pos, Vector2 size);

    // デストラクタ
    ~ButtonUI(void);

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void)const override;

	// ボタンUIとしての関数を設定する
    // 文字の設定
    void SetText(const std::wstring text) { text_ = text; }

    // 関数の設定
    void SetFunc(const std::function<void()> func) { function_ = std::bind(func); }

	// クリック時の処理
	void OnClick() { if (function_) function_(); }

	// ゲッター＆セッター
	const Vector2& GetPos(void)const { return pos_; }
	const Vector2& GetPosLocal(void)const { return posLocal_; }
	void SetPosLocal(const Vector2 pos) { posLocal_ = pos; }

	// 引数の座標がボタンの範囲内かどうかを判定する関数
    bool IsInside(const Vector2& point) const
    {
        Vector2 halfSize = { size_.x / 2 ,size_.y / 2 };
        Vector2 pos = { pos_.x + posLocal_.x ,pos_.y + posLocal_.y };
        return (pos.x - halfSize.x <= point.x && pos.x + halfSize.x >= point.x
            && pos.y - halfSize.y <= point.y && pos.y + halfSize.y >= point.y);
	}

private:

    Vector2 pos_;
    Vector2 posLocal_;
    Vector2 size_;
    std::wstring text_;

	// クリック時に実行する関数
    std::function<void()> function_;
};

