#pragma once
#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../Application.h"
#include "../Common/Vector2.h"
#include "SceneBase.h"

class InputController;

class InputTextArea;
class PixelMaterial;
class PixelRenderer;

class SceneUI;
class ButtonUI;

class TitleScene : 
	public SceneBase,
	public std::enable_shared_from_this<TitleScene>
{
public:

	enum class MENU
	{
		USER_SELECT,
		GAME_START,
		GAME_END,
		MAX,
	};

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	//UI
	std::shared_ptr<ButtonUI> ChangeUI_;

	//設定
	std::unique_ptr<InputController> inputController_;

	// 背景、カーソル画像
	int titleImg_;
	int backImg_;
	int cursorImg_;


	std::unique_ptr<PixelMaterial> Material_;
	std::unique_ptr<PixelRenderer> Renderer_;

	void GameStart(void);
	void GameEnd(void);
};

