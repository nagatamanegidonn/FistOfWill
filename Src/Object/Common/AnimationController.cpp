#include <DxLib.h>
#include "../../Manager/General/SceneManager.h"
#include "../../Utility/ModelFrameUtility.h"
#include "../../Common/Quaternion.h"
#include "AnimationController.h"

AnimationController::AnimationController(int modelId, std::wstring baseFream)
	:
	modelId_(modelId),
	rootFrame_(baseFream),
	localPos_({ 0.0f,0.0f,0.0f }),
	playAnimTypeCount_(0)
{
	playAnimIdx_ = 0;
	playType_ = L"";
	isLoop_ = false;
	isStop_ = false;
	switchLoopReverse_ = 0.0f;
	endLoopSpeed_ = 0.0f;
	stepEndLoopStart_ = 0.0f;
	stepEndLoopEnd_ = 0.0f;
}

AnimationController::~AnimationController(void)
{
	// アニメーションモデルはリソースキャッシュされているので、解放してはいけない
	// ただし、アニメーションモデルのハンドルを保持している map はクリアする
	animModelCache_.clear();
	animations_.clear();
	blend_.data.clear();
}

void AnimationController::Set(const std::wstring& type, const std::wstring& path, float speed, int animNo
	, float sTime, float eTime)
{
	Animation anim;

	anim.model = LoadAnimModel(path);
	if (anim.model == -1)
	{
		return;
	}

	// Animation 構造体側の animIndex も std::wstring にする場合はここを調整する
	anim.animIndex = type;
	anim.speed = speed;
	anim.attachNo = animNo;
	// 挿入
	animations_[type].emplace_back(anim);

	// [0] ではなく、今挿入したばかりの要素（.back()）に対して設定を行う！
	auto& targetAnim = animations_[type].back();

	// 再生方向の変更
	if (sTime > eTime && eTime != -1.0f)
	{
		targetAnim.switchLoopReverse_ = -1.0f;
	}
	targetAnim.startStep = sTime;
	targetAnim.endStep = eTime;
}
void AnimationController::AddAnim(const std::wstring& type, const std::wstring& path, float speed, int animNo, float sTime, float eTime)
{
	Animation anim;
	int addIndex = animations_[type].size();

	// すでに同じ path が読み込まれていれば、それを再利用
	if (animModelCache_.count(path))
	{
		anim.model = animModelCache_[path];
	}
	else
	{
		anim.model = MV1LoadModel(path.c_str());
		animModelCache_[path] = anim.model;
	}
	anim.animIndex = type;
	anim.speed = speed;

	anim.attachNo = animNo;

	// 挿入
	animations_[type].emplace_back(anim);

	// 再生方向の変更
	if (sTime > eTime && eTime != -1.0f)
	{
		animations_[type][addIndex].switchLoopReverse_ = -1.0f;
	}
	animations_[type][addIndex].startStep = sTime;
	animations_[type][addIndex].endStep = eTime;
}

// ブレンド設定
void AnimationController::SetIsBlend(const std::wstring& type, bool isBlend, float blendSpeed
	, bool isAnim, int animID)
{
	// 指定された animID が有効な範囲内であれば、そのアニメーションにのみ設定を適用
	if (animID >= 0 && animID < animations_[type].size())
	{
		animations_[type][animID].isBlend = isBlend;
		animations_[type][animID].blendSpeed = blendSpeed;
		animations_[type][animID].isUpdateAnim = isAnim;
	}
	// そうでなければ、全てのアニメーションに設定を適用
	else
	{
		for (auto& anim : animations_[type])
		{
			anim.isBlend = isBlend;
			anim.blendSpeed = blendSpeed;
			anim.isUpdateAnim = isAnim;
		}
	}
}

// アニメーション開始処理
void AnimationController::Play(const std::wstring& type, bool isLoop,
	bool isStop, bool isForce)
{
	if (blend_.isBlending) {
		// ブレンド中、現在のアニメーションと同じなら変更不可
		if (blend_.data.back().animType == type)
		{
			return;
		}
	}

	// 現在再生中のアニメーションと同じタイプで、強制再生でない場合は無視
	if (playType_ == type && !isForce)
	{
		return;
	}

	playAnimIdx_ = 0;

	LocalPlay(type, isLoop, isStop, isForce);

}

