#pragma once
#include "PlayerBase.h"

class JoinPlayer :
    public PlayerBase
{
public:

	// コンストラクタ
	JoinPlayer(int _key);
	// デストラクタ
	virtual ~JoinPlayer(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
};

