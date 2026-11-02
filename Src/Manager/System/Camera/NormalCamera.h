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

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Exit(void) override;

private:

};

