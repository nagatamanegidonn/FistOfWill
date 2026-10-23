#pragma once
#include <DxLib.h>
#include <string>
#include <map>
#include <list>
#include <vector>

class SceneManager;

class AnimationController
{
	
public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		std::wstring animIndex = L"";		//アニメーション番号
		float speed = 0.0f;		//再生速度
		float totalTime = 0.0f;
		float step = 0.0f;

		// アニメーション終了後に繰り返すループステップ
		float startStep = 0.0f;
		float endStep = -1.0f;

		// 逆再生
		float switchLoopReverse_ = 1.0f;
		//ブレンドするかどうか
		bool isBlend = false;
		float blendSpeed = 1.0f;
		bool isUpdateAnim = true;

		VECTOR firstPos = { 0.0f,0.0f,0.0f };
		VECTOR	movePow = { 0.0f,0.0f,0.0f };
	};

	// ブレンド情報
	struct BlendData {
		int fromAttachNo = -1;
		std::wstring animType = L""; // ← int から std::wstring に変更[cite: 6]
		float blendRate = 0.0f;
		VECTOR firstPos = { 0.0f,0.0f,0.0f };
	};
	struct BlendState {
		std::list<BlendData> data;
		int toAttachNo = -1;
		bool isBlending = false;
		float blendSpeed = 1.0f; // 1秒で100%に
		bool isUpdateAnim = true;
	};

	// コンストラクタ
	AnimationController(int modelId, std::wstring baseFream = L"mixamorig:Hips");
	// デストラクタ
	~AnimationController(void);

	// アニメーションの追加
	// sTimeとeTimeを入れるときに再生方向を決めれる
	void Set(const std::wstring& type, const std::wstring& path, float speed, int animNo = 0
		, float sTime = 0.0f, float eTime = -1.0f);
	void AddAnim(const std::wstring& type, const std::wstring& path, float speed, int animNo = 0
		, float sTime = 0.0f, float eTime = -1.0f);

	/// <summary>
	/// アニメーションのブレンド設定
	/// </summary>
	/// <param name="type"> アニメーションの種類</param>
	/// <param name="isBlend"> ブレンドするかどうか</param>
	/// <param name="blendSpeed"> ブレンドスピード（1秒で100%に）</param>
	/// <param name="isAnim"> ブレンド中にアニメーションも更新するかどうか</param>
	/// <param name="animID"> 設定を適用するアニメID（-1のときは全てに適用）</param>
	void SetIsBlend(const std::wstring& type, bool isBlend, float blendSpeed = 3.0f
		, bool isAnim = true, int animID = -1);

	// アニメーション再生
	void Play(const std::wstring& type, bool isLoop = true,
		bool isStop = false, bool isForce = false);
	void LocalPlay(const std::wstring& type, bool isLoop = true,
		bool isStop = false, bool isForce = false);

	void Update(float speedRate = 1.0f);

	// アニメーション終了後に繰り返すループステップ
	void SetEndLoop(float startStep, float endStep, float speed);

	// 再生中のアニメーション
	//int GetPlayType(void) const;
	const std::wstring GetPlayType(void) const { return playType_; }

	// 再生終了
	bool IsAnimEnd(void) const;

	const float GetStepTime(void) const { return playAnim_.step; }
	const VECTOR GetPos(void) const { return localPos_; }
	const bool IsBlend(void) const { return blend_.isBlending; }

	void DebugDraw(void);

	// 特定のポーズで完全に固定する（残像用）
	void SetPose(const std::wstring& type, float step);

	// 指定した時間（step）から再生を開始する
	void PlayStep(const std::wstring& type, float step, bool isLoop = true, bool isStop = false, bool isForce = true);

	// 再生中のアニメーションのステップ
	float GetPlayStep(void) const { return playAnim_.step; }

	// 指定したタイプのアニメーションの生のデータを取得する関数
	const Animation& GetAnimationData(const std::wstring& type) const
	{
		return animations_.at(type)[0];
	}

private :

	std::map<std::wstring, int> animModelCache_;

	// モデルのハンドルID
	int modelId_;
	// ベースフレームのTCHER
	std::wstring rootFrame_;

	// 種類別のアニメーションデータ
	std::map<std::wstring, std::vector<Animation>> animations_;
	int playAnimIdx_;

	std::wstring playType_;
	Animation playAnim_;
	int playAnimTypeCount_;

	BlendState blend_;
	float blendSpeed = 1.0f;
	int nextAnimType_ = -1; // ブレンド完了後に移行するアニメ（未使用なら -1）

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーションを止めたままにする
	bool isStop_;

	// アニメーション終了後に繰り返すループステップ
	float stepEndLoopStart_;
	float stepEndLoopEnd_;
	float endLoopSpeed_;

	// 逆再生
	float switchLoopReverse_;

	VECTOR localPos_;

	int GetAttrchNo(const std::wstring& animType);

	int LoadAnimModel(const std::wstring& path);

};

