#include <iostream>
#include <algorithm>
#include <execution>
#include "GeneticAlgorithm.hpp"

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <implot.h>

#include <GLFW/glfw3.h>

namespace ImGui {
    bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;

        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

        const float t = g.Time;
        const float r = size.y / 2;
        const float speed = 1.5f;

        const float a = speed * 0;
        const float b = speed * 0.333f;
        const float c = speed * 0.666f;

        const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
        const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
        const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
    }

}

int greedy_algorithm(const Instance& inst){
    std::vector<int> processors;
    processors.resize(inst.GetProcessorCount(), 0);

    for(auto it = inst.beginIter(); it != inst.endIter(); it++){
        int smallest = INT_MAX;
        int smallest_idx = -1;
        for(int i = 0; i < processors.size(); ++i){
            if(processors.at(i) < smallest){
                smallest = processors.at(i);
                smallest_idx = i;
            }
        }
        processors.at(smallest_idx) += (*it);
    }

    int biggest = -1;

    for(int i = 0; i < processors.size(); ++i){
        if(processors.at(i) > biggest){
            biggest = processors.at(i);
        }
    }

    return biggest;
}


#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main() {
    //setup window
    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Job scheduling Genetic Algorithm", NULL, NULL);
    if (window == NULL) {
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL();
    

    //setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    const std::vector<std::string> instance_files = {
        "instancje/m25.txt",
        "instancje/m50.txt",
        "instancje/m50n200.txt",
        "instancje/m50n1000.txt",
        "instancje/m10n200.txt",
        "instancje/m30.txt",
    };


    bool show_demo = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::string instance_choice;
    int greedy_value = 0;
    int genetic_value = 0;
    std::unique_ptr<Instance> p_instance;
    std::unique_ptr<GeneticAlgorithm> p_GeneticAlgorith;
    int populationSize = 250;
    float crossoverProb = 0.3;
    float mutationProb = 0.15;
    int maxGenNumber = 10000;
    bool timeLimit = true;
    bool hasRan = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

        //DOCKSPACE
        {
            auto vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->Pos);
            ImGui::SetNextWindowSize(vp->Size);
            ImGui::SetNextWindowViewport(vp->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
            ImGui::DockSpace(ImGui::GetID("Main"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::PopStyleVar(2);
            ImGui::End();
        }
        {
            ImGui::Begin("Wybor instancji");
            if(ImGui::BeginCombo("##instancje", instance_choice.c_str())){
                for(auto& instance_file : instance_files){
                    if (ImGui::Selectable(instance_file.c_str())) {
                        instance_choice = instance_file;
                        p_instance = std::make_unique<Instance>(instance_choice);
                        greedy_value = 0;
                        genetic_value = 0;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Algorytm zachlanny");
            ImGui::Text("Wynik: ");
            ImGui::InputInt("##greedy", &greedy_value, 0, 0, ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Start")) {
                if (p_instance != nullptr) {
                    greedy_value = greedy_algorithm(*p_instance);
                }
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Algorytm Genetyczny");
            ImGui::Text("Wynik: ");
            ImGui::InputInt("##genetic", &genetic_value, 0, 0, ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Start") && instance_choice != "") {
                p_GeneticAlgorith = std::make_unique<GeneticAlgorithm>(instance_choice, populationSize, crossoverProb, mutationProb, maxGenNumber);
                p_GeneticAlgorith->RunInAnotherThread(timeLimit);
                hasRan = true;
            }
            if (hasRan) {
                auto opt_int = p_GeneticAlgorith->GetValue();
                if (opt_int.has_value()) {
                    genetic_value = opt_int.value();
                    hasRan = false;
                }
            }
            
            if (hasRan) {
                ImGui::BufferingBar("##progress", p_GeneticAlgorith->Progress, { ImGui::GetCurrentWindow()->ContentSize.x, 10 }, 0xFF313131, 0xFF1818FF);
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Ustawienia");
            ImGui::Checkbox("Limit czasowy", &timeLimit);
            ImGui::InputInt("Rozmiar populacji", &populationSize, 2);
            ImGui::DragFloat("Proporcja crossoveru", &crossoverProb, 0.05, 0.05, 0.95);
            ImGui::DragFloat("Szansa mutacji", &mutationProb, 0.01, 0.010, 1.0);
            ImGui::DragInt("Ilosc generacji", &maxGenNumber, 100, 100, 100000);
            ImGui::End();
        }
        {
            ImGui::Begin("Wykres");
            if (p_GeneticAlgorith != nullptr) {
                if (ImPlot::BeginPlot("Fitness")) {
                    static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
                    static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit;
                    ImPlot::SetupAxes("Generacja", "Fitness", xflags, yflags);
                    ImPlot::PlotLine("##line", p_GeneticAlgorith->HistoricalBest.data(), p_GeneticAlgorith->HistoricalBest.size());

                    ImPlot::EndPlot();
                }
            }
            ImGui::End();
        }
            
       
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x , clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;


}


