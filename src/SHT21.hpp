#pragma once

#include <Poco/ExpireCache.h>
#include <Poco/Mutex.h>
#include <cstdlib> /* for strtod */
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <stdexcept>
#include <string_view>

using namespace Poco;
namespace fs = std::filesystem;

// TODO LORIS: throw Poco Exceptions for consistency https://docs.pocoproject.org/current/Poco.Exception.html

// TODO LORIS: split up SHT21.cpp from header file

// TODO LORIS: env variables
constexpr const Int64 g_expire_cache_timeout_ms{1000 * 20}; /* 20 seconds */

class SHT21
{
    /* Public Data Types */
  public:
    struct Data;

    /* Public Methods */
  public:
    static SHT21 &instance(); /* singleton */
    Data get();

    /* Private Constructors */
  private:
    SHT21() : m_cache(g_expire_cache_timeout_ms){};
    SHT21(SHT21 const &) = delete;
    void operator=(SHT21 const &) = delete;

    /* Private Methods */
  private:
    std::optional<fs::path> search_file_in_dir(const fs::path &dir, const fs::path &filename);
    double read_sysfs_value(const fs::path &absolute_path);
    double read_temperature();
    double read_humidity();

    /* Private Members */
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
            m_cache.add("temperature", read_temperature());
            m_cache.add("humidity", read_humidity());
        }
        temperature = *(m_cache.get("temperature"));
        humidity = *(m_cache.get("humidity"));
    }
    return {.temperature = temperature, .humidity = humidity, .cached = cached};
}

std::optional<fs::path> SHT21::search_file_in_dir(const fs::path &dir, const fs::path &filename)
{
    for (const auto &dir_entry : fs::directory_iterator{dir})
    {
        for (const auto &nested_dir_entry : fs::directory_iterator{dir_entry})
        {
            if (nested_dir_entry.path().filename() == filename)
            {
                return nested_dir_entry.path();
            }
        }
    }
    return {};
}

double SHT21::read_sysfs_value(const fs::path &absolute_path)
{
    std::ifstream inf{absolute_path};
    char buf[20];
    inf >> std::setw(20) >> buf; // TODO LORIS: we may get "read error  (press RETURN)"
    char *end;
    // TODO LORIS: check for errors in strtod
    return std::strtod(buf, &end);
}

double SHT21::read_temperature()
{
    auto found{search_file_in_dir("/sys/class/hwmon", "temp1_input")};
    if (!found)
    {
        throw std::runtime_error("Could not find sysfs entry for \"temp1_input\"");
    }
    return read_sysfs_value(*found) / 1000;
}

double SHT21::read_humidity()
{
    auto found{search_file_in_dir("/sys/class/hwmon", "humidity1_input")};
    if (!found)
    {
        throw std::runtime_error("Could not find sysfs entry for \"humidity1_input\"");
    }
    return read_sysfs_value(*found) / 1000;
}
