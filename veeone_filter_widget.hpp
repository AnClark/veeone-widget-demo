#pragma once

#include <cstdint>

namespace VeeOneWidgets
{

/**
 * Enum: DCF Type
 */
enum class DcfType
{
    t_LPF = 0,
    t_BPF,
    t_HPF,
    t_BRF
};

/*
 * Enum: Filter slope
 */
enum class FilterSlope
{
    s_12dbPerOct = 0, // 12dB/oct
    s_24dBPerOct,     // 24dB/oct
    s_biquad,
    s_formant
};

void DCFCurve(const char *id, float *cutoff, float *resonance, int *type, int *slope, uint32_t width, uint32_t height);

} // namespace VeeOneWidgets
