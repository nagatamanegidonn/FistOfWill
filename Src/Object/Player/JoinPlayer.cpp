#include "JoinPlayer.h"

JoinPlayer::JoinPlayer(int _key):
	PlayerBase(_key)
{
}
JoinPlayer::~JoinPlayer(void)
{
}

void JoinPlayer::Init(void)
{
	// 基底クラスの初期化
	PlayerBase::Init();
}

void JoinPlayer::Update(void)
{

	/*const auto& pos = nIns.GetPostion(key_);
	const auto& rot = nIns.GetPlayRot(key_);


	transform_.pos = pos;
	transform_.quaRot = rot;
	transform_.Update();*/

}

void JoinPlayer::Draw(void)
{
	// 基底クラスの描画
	PlayerBase::Draw();
}

