#pragma once

#include <imgui.h>

#include "synthv1_wave.h"

#include <cstdint>

namespace VeeOneWidgets
{

void Wave(const char *id, synthv1_wave_lf *waveData, uint32_t shape, float waveWidth, uint32_t width, uint32_t height);

} // namespace VeeOneWidgets
