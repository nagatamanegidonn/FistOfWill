
#include "../Manager/General/SceneManager.h"
#include "../../Object/ActorBase.h"
#include "../../Object/Player/PlayerBase.h"
#include "../../Object/Enemy/EnemyBase.h"
#include "../../Object/Enemy/DemoEnemy.h"
#include "../../Object/Enemy/BossEnemy.h"
#include "../../Object/Enemy/TutorialEnemy.h"
#include "../../Object/CharaStatus.h"
#include "EnemyManager.h"
#include "ActorManager.h"

#include "../../Application.h"
#include "../../Utility/JsonUtility.h"
#include "../../Utility/CSVDataLoader.h"
#include "../../Utility/ShadowMap.h"
#include "../Resource/SoundManager.h"


// 静的ポインタの実体を定義（最初はどこも指していない）
ActorManager* ActorManager::s_activeInstance = nullptr;

ActorManager::ActorManager() {}
ActorManager::~ActorManager() {}

void ActorManager::Entry(std::shared_ptr<ActorBase> _actor)
{
	// すでに登録されているならしない
	auto it = std::find(actors_.begin(), actors_.end(), _actor);
	if (it != actors_.end())return;

	// アクターの初期化
	_actor->Init();

	// 直接追加せず、予約配列にエントリする
	addActors_.emplace_back(_actor);

	// 敵なら
	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(_actor);
	if (!enemy.expired())
	{
		enemyManager_->Entry(std::dynamic_pointer_cast<EnemyBase>(_actor));
		return;
	}
	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(_actor);
	if (!player.expired())
	{
		players_.push_back(player);
		return;
	}

}

void ActorManager::Exit(std::shared_ptr<ActorBase> _actor)
{
	// 登録されていないならしない
	auto it = std::find(actors_.begin(), actors_.end(), _actor);
	if (it == actors_.end())return;
	actors_.erase(it);
}

void ActorManager::Init()
{
	// 初期化されたら「自分が今使われるマネージャーである」と登録する
	s_activeInstance = this;

	// ゲームオーバーフラグの初期化
	isGameover_ = false;

	// 敵マネージャーの作成
	enemyManager_ = std::make_shared<EnemyManager>(shared_from_this());
	enemyManager_->Init();
}

void ActorManager::Start()
{
	// ループ中に追加されたアクターを、メイン配列に結合する
	if (!addActors_.empty())
	{
		// メイン配列の末尾に、予約配列の中身をすべて結合
		actors_.insert(actors_.end(), addActors_.begin(), addActors_.end());
		addActors_.clear(); // 予約配列を空にする
	}
}

void ActorManager::Update()
{
	// 敵マネージャーの更新 (動くかどうかなどの管理を行う）
	enemyManager_->Update();

	// プレイヤーの敵ターゲット
	PlayerEnemyTarget();

	// ループ中に追加されたアクターを、メイン配列に結合する
	if (!addActors_.empty())
	{
		// メイン配列の末尾に、予約配列の中身をすべて結合
		actors_.insert(actors_.end(), addActors_.begin(), addActors_.end());
		addActors_.clear(); // 予約配列を空にする
	}

	// アクターの更新
	for (auto& actor : actors_)
	{
		actor->Update();
	}
	CheckAlive();

}

void ActorManager::Draw()
{
	// 影描画
	SceneManager::GetInstance().GetShadowMap()->DrawStartSetUp();

	// 影の形をとるためにActorのMV1描画を行う
	for (auto& actor : actors_)
	{
		actor->DrawShadow();
	}

	// 中間
	SceneManager::GetInstance().GetShadowMap()->DrawSetUp();


	// アクターの描画
	for (auto& actor : actors_)
	{
		actor->Draw();
	}

	// シャドウマップの描画終了
	SceneManager::GetInstance().GetShadowMap()->DrawEndSetUp();

#if _DEBUG
	//m_pEnemyManager->DebugDraw();
#endif
}

void ActorManager::End()
{
	// ★ 追加：終了したらポインタをクリアする
	if (s_activeInstance == this)
	{
		s_activeInstance = nullptr;
	}

	// アクターの終了処理
	std::vector<std::shared_ptr<ActorBase>> deleteActer;
	for (auto& actor : actors_)
	{
		deleteActer.emplace_back(actor);
	}
	for (auto& actor : deleteActer)
	{
		Exit(actor);
	}
	deleteActer.clear();

	enemyManager_.reset();
}

void ActorManager::Restart()
{
	//アクターの初期化
	for (auto& actor : actors_)
	{
		actor->Init();
	}

	//エネミーマネージャーの初期化
	enemyManager_->Init();

	//フラグリセット
	/*m_isGameover = false;
	m_isBossBattle = false;*/
}

