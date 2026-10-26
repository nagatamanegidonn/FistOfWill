#include "StageManager.h"

#include"../../Manager/General/ActorManager.h"
#include"../../Manager/Resource/ResourceManager.h"

#include"../Common/Collider/Collider.h"
#include"../Common/Collider/Geometry/Model.h"

#include"../../Object/ActorBase.h"
#include"../../Object/Stage/StageObject.h"

#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/JsonUtility.h"

#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

StageManager::StageManager(std::shared_ptr<ActorManager> _actorMng)
	:
	actorMng_(_actorMng)
{
}

StageManager::~StageManager()
{
}

void StageManager::Init()
{
	/*CreatePlane();
	CreateWall({ 500.0f,0.0f,500.0f });
	CreateWall({ 500.0f,0.0f,-500.0f }, 90.0f);
	CreateWall({ -500.0f,0.0f,-500.0f }, 180.0f);
	CreateWall({ -500.0f,0.0f,500.0f }, 270.0f);
	CreateWallStraight({ 0.0f,0.0f,2000.0f });
	CreateWallStraight({ 0.0f,0.0f,-2000.0f });
	CreateWallStraight({ -2000.0f,0.0f,0.0f });
	CreateSlope({ -0.0f,0.0f,1000.0f });*/
}

void StageManager::Update()
{
}

void StageManager::Draw()
{
}

void StageManager::End()
{
}

void StageManager::Restart()
{
}

void StageManager::LoadStageJson(const std::wstring& jsonText)
{
	// JSONデータのロード
	auto jsonData = JsonUtility::LoadJson(Application::PATH_JSON + jsonText);
	if (jsonData.is_null()) return;

	// 解析処理
	for (const auto& stageData : jsonData)
	{
		std::string type = stageData.value("type", "");

		// Transformパラメータの取得（デフォルト値を安全に指定）
		VECTOR pos, rot, scl;

		pos.x = stageData["pos"].value("x", 0.0f);
		pos.y = stageData["pos"].value("y", 0.0f);
		pos.z = stageData["pos"].value("z", 0.0f);

		rot.x = stageData["rot"].value("x", 0.0f);
		rot.y = stageData["rot"].value("y", 0.0f);
		rot.z = stageData["rot"].value("z", 0.0f);

		scl.x = stageData["scl"].value("x", 1.0f);
		scl.y = stageData["scl"].value("y", 1.0f);
		scl.z = stageData["scl"].value("z", 1.0f);

		// タイプに応じて生成処理を分岐
		if (type == "plane") {
			CreatePlane(pos, rot, scl);
		}
		else if (type == "wall") {
			CreateWall(pos, rot, scl);
		}
		else if (type == "wall_straight") {
			CreateWallStraight(pos, rot, scl);
		}
		else if (type == "slope") {
			CreateSlope(pos, rot, scl);
		}
	}
}

void StageManager::CreatePlane(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl)
{
	Transform trans;
	trans.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"Plane"));
	trans.pos = pos;
	trans.scl = scl;
	trans.quaRot = Quaternion::Euler(
		Utility::Deg2RadF(rot.x),
		Utility::Deg2RadF(rot.y),
		Utility::Deg2RadF(rot.z)
	);

	std::unique_ptr<Geometry> geo = std::make_unique<Model>(
		trans.pos, trans.pos, trans.quaRot, trans.modelId
	);

	trans.Update();

	VECTOR light = GetLightDirection();

	std::unique_ptr<ModelMaterial> material = std::make_unique<ModelMaterial>(
		L"StdModelVS.cso", 4, L"StdModelPS.cso", 2);
	material->AddConstBufPS({ 100.0f, 100.0f, 1.0f, 1.0f });
	material->AddConstBufPS({ light.x, light.y, light.z, 1.0f });
	material->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	material->SetTextureBuf(0, ResourceManager::GetInstance().Load(L"PlaneTex").handleId_);
	material->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);
	// VS用の定数バッファを追加
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });

	std::shared_ptr<StageObject> obj = std::make_shared<StageObject>(trans, std::move(material));
	obj->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });

	actorMng_.lock()->Entry(obj);
}

