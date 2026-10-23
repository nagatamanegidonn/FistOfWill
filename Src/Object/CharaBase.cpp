#include "../Manager/General/SceneManager.h"

#include "../Utility/Utility.h"
#include "../Utility/CSVDataLoader.h"

#include "Common/AnimationController.h"
#include "Common/SoundController.h"
#include "../Object/Attack/AttackData.h"

#include "CharaStatus.h"
#include "CharaBase.h"

CharaBase::CharaBase(void)
	:
	ActorBase(),
	prePos_(VECTOR()),
	speed_(0.0f),
	moveDir_(VECTOR()),
	movePow_(VECTOR()),
	isDash_(false),

	playerRotY_(Quaternion()),
	goalQuaRot_(Quaternion()),
	stepRotTime_(0.0f),
	// ジャンプ関係	
	jumpPow_(VECTOR()),
	isGround_(false),

	areaId_(-1),
	
	// 被ダメ判定
	isReaction_(false),
	isAbsoluteReaction_(false),

	isJust_(false)
{
	animationController_ = nullptr;
	//inputController_ = nullptr;
	soundController_.reset();
	soundController_ = nullptr;

	attackDataList_ = { };

	// ステータス
	charaStatus_ = std::make_shared<CharaStatus>();

	prePos_ = VECTOR();

}

std::shared_ptr<CharaStatus> CharaBase::GetCharaStatus() const
{
	std::shared_ptr<CharaStatus> status;
	if (charaStatus_)
	{
		status = charaStatus_;
	}
	return status;
}

