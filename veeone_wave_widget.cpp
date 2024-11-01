#include "veeone_wave_widget.hpp"
#include "imgui.h"

#ifdef PADTHV1_STUFF
#include <vector>
#endif

// 定义边框颜色（白色）
static const ImU32 white = IM_COL32(255, 255, 255, 255);

// Safe value capping.
constexpr inline float safe_value(float x)
{
    return (x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x));
}

// Exported from Qt qnumeric.h
template <typename T> constexpr inline T qAbs(const T &t)
{
    return t >= 0 ? t : -t;
}

void VeeOneWidgets::OscWave(const char *id, synthv1_wave_lf *waveData, int *shape, float *waveWidth, uint32_t width,
                            uint32_t height)
{
    // 在你的渲染循环中，你需要获取当前的ImDrawList
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // 定义矩形的位置。
    // 矩形的大小由函数参数 width 与 height 确定。
    ImVec2 pos = ImGui::GetCursorScreenPos(); // 获取当前控件插入点

    // 绘制矩形边框
    draw_list->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height), white);

    // 用这个来占位
    ImGui::InvisibleButton(id, ImVec2(width, height));

    // 设置绘图区域
    draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height), true);

    // 常用的控件尺寸
    const int half_height = (height >> 1); // 控件高度的一半，用于计算y轴的中心位置
    const int half_width = (width >> 1);   // 控件宽度的一半，用于绘制中心垂直线

    // 设置波形类型
    if (*shape != int(waveData->shape()))
    {
        if (*shape < int(synthv1_wave::Pulse))
            *shape = int(synthv1_wave::Noise);
        else if (*shape > int(synthv1_wave::Noise))
            *shape = int(synthv1_wave::Pulse);
        waveData->reset(synthv1_wave::Shape(*shape), waveData->width());
    }

    // 设置波形宽度
    float wave_width_internal =
        *waveWidth * 0.01; // 将波形宽度转换为 [0.0, 1.0] 的取值范围，才能被 synthv1_wave 类所支持
    if (qAbs(wave_width_internal - waveData->width()) > 0.001f)
    {
        waveData->reset(waveData->shape(), safe_value(wave_width_internal));
    }

    ImVec2 line_begin(pos.x, half_height);

    // 遍历控件的宽度，计算每个点的y值，并添加到路径中
    for (int x = 1; x < width; ++x)
    {
        ImVec2 line_end(pos.x + x,
                        pos.y + half_height - int(waveData->value(float(x) / float(width)) * float(half_height - 2)));

        draw_list->AddLine(line_begin, line_end, white);

        line_begin = line_end;
    }

    draw_list->AddLine(line_begin, ImVec2(pos.x + width, half_width), white);

    draw_list->PopClipRect();
}

#ifdef PADTHV1_STUFF
void VeeOneWidgets::SampleWave(const char *id, padthv1_sample *sampleData, int *freq0, float *sampleWidth, float *scale,
                               uint16_t *nh, uint32_t width, uint32_t height)
{
    // 在你的渲染循环中，你需要获取当前的ImDrawList
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // 定义矩形的位置。
    // 矩形的大小由函数参数 width 与 height 确定。
    ImVec2 pos = ImGui::GetCursorScreenPos(); // 获取当前控件插入点

    // 绘制矩形边框
    draw_list->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height), white);

    // 用这个来占位
    ImGui::InvisibleButton(id, ImVec2(width, height));

    // 设置绘图区域
    draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height), true);

    // 常用的控件尺寸
    const int half_height = (height >> 1); // 控件高度的一半
    const int half_width = (width >> 1);   // 控件宽度的一半

    //
    // 确定多边形的点。
    //
    std::vector<ImVec2> m_pPolyg(width);

    const uint32_t nframes = sampleData->size();
    const uint32_t nperiod = nframes / half_width;
    const float phase_inc = 1.0f / float(nframes);
    float phase = 0.0f;
    float vmax = 0.0f;
    float vmin = 0.0f;
    int n = 0;
    int x = 1;
    uint32_t j = 0;
    for (uint32_t i = 0; i < nframes; ++i)
    {
        const float v = sampleData->value(phase);
        if (vmax < v || j == 0)
            vmax = v;
        if (vmin > v || j == 0)
            vmin = v;
        if (++j > nperiod)
        {
            m_pPolyg[n] = ImVec2(pos.x + (x), pos.y + (half_height - int(vmax * half_height)));
            m_pPolyg[width - n - 1] = ImVec2(pos.x + (x), pos.y + (half_height - int(vmin * half_height)));
            vmax = vmin = 0.0f;
            ++n;
            x += 2;
            j = 0;
        }
        phase += phase_inc;
    }
    while (n < half_width)
    {
        m_pPolyg[n] = ImVec2(pos.x + x, pos.y + half_height);
        m_pPolyg[width - n - 1] = ImVec2(pos.x + x, pos.y + half_height);
        ++n;
        x += 2;
    }
#if 0
    // Sample harmonics...
    m_nrects = m_pSample->nh();
    if (m_nrects > 0)
    {
        m_pRects = new QRect[m_nrects];
        const int h1 = h - 8;
        const float dx = float(w - 8) / float(m_nrects);
        for (n = 0; n < m_nrects; ++n)
        {
            const int x = int(dx * (0.5f + float(n)));
            const int y = h1 - int(m_pSample->harmonic(n) * float(h1));
            m_pRects[n].setRect(x, y, 8, 8);
        }
    }
#endif

    //
    // 临时开启抗锯齿功能，并用直线绘制波形图
    // 【注意】只有画采样波形突才需要使用抗锯齿；OSC与LFO的波形图不应使用抗锯齿。
    //
    const auto orig_draw_list_flags = draw_list->Flags;   // 先备份原有的绘制参数
    draw_list->Flags |= ImDrawListFlags_AntiAliasedLines; // 启用抗锯齿

    // 绘制波形图
    draw_list->AddPolyline(m_pPolyg.data(), width, white, ImDrawFlags_Closed, 1.0f);

    draw_list->Flags = orig_draw_list_flags; // 恢复绘制参数，以禁用抗锯齿

    //
    // 记得出栈！
    //
    draw_list->PopClipRect();

    ImGui::Text("sample size: %d", sampleData->size());
    ImGui::Text("Half polygon point: (%.0f, %.0f)", m_pPolyg[half_width].x, m_pPolyg[half_width].y);
}
#endif
