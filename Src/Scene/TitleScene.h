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

	void GameStart(void);
	void GameEnd(void);
};