// アニメーション開始処理
void AnimationController::LocalPlay(const std::wstring& type, bool isLoop, bool isStop, bool isForce)
{
	// エラー防止：指定されたアニメーションタイプが存在しない場合は無視
	if(animations_.at(type).empty())
	{
		printfDx(L"Warning: Animation type '%s' does not exist.\n", type.c_str());
		return;
	}

	//最初の再生は通さない（playType_ != -1）
	if (playType_ != L"" && !isForce && animations_[type][playAnimIdx_].isBlend)
	{
		//ブレンド設定
#pragma region ブレンド	
		// 新しいアニメをアタッチする前に、今再生しているものを「ブレンド元」として保存
		BlendData bData;
		bData.fromAttachNo = playAnim_.attachNo; // これが重要
		bData.animType = playType_; // 現在のタイプ
		bData.blendRate = 1.0f;
		bData.firstPos = playAnim_.firstPos;

		// すでに他にブレンド中のものがあれば、それらの合計レートを引く
		if (!blend_.data.empty())
		{
			float total = 0.0f;
			for (auto& d : blend_.data) total += d.blendRate;
			bData.blendRate = 1.0f - total;
			if (bData.blendRate < 0.0f) bData.blendRate = 0.0f;
		}
		blend_.data.push_back(bData);
#pragma endregion

		// --- ここで新しいアニメの設定に切り替える ---
		playType_ = type;
		playAnim_ = animations_[type][playAnimIdx_]; // ここで上書き

		//アタッチナンバーの取得
		int attachNo = GetAttrchNo(type);

		//アニメーションの総時間を獲得
		float animTotal = MV1GetAttachAnimTotalTime(modelId_, attachNo);
		if (animTotal <= 0.0f)
		{
			printfDx(L"Warning: totalTime is invalid (attachNo=%d totalTime=%f)\n", attachNo, animTotal);
			// 適当な仮の時間を設定する（もしくは return でもOK）
			animTotal = 1.0f;
		}

		// ブレンド設定
		blend_.toAttachNo = attachNo;
		blend_.isBlending = true;

		// ブレンドスピード設定
		blendSpeed = animations_[type][playAnimIdx_].blendSpeed;
		blend_.isUpdateAnim = animations_[type][playAnimIdx_].isUpdateAnim;

		playAnim_.attachNo = attachNo;
		playAnim_.step = animations_[type][playAnimIdx_].startStep;

		if (animations_[type][playAnimIdx_].switchLoopReverse_ >= 0.0f)
		{
			// endStepを調べ－１じゃないならそれを入れる
			playAnim_.totalTime = animations_[type][playAnimIdx_].endStep > -1.0f ? animations_[type][playAnimIdx_].endStep : animTotal;
		}
		else
		{
			playAnim_.totalTime = animations_[type][playAnimIdx_].endStep;
		}

		// アニメーションループ
		isLoop_ = isLoop;

		// アニメーションしない
		isStop_ = isStop;

		// 再生変数
		stepEndLoopStart_ = animations_[type][playAnimIdx_].startStep;
		stepEndLoopEnd_ = playAnim_.totalTime;

		switchLoopReverse_ = animations_[type][playAnimIdx_].switchLoopReverse_;
	}
	// ブレンドをしない場合(複種用のブレンド処理を入れる<-入れるのはUpdateでは？)
	else
	{
		// 現在アタッチされている可能性のあるものをすべて外す
		// ブレンド中のデータ
		for (auto& data : blend_.data)
		{
			if (data.fromAttachNo != -1)
			{
				MV1DetachAnim(modelId_, data.fromAttachNo);
			}
		}
		blend_.data.clear();
		blend_.isUpdateAnim = true;

		// ブレンド先のターゲット
		if (blend_.toAttachNo != -1)
		{
			MV1DetachAnim(modelId_, blend_.toAttachNo);
			blend_.toAttachNo = -1;
		}

		// 現在再生中のメインアニメ
		if (playAnim_.attachNo != -1)
		{
			MV1DetachAnim(modelId_, playAnim_.attachNo);
			playAnim_.attachNo = -1;
		}

		// 状態のリセット
		blend_.isBlending = false;

		// 新しいアニメーション種別のセット
		playType_ = type;
		playAnim_ = animations_[type][playAnimIdx_];

		// 初期化とアタッチ
		playAnim_.step = animations_[type][playAnimIdx_].startStep;
		playAnim_.attachNo = GetAttrchNo(type);

		// 総時間の決定
		if (animations_[type][playAnimIdx_].switchLoopReverse_ >= 0.0f)
		{
			playAnim_.totalTime = (animations_[type][playAnimIdx_].endStep > -1.0f) ?
				animations_[type][playAnimIdx_].endStep : MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);
		}
		else
		{
			playAnim_.totalTime = animations_[type][playAnimIdx_].endStep;
		}

		// 再生パラメータ同期
		isLoop_ = isLoop;
		isStop_ = isStop;
		stepEndLoopStart_ = animations_[type][playAnimIdx_].startStep;
		stepEndLoopEnd_ = playAnim_.totalTime;
		switchLoopReverse_ = animations_[type][playAnimIdx_].switchLoopReverse_;
	}

	// アニメーション進行前のルートのローカル座標
	int rootFrame = MV1SearchFrame(modelId_, rootFrame_.c_str());
	playAnim_.firstPos = MV1GetAttachAnimFrameLocalPosition(
		modelId_, playAnim_.attachNo, rootFrame);
	playAnim_.movePow = { 0.0f,0.0f,0.0f };
}

