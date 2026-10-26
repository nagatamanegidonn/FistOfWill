#include "SelectUI.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/InputManager.h"
#include "../Object/Common/InputController.h"
#include "../ButtonUI.h"

namespace
{
	// ボタンサイズ
	const int WIDTH = 200;
	const int HEIGHT = 30;
}

SelectUI::SelectUI(void)
{
	//コントローラーの登録
	inputController_ = std::make_unique<InputController>(SceneManager::GetInstance().GetControllId());
	inputController_->Update();

	cursorPos_ = Vector2(0, 0);
	agoMousePos_ = Vector2(0, 0);
	currentIndex_ = 0;
}

SelectUI::~SelectUI(void)
{
}

void SelectUI::Update(void)
{
	// コントローラーの更新
	inputController_->Update();

	// 選択中のUIが有効か確認
	if (selectUI_.expired())
	{
		return;
	}

	auto& input = InputManager::GetInstance();
	Vector2 moPos = input.GetMousePos();

	// マウスカーソルがUIの上にあるか判定
	if (agoMousePos_.x != moPos.x
		|| agoMousePos_.y != moPos.y)
	{
		for (auto& button : menuButtons_.at(currentIndex_))
		{
			if (auto& activeButton = button)
			{
				if (activeButton->IsInside(moPos))
				{
					// 選択中のUIを更新
					selectUI_ = activeButton;
				}
			}
		}
	}

	// メニューの選択
	UpdateManu();

	// 決定キー処理
	if (inputController_->IsTriggered(InputController::KEY::OK_SECOND))
	{
		// 現在選択されているUIの登録関数を実行する
		if (auto activeUI = selectUI_.lock())
		{
			// ※ButtonUI内部に、SetFuncしたstd::functionを実行する公開関数があると仮定しています。
			// 名前が異なれば (例: OnClicked()、Pressed() など) 適宜変更してください。
			activeUI->OnClick();
		}
	}
	// クリックしたとき
	if (input.IsTrgMouseLeft())
	{
		// 現在選択されているUIの登録関数を実行する
		if (auto activeUI = selectUI_.lock())
		{
			if (activeUI->IsInside(moPos))
			{
				// ※ButtonUI内部に、SetFuncしたstd::functionを実行する公開関数があると仮定しています。
				// 名前が異なれば (例: OnClicked()、Pressed() など) 適宜変更してください。
				activeUI->OnClick();
			}
		}
	}

	// 選択中のUIにカーソルを移動させる処理
	if (auto activeUI = selectUI_.lock())
	{
		// ButtonUIから現在の中心座標を取得するメソッド（GetCenterPos() 等）があると仮定
		// もし無ければ、各UIの生成位置の定数(B1_C_POSなど)を元にするか、ButtonUI側に座標ゲッターを追加してください。
		Vector2 targetPos = Vector2(activeUI->GetPos().x + activeUI->GetPosLocal().x
			, activeUI->GetPos().y + activeUI->GetPosLocal().y);
		//cursorPos_ = targetPos;
		cursorPos_.x = targetPos.x;
		cursorPos_.y = std::lerp(cursorPos_.y, targetPos.y, 0.3f);
	}
	
	// 現在のメニューIdごとによるUIローカル移動
	if(currentIndex_ == 0)
	{
		Vector2 target = Vector2(0, 0);
		
		// メインメニューのUI移動処理
		for (auto& button : menuButtons_.at(0))
		{
			Vector2 currentPos = button->GetPosLocal();

			currentPos.x = std::lerp(currentPos.x, target.x, 0.3f);
			currentPos.y = std::lerp(currentPos.y, target.y, 0.3f);

			button->SetPosLocal(currentPos);
		}
	}
	else if (currentIndex_ == 1)
	{
		Vector2 target = Vector2(-WIDTH - 100, 0);
		
		// メインメニューのUI移動処理
		for (auto& button : menuButtons_.at(0))
		{
			Vector2 currentPos = button->GetPosLocal();

			currentPos.x = std::lerp(currentPos.x, target.x, 0.3f);
			currentPos.y = std::lerp(currentPos.y, target.y, 0.3f);

			button->SetPosLocal(currentPos);
		}
	}

	agoMousePos_ = moPos;
}

void SelectUI::Draw(void) const
{
	// 選択中のUIが有効か確認
	if (auto selectedButton = selectUI_.lock())
	{
		// 選択中のUIの描画処理を呼び出す
		selectedButton->Draw();
	}

	if (auto activeUI = selectUI_.lock())
	{
		int dis = WIDTH / 2 + 50;
		// カーソルの描画
		DrawTriangle(cursorPos_.x - dis, cursorPos_.y - 10,
			cursorPos_.x - dis, cursorPos_.y + 10,
			cursorPos_.x - dis + 20, cursorPos_.y,
			0xFFFFFF, true);
		DrawTriangle(cursorPos_.x - dis, cursorPos_.y - 10,
			cursorPos_.x - dis, cursorPos_.y + 10,
			cursorPos_.x - dis + 20, cursorPos_.y,
			0x00000, false);
	}
}

void SelectUI::AddButtonUI(std::shared_ptr<ButtonUI> buttonUI)
{
	menuButtons_[0].push_back(buttonUI);
	// 最初のUIを追加した場合、選択中のUIとして設定
	if (menuButtons_.size() == 1)
	{
		selectUI_ = buttonUI;
	}
}

void SelectUI::AddButtonUI(int menuId, std::shared_ptr<ButtonUI> buttonUI)
{
	menuButtons_[menuId].push_back(buttonUI);
	// 最初のUIを追加した場合、選択中のUIとして設定
	if (menuButtons_.size() == 1)
	{
		selectUI_ = buttonUI;
	}
}

void SelectUI::ChangeMenuState(int menuId)
{
	currentIndex_ = menuId;

	// 新しいメニューの先頭のボタンを選択状態にする
	if (!menuButtons_.empty())
	{
		selectUI_ = menuButtons_.at(currentIndex_).front();
	}

	for (auto& otherMenu : menuButtons_)
	{
		if (otherMenu.first > currentIndex_)
		{
			for (auto& otherButton : otherMenu.second)
			{
				// 描画無効化
				otherButton->DisableDraw();
			}
		}
		else
		{
			for (auto& otherButton : otherMenu.second)
			{
				// 描画有効化
				otherButton->EnableDraw();
			}
		}
	}
}

void SelectUI::UpdateManu(void)
{
	// UIの上下選択移動処理
	int moveDir = 0;
	if (inputController_->IsTriggered(InputController::KEY::FORWARD))
	{
		moveDir = -1; // 上へ
	}
	else if (inputController_->IsTriggered(InputController::KEY::BACK))
	{
		moveDir = 1;  // 下へ
	}

	if (moveDir != 0)
	{
		//SoundManager::GetInstance().Play(SoundManager::SRC::SELECT, Sound::TIMES::ONCE, true);

		// 現在選択されているUIのイテレータ（位置）を探す
		auto currentSelect = selectUI_.lock();
		int currentIndex = 0;

		for (size_t i = 0; i < menuButtons_.at(currentIndex_).size(); ++i)
		{
			if (menuButtons_.at(currentIndex_)[i] == currentSelect)
			{
				currentIndex = static_cast<int>(i);
				break;
			}
		}

		// 次のインデックスを計算 (ループするように処理)
		int maxCount = static_cast<int>(menuButtons_.at(currentIndex_).size());
		int nextIndex = (currentIndex + moveDir + maxCount) % maxCount;

		// 選択中のUIを更新
		selectUI_ = menuButtons_.at(currentIndex_)[nextIndex];
	}
}


