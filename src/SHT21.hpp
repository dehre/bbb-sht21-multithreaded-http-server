#pragma once

#include <Poco/ExpireCache.h>
#include <Poco/Mutex.h>
#include <filesystem>

class SHT21
{
  public:
    /* Public Data Types */
    struct Data;

    /* Public Methods */
    static SHT21 &instance(Poco::Int64 expire_cache_timeout_ms);
    Data get();

  private:
    /* Private Constructors */
    SHT21(Poco::Int64 expire_cache_timeout_ms);
    SHT21(SHT21 const &) = delete;
    void operator=(SHT21 const &) = delete;

    /* Private Methods */
    std::optional<std::filesystem::path> search_file_in_dir(const std::filesystem::path &dir,
                                                            const std::filesystem::path &filename);
    double read_sysfs_value(const std::filesystem::path &absolute_path);
    double read_temperature();
    double read_humidity();

    /* Private Members */
    Poco::ExpireCache<std::string_view, double> m_cache{};
    Poco::Mutex m_mutex{};
};

struct SHT21::Data
{
    double temperature;
    double humidity;
    bool cached;
};
