#include "veeone_filter_widget.hpp"
#include "imgui.h"

#include <cstdio>

/**
   DCFCurve - Vee-One 滤波器曲线控件。

   特别声明：滤波器曲线常常包含直线段和曲线段。
   函数中带有“curve”的变量，【其中的“curve”指的是曲线段，而非整条曲线。】
 */
void VeeOneWidgets::DCFCurve(const char *id, float *cutoff, float *resonance, int *type, int *slope, uint32_t width,
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

    //
    // Formant 有自己独特的曲线画法，不适用 DCF Type 参数（synthv1 会将该参数禁用）。
    //
    if (FilterSlope(*slope) == FilterSlope::s_formant)
    {
        _drawFormantCurve(pos, cutoff, resonance, type, slope, width, height);
        return;
    }

    switch (DcfType(*type))
    {
    case DcfType::t_LPF: { // LPF 模式的曲线（低通滤波器）
        // 低通滤波器的曲线有两段：第一是直线，第二是向下的曲线。
        // 当 cutoff = 100 时，直线、曲线的宽度之和为控件之和。
        // 当 slope 为 12dB/oct 时，曲线段占比为控件宽度的四分之一。
        // 直线段高度恒为控件高度的二分之一。

        // 先画直线部分
        const float half_height_y = pos.y + (float)height / 2;
        const float width_1div4 = (float)width / 4;
        const float width_1div8 = (float)width / 8;
        const float width_1div16 = (float)width / 16;
        const float line_length = ((float)width - width_1div4) * (*cutoff * 0.01);

        ImVec2 line_begin(pos.x, half_height_y);             // 直线的起点
        ImVec2 line_end(pos.x + line_length, half_height_y); // 直线的终点
        draw_list->AddLine(line_begin, line_end, white);

        // 再画曲线部分
        const float min_curve_height_y = (float)half_height_y; // 这个y值，对应的是控件的一半高度
        const float max_curve_height_y = (float)(pos.y - height + 20); // 这个y值，对应的是控件顶部，略往下一点点
        const float resonance_delta_y = (max_curve_height_y - min_curve_height_y) *
                                        (*resonance * 0.01); // 根据当前 resonance 值，曲线最高点需要升高的纵轴高度

        //
        // 计算出贝塞尔曲线4个控制点的坐标，并绘制曲线
        //

        // 注：适当移动p2、p3的横坐标，使曲线看起来更自然，更接近原版合成器。以控件宽度200为基准，推算出不同宽度下的偏移量。
        int p2_x_offset = 10 * width / 200; // 当控件宽度为200时，p2 的 x 坐标偏移量取10。以此来推算
        int p3_x_offset = p2_x_offset * 0.5; // 取 p2_offset 的一半

        // 横坐标以 line_end.x 为基准，向右偏移的量均为控件宽度的2^n分之一，旨在让曲线形状尽可能接近原版
        ImVec2 p1(line_end); // 曲线起点
        ImVec2 p2(line_end.x + width_1div4 / 4 + p2_x_offset,
                  half_height_y); // 曲线爬升期间的拐点，旨在使曲线转折不突兀
        ImVec2 p3(line_end.x + width_1div4 / 2 - p3_x_offset, min_curve_height_y + resonance_delta_y); // 曲线最高点
        ImVec2 p4(line_end.x +
                      ((FilterSlope(*slope) == FilterSlope::s_24dBPerOct) ? (width_1div8 + width_1div16) : width_1div4),
                  pos.y + height); // 曲线结束点

        draw_list->AddBezierCubic(p1, p2, p3, p4, white, 0);
    }
    break;
    case DcfType::t_HPF: { // HPF 模式的曲线（高通滤波器）
        // 高通滤波器的曲线与低通滤波器大同小异，区别在于曲线在前、直线在后。你可以把二者的曲线看成互为水平镜像。

        // 先画直线部分
        const float half_height_y = pos.y + (float)height / 2;
        const float width_1div4 = (float)width / 4;
        const float width_1div8 = (float)width / 8;
        const float width_1div16 = (float)width / 16;
        const float line_length = ((float)width - width_1div4) * (*cutoff * 0.01);

        ImVec2 line_begin(pos.x + width, half_height_y);             // 直线的起点
        ImVec2 line_end(pos.x + width - line_length, half_height_y); // 直线的终点
        draw_list->AddLine(line_begin, line_end, white);

        // 再画曲线部分
        const float min_curve_height_y = (float)half_height_y; // 这个y值，对应的是控件的一半高度
        const float max_curve_height_y = (float)(pos.y - height + 20); // 这个y值，对应的是控件顶部，略往下一点点
        const float resonance_delta_y = (max_curve_height_y - min_curve_height_y) *
                                        (*resonance * 0.01); // 根据当前 resonance 值，曲线最高点需要升高的纵轴高度

        // 计算出贝塞尔曲线4个控制点的坐标，并绘制曲线
        // 注：适当移动p2、p3的横坐标，使曲线看起来更自然，更接近原版合成器。以控件宽度200为基准，推算出不同宽度下的偏移量。
        int p2_x_offset = 10 * width / 200; // 当控件宽度为200时，p2 的 x 坐标偏移量取10。以此来推算
        int p3_x_offset = p2_x_offset * 0.5; // 取 p2_offset 的一半

        // 横坐标以 line_end.x 为基准，向右偏移的量均为控件宽度的2^n分之一，旨在让曲线形状尽可能接近原版
        ImVec2 p1(line_end); // 曲线起点
        ImVec2 p2(line_end.x - width_1div4 / 4 - p2_x_offset,
                  half_height_y); // 曲线爬升期间的拐点，旨在使曲线转折不突兀
        ImVec2 p3(line_end.x - width_1div4 / 2 + p3_x_offset, min_curve_height_y + resonance_delta_y); // 曲线最高点
        ImVec2 p4(line_end.x -
                      ((FilterSlope(*slope) == FilterSlope::s_24dBPerOct) ? (width_1div8 + width_1div16) : width_1div4),
                  pos.y + height); // 曲线结束点

        draw_list->AddBezierCubic(p1, p2, p3, p4, white, 0);
    }
    break;
    case DcfType::t_BPF: { // BPF 模式的曲线（带通滤波器）

        // 尝试画曲线（BPF 模式）
        // Resonance 参数：决定曲线的高度，最低为控件的一半高度，最高为控件高度稍微低一点点
        const float min_curve_height_y = (float)(pos.y); // 这个y值，似乎对应的是控件的一半高度
        const float max_curve_height_y = (float)(pos.y - height + 10); // 这个y值，对应的是控件顶部，略往下一点点
        const float resonance_delta_y = (max_curve_height_y - min_curve_height_y) *
                                        (*resonance * 0.01); // 根据当前 resonance 值，曲线中间点需要升高的纵轴高度

        // Cutoff 参数：决定曲线横坐标的起始位置。调节该参数，相当于水平方向平移曲线。
        float curve_width = 0.0f; // BPF 曲线的宽度（横坐标跨度）
        float x_beyond_boundary_offset = 0.0f; // 当 cutoff 值位于最小或最大时，曲线左侧／右侧控制点超出边框的长度
                                               // 用于模拟原版 synthv1 的曲线
        // 根据不同的 slope 类型，选择相应的参数值
        switch (FilterSlope(*slope))
        {
        case FilterSlope::s_12dbPerOct: // 12dB/oct
        case FilterSlope::s_biquad:     // biquad
            curve_width = (float)width / 2;
            x_beyond_boundary_offset = 20;
            break;

        case FilterSlope::s_24dBPerOct: // 24db/oct
            curve_width = (float)width / 3;
            x_beyond_boundary_offset = 15;
            break;

        default: // Formant, 有自己独立的曲线画法。
            break;
        }

        const float min_curve_x = (float)pos.x - x_beyond_boundary_offset; // BPF 曲线位于最左侧时的横坐标
        const float max_curve_x =
            (float)pos.x + width + x_beyond_boundary_offset - curve_width; // BPF 曲线位于最右侧时的横坐标
        const float curve_left_begin_x =
            (pos.x - x_beyond_boundary_offset) +
            (max_curve_x - min_curve_x) * (*cutoff * 0.01); // 根据当前 cutoff 值，计算出曲线左侧控制点的坐标

        // 设置绘图区域。（参考自 imgui_demo.cpp）
        draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height));

        // 计算出贝塞尔曲线3个控制点的坐标，并绘制曲线
        const auto p1 = ImVec2(curve_left_begin_x, pos.y + height);
        const auto p2 = ImVec2(curve_left_begin_x + curve_width / 2, min_curve_height_y + resonance_delta_y);
        const auto p3 = ImVec2(curve_left_begin_x + curve_width, pos.y + height);
        draw_list->AddBezierQuadratic(p1, p2, p3, white, 0);

        draw_list->PopClipRect();
    }
    break;
    case DcfType::t_BRF: { // BRF 模式的曲线（带阻滤波器）
        // 带阻滤波器的曲线有两段，相当于高通、低通滤波器曲线在“左右对峙”，绘制的难度显著高于另外三个滤波器曲线。
        // 左段曲线是低通滤波器曲线，resonance 值决定它的高度，只需在原有的代码上稍作调整。
        // 右段曲线相当于 resonance 值恒为零的高通滤波器曲线。

        // 基础的常量。
        const uint32_t half_height_y = pos.y + (height >> 1); // 整数右移1位，就是除以2
        const uint32_t half_width_x = pos.x + (width >> 1);
        const uint32_t width_1div4 = width >> 2;  // 整数右移2位，就是除以4
        const uint32_t width_1div8 = width >> 3;  // 整数右移3位，就是除以8
        const uint32_t width_1div16 = width >> 4; // 整数右移4位，就是除以16

        // 通用的量，画左右半边曲线都要用到。
        const float left_line_length = float(width - width_1div4 - width_1div8) * (*cutoff * 0.01); // 左侧直线的长度
        const float min_curve_height_y = (float)half_height_y; // 这个y值，对应的是控件的一半高度
        const float max_curve_height_y = (float)(pos.y - height + 20); // 这个y值，对应的是控件顶部，略往下一点点

        // 设置绘图区域。（参考自 imgui_demo.cpp）
        // 这可以允许我们将曲线绘制到控件的区域外，从而保证曲线的顺滑。
        // 否则，若如果简单将右侧曲线段或直线段设为右侧边缘，那么得出来的线会变成直线，不够自然。
        draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height));

        //
        // 在 synthv1 中，基准点为控件下边缘正中间的点，对应的 cutoff 值为50.0。我们就以这个点为中心，向两侧画曲线。
        //

        // 计算不同的 cutoff 值下，基准点横坐标的偏移量
        const float curve_base_delta =
            (float)(width - 2 * (width_1div8 + float(width_1div16 >> 1))) * ((*cutoff - 50) * 0.01);
        ImVec2 curve_begin(half_width_x + curve_base_delta, pos.y + height); // 基准点坐标

        //
        // 一，画左半部分的曲线。
        //
        {
            // 根据当前 resonance 值，确定曲线最高点需要升高的纵轴高度
            const float resonance_delta_y = (max_curve_height_y - min_curve_height_y) * (*resonance * 0.01);

            // 计算出贝塞尔曲线4个控制点的坐标，并绘制曲线
            // 注：适当移动p2、p3的横坐标，使曲线看起来更自然，更接近原版合成器。以控件宽度200为基准，推算出不同宽度下的偏移量。
            int p2_x_offset = 10 * width / 200; // 当控件宽度为200时，p2 的 x 坐标偏移量取10。以此来推算
            int p3_x_offset = p2_x_offset * 0.5; // 取 p2_offset 的一半

            ImVec2 p1(
                ((FilterSlope(*slope) == FilterSlope::s_24dBPerOct) ? curve_begin.x - width_1div16 : curve_begin.x),
                curve_begin.y);
            ImVec2 p2(curve_begin.x - (width_1div8 >> 1) - p2_x_offset,
                      min_curve_height_y + resonance_delta_y); // 曲线最高点
            ImVec2 p3(p2.x - (width_1div16 >> 2) - p3_x_offset,
                      half_height_y); // 曲线爬升期间的拐点，旨在使曲线转折不突兀
            ImVec2 p4(p3.x - width_1div16, half_height_y);

            draw_list->AddBezierCubic(p1, p2, p3, p4, white, 0);

            //
            // 绘制其余的直线段
            //
            draw_list->AddLine(ImVec2(pos.x, half_height_y), p4, white);
        }

        // 二，画右半部分的曲线。
        // 右半部分是在剩余的空白区域内作画，它的宽度取决于左半部分曲线的宽度。
        // 考虑到这一特殊性，我们需要先画曲线，剩下空间足够的话再画直线。
        {
            //
            // 计算出贝塞尔曲线4个控制点的坐标，并绘制曲线
            //

            // 同样要适当移动p2、p3的横坐标，使曲线看起来更自然，更接近原版合成器。
            const int p2_x_offset = 10 * width / 200; // 当控件宽度为200时，p2 的 x 坐标偏移量取10。以此来推算
            const int p3_x_offset = p2_x_offset * 0.5; // 取 p2_offset 的一半

            ImVec2 p1(
                ((FilterSlope(*slope) == FilterSlope::s_24dBPerOct) ? curve_begin.x + width_1div16 : curve_begin.x),
                curve_begin.y);
            ImVec2 p2(curve_begin.x + (width_1div8 >> 1) + p2_x_offset, min_curve_height_y); // 曲线最高点
            ImVec2 p3(p2.x + (width_1div16 >> 2) + p3_x_offset,
                      half_height_y); // 曲线爬升期间的拐点，旨在使曲线转折不突兀
            ImVec2 p4(p3.x + width_1div16, half_height_y);

            draw_list->AddBezierCubic(p1, p2, p3, p4, white, 0);

            //
            // 绘制其余的直线段
            //
            draw_list->AddLine(p4, ImVec2(p4.x + width, half_height_y), white);
        }

        draw_list->PopClipRect(); // 一定要记得出栈!
    }
    break;

    default: // noop action
        break;
    }
}