// アニメーション再生
void AnimationController::Update(float speedRate)
{
	//再生速度
	float deltaTime = SceneManager::GetInstance().GetDeltaTime() * speedRate;

	bool isNext = false;

	//再生処理
	if (!isStop_ && blend_.isUpdateAnim)
	{
		playAnim_.step += (deltaTime * playAnim_.speed * switchLoopReverse_);
		bool isEnd = false;
		//再生方向が正か負か
		if (switchLoopReverse_ > 0.0f)
		{
			// 通常再生の場合
			if (playAnim_.step > playAnim_.totalTime)
			{
				isEnd = true;
			}
		}
		else
		{
			// 逆再生の場合
			if (playAnim_.step < playAnim_.totalTime)
			{
				isEnd = true;
			}
		}

		//アニメーションが終わったら
		if (isEnd)
		{
			//ループするか
			if (isLoop_)
			{
				//再生方向の変更がだいぶ変わっているのでこのままでおｋ
				playAnim_.step = stepEndLoopStart_;
				playAnim_.totalTime = stepEndLoopEnd_;
			}
			else
			{
				isStop_ = true; // 再生停止
			}

			auto& animList = animations_[playAnim_.animIndex];

			// アニメが複数ある場合のみ処理する
			if (animList.size() > 1)
			{
				isNext = true;
				playAnimIdx_++;

				// 最後まで行ったら0に戻す
				if (playAnimIdx_ >= animList.size())
				{
					playAnimIdx_ = 0;
				}

				LocalPlay(playAnim_.animIndex);
			}
		}
	}

	// ブレンド処理
	if (blend_.isBlending)
	{
		// deltaTimeを使って先頭（古いアニメ）のレートを減らす
		blend_.data.front().blendRate -= blend_.blendSpeed * deltaTime * blendSpeed;

		// 先頭のアニメが完全に消えたらデタッチ
		if (blend_.data.front().blendRate <= 0.0f)
		{
			MV1DetachAnim(modelId_, blend_.data.front().fromAttachNo);
			blend_.data.pop_front();

			// もしリストが空になったらブレンド終了
			if (blend_.data.empty())
			{
				blend_.isBlending = false;
				blend_.isUpdateAnim = true;
			}
		}

		// 各アニメのブレンド率をモデルに適用
		float totalOtherRate = 0.0f;
		for (auto& data : blend_.data)
		{
			MV1SetAttachAnimBlendRate(modelId_, data.fromAttachNo, data.blendRate);
			totalOtherRate += data.blendRate;
		}
		// メインのアニメーション（playAnim_）は残りの比率
		float mainRate = 1.0f - totalOtherRate;
		if (mainRate < 0.0f) mainRate = 0.0f;

		// ブレンド率を設定
		MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, mainRate);
	}

	// アニメーションがアタッチされていない場合は処理しない
	if (playAnim_.attachNo == -1)
	{
#ifdef _DEBUG
		printfDx(L"Warning: playAnim_ にアニメがアタッチされていません\n");
#endif // _DEBUG
		return;
	}

	// 腰のフレーム
	int rootFrame = MV1SearchFrame(modelId_, rootFrame_.c_str());

	// アニメーション進行前のルートのローカル座標
	VECTOR pre = MV1GetAttachAnimFrameLocalPosition(
		modelId_, playAnim_.attachNo, rootFrame);

	//アニメションアタッチ
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);

	// アニメーション進行後のルートのローカル座標
	VECTOR post = MV1GetAttachAnimFrameLocalPosition(
		modelId_, playAnim_.attachNo, rootFrame);

	// アニメーション移動量を取得
	playAnim_.movePow = VSub(post, pre);

	// 腰の位置がずれるので補正
	playAnim_.firstPos.y = post.y;

	// ブレンド中ならローカル移動量分モデルをずらす
	if (blend_.isBlending)
	{
		VECTOR fPos = { 0.0f,0.0f,0.0f };

		float mRate_ = 0.0f;
		for (auto& data : blend_.data)
		{
			fPos = VAdd(fPos, VScale(data.firstPos, data.blendRate));
			mRate_ += data.blendRate;

		}

		fPos = VAdd(fPos, VScale(playAnim_.firstPos, 1.0f - mRate_));

		std::list<ModelFrameUtility::BlendData> blendList;
		for (auto& d : blend_.data)
		{
			blendList.push_back({ modelId_, d.fromAttachNo, d.blendRate, d.firstPos });
		}

		// モデルフレームの位置を調整
		ModelFrameUtility::SetFrameAnimAttachLocalMatrixPos(
			blendList,
			rootFrame,
			playAnim_.attachNo,		// to 側 attachNo
			fPos					// to 側 firstPos
		);
	}
	else
	{
		// 移動量を打ち消す
		// 単独アニメのときは従来通り
		ModelFrameUtility::SetFrameAnimAttachLocalMatrixPos(
			modelId_, playAnim_.attachNo, rootFrame, playAnim_.firstPos);
	}

