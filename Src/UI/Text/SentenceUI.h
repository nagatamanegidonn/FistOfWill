
#pragma once
#include <vector>
#include <map>
#include <string>
#include "../UIBase.h"
#include "../Common/Vector2.h"

class InputController;

class SentenceUI :
    public UIBase
{
public:

    // コンストラクタ
    SentenceUI(void);

    // デストラクタ
    ~SentenceUI(void);

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void)const override;

	// 文章の設定
    void SetSentence(const std::wstring text);
    void AddSentence(const std::wstring text);

protected:

    // 文章
	std::wstring sentence_;
    
    size_t visibleCharCount_;   // 現在表示している文字数
    float timer_;               // 文字進捗用のタイマー
    float charInterval_;        // 1文字表示する間隔（秒）

    // 操作系
    // カーソルの位置
    Vector2 cursorPos_;
    // マウスの前フレームの位置
    Vector2 agoMousePos_;
};

