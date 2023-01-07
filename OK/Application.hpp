#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <string>
#include "GeneticAlgorithm.hpp"

class GLFWwindow;

class Application {
public:
	Application();
	~Application();
private:
    void Loop();
    GLFWwindow* m_window;
    bool m_show_demo = false;
    ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::string m_instance_choice;
    int m_greedy_value = 0;
    int m_genetic_value = 0;
    std::unique_ptr<Instance> mp_instance;
    std::unique_ptr<GeneticAlgorithm> mp_GeneticAlgorith;
    int m_populationSize = 250;
    float m_crossoverProb = 0.3;
    float m_mutationProb = 0.15;
    int m_maxGenNumber = 10000;
    bool m_timeLimit = true;
    bool m_hasRan = false;
    const std::vector<std::string> m_instance_files = {
        "instancje/m25.txt",
        "instancje/m50.txt",
        "instancje/m50n200.txt",
        "instancje/m50n1000.txt",
        "instancje/m10n200.txt",
        "instancje/m30.txt",
    };
};