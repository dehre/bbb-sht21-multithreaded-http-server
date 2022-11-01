#pragma once

#include <Poco/ExpireCache.h>
#include <Poco/Mutex.h>
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
    Mutex m_mutex{};
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
    bool cached{true};
    double temperature{};
    double humidity{};
    {
        ScopedLock lock(m_mutex);
        if (!m_cache.has("temperature") || !m_cache.has("humidity"))
        {
            cached = false;
            m_cache.add("temperature", 12.34);
            m_cache.add("humidity", 12.34);
        }
        temperature = *(m_cache.get("temperature"));
        humidity = *(m_cache.get("humidity"));
    }
    return {.temperature = temperature, .humidity = humidity, .cached = cached};
}
