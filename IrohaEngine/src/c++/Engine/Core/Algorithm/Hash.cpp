#include "hash.h"
#include "CRC32.h"

Hash::Hash(const char* str)
{
    m_digest = GetDigest(str, std::string(str).length());

#if ENABLE_HASH_DEBUG
    m_debug_str = str;
#endif
}

constexpr uint32_t Hash::GetDigest(const char* str, const size_t length)
{
    uint32_t digest = 0xffffffff;
    for (size_t i = 0; i < length; i++)
    {
        digest = (digest << 8) ^ s_crc32_table[((digest >> 24) ^ str[i]) & 0xff];
    }

    return digest;
}
