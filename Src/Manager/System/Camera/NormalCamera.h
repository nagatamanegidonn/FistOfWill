#pragma once
#include "CameraBase.h"

class NormalCamera :
    public CameraBase,
	public std::enable_shared_from_this<NormalCamera>
{
public:

	// コンストラクタ
	NormalCamera(void);
	// デストラクタ
	~NormalCamera(void) override = default;

	void Init() override;
	void Update() override;
	void Draw() override;
	void Exit() override;

private:

};

