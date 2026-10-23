
#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <DxLib.h>
#include "../Lib/nlohmann/json.hpp"

// 長いのでnamespaceの省略
using json = nlohmann::json;

class Quaternion;
class Transform; // Transformクラスの前方宣言

class JsonUtility
{
public:

    // Jsonデータの読み込み
    // ファイルパスを指定して、読み込んだjsonオブジェクトを返す
    // 失敗した場合は空のjsonオブジェクトを返す
    static nlohmann::json LoadJson(const std::wstring& loadPath);
};

// std::wstring変換
static std::wstring UTF8ToWstring(const std::string& str) {
    if (str.empty()) return L"";

    // 必要なバッファサイズを取得 (CP_UTF8 を指定)
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    std::wstring wstrTo(size_needed, 0);
    // 実際の変換
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}