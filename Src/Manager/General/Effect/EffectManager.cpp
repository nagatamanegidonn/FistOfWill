#include <EffekseerForDXLib.h>
#include "EffectManager.h"
#include "NormalEffect.h"
#include "..//Application.h"
#include "../Manager/Resource/ResourceManager.h"
#include "../Manager/Resource/Resource.h"

namespace
{
	constexpr int kMaxParticleNum = 8000;
}

//void EffectManager::InitEffekseer()
//{
//	//パーティクル数を設定
//	Effekseer_Init(kMaxParticleNum);
//	//歪みエフェクトの初期化
//	Effekseer_InitDistortion();
//	//フルスク切り替え時におかしくならないように設定
//	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
//}

//void EffectManager::EndEffekseer()
//{
//	//終了
//	Effkseer_End();
//}

void EffectManager::Entry(std::shared_ptr<NormalEffect> eff)
{
	//すでに登録されているならしない
	auto it = std::find(m_effects.begin(), m_effects.end(), eff);
	if (it != m_effects.end())return;
	//登録
	m_effects.emplace_back(eff);
}

void EffectManager::Exit(std::shared_ptr<NormalEffect> eff)
{
	//登録されていないならしない
	auto it = std::find(m_effects.begin(), m_effects.end(), eff);
	if (it == m_effects.end())return;
	//解除
	m_effects.remove(eff);
}
void EffectManager::Init()
{
	//初期化
	m_isUpdate = true;
	m_delayFrame = 0;
}

void EffectManager::Update()
{
	//遅延処理
	//UpdateDelay();
	
	//更新をしないなら
	if (!m_isUpdate || m_delayFrame > 0)
	{
		//再生ストップ
		StopEffect();
		return;
	}
	else
	{
		//再生
		StartEffect();
	}
	//更新
	for (auto& eff : m_effects)
	{
		eff->Update();
	}
	//削除予定のものを削除
	//CheckDeleteEffect();
	CheckAlive();

	//全てのエフェクトの更新
	UpdateEffekseer3D();
	UpdateEffekseer2D();
}

void EffectManager::Draw() const
{
	//Effekseerに3D表示の設定をDXライブラリの3D表示の設定に同期させる
	Effekseer_Sync3DSetting();

	//全てのエフェクトの描画
	DrawEffekseer3D_Begin();
	DrawEffekseer2D_Begin();

	for (auto& eff : m_effects)
	{
		if (!eff->IsDraw())continue;
		int playHandle = eff->GetPlayHandle();
		if (playHandle == -1)continue;
		DrawEffekseer3D_Draw(playHandle);
	}

	DrawEffekseer3D_End(); 
	DrawEffekseer2D_End();
}

void EffectManager::End()
{
	//エフェクト削除
	Reset();
}

void EffectManager::Reset()
{
	//削除
	for (auto& eff : m_effects)
	{
		eff->End();
	}
	m_effects.clear();

}

std::weak_ptr<NormalEffect> EffectManager::CreateEffect(std::wstring path, VECTOR pos)
{
	int handle = ResourceManager::GetInstance().Load(path).handleId_;
	int playHandle = PlayEffekseer3DEffect(handle);

	std::shared_ptr<NormalEffect> effect;
	effect = std::make_shared<NormalEffect>(playHandle, pos);

	Entry(effect);
	return effect;
}

void EffectManager::StopEffect()
{
	//止める
	m_isUpdate = false;
}

void EffectManager::StartEffect()
{
	//再生
	m_isUpdate = true;
}

void EffectManager::DelayUpdate(int frame)
{
	m_delayFrame = frame;
	m_isUpdate = false;
}

//エフェクトの消滅フラグをチェックして削除
void EffectManager::CheckAlive()
{
	// 配列（actors_）から、条件（IsDeleteがtrue）に一致する要素を削除する
	std::erase_if(m_effects, [this](const std::shared_ptr<NormalEffect>& eff)
		{
			// 1. そもそもアクターが有効か、消滅フラグが立っているかチェック
			if (eff == nullptr || eff->IsDelete())
			{
				//終了処理
				eff->End();
				return true; // 条件一致：m_effects から削除する
			}
			return false; // 生存：削除しない
		});
}