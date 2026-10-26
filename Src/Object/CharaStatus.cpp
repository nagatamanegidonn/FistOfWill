#include "CharaStatus.h"

CharaStatus::CharaStatus(void)
{
	// ステータス
	m_maxHp = 10;				// 最大体力
	m_nowHp = 10;				// 現在体力
	m_at = 10;					// 攻撃力
	m_df = 10;					// 防御力
	m_ms = 10;					// 移動速度
	m_jp = 10;					// ジャンプ力
	m_ar = Armor::Light;		// アーマー
	m_searchRange = 1000.0f;	// 索敵半径
	m_searchAngle = 10;			// 索敵アングル
	m_meleeAttackRange = 10;	// 近接攻撃距離

	// 状態
	// 無敵
	m_isNoDamage = false;
	// 攻撃を受けた
	m_isHit = false;
	// リアクションをするか
	m_isHitReaction = true;

	//1フレーム前の体力
	m_beforeHp = m_nowHp;
}

CharaStatus::~CharaStatus(void)
{
}

bool CharaStatus::CheckHitReaction(CharaStatus::AttackWeight aw, CharaStatus::Armor am)
{
	//攻撃の大きさがアーマーより大きいならtrue
	return (aw > am);
}

// ダメージ処理
bool CharaStatus::OnDamage(int damage, CharaStatus::AttackWeight weight)
{
	//攻撃は当たっているので
	m_isHit = true;

	//無敵中はダメージを食らわない
	if (m_isNoDamage)return false;

	//ダメージ計算
	m_nowHp -= damage;
	if (m_nowHp < 0)
	{
		m_nowHp = 0;
	}

	//ひるむか
	if (CheckHitReaction(weight, m_ar))
	{
		m_isHitReaction = true;
		return true;
	}
	return false;
}
