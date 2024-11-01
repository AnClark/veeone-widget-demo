#pragma once

#include <imgui.h>

#include "synthv1_wave.h"
#ifdef PADTHV1_STUFF
#include "padthv1_sample.h"
#endif

#include <cstdint>

namespace VeeOneWidgets
{

void OscWave(const char *id, synthv1_wave_lf *waveData, int *shape, float *waveWidth, uint32_t width, uint32_t height);

#ifdef PADTHV1_STUFF
void SampleWave(const char *id, padthv1_sample *sampleData, int *freq0, float *sampleWidth, float *scale,
                uint16_t *nh, uint32_t width, uint32_t height);
#endif

} // namespace VeeOneWidgets
