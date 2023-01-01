#include <iostream>
#include <algorithm>
#include <execution>
#include "GeneticAlgorithm.hpp"

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>


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

    bool show_demo = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;



    std::vector<std::string> instances_files = {
        "instancje/m25.txt",
        "instancje/m50.txt",
        "instancje/m50n200.txt",
        "instancje/m50n1000.txt",
        "instancje/m10n200.txt",
        "instancje/m30.txt",
        };

    for(auto& instance_file:instances_files){
        std::cout << instance_file << std::endl;
        Instance inst = Instance(instance_file);
        std::cout << "greedy: " << greedy_algorithm(inst) << std::endl;
        
        GeneticAlgorithm GA = GeneticAlgorithm(instance_file, 250, 0.3, 0.15, 100000);
        int wynik =  GA.Run(true);
        std::cout << "genetic algorithm: " << wynik << std::endl << std::endl;
    };
    
    return 0;
}


