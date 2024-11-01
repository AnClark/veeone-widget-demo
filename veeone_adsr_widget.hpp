#pragma once

#include <imgui.h>

#include <cstdint>

namespace VeeOneWidgets
{

void ADSREnvelope(const char *id, float *attack, float *decay, float *sustain, float *release, uint32_t width, uint32_t height);

}