void StageManager::CreateWall(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl)
{
	Transform trans;
	trans.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"Wall"));
	trans.pos = pos;
	trans.scl = scl;
	trans.quaRot = Quaternion::Euler(
		Utility::Deg2RadF(rot.x),
		Utility::Deg2RadF(rot.y),
		Utility::Deg2RadF(rot.z)
	);

	std::unique_ptr<Geometry> geo = std::make_unique<Model>(
		trans.pos, trans.pos, trans.quaRot, trans.modelId
	);

	trans.Update();

	VECTOR light = GetLightDirection();

	std::unique_ptr<ModelMaterial> material = std::make_unique<ModelMaterial>(
		L"StdModelVS.cso", 4, L"StdModelPS.cso", 2);
	material->AddConstBufPS({ 2.0f, 2.0f, 1.0f, 1.0f });
	material->AddConstBufPS({ light.x, light.y, light.z, 1.0f });
	material->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	material->SetTextureBuf(0, ResourceManager::GetInstance().Load(L"WallTex").handleId_);
	material->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);
	// VS用の定数バッファを追加
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });

	std::shared_ptr<StageObject> obj = std::make_shared<StageObject>(trans, std::move(material));
	obj->MakeCollider(Collider::TAG::NORMAL_OBJECT, std::move(geo), { Collider::TAG::GROUND });

	actorMng_.lock()->Entry(obj);
}

void StageManager::CreateWallStraight(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl)
{
	Transform trans;
	trans.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"WallStraight"));
	trans.pos = pos;
	trans.scl = scl;
	trans.quaRot = Quaternion::Euler(
		Utility::Deg2RadF(rot.x),
		Utility::Deg2RadF(rot.y),
		Utility::Deg2RadF(rot.z)
	);

	std::unique_ptr<Geometry> geo = std::make_unique<Model>(
		trans.pos, trans.pos, trans.quaRot, trans.modelId
	);

	trans.Update();

	VECTOR light = GetLightDirection();

	std::unique_ptr<ModelMaterial> material = std::make_unique<ModelMaterial>(
		L"StdModelVS.cso", 4, L"StdModelPS.cso", 2);
	material->AddConstBufPS({ 2.0f, 2.0f, 1.0f, 1.0f });
	material->AddConstBufPS({ light.x, light.y, light.z, 1.0f });
	material->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	material->SetTextureBuf(0, ResourceManager::GetInstance().Load(L"WallTex").handleId_);
	material->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);
	// VS用の定数バッファを追加
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });

	std::shared_ptr<StageObject> obj = std::make_shared<StageObject>(trans, std::move(material));
	obj->MakeCollider(Collider::TAG::NORMAL_OBJECT, std::move(geo), { Collider::TAG::GROUND });

	actorMng_.lock()->Entry(obj);
}

void StageManager::CreateSlope(const VECTOR& pos, const VECTOR& rot, const VECTOR& scl)
{
	Transform trans;
	trans.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(L"Slope"));
	trans.pos = pos;
	trans.scl = scl;
	trans.quaRot = Quaternion::Euler(
		Utility::Deg2RadF(rot.x),
		Utility::Deg2RadF(rot.y),
		Utility::Deg2RadF(rot.z)
	);

	std::unique_ptr<Geometry> geo = std::make_unique<Model>(
		trans.pos, trans.pos, trans.quaRot, trans.modelId
	);

	trans.Update();

	VECTOR light = GetLightDirection();

	std::unique_ptr<ModelMaterial> material = std::make_unique<ModelMaterial>(
		L"StdModelVS.cso", 4, L"StdModelPS.cso", 2);
	material->AddConstBufPS({ 2.0f, 2.0f, 1.0f, 1.0f });
	material->AddConstBufPS({ light.x, light.y, light.z, 1.0f });
	material->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	material->SetTextureBuf(0, ResourceManager::GetInstance().Load(L"PlaneTex").handleId_);
	material->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);
	// VS用の定数バッファを追加
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });
	material->AddConstBufVS({ 1.0f,1.0f,1.0f,1.0f });

	std::shared_ptr<StageObject> obj = std::make_shared<StageObject>(trans, std::move(material));
	obj->MakeCollider(Collider::TAG::NORMAL_OBJECT, std::move(geo), { Collider::TAG::NORMAL_OBJECT });

	actorMng_.lock()->Entry(obj);
}

