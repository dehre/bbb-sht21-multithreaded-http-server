#pragma once

class SHT21
{
  public:
    struct Data;
    Data get();

    /* singleton pattern */
    static SHT21 &instance();
    SHT21(SHT21 const &) = delete;
    void operator=(SHT21 const &) = delete;

  private:
    SHT21()
    {
    }
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
};

SHT21::Data SHT21::get()
{
    return {
        .temperature = 23.32,
        .humidity = 45.54,
    };
}
