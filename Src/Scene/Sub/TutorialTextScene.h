
#pragma once
#include <memory>
#include "../SceneBase.h"

class SentenceUI;

class TutorialTextScene :
	public SceneBase
{

public:

	TutorialTextScene(const std::wstring text = L"");
	~TutorialTextScene(void);

	void Update(void) override;
	void Draw(void) override;

private:

	void ResetGame(void) const;

	float timeRate_;

	// UI
	std::shared_ptr<SentenceUI> SentenceUI_;
};


