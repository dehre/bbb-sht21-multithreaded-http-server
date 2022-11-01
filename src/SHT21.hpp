#pragma once

#include <Poco/ExpireCache.h>
#include <string_view>

using namespace Poco;

// TODO LORIS: env variables
constexpr const Int64 g_expire_cache_timeout_ms{1000 * 20}; /* 20 seconds */

class SHT21
{
  public:
    struct Data;

  public:
    static SHT21 &instance(); /* singleton */
    Data get();

  private:
    SHT21() : m_cache(g_expire_cache_timeout_ms){};
    SHT21(SHT21 const &) = delete;
    void operator=(SHT21 const &) = delete;

  private:
    ExpireCache<std::string_view, double> m_cache{};
};

SHT21 &SHT21::instance()
{
    static SHT21 singleton_instance;
    return singleton_instance;
}

struct SHT21::Data
{
    double temperature;
    double humidity;
    bool cached;
};

SHT21::Data SHT21::get()
{
    // TODO LORIS: mutex here, or in constructor argument?

    bool cached{true};
    if (!m_cache.has("temperature"))
    {
        cached = false;
        m_cache.add("temperature", 12.34);
        m_cache.add("humidity", 12.34);
    }

    const auto temp{m_cache.get("temperature")};
    const auto humid{m_cache.get("humidity")};
    return {.temperature = *temp, .humidity = *humid, .cached = cached};
}
