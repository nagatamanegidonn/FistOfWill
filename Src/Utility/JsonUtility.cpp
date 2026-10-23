#include <iostream>

#include "JsonUtility.h"
#include "../Application.h"


nlohmann::json JsonUtility::LoadJson(const std::wstring& loadPath)
{
    // wstringからstringへの変換が必要な場合は適宜行ってください（std::ifstreamの仕様による）
    // C++17以降であれば、std::filesystem::path を使うとwstringのまま開けます
    std::ifstream ifs(loadPath);

    nlohmann::json jsonData;
    if (!ifs)
    {
        std::wcerr << L"Failed to open file: " << loadPath << std::endl;
        return jsonData; // 空のオブジェクトを返す
    }

    try {
        ifs >> jsonData;
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    }

    return jsonData;
}