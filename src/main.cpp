#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>

// 播放音频的原生 Windows 函数
void play_bell(const char* filepath) {
    char cmd[512];
    mciSendStringA("close school_bell", NULL, 0, NULL);
    snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias school_bell", filepath);
    mciSendStringA(cmd, NULL, 0, NULL);
    mciSendStringA("play school_bell", NULL, 0, NULL);
}

// 辅助函数：绘制带颜色的按钮
bool ColoredButton(const char* label, ImU32 bg_color, ImVec2 size = ImVec2(0, 0)) {
    ImGui::PushStyleColor(ImGuiCol_Button, bg_color);
    // 简单处理 Hover 和 Active 状态的颜色（稍微变亮/变暗）
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg_color | 0x00333333); 
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg_color & 0xFFDDDDDD);
    bool pressed = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return pressed;
}

// 数据结构
struct BellSchedule {
    const char* name;
    const char* prep;
    const char* start;
    const char* exit;
    const char* duration;
    bool active;
};

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // 设置初始窗口大小，与截图比例相近
    GLFWwindow* window = glfwCreateWindow(900, 600, "School Bell Program", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // 1. 切换到浅色主题 (Light Theme) 以匹配截图
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.FrameRounding = 3.0f; // 按钮微圆角
    style.ItemSpacing = ImVec2(8, 8);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 模拟截图中的真实数据
    BellSchedule schedules[] = {
        {"1st Period", "08:50 AM", "09:00 AM", "09:40 AM", "15s", true},
        {"Teneffus 1", "09:40 AM", "-", "-", "10s", true},
        {"2nd Period", "09:50 AM", "10:00 AM", "10:40 AM", "15s", true},
        {"Ogle Arasi", "11:55 AM", "12:00 PM", "1:00 PM",  "20s", true},
        {"3rd Period", "1:00 PM",  "1:10 PM",  "1:50 PM",  "15s", true},
        {"4th Period", "2:00 PM",  "2:10 PM",  "2:50 PM",  "15s", false}
    };
    int num_schedules = sizeof(schedules) / sizeof(schedules[0]);
    int bell_duration = 15;

    // 颜色定义 (匹配截图)
    ImU32 col_green = IM_COL32(66, 150, 76, 255);
    ImU32 col_blue  = IM_COL32(40, 95, 150, 255);
    ImU32 col_red   = IM_COL32(180, 80, 60, 255);
    ImU32 col_orange= IM_COL32(200, 120, 40, 255);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("MainWorkspace", NULL, window_flags);

        // --- 顶部标题 ---
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("School Bell Program").x) / 2.0f);
        ImGui::Text("School Bell Program"); // 在实际项目中可以通过加载大号字体来加粗
        ImGui::Spacing();

        // --- 数据表格 ---
        // 7列布局，固定列宽策略
        if (ImGui::BeginTable("ScheduleTable", 7, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Bell Name");
            ImGui::TableSetupColumn("Prep Bell");
            ImGui::TableSetupColumn("Start Bell");
            ImGui::TableSetupColumn("Exit Bell");
            ImGui::TableSetupColumn("Duration");
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 150.0f); // 操作列宽一点
            ImGui::TableHeadersRow();

            for (int row = 0; row < num_schedules; row++) {
                ImGui::TableNextRow();
                
                // 文本列
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", schedules[row].name);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", schedules[row].prep);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%s", schedules[row].start);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", schedules[row].exit);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%s", schedules[row].duration);
                
                // 状态列 (Active 按钮)
                ImGui::TableSetColumnIndex(5);
                char btn_id[32];
                if (schedules[row].active) {
                    sprintf(btn_id, "Active##%d", row);
                    ColoredButton(btn_id, col_green, ImVec2(-FLT_MIN, 0)); // 充满列宽
                } else {
                    ImGui::Button("Inactive", ImVec2(-FLT_MIN, 0));
                }

                // 操作列 (Edit / Delete 按钮)
                ImGui::TableSetColumnIndex(6);
                sprintf(btn_id, "Edit##%d", row);
                ColoredButton(btn_id, col_blue, ImVec2(65, 0));
                ImGui::SameLine();
                sprintf(btn_id, "Delete##%d", row);
                ColoredButton(btn_id, col_red, ImVec2(75, 0));
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();

        // --- 居中的功能按钮 (Ekle, Duzenle, Sil) ---
        float center_btns_width = 300.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - center_btns_width) / 2.0f);
        ColoredButton("Ekle", col_green, ImVec2(90, 35));
        ImGui::SameLine();
        ColoredButton("Duzenle", col_orange, ImVec2(90, 35));
        ImGui::SameLine();
        ColoredButton("Sil", col_red, ImVec2(90, 35));

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // --- 底部控制栏布局 ---
        // 使用列布局将底部平分为三部分：滑块、大按钮、时钟
        if (ImGui::BeginTable("BottomPanel", 3, ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            
            // 左侧：滑块
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Bell Duration (Seconds)");
            ImGui::SameLine();
            ImGui::PushItemWidth(50);
            ImGui::InputInt("##dur_input", &bell_duration, 0, 0); // 隐藏加减号的数字输入
            ImGui::PopItemWidth();
            ImGui::SetNextItemWidth(-FLT_MIN); // 充满可用宽度
            ImGui::SliderInt("##dur_slider", &bell_duration, 1, 60, "");

            // 中间：巨大的 Test Sound 按钮
            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20); // 稍微居中调整
            if (ColoredButton("Test Sound", col_blue, ImVec2(ImGui::GetContentRegionAvail().x - 40, 60))) {
                play_bell("C:\\test.mp3");
            }

            // 右侧：时钟盒子
            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(230, 230, 230, 255)); // 灰色背景
            ImGui::BeginChild("ClockBox", ImVec2(-FLT_MIN, 60), true);
            
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Bilgisayar Saati").x) / 2.0f);
            ImGui::TextDisabled("Bilgisayar Saati");
            
            // 获取系统时间
            time_t rawtime;
            struct tm* timeinfo;
            char time_str[80];
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
            
            // 居中放大显示时间
            ImGui::SetWindowFontScale(1.5f); // 放大字体
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(time_str).x) / 2.0f);
            ImGui::Text("%s", time_str);
            ImGui::SetWindowFontScale(1.0f); // 恢复字体大小
            
            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::EndTable();
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        // 背景颜色设置为白色 (虽然被窗口完全挡住了)
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}