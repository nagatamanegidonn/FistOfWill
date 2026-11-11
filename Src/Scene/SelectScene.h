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
	void Release(void) override;

private:

	// アクター管理
	std::shared_ptr<ActorManager> actorManager_;
	std::shared_ptr<StageManager> stageManager_;

	//UI
	std::shared_ptr<ButtonUI> ExitUI_;

	// 更新関数
	void NormalUpdate(void) override;
	void NormalDraw(void) override;

	void GameStart(void);
	void TutorialStart(void);
	void GameExit(void);

};