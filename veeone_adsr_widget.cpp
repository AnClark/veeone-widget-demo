#include "veeone_adsr_widget.hpp"
#include "imgui.h"

// 定义边框颜色（白色）
static const ImU32 white = IM_COL32(255, 255, 255, 255);

void VeeOneWidgets::ADSREnvelope(const char *id, float *attack, float *decay, float *sustain, float *release,
                                 uint32_t width, uint32_t height)
{
    // 在你的渲染循环中，你需要获取当前的ImDrawList
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // 定义矩形的位置。
    // 矩形的大小由函数参数 width 与 height 确定。
    ImVec2 pos = ImVec2(ImGui::GetCursorScreenPos()); // 获取当前控件插入点

    // 绘制矩形边框
    draw_list->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height), white);

    // 用这个来占位
    ImGui::InvisibleButton(id, ImVec2(width, height));

    // 设置绘图区域
    draw_list->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + width, pos.y + height));

    // 给绘图区域设置边距，以容纳后续要加入的控制按钮
    const int PADDING = 5;
    pos.x += PADDING;
    pos.y += PADDING;
    width -= PADDING * 2; // 如果不乘以二，pos.x 增加的值会在后续运算中被抵消掉，右侧边距将形同虚设
    height -= PADDING * 2; // 同理

    // 常用的尺寸。使用位运算来提升性能。
    const int half_width = width >> 1;
    const int width_1div4 = width >> 2;

    const int half_height = height >> 1;
    const int height_1div4 = height >> 2;

    //
    // 绘制 attack 曲线
    //
    const int attack_width = width_1div4 * (*attack * 0.01);

    ImVec2 p1_attack(pos.x, pos.y + height); // Attack 曲线的起点固定为控件左下角
    ImVec2 p2_attack(pos.x + (attack_width >> 3), pos.y + height_1div4);
    ImVec2 p3_attack(pos.x + attack_width, pos.y); // attack 曲线的终点始终位于控件上边缘
    draw_list->AddBezierQuadratic(p1_attack, p2_attack, p3_attack, white, 0);

    //
    // 绘制 decay、sustain 曲线
    //
    // sustain 的值决定第三段曲线的高度，也影响 decay 曲线终点的高度。同时，sustain 的宽度是恒定的。
    const int decay_width = width_1div4 * (*decay * 0.01);
    const int sustain_width = width_1div4;
    const int sustain_height = height * (*sustain * 0.01);

    ImVec2 p1_decay(p3_attack);
    ImVec2 p2_decay(p1_decay.x + (decay_width >> 3),
                    pos.y + ((height - sustain_height) >> 1) + ((height - sustain_height) >> 2));
    ImVec2 p3_decay(p1_decay.x + decay_width, pos.y + height - sustain_height);
    draw_list->AddBezierQuadratic(p1_decay, p2_decay, p3_decay, white, 0);

    ImVec2 sustain_begin(p3_decay);
    ImVec2 sustain_end(p3_decay.x + sustain_width, pos.y + height - sustain_height);
    draw_list->AddLine(sustain_begin, sustain_end, white);

    //
    // 绘制 release 曲线
    //
    const int release_width = width_1div4 * (*release * 0.01);

    ImVec2 p1_release(sustain_end);
    ImVec2 p2_release(p1_release.x + (release_width >> 3), p1_release.y + (sustain_height >> 1) + (sustain_height >> 2));
    ImVec2 p3_release(p1_release.x + release_width, pos.y + height);
    draw_list->AddBezierQuadratic(p1_release, p2_release, p3_release, white, 0);

    // 还原绘图区域设置
    draw_list->PopClipRect();
}
