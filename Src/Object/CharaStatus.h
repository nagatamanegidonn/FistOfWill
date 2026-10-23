#pragma once

class CharaStatus
{
public:

	// コンストラクタ
	CharaStatus(void);

	// デストラクタ
	~CharaStatus(void);

	// 攻撃の重さ
	enum class AttackWeight : int
	{
		Light = 0,
		Middle = 1,
		Heavy = 2,
		Heaviest = 3,
	};
	// 攻撃でひるまない強さ
	typedef CharaStatus::AttackWeight Armor;

	/// <summary>
	/// 攻撃とアーマーの比較
	/// 攻撃のほうが大きいならtrue
	/// </summary>
	/// <param name="aw"></param>
	/// <param name="am"></param>
	/// <returns></returns>
	bool CheckHitReaction(CharaStatus::AttackWeight aw, CharaStatus::Armor am);

	// 最大体力
	int GetMaxHP(void) const { return m_maxHp; };
	void SetMaxHP(unsigned int maxHP) { m_maxHp = maxHP; };

	// 現在体力
	int GetNowHP(void) const { return m_nowHp; };
	void SetNowHP(unsigned int nowHP) { m_nowHp = nowHP; };

	// 攻撃力
	int GetAT(void) const { return m_at; };
	void SetAT(int at) { m_at = at; };

	// 防御力
	
	// 移動速度
	
	// ジャンプ力
	
	// アーマー
	CharaStatus::Armor GetArmor(void) const { return m_ar; }
	void SetArmor(CharaStatus::Armor armor) { m_ar = armor; }
	
	// 索敵半径
	float GetSearchRange(void) const { return m_searchRange; }

	// 索敵アングル
	
	// 近接攻撃距離

	// ダメージ
	bool OnDamage(int damage, CharaStatus::AttackWeight weight);

private:

	// ステータス
	unsigned int		m_maxHp;	// 最大体力
	unsigned int		m_nowHp;	// 現在体力
	int					m_at;		// 攻撃力
	int					m_df;		// 防御力
	float				m_ms;		// 移動速度
	float				m_jp;		// ジャンプ力
	CharaStatus::Armor  m_ar;		// アーマー
	float 			m_searchRange;	// 索敵半径
	float 			m_searchAngle;	// 索敵アングル
	float 			m_meleeAttackRange; // 近接攻撃距離

	// 状態
	// 無敵
	bool m_isNoDamage;
	// 攻撃を受けた
	bool m_isHit;
	// リアクションをするか
	bool m_isHitReaction;

	//1フレーム前の体力
	unsigned int m_beforeHp;
};

