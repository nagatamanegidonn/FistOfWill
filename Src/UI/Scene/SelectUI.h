#pragma once
#include <vector>
#include <map>
#include "../UIBase.h"
#include "../Common/Vector2.h"

class InputController;
class ButtonUI;

class SelectUI :
    public UIBase
{
public:

    // コンストラクタ
    SelectUI(void);

    // デストラクタ
    ~SelectUI(void);

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void)const override;

	// UIの追加
    void AddButtonUI(std::shared_ptr<ButtonUI> buttonUI);
    void AddButtonUI(int menuId, std::shared_ptr<ButtonUI> buttonUI);

    // メニュー状態（更新関数）を切り替える関数
    void ChangeMenuState(int menuId);

private:

    // メニュー選択
	void UpdateManu(void);

	// カーソルの位置
	Vector2 cursorPos_ ;

	// マウスの前フレームの位置
	Vector2 agoMousePos_ ;

    // ループで選択を切り替えるためのUIリスト
    std::map<int,std::vector<std::shared_ptr<ButtonUI>>> menuButtons_;

	// 選択中のUIのインデックす
	int currentIndex_;

    // 選択中のUI
    std::weak_ptr<ButtonUI> selectUI_;

	// コントローラー
    std::unique_ptr<InputController> inputController_;

};

