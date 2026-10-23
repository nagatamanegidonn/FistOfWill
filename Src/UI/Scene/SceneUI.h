#pragma once
#include <vector>
#include <map>
#include "../UIBase.h"
#include "../Common/Vector2.h"

class InputController;
class ButtonUI;

class SceneUI :
    public UIBase
{
public:

    // コンストラクタ
    SceneUI(void);

    // デストラクタ
    ~SceneUI(void);

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void) const override;

    // UIの追加
    void AddButtonUI(std::shared_ptr<ButtonUI> buttonUI);
    void AddButtonUI(int menuId, std::shared_ptr<ButtonUI> buttonUI);

	void AddOtherUI(std::shared_ptr<UIBase> ui);

    // メニュー状態（更新関数）を切り替える関数
    void ChangeMenuState(int menuId);

    // 描画を有効にする
    virtual void EnableDraw(void) override;
    // 描画を無効にする
    virtual void DisableDraw(void) override;

protected:

    // メニュー選択
    void UpdateManu(void);

    // カーソルの位置
    Vector2 cursorPos_;

    // マウスの前フレームの位置
    Vector2 agoMousePos_;

    // ループで選択を切り替えるためのUIリスト(ボタン限定)
    // 引数intはページ番号のような扱い
    std::map<int, std::vector<std::shared_ptr<ButtonUI>>> menuButtons_;
    std::vector<std::shared_ptr<UIBase>> otherUIs_;

    // 選択中のUIのインデックス
    int currentIndex_;

    // 選択中のUI
    std::weak_ptr<ButtonUI> selectUI_;

    // コントローラー
    std::unique_ptr<InputController> inputController_;

};

