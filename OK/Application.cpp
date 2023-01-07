#include "Application.hpp"

#include <glad/glad.h>



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

Application::Application()
{
    //setup window
    if (!glfwInit())
    {
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_window = glfwCreateWindow(1280, 720, "Job scheduling Genetic Algorithm", NULL, NULL);
    if (m_window == NULL) {
        exit(1);
    }

    glfwMakeContextCurrent(m_window);
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

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Loop();
}

void Application::Loop()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (m_show_demo)
            ImGui::ShowDemoWindow(&m_show_demo);

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
            if (ImGui::BeginCombo("##instancje", m_instance_choice.c_str())) {
                for (auto& instance_file : m_instance_files) {
                    if (ImGui::Selectable(instance_file.c_str())) {
                        m_instance_choice = instance_file;
                        mp_instance = std::make_unique<Instance>(m_instance_choice);
                        m_greedy_value = 0;
                        m_genetic_value = 0;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::End();
        }
        {
            auto greedy_algorithm = [](const Instance& inst)->int {
                std::vector<int> processors;
                processors.resize(inst.GetProcessorCount(), 0);

                for (auto it = inst.beginIter(); it != inst.endIter(); it++) {
                    int smallest = INT_MAX;
                    int smallest_idx = -1;
                    for (int i = 0; i < processors.size(); ++i) {
                        if (processors.at(i) < smallest) {
                            smallest = processors.at(i);
                            smallest_idx = i;
                        }
                    }
                    processors.at(smallest_idx) += (*it);
                }

                int biggest = -1;

                for (int i = 0; i < processors.size(); ++i) {
                    if (processors.at(i) > biggest) {
                        biggest = processors.at(i);
                    }
                }

                return biggest;
            };
            ImGui::Begin("Algorytm zachlanny");
            ImGui::Text("Wynik: ");
            ImGui::InputInt("##greedy", &m_greedy_value, 0, 0, ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Start")) {
                if (mp_instance != nullptr) {
                    m_greedy_value = greedy_algorithm(*mp_instance);
                }
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Algorytm Genetyczny");
            ImGui::Text("Wynik: ");
            ImGui::InputInt("##genetic", &m_genetic_value, 0, 0, ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Start") && m_instance_choice != "") {
                mp_GeneticAlgorith = std::make_unique<GeneticAlgorithm>(m_instance_choice, m_populationSize, m_crossoverProb, m_mutationProb, m_maxGenNumber);
                mp_GeneticAlgorith->RunInAnotherThread(m_timeLimit);
                m_hasRan = true;
            }
            if (m_hasRan) {
                auto opt_int = mp_GeneticAlgorith->GetValue();
                if (opt_int.has_value()) {
                    m_genetic_value = opt_int.value();
                    m_hasRan = false;
                }
            }

            if (m_hasRan) {
                ImGui::BufferingBar("##progress", mp_GeneticAlgorith->Progress, { ImGui::GetCurrentWindow()->ContentSize.x, 10 }, 0xFF313131, 0xFF1818FF);
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Ustawienia");
            ImGui::Checkbox("Limit czasowy", &m_timeLimit);
            ImGui::InputInt("Rozmiar populacji", &m_populationSize, 2);
            ImGui::DragFloat("Proporcja crossoveru", &m_crossoverProb, 0.05, 0.05, 0.95);
            ImGui::DragFloat("Szansa mutacji", &m_mutationProb, 0.01, 0.010, 1.0);
            ImGui::DragInt("Ilosc generacji", &m_maxGenNumber, 100, 100, 100000);
            ImGui::End();
        }
        {
            ImGui::Begin("Wykres");
            if (mp_GeneticAlgorith != nullptr) {
                if (ImPlot::BeginPlot("Fitness")) {
                    static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
                    static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit;
                    ImPlot::SetupAxes("Generacja", "Fitness", xflags, yflags);
                    ImPlot::PlotLine("##line", mp_GeneticAlgorith->HistoricalBest.data(), mp_GeneticAlgorith->HistoricalBest.size());

                    ImPlot::EndPlot();
                }
            }
            ImGui::End();
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(m_window);
    }

}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}