#pragma region ローカル回転補正

	// 背中を少し傾ける(ブレンドとは別のフレームなので問題なし)
	Quaternion goalRot = Quaternion::AngleAxis(-DX_PI_F / 4.0f, VGet(1.0f, 0.0f, 0.0f));//45度傾ける(ローカル)
	goalRot = goalRot.Mult(Quaternion::AngleAxis(-DX_PI_F / 2.0f, VGet(0.0f, 1.0f, 0.0f)));

	// 背中のフレーム
	rootFrame = MV1SearchFrame(modelId_, L"mixamorig:Spine");
	//ModelFrameUtility::SetFreamRotation(modelId_, rootFrame, goalRot);

#pragma endregion
}

void AnimationController::SetEndLoop(float startStep, float endStep, float speed)
{
	stepEndLoopStart_ = startStep;
	stepEndLoopEnd_ = endStep;
	endLoopSpeed_ = speed;
}

bool AnimationController::IsAnimEnd(void) const
{
	bool ret = false;

	if (isLoop_)
	{
		// ループ設定されているなら、
		// 無条件で終了しないを返す
		return ret;
	}
	
	if (switchLoopReverse_ > 0.0f)
	{
		// 通常再生の場合
		if (playAnim_.step >= playAnim_.totalTime)
		{
			// 再生時間を過ぎたらtrue
			return true;
		}
	}
	else
	{
		// 逆再生の場合
		if (playAnim_.step <= playAnim_.totalTime)
		{
			// 再生時間を過ぎたらtrue
			return true;
		}
	}

	return ret;
}

