#pragma once
#include <vector>
#include <map>
#include <memory>
#include "ActorBase.h"

class StateBase;
class AnimationController;
class SoundController;
class CharaStatus;
class AttackData;

class CharaBase :
	public ActorBase,
	public std::enable_shared_from_this<CharaBase>
{
public:

	// 回転完了までの時間
	static constexpr float TIME_ROT = 0.1f;

	// アタックデータ
	struct AttrckData
	{
		bool isCharge = false;	//チャージ攻撃か
		int chargeId = -1;		//charge中のナンバー

		float sHitTime = 0.0f;	//判定発生時間
		float HitTime = 0.0f;	//判定終了時間

		float sNewTime = 0.0f;	//入力受付時間
		float NewTime = 0.0f;	//入力受付終了時間

		int nextAttrck = -1;	//次の攻撃ID	 
	};

	// コンストラクタ
	CharaBase(void);

	// デストラクタ
	virtual ~CharaBase(void) override = default; // ← virtual を必ずつける！

	virtual void Init(void) override = 0;
	virtual void Update(void) override = 0;
	virtual void Draw(void) override = 0;

	// アニメーションの取得
	std::weak_ptr<AnimationController> GetAnimController(void) const { return animationController_; }

	// ステータス
	std::shared_ptr<CharaStatus> GetCharaStatus(void) const;

	// 攻撃データ
	std::shared_ptr<AttackData> GetAttackData(int index) const { return attackDataList_.at(index); }

	// 移動関係の関数
	void SetPrePos(const VECTOR prePos) { prePos_ = prePos; }
	void SetMovePow(const VECTOR movePow) { movePow_ = movePow; }
	void SetMoveSpeed(const float speed) { speed_ = speed; }
	void SetDash(bool isDash) { isDash_ = isDash; }

	const VECTOR GetPrePos(void) const { return prePos_; }
	const VECTOR GetMovePow(void) const { return movePow_; }

	bool IsDash(void) const { return isDash_; }

	// 回転関係の関数
	void Rotate(void);	// 回転の更新
	void SetGoalQuaRot(const Quaternion& rot) { goalQuaRot_ = rot; }
	const Quaternion& GetGoalQuaRot(void) { return goalQuaRot_; }

	void SetPlayerRotY(const Quaternion& rot) { playerRotY_ = rot; }
	void SetStepRotTime(const float time) { stepRotTime_ = time; }

	// ジャンプ力の設定
	void JumpReset(void) { jumpPow_ = VGet(0.0f, 0.0f, 0.0f); }
	void SetJumpPow(const VECTOR jumpPow) { jumpPow_ = jumpPow; }
	void AddJumpPow(const VECTOR& jumpPow) { jumpPow_ = VAdd(jumpPow_, jumpPow); }
	const VECTOR GetJumpPow(void) const { return jumpPow_; }

	// 接地関係
	void SetIsGround(bool isGround) { isGround_ = isGround; }
	bool GetIsGround(void) const { return isGround_; }

	// 現エリアの設定：取得
	void SetAreaId(int id) { areaId_ = id; };
	const int& GetAreaId(void) const { return areaId_; }

	// リアクション関係
	void SetIsReaction(bool isReaction) { isReaction_ = isReaction; }
	bool GetIsReaction(void) const { return isReaction_; }

	// ジャスト関係
	void SetIsJust(bool just) { isJust_ = just; }
	bool GetIsJust(void) const { return isJust_; }

	// ターゲット関連
	// 追従対象の設定(場合によってはActorに変更するかも)
	void SetClosestTarget(std::shared_ptr<ActorBase> target) {
		closestTarget_ = target; // weak_ptr で安全に保持
	}
	void SetTarget(std::shared_ptr<ActorBase> target){
		target_ = target; // weak_ptr で安全に保持
	}
	void ResetTarget(void) { target_.reset(); }

	// 周囲のプレイヤーから最適なターゲットを探して更新する関数
	virtual void UpdateTarget(void);

	// 追従対象
	std::weak_ptr<ActorBase> GetTarget(void) const { return target_; }

protected:

	// 状態管理
	std::shared_ptr<StateBase> state_;

	// アニメーションコントローラー
	std::shared_ptr<AnimationController> animationController_;

	// サウンドコントローラー
	std::unique_ptr<SoundController> soundController_;

	// ステータス
	std::shared_ptr<CharaStatus> charaStatus_;

	// 攻撃データ
	std::vector<std::shared_ptr<AttackData>> attackDataList_;

#pragma region 移動,回転,重力

	// 更新前の座標
	VECTOR prePos_;

	// 移動	
	float speed_;		//移動スピード	
	VECTOR moveDir_;	//移動方向	
	VECTOR movePow_;	//移動量(横移動と前後移動の合成値)
	bool isDash_;		//ダッシュしているか

	// 回転
	Quaternion playerRotY_;	//高さの回転軸	
	Quaternion goalQuaRot_;	//最終回転
	float stepRotTime_;		//回転終了時間

	// ジャンプ量
	VECTOR jumpPow_;	//ジャンプ量(Y軸の移動量)
	bool isGround_;		//接地しているか

#pragma endregion

	// State等でのリアクション管理
	bool isReaction_;		//攻撃を受けたときのリアクションが発生するかの判定
	bool isAbsoluteReaction_;	// 攻撃を受けたときのリアクションが絶対に発生するかの判定

	// ジャスト判定
	bool isJust_;

	// エリア管理
	int areaId_;

	// アニメーションの取得
	void InitAnimation(std::wstring path);

	// 時間のカウント
	void CountTime(float& time);

	// 回転の反映
	virtual void UpdateRotate(void);
	
	// 外部CSVデータから攻撃データを読み込む関数
	void LoadCsvAttackData(std::wstring path);

	// 種類別のアニメーションデータ
	std::map<int, std::unique_ptr<AttrckData>> atkData_;
	int attrckType_ = 0;//アタックデータ使用ID

	// ターゲット（安全な弱参照で保持）
	std::weak_ptr<ActorBase> closestTarget_;	// マネージャーから教えてもらった一番近いターゲット
	std::weak_ptr<ActorBase> target_;		// 実際にロックオンしているターゲット

};
