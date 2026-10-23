#pragma once
#include <vector>
#include <DirectXMath.h>
#include <DxLib.h>

// 1. 履歴を保存するための構造体
struct PosBuffer {
    VECTOR headPos = { 0,0,0 }; // 剣の先
    VECTOR tailPos = { 0,0,0 }; // 剣の根元
    bool isUsed = false;
};

// 2. Renderer側のシェーダー（SwordTrailVS.hlsl）の入力構造に合わせた頂点構造体
struct SwordTrailVertex {
    VECTOR pos;           // 座標 (12byte)
    unsigned char dif[4]; // 頂点カラー (4byte: GetColorU8の結果を格納)
    DirectX::XMFLOAT2 uv; // UV座標 (8byte)
};

class SwordTrailData {
private:
    std::vector<PosBuffer> posArray;        // 履歴
    std::vector<SwordTrailVertex> vertices; // 計算後の頂点配列
    int maxBufferSize;

public:
    SwordTrailData(int bufferSize);

    // 現在の剣の先端・根元の位置をセット
    void SetPos(const VECTOR& head, const VECTOR& tail);

    // 全ての履歴と頂点データを指定した座標（または初期値）でリセットする
    void Reset(const VECTOR& head = VGet(0, 0, 0), const VECTOR& tail = VGet(0, 0, 0));

    // 履歴を更新し、描画用の頂点データを作成
    void Update();

    // Rendererの描画関数に渡すためのゲッター
    const SwordTrailVertex* GetVertices() const { return vertices.data(); }
    size_t GetVertexCount() const { return vertices.size(); }
};