void AnimationController::DebugDraw(void)
{
	// 実際にアタッチされているアニメーションの数を数える
	// 現在のアタッチ数をカウント
	int totalAttached = 0;
	while (MV1GetAttachAnim(modelId_, totalAttached) != -1)
	{
		totalAttached++;
	}

	// アタッチされているアニメーション数を表示
	DrawFormatString(100, 0, 0xffffff, L"DXLib Total Attached: %d", totalAttached);

	int py = 16;
	float totalWeight = 0.0f;

	int count = 1;

	for (auto& data : blend_.data)
	{
		DrawFormatString(100, 100 + py, 0xff0000, L"%d目のアニメブレンド%.2f", count, data.blendRate);
		py += 16;
		totalWeight += data.blendRate;

		count += 1;
	}

	DrawFormatString(100, 100 + py, 0xff0000, L"再生中のアニメブレンド%.2f", 1.0f - totalWeight);
	py += 16; 

	DrawFormatString(100, 48, 0x0000ff, L"現在のアニメフレーム%.2f", playAnim_.step);
	DrawFormatString(100, 32, 0x0000ff, L"animation[%d][%d]", playAnim_.animIndex, playAnimIdx_);
	DrawFormatString(100, 16, 0xff0000, L"現在のアニメブレンド状態: %s",
		blend_.isBlending ? L"true" : L"false");
	
	DrawFormatString(400, 16, 0xff0000, L"アニメーション終了: %s",
		IsAnimEnd() ? L"true" : L"false");
}

// ☆blend想定をしていないので修正必要
void AnimationController::SetPose(const std::wstring& type, float step)
{
	// 現在アタッチされているアニメーションをすべて解除（残像描画用なので一度クリアする）
	for (auto& data : blend_.data)
	{
		if (data.fromAttachNo != -1) MV1DetachAnim(modelId_, data.fromAttachNo);
	}
	blend_.data.clear();
	if (blend_.toAttachNo != -1)
	{
		MV1DetachAnim(modelId_, blend_.toAttachNo);
		blend_.toAttachNo = -1;
	}
	if (playAnim_.attachNo != -1)
	{
		MV1DetachAnim(modelId_, playAnim_.attachNo);
	}

	blend_.isBlending = false;

	// データのセット
	playType_ = type;
	playAnimIdx_ = 0; // 複種アニメの先頭を基準にする
	playAnim_ = animations_[type][playAnimIdx_];

	// 指定されたステップを設定
	playAnim_.step = step;
	playAnim_.attachNo = GetAttrchNo(type); // アタッチ実行

	// 時間を適用
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);

	// ルートフレーム（腰など）の位置補正
	int rootFrame = MV1SearchFrame(modelId_, rootFrame_.c_str());
	playAnim_.firstPos = MV1GetAttachAnimFrameLocalPosition(modelId_, playAnim_.attachNo, rootFrame);

	// 移動量を打ち消す設定（モデルが移動しないように）
	ModelFrameUtility::SetFrameAnimAttachLocalMatrixPos(modelId_, playAnim_.attachNo, rootFrame, playAnim_.firstPos);
}

void AnimationController::PlayStep(const std::wstring& type, float step, bool isLoop, bool isStop, bool isForce)
{
	// 通常のPlayを呼び出す（isForce=trueにすることで強制切り替え）
	Play(type, isLoop, isStop, isForce);

	// Playの中で初期化されたstepを、指定された時間に上書き
	playAnim_.step = step;

	// DXLib側に即時反映
	if (playAnim_.attachNo != -1)
	{
		MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
	}
}

int AnimationController::GetAttrchNo(const std::wstring& animType)
{
	auto& playAnim = animations_[animType][playAnimIdx_];

	int animIdx = 0;
	if (MV1GetAnimNum(modelId_) > 2)
	{
		animIdx = animations_[animType][playAnimIdx_].attachNo;
	}
	else if (MV1GetAnimNum(modelId_) > 1)
	{
		animIdx = 1;
	}

	int attachNo = MV1AttachAnim(modelId_, animIdx, playAnim.model);
	if (attachNo == -1) {
		printfDx(L"MV1AttachAnim failed (modelId=%d animIdx=%d)\n", modelId_, animIdx);
		return -1;
	}
	return attachNo;
}

int AnimationController::LoadAnimModel(const std::wstring& path)
{
	auto it = animModelCache_.find(path);

	if (it != animModelCache_.end())
	{
		return it->second;
	}

	const int modelHandle = MV1LoadModel(path.c_str());

	if (modelHandle == -1)
	{
		printfDx(L"Failed to load animation model: %s\n",
			path.c_str());
		return -1;
	}

	animModelCache_.emplace(path, modelHandle);

	return modelHandle;
}

