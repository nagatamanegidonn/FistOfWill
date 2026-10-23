#pragma once
#include "SceneBase.h"
#include <memory>

class StageManager;
class ActorManager;
class ButtonUI;

class SelectScene :
    public SceneBase,
	public std::enable_shared_from_this<SelectScene>
{
public:

	// コンストラクタ
	SelectScene(void);

	// デストラクタ
	~SelectScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// アクター管理
	std::shared_ptr<ActorManager> actorManager_;
	std::shared_ptr<StageManager> stageManager_;

	//UI
	/*std::weak_ptr<SelectTitleUI> m_selectTitleUI;
	std::weak_ptr<DialogUI> m_dialogUI;*/
	std::shared_ptr<ButtonUI> ExitUI_;

	void GameStart(void);
	void TutorialStart(void);
	void GameExit(void);

};

//#pragma once
//#include <map>
//#include <functional>
//#include <memory>
//#include <string>
//
//#include "../Application.h"
//#include "../Common/Vector2.h"