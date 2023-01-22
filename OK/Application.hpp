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
        "generated/progresja/m20n100.txt",
        "generated/progresja/m20n150.txt",
        "generated/progresja/m20n200.txt",
        "generated/progresja/m20n250.txt",
        "generated/progresja/m20n300.txt",
        "generated/progresja/m20n350.txt",
        "generated/progresja/m20n400.txt",
        "generated/progresja/m20n450.txt",
        "generated/progresja/m20n500.txt",
        "generated/progresja/m20n550.txt",
        "generated/progresja/m20n600.txt",
        "generated/progresja/m20n650.txt",
        "generated/progresja/m20n700.txt",
        "generated/progresja/m20n750.txt",
        "generated/progresja/m20n800.txt",
        "generated/m100n1000/0.txt",
        "generated/m100n1000/1.txt",
        "generated/m100n1000/2.txt",
        "generated/m100n1000/3.txt",
        "generated/m100n1000/4.txt",
        "generated/m100n1000/5.txt",
        "generated/m100n1000/6.txt",
        "generated/m100n1000/7.txt",
        "generated/m100n1000/8.txt",
        "generated/m100n1000/9.txt",
        "generated/m100n1000/10.txt",
        "generated/m100n1000/11.txt",
        "generated/m100n1000/12.txt",
        "generated/m100n1000/13.txt",
        "generated/m100n1000/14.txt",
    };
};