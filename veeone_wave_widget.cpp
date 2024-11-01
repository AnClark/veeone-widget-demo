#include "veeone_wave_widget.hpp"
#include "imgui.h"

// 定义边框颜色（白色）
static const ImU32 white = IM_COL32(255, 255, 255, 255);

// Safe value capping.
constexpr inline float safe_value(float x)
{
    return (x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x));
}

void VeeOneWidgets::Wave(const char *id, synthv1_wave_lf *waveData, uint32_t shape, float waveWidth, uint32_t width,
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
    draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height));

    // 常用的控件尺寸
    const int half_height = (height >> 1); // 控件高度的一半，用于计算y轴的中心位置
    const int half_width = (width >> 1);   // 控件宽度的一半，用于绘制中心垂直线

    // 设置波形及其宽度
    if (shape != int(waveData->shape()))
    {
        if (shape < int(synthv1_wave::Pulse))
            shape = int(synthv1_wave::Noise);
        else if (shape > int(synthv1_wave::Noise))
            shape = int(synthv1_wave::Pulse);
        waveData->reset(synthv1_wave::Shape(shape), waveData->width());
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