// 回転の球面補間
void CharaBase::Rotate(void)
{
	// 回転の球面補間を行う。
	// TIME_ROT定数で指定された時間をかけて、ゆっくりゴールとなる
	// 回転に向かって近づくような回転を行う
	stepRotTime_ -= SceneManager::GetInstance().GetDeltaTime();
	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void CharaBase::UpdateTarget(void)
{
	// すでにロックオン中のターゲットが健在なら維持
	if (auto currentTarget = target_.lock())
	{
		// ターゲットのHPが0ならターゲットを解除
		if (auto charaTarget = std::dynamic_pointer_cast<CharaBase>(currentTarget))
		{
			if (charaTarget->GetCharaStatus()->GetNowHP() <= 0.0f)
			{
				ResetTarget();
				return;
			}
		}
		// 必要ならここで「離れすぎたら解除」などのロジックを入れる
		float distance = VSize(VSub(currentTarget->GetTransform().pos, transform_.pos));
		if (distance > charaStatus_->GetSearchRange())
		{
			ResetTarget();
			return;
		}
		// 一番近いターゲットとの変更判断
		if (auto closest = closestTarget_.lock())
		{
			float closestDistance = VSize(VSub(closest->GetTransform().pos, transform_.pos));
			if (closestDistance < distance)
			{
				target_ = closestTarget_;
			}
		}

		return;
	}

	// マネージャーが調べてくれた「一番近いプレイヤー」をチェック
	if (auto closest = closestTarget_.lock())
	{
		// 自身の位置と一番近いプレイヤーの距離を計算
		VECTOR myPos = transform_.pos;
		VECTOR targetPos = closest->GetTransform().pos;
		float distance = VSize(VSub(targetPos, myPos));

		// （追跡を諦める1000.0fより小さくすることで、境界線でのブルブル震えるループを防ぎます）
		if (distance <= charaStatus_->GetSearchRange())
		{
			target_ = closest; // 条件をすべて満たしたらロックオン！

			// 敵の個性：自分自身の視野に入っているかだけを判断する！
			//if (IsTargetInFOV(targetPos, 120.0f))
			//{
			//	target_ = closest; // 条件をすべて満たしたらロックオン！
			//}
		}
	}
}

void CharaBase::InitAnimation(std::wstring path)
{
	// 既存の CSVDataLoader から文字列データのリストを取得
	auto csvDatas = CSVDataLoader::GetInstance().LoadCSV(path.c_str());
	for (const auto& csvData : csvDatas)
	{
		std::wstring type = csvData->GetData(0);
		if (type == L"Set")
		{
			std::wstring AnimId = csvData->GetData(1);		// アニメーションID
			std::wstring path = csvData->GetData(2);		// ファイルの場所
			float speed = std::stof(csvData->GetData(3));	// 再生速度
			int animNo = std::stoi(csvData->GetData(4));	// アニメーション番号
			float sTime = std::stof(csvData->GetData(5));	// 開始時間
			float eTime = std::stof(csvData->GetData(6));	// 終了時間
			// 基礎アニメーション
			animationController_->Set(AnimId, path, speed, animNo, sTime, eTime);
		}
		else if (type == L"Add")
		{
			std::wstring AnimId = csvData->GetData(1);		// アニメーションID
			std::wstring path = csvData->GetData(2);		// ファイルの場所
			float speed = std::stof(csvData->GetData(3));	// 再生速度
			int animNo = std::stoi(csvData->GetData(4));	// アニメーション番号
			float sTime = std::stof(csvData->GetData(5));	// 開始時間
			float eTime = std::stof(csvData->GetData(6));	// 終了時間
			// 追加アニメーション
			animationController_->AddAnim(AnimId, path, speed, animNo, sTime, eTime);
		}
		else if (type == L"Blend")
		{
			std::wstring AnimId = csvData->GetData(1);		// アニメーションID
			std::wstring check = csvData->GetData(2);		// bool値（true/false）でブレンドするかどうか
			float speed = std::stof(csvData->GetData(3));	// ブレンド速度
			int animNo = std::stoi(csvData->GetData(4));	// アニメーション番号
			if (check == L"TRUE/TRUE")
			{
				animationController_->SetIsBlend(AnimId, true, speed, true, animNo);
			}
			else if (check == L"TRUE/FALSE")
			{
				animationController_->SetIsBlend(AnimId, true, speed, false, animNo);
			}
			else if (check == L"FALSE/TRUE")
			{
				animationController_->SetIsBlend(AnimId, false, speed, true, animNo);
			}
			else if (check == L"FALSE/FALSE")
			{
				animationController_->SetIsBlend(AnimId, false, speed, false, animNo);
			}
		}
	}
}

void CharaBase::CountTime(float& time)
{
	if (time > 0.0f)
	{
		time -= SceneManager::GetInstance().GetDeltaTime();
	}
}

void CharaBase::UpdateRotate(void)
{
	// 重力方向に沿って回転させる
	//transform_.quaRot = grvMng_.GetTransform().quaRot;
	transform_.quaRot = Quaternion();//grvMng_がないので代わりに
	transform_.quaRot = transform_.quaRot.Mult(playerRotY_);
}


void CharaBase::LoadCsvAttackData(std::wstring path)
{
	// 既存の CSVDataLoader から文字列データのリストを取得
	//auto csvDatas = CSVDataLoader::GetInstance().LoadCSV(L"Data/CSV/PlayerAttackData.csv");
	auto csvDatas = CSVDataLoader::GetInstance().LoadCSV(path.c_str());

	// 今回目的としている shared_ptr<AttackData> の配列を用意
	std::vector<std::shared_ptr<AttackData>> attackDataList;

	for (const auto& csvData : csvDatas)
	{
		// 1行分の AttackData をヒープに生成
		auto data = std::make_shared<AttackData>();

		// 0番目は Index なので、1番目（AnimId）から順番に変換して代入
		data->animationId = (csvData->GetData(1));				// アニメーションID
		data->attackPower = std::stoi(csvData->GetData(2));		// 攻撃力 (int)
		data->m_attackWeight = static_cast<CharaStatus::Armor>(std::stoi(csvData->GetData(3)));// 攻撃の重さ
		data->startTime = std::stof(csvData->GetData(4));		// 発生時間 (float)
		data->cancelableTime = std::stof(csvData->GetData(5));	// 受付時間 (float)
		data->frameName = csvData->GetData(6);					// ボーン名 (std::wstring)
		data->colliderRadius = std::stof(csvData->GetData(7));	// 半径 (float)
		data->forwardSpeed = std::stof(csvData->GetData(8));	// 前進速度 (float)
		data->nextAttackId = std::stof(csvData->GetData(9));	// 攻撃ID (int)
		std::wstring typeStr = csvData->GetData(10);
		if (typeStr == L"Melee")
		{
			data->attackType = AttackType::Melee;
		}
		else if (typeStr == L"Bullet")
		{
			data->attackType = AttackType::Bullet;
		}
		else if (typeStr == L"AOE")
		{
			data->attackType = AttackType::AOE;
		}
		else
		{
			data->attackType = AttackType::Melee; // どれにも当てはまらない場合のデフォルト
		}
		std::wstring tagStr = csvData->GetData(11);
		if (tagStr == L"HOSTILITY") {
			data->colliderTag = Collider::TAG::HOSTILITY;	// 敵対
		}
		else {
			data->colliderTag = Collider::TAG::NEUTRAL;		// 中立
		}
		// 配列に追加
		attackDataList.emplace_back(data);
	}

	// マップに登録（L"PlayerAttack" などのキーで検索できるように保持）
	attackDataList_ = std::move(attackDataList);
}