void ActorManager::CheckAlive()
{
	// 配列（actors_）から、条件（IsDeleteがtrue）に一致する要素を削除する
	std::erase_if(actors_, [this](const std::shared_ptr<ActorBase>& actor)
		{
			// 1. そもそもアクターが有効か、消滅フラグが立っているかチェック
			if (actor == nullptr || actor->IsDelete())
			{
				// 2. もし削除されるアクターが「敵」だった場合、EnemyManagerからも削除する
				auto enemy = std::dynamic_pointer_cast<EnemyBase>(actor);
				if (enemy != nullptr)
				{
					enemyManager_->Exit(enemy);
				}

				// 3. 【オプション】プレイヤーが死亡した時のゲームオーバー処理などを入れる場合
				auto player = std::dynamic_pointer_cast<PlayerBase>(actor);
				if (player != nullptr) {
					// ゲームオーバー判定
					isGameover_ = true;
					return false; // 配列からはまだ消さない場合はfalse
				}

				// 4. アクター自身の終了処理（解放など）を呼ぶ場合
				// actor->End();

				return true; // 条件一致：actors_ から削除する
			}

			return false; // 生存：削除しない
		});

}

std::vector<std::shared_ptr<PlayerBase>> ActorManager::GetPlayers(void)
{
	return players_.empty() ? std::vector<std::shared_ptr<PlayerBase>>() : std::vector<std::shared_ptr<PlayerBase>>(players_.begin(), players_.end());

	std::vector<std::shared_ptr<PlayerBase>> playerList;
	for (const auto& actor : actors_)
	{
		auto player = std::dynamic_pointer_cast<PlayerBase>(actor);
		// 生存しているプレイヤーのみ
		if (player && !player->IsDelete())
		{
			playerList.emplace_back(player);
		}
	}

	return playerList;
}

bool ActorManager::IsAllDeadEnemys()
{
	if (!enemyManager_)return true;


	auto val = enemyManager_->IsAllDeadEnemys();

	// 全滅したらクリアBGMを再生する
	if (val)
	{
		SoundManager::GetInstance().Stop(L"BATTLE_BGM");
	}

	return val;
}

void ActorManager::CreateActorJson(const std::wstring jsonText)
{
	// Utilityを使ってJSONを取得
	auto jsonData = JsonUtility::LoadJson(Application::PATH_JSON + jsonText);

	if (jsonData.is_null()) return; // 読み込み失敗時は抜ける

	// このステージ固有の解析処理（JSONが配列の場合）
	for (const auto& actorData : jsonData)
	{
		// 敵の種類（type）を取得（取得できない場合は "enemy1" を取得させる）
		std::string type = actorData.value("type", "enemy1");

		std::shared_ptr<ActorBase> actor = nullptr;

		// ここを変更すれば他のクラスも生成可能
		// 最初の質問にあった「Factory」の簡易版です。種類に応じてクラスを切り替えます。
		if (type == "enemy1") {
			actor = std::make_shared<DemoEnemy>(-1);
		}
		else if (type == "enemy2") {
			// もし別の敵クラス（例: BossEnemyなど）があればここで切り替え
			actor = std::make_shared<BossEnemy>(-1);
		}
		else if (type == "tutorial") {
			// もし別の敵クラス（例: BossEnemyなど）があればここで切り替え
			actor = std::make_shared<TutorialEnemy>(-1);
		}

		// 生成に成功したら、Jsonからパラメータを読み込んで設定する
		if (actor != nullptr)
		{
			InitParameter param = InitParameter();

			// JSONからXYZの数値を安全に取得（万が一キーがなくても defaultの値 0.0f や 1.0f が入るように value() を使うと安全です）
			param.scl.x = actorData["scl"].value("x", 1.0f);
			param.scl.y = actorData["scl"].value("y", 1.0f);
			param.scl.z = actorData["scl"].value("z", 1.0f);

			param.rot.x = actorData["rot"].value("x", 0.0f);
			param.rot.y = actorData["rot"].value("y", 0.0f);
			param.rot.z = actorData["rot"].value("z", 0.0f);

			param.lrot.x = actorData["lrot"].value("x", 0.0f);
			param.lrot.y = actorData["lrot"].value("y", 0.0f);
			param.lrot.z = actorData["lrot"].value("z", 0.0f);

			param.pos.x = actorData["pos"].value("x", 0.0f);
			param.pos.y = actorData["pos"].value("y", 0.0f);
			param.pos.z = actorData["pos"].value("z", 0.0f);

			// パラメータを反映させる（ここで transform_ に値が入る）
			actor->InitParam(param);

			// マネージャーへの登録
			Entry(actor);
		}
	}
}

void ActorManager::PlayerEnemyTarget(void)
{
	const auto& enemys = enemyManager_->GetEnemys();
	auto players = GetPlayers();

	std::shared_ptr<EnemyBase> closestEnemy = nullptr;
	float minDistance = FLT_MAX;

	// 一番距離の小さい敵を選定する
	for (const auto& player : players)
	{
		for (const auto& enemy : enemys)
		{
			if (enemy.lock()->GetCharaStatus()->GetNowHP() <= 0) continue;

			float dist = VSize(VSub(enemy.lock()->GetTransform().pos, player->GetTransform().pos));
			if (dist < minDistance)
			{
				minDistance = dist;
				closestEnemy = enemy.lock();
			}
		}

		// プレイヤーに「お前に一番近いのはこの敵だよ」と教えてあげる
		player->SetClosestTarget(closestEnemy);
	}
}

