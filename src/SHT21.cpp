#include "SHT21.hpp"
#include <cstdlib> /* for strtod */
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>

SHT21 &SHT21::instance(Int64 expire_cache_timeout_ms)
{
    static SHT21 singleton_instance(expire_cache_timeout_ms);
    return singleton_instance;
}

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

SHT21::SHT21(Int64 expire_cache_timeout_ms) : m_cache(expire_cache_timeout_ms)
{
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
    char buf[30];
    inf >> std::setw(30) >> buf;
    char *end;
    double result{std::strtod(buf, &end)};
    if (buf == end) /* see: https://en.cppreference.com/w/cpp/string/byte/strtof */
    {
        std::ostringstream errmsg{};
        errmsg << "Failed to convert \"" << buf << "\" into number when reading " << absolute_path;
        throw DataFormatException(errmsg.str());
    }
    return result;
}

double SHT21::read_temperature()
{
    auto found{search_file_in_dir("/sys/class/hwmon", "temp1_input")};
    if (!found)
    {
        throw FileNotFoundException("Could not find sysfs entry for \"temp1_input\"");
    }
    return read_sysfs_value(*found) / 1000;
}

double SHT21::read_humidity()
{
    auto found{search_file_in_dir("/sys/class/hwmon", "humidity1_input")};
    if (!found)
    {
        throw FileNotFoundException("Could not find sysfs entry for \"humidity1_input\"");
    }
    return read_sysfs_value(*found) / 1000;
}
