#pragma once
#include <list>
#include <memory>
#include <string>
#include <DxLib.h>

class NormalEffect;

class EffectManager final
{
private:

	// シングルトンの準備
	EffectManager() = default;
	~EffectManager() = default;
	// コピー禁止
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator = (const EffectManager&) = delete;
	// ムーブ禁止
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator = (EffectManager&&) = delete;

public:

	// インスタンスを取得
	static EffectManager& GetInstance()
	{
		static EffectManager instance;
		return instance;
	}

	// 登録
	void Entry(std::shared_ptr<NormalEffect> eff);
	// 解除
	void Exit(std::shared_ptr<NormalEffect> eff);

	// 初期化
	void Init();

	// 更新処理
	void Update();

	// 描画
	void Draw()const;

	// 終了処理
	void End();

	// すべてのエフェクトを削除
	void Reset();

	// エフェクトを作成
	std::weak_ptr<NormalEffect> CreateEffect(std::wstring path, VECTOR pos);

	// エフェクトを止める
	void StopEffect();
	// エフェクトを開始
	void StartEffect();
	// 数フレーム止める
	void DelayUpdate(int frame);

private:

	// エフェクトの配列
	std::list<std::shared_ptr<NormalEffect>> m_effects;
	// 更新するか
	bool m_isUpdate = true;
	// 更新を遅らせる
	int m_delayFrame = 0;

	// Effectの生存確認
	void CheckAlive();

};

