#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MAX_SIZE 32

typedef struct {
  int PID;
  int priority;
  float arrivalTime;
  float burstTime;
  float scheduledTime;
} Process;

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

    static bool showDemoWindow = true;
    static int numberOfProcesses = 0;
    static Process processes[MAX_SIZE];

    ImGui::ShowDemoWindow(&showDemoWindow);

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

    for (int i = 0; i < numberOfProcesses; i++) {
      ImGui::PushID(i);
      ImGui::Text("%i", i);
      processes[i].PID = i;
      ImGui::NextColumn();
      if (ImGui::DragInt("##Priority", &processes[i].priority)) {
        processes[i].priority =
            processes[i].priority > 0 ? processes[i].priority : 0;
      }
      ImGui::NextColumn();
      if (ImGui::DragFloat("##Arrival Time", &processes[i].arrivalTime,
                           0.005f)) {
        processes[i].arrivalTime =
            processes[i].arrivalTime > 0.0f ? processes[i].arrivalTime : 0.0f;
      }
      ImGui::NextColumn();
      if (ImGui::DragFloat("##Burst Time", &processes[i].burstTime, 0.005f)) {
        processes[i].burstTime =
            processes[i].burstTime > 0.0f ? processes[i].burstTime : 0.0f;
      }
      ImGui::NextColumn();
      ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::Spacing();
    if (ImGui::Button("Add Process")) {
      if (numberOfProcesses < MAX_SIZE) {
        processes[numberOfProcesses].priority = numberOfProcesses;
        processes[numberOfProcesses].arrivalTime = (float)numberOfProcesses;
        processes[numberOfProcesses].burstTime = 1.0f;
        numberOfProcesses++;
      }
    }
    ImGui::Spacing();

    ImGui::Separator();

    ImGui::Spacing();
    static float timeScale = 100.0f;
    ImGui::SliderFloat("Time Scale", &timeScale, 1.0f, 100.0f);
    ImGui::Spacing();

    ImGuiStyle &style = ImGui::GetStyle();
    float height = ImGui::GetFrameHeightWithSpacing() * numberOfProcesses +
                   style.ScrollbarSize + style.WindowPadding.y * 2.0f;
    ImGui::BeginChild("Scrolling", ImVec2(0, height), true,
                      ImGuiWindowFlags_HorizontalScrollbar);

    for (int i = 0; i < numberOfProcesses; i++) {
      ImGui::PushID(i);
      ImGui::AlignTextToFramePadding();
      ImGui::Text("%i", i);
      ImGui::SameLine();
      ImGui::Dummy(ImVec2(processes[i].arrivalTime * timeScale, 0));
      ImGui::SameLine();
      ImGui::Button("", ImVec2(processes[i].burstTime * timeScale, 0));
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Waiting Time: TODO");
        ImGui::TextUnformatted("Turnaround Time: TODO");
        ImGui::TextUnformatted("Reponse Time: TODO");
        ImGui::EndTooltip();
      }
      ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::TextUnformatted("Average Waiting Time: TODO");
    ImGui::TextUnformatted("Average Turnaround Time: TODO");
    ImGui::TextUnformatted("Average Reponse Time: TODO");

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
