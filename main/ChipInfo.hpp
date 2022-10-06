#pragma once

#include "esp_chip_info.h"

class ChipInfo
{
public:
    ChipInfo();

    void PrintInfo();
private:
    static constexpr char TAG[] = "ChipInfo";
    esp_chip_info_t chip_info;
};