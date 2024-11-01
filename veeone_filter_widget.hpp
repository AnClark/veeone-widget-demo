#pragma once

#include <imgui.h>

#include <cstdint>

class VeeOneWidgets
{
  public:
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

    static void DCFCurve(const char *id, float *cutoff, float *resonance, int *type, int *slope, uint32_t width,
                         uint32_t height);

  private:
    // 定义边框颜色（白色）
    static const ImU32 white = IM_COL32(255, 255, 255, 255);

    static void _drawFormantCurve(ImVec2 &pos, float *cutoff, float *resonance, int *type, int *slope, uint32_t width,
                                  uint32_t height);
};
