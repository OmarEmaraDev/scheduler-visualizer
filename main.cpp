#include <stdio.h>

#include "imgui/imgui.h"

#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "Scheduler Visualizer", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glewInit();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 430 core");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool b = true;
    static int priority = 0;
    static float arrivalTime = 0.0f;
    static float burstTime = 0.0f;
    static int counter = 0;

    ImGui::ShowDemoWindow(&b);

    ImGui::Begin("Scheduler Visualizer");

    ImGui::Columns(4, "data");
    ImGui::SetColumnWidth(0, 40);

    ImGui::Separator();

    ImGui::Text("PID");
    ImGui::NextColumn();
    ImGui::Text("Priority");
    ImGui::NextColumn();
    ImGui::Text("Arrival Time");
    ImGui::NextColumn();
    ImGui::Text("Burst Time");
    ImGui::NextColumn();

    ImGui::Separator();

    ImGui::Text("1");
    ImGui::NextColumn();
    ImGui::InputInt("##Priority", &priority);
    ImGui::NextColumn();
    ImGui::InputFloat("##Arrival Time", &arrivalTime);
    ImGui::NextColumn();
    ImGui::InputFloat("##Burst Time", &burstTime);

    ImGui::Separator();

    ImGui::Columns(1);

    ImGui::End();

    ImGui::Render();
    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(0.05, 0.05, 0.05, 1.0);
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
