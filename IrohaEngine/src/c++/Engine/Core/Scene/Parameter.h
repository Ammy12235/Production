#pragma once

#include "Core/Base.h"

//パラメータークラス：スタック構造においてあるシーンのデータを次のシーンに渡す。データを取り出す場合各シーン固有のキーを使用する。
class Parameter:public CELEMENT
{
public:
    const static int Error = -1;

    Parameter() = default;
    virtual ~Parameter() = default;

    void set(std::string key, int val);
    int  get(std::string key) const;

private:
    std::map<std::string, int> _map;
};
