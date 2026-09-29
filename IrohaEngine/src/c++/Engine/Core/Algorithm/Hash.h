#pragma once

#include "Core/Base.h"
#define ENABLE_HASH_DEBUG (1) // 1: Hashのデバッグ機能を有効, 0: 無効


//ハッシュクラス：ハッシュ値を生成するクラス。
class Hash
{
public:
    Hash(const char* str);

    //! ハッシュ値を取得
    uint32_t GetDigest() const { return m_digest; }

    //! 文字列からハッシュ値を作成して取得
    static constexpr uint32_t GetDigest(const char* str, const size_t length);

private:
    uint32_t m_digest = 0; // ハッシュ値

#if ENABLE_HASH_DEBUG
public:
    //! (デバッグ用) 文字列を取得
    const char* GetDebugStr() const { return m_debug_str.c_str(); }

private:
    std::string m_debug_str = ""; // (デバッグ用) 文字列
#endif
};

