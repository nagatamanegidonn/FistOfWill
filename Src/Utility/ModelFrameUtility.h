#pragma once
#include <string>
#include <vector>
#include <list>
#include <DxLib.h>

class Quaternion;
class Transform; // Transformクラスの前方宣言

class ModelFrameUtility
{
public:

    struct BlendData {
        int modelId;     // モデルID
        int attachNo;    // ブレンド用attachNo
        float weight;    // blendRate
        VECTOR firstPos; // ブレンド開始時の位置
    };

    static void SetFrameLocalMatrixPos(int modelId, int frameIdx, VECTOR& pos);
    static void GetFrameLocalMatrix(int modelId, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos);

    static void SetFrameAnimAttachLocalMatrixPos(int modelId, int attachNo, int frameIdx, const VECTOR& pos);
    static void GetFrameAnimAttachLocalMatrix(int modelId, int attachNo, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos);

    static void SetFrameAnimAttachLocalMatrixPos(const std::list<BlendData>& list, int rootFrame, int toAttachNo, const VECTOR& toFirstPos);

    // ボーンフレームの回転を設定
    static void SetFreamRotation(int _modelId, int _rootFrame, Quaternion _rot);

	// ボーンから指定された距離だけ離れた位置を取得する
    static VECTOR GetFreamPosFromDistance(Transform modelTrans, std::wstring boneName, VECTOR distance);

    // SyncWeaponToFreamの宣言（引数型を一致させる）
    static void SyncWeaponToFream(const TCHAR* frameName, const VECTOR& offsetRot, const VECTOR& offsetPos,
        const Transform& modelTransform, Transform& outWeaponTransform);
};