void VeeOneWidgets::_drawFormantCurve(ImVec2 &pos, float *cutoff, float *resonance, int *type, int *slope,
                                      uint32_t width, uint32_t height)
{
    // 获取当前的 ImDrawList
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // 定义边框颜色（白色）
    const ImU32 white = IM_COL32(255, 255, 255, 255);

    // 设置绘图区域。（参考自 imgui_demo.cpp）
    // 这可以允许我们将曲线绘制到控件的区域外，从而保证曲线的顺滑。
    // 否则，若如果简单将右侧曲线段或直线段设为右侧边缘，那么得出来的线会变成直线，不够自然。
    draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height));

    {
        // 常用的尺寸，为控件宽度、高度的一部分。
        // 【注意】切勿使用无符号整数类型，否则会导致点坐标溢出，造成显示异常！
        const int width_1div4 = (width >> 2);
        const int width_1div8 = (width >> 3);
        const int width_1div16 = (width >> 4);

        const int half_height = (height >> 1);
        const int height_1div4 = (height >> 2);
        const int height_1div8 = (height >> 3);

        // x 是截止频率对应的横坐标，它是基于截止频率参数 m_fCutoff 和控件宽度 w 计算得出的
        int x = width_1div8 + int((*cutoff * 0.01) * float(width - width_1div4));

        // y 坐标代表了共振频率在控件高度上的位置，它决定了formant曲线的垂直位置
        int y = half_height - int((*resonance * 0.01) * float(height + height_1div4));

        // x2 是formant曲线左侧的关键点的横坐标。它是基于截止频率对应的横坐标 x 和控件宽度的四分之一 w4 计算得出的
        const int x2 = (x - width_1div4) >> 2;

        // y2 是formant曲线最低点的纵坐标。它是基于共振频率对应的纵坐标 y 和控件高度的四分之一 h4 计算得出的
        const int y2 = (y - height_1div4) >> 2;

        // ws 是根据滤波器斜率计算的宽度偏移。它是基于控件宽度的八分之一 w8 和斜率 m_iSlope 计算得出的
        const int ws = width_1div8 - ((FilterSlope(*slope) == FilterSlope::s_24dBPerOct) ? (width_1div8 >> 1) : 0);

        const int n3 = 5; // num.formants
        const int w3 = (x + ws - x2) / n3 - 1;
        const int w6 = (w3 >> 1);
        const int h3 = (height_1div4 >> 1);
        int x3 = x2;
        int y3 = y2;

        ImVec2 last_p0(pos.x, pos.y + half_height);

        for (int i = 0; i < n3; ++i)
        {

            ImVec2 p0(last_p0);
            ImVec2 p1(pos.x + x3, pos.y + half_height);
            ImVec2 p2(pos.x + x3 + w6, pos.y + y3);
            ImVec2 p3(pos.x + x3 + w3, pos.y + y3 + half_height);

            draw_list->AddBezierCubic(p0, p1, p2, p3, white, 0);

            x3 += w3;
            y3 += h3;

            last_p0 = p3;
        }

        // 绘制最后一段曲线到控件底部的曲线
        {
            [[maybe_unused]] const int p1_x_offset = 10 * width / 200;
            const int p2_x_offset = 10 * width / 200, p2_y_offset = 10 * height / 80;
            const int p3_x_offset = 10 * width / 200;

            ImVec2 p0(last_p0);
            ImVec2 p1(p0.x + (width_1div16 >> 3), p0.y + (int(pos.y + height - p0.y) >> 1));
            ImVec2 p2(p0.x + (width_1div16 >> 2) + p2_x_offset, p1.y + p2_y_offset);
            ImVec2 p3(p0.x + p3_x_offset, pos.y + height);

            draw_list->AddBezierCubic(p0, p1, p2, p3, white, 0);
        }
    }

    draw_list->PopClipRect();
}
