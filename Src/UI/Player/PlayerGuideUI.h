
#pragma once
#include <vector>
#include <map>
#include "../UIBase.h"
#include "../Common/Vector2.h"

class InputController;
class ButtonUI;
class CharaStatus;

class PlayerGuideUI :
    public UIBase
{
public:

    // コンストラクタ
    PlayerGuideUI(std::shared_ptr<CharaStatus> status,std::shared_ptr<InputController> input);

    // デストラクタ
    ~PlayerGuideUI(void);

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void) const override;

    // UIの追加
    void AddOtherUI(std::shared_ptr<UIBase> ui);
	void AddGuideUI(std::shared_ptr<UIBase> guideUI);

    // メニュー状態（更新関数）を切り替える関数
    void ChangeMenuState(int menuId);

    // 描画を有効にする
    virtual void EnableDraw(void) override;
    // 描画を無効にする
    virtual void DisableDraw(void) override;

protected:

    // プレイヤーのステータス
    std::weak_ptr<CharaStatus> m_charaStatus;

    // カーソルの位置
    Vector2 cursorPos_;

    // マウスの前フレームの位置
    Vector2 agoMousePos_;

    // ループで選択を切り替えるためのUIリスト(ボタン限定)
    std::vector<std::weak_ptr<UIBase>> otherUIs_;

    // 変動するガイドのUI
    std::vector<std::weak_ptr<UIBase>> guideUIs_;

    // コントローラー
    std::weak_ptr<InputController> inputController_;

};

