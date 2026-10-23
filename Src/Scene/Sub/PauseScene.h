#pragma once
#include <memory>
#include "../SceneBase.h"

class PauseScene :
    public SceneBase
{

public:

	PauseScene(void);
	~PauseScene(void);

	void Update(void) override;
	void Draw(void) override;

private:

	void ResetGame(void) const;

	float timeRate_;
};


