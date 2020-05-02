#include <stdio.h>
#include <stdlib.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <queue>
#include <vector>

#define MAX_SIZE 32

typedef struct {
  int PID;
  int priority;
  float arrivalTime;
  float burstTime;
  float scheduledTime;
} Process;

inline float computeProcessWaitingTime(Process *process) {
  return process->scheduledTime - process->arrivalTime;
}

inline float computeProcessTurnAroundTime(Process *process) {
  return computeProcessWaitingTime(process) + process->burstTime;
}

inline float computeProcessResponseTime(Process *process) {
  return computeProcessWaitingTime(process);
}

float computeAverageProcessWaitingTime(Process processes[MAX_SIZE],
                                       int numberOfProcesses) {
  float averageWaitingTime = 0;
  for (int i = 0; i < numberOfProcesses; i++) {
    averageWaitingTime += computeProcessWaitingTime(processes + i);
  }
  return averageWaitingTime / (float)numberOfProcesses;
}

float computeAverageProcessTurnAroundTime(Process processes[MAX_SIZE],
                                          int numberOfProcesses) {
  float averageTurnAroundTime = 0;
  for (int i = 0; i < numberOfProcesses; i++) {
    averageTurnAroundTime += computeProcessTurnAroundTime(processes + i);
  }
  return averageTurnAroundTime / (float)numberOfProcesses;
}

float computeAverageProcessResponseTime(Process processes[MAX_SIZE],
                                        int numberOfProcesses) {
  float averageResponseTime = 0;
  for (int i = 0; i < numberOfProcesses; i++) {
    averageResponseTime += computeProcessResponseTime(processes + i);
  }
  return averageResponseTime / (float)numberOfProcesses;
}

static int compareInt(int left, int right) { return left - right; }

static int compareFloat(float left, float right) {
  if (left == right) {
    return 0;
  } else if (left > right) {
    return 1;
  } else {
    return -1;
  }
}

static int compareProcessByArrivalTime(const void *left, const void *right) {
  return compareFloat(((Process *)left)->arrivalTime,
                      ((Process *)right)->arrivalTime);
}

static int compareProcessByPID(const void *left, const void *right) {
  return compareInt(((Process *)left)->PID, ((Process *)right)->PID);
}

struct PriorityComparator {
  bool operator()(Process *left, Process *right) {
    return left->priority > right->priority;
  }
};

static void
scheduleProcesses(Process processes[MAX_SIZE], int numberOfProcesses) {
  qsort(processes, numberOfProcesses, sizeof(Process),
        compareProcessByArrivalTime);

  std::priority_queue<Process *, std::vector<Process *>, PriorityComparator>
      priorityQueue;

  float timeElapsed = 0.0f;
  int numberOfScheduledProcesses = 0;
  while (numberOfScheduledProcesses < numberOfProcesses) {
    do {
      priorityQueue.push(processes + numberOfScheduledProcesses);
      timeElapsed = std::max(processes[numberOfScheduledProcesses].arrivalTime,
                             timeElapsed);
      numberOfScheduledProcesses++;
    } while (numberOfScheduledProcesses < numberOfProcesses &&
             processes[numberOfScheduledProcesses].arrivalTime <= timeElapsed);

    while (!priorityQueue.empty()) {
      Process *process = priorityQueue.top();
      process->scheduledTime = timeElapsed;
      timeElapsed += process->burstTime;
      priorityQueue.pop();
    }
  }

  qsort(processes, numberOfProcesses, sizeof(Process), compareProcessByPID);
}

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
      Process *process = processes + i;
      ImGui::PushID(i);
      ImGui::Text("%i", i);
      process->PID = i;
      ImGui::NextColumn();
      if (ImGui::DragInt("##Priority", &process->priority)) {
        process->priority = process->priority > 0 ? process->priority : 0;
      }
      ImGui::NextColumn();
      if (ImGui::DragFloat("##Arrival Time", &process->arrivalTime, 0.005f)) {
        process->arrivalTime =
            process->arrivalTime > 0.0f ? process->arrivalTime : 0.0f;
      }
      ImGui::NextColumn();
      if (ImGui::DragFloat("##Burst Time", &process->burstTime, 0.005f)) {
        process->burstTime =
            process->burstTime > 0.0f ? process->burstTime : 0.0f;
      }
      ImGui::NextColumn();
      ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::Spacing();
    if (ImGui::Button("Add Process")) {
      if (numberOfProcesses < MAX_SIZE) {
        Process *process = processes + numberOfProcesses;
        process->PID = numberOfProcesses;
        process->priority = numberOfProcesses;
        process->arrivalTime = (float)numberOfProcesses;
        process->burstTime = 1.0f;
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

    scheduleProcesses(processes, numberOfProcesses);

    for (int i = 0; i < numberOfProcesses; i++) {
      Process process = processes[i];
      ImGui::PushID(i);
      ImGui::AlignTextToFramePadding();
      ImGui::Text("%i", i);
      ImGui::SameLine();
      ImGui::Dummy(ImVec2(process.scheduledTime * timeScale, 0));
      ImGui::SameLine();
      ImGui::Button("", ImVec2(process.burstTime * timeScale, 0));
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Waiting Time: %.2f", computeProcessWaitingTime(&process));
        ImGui::Text("Turn Around Time: %.2f",
                    computeProcessTurnAroundTime(&process));
        ImGui::Text("Reponse Time: %.2f", computeProcessResponseTime(&process));
        ImGui::EndTooltip();
      }
      ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::Text("Average Waiting Time: %.2f",
                computeAverageProcessWaitingTime(processes, numberOfProcesses));
    ImGui::Text(
        "Average Turn Around Time: %.2f",
        computeAverageProcessTurnAroundTime(processes, numberOfProcesses));
    ImGui::Text(
        "Average Reponse Time: %.2f",
        computeAverageProcessResponseTime(processes, numberOfProcesses));

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
