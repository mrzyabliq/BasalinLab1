#include "Plot.h"

void glfwerror__callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Plot::Plot(output results, std::vector<std::string> Xstring,
           std::vector<std::string> Ystring)
    : Xstring(Xstring), Ystring(Ystring) {
  for (size_t i = 0; i < results.size(); i++) {
    t.push_back(std::get<0>(results[i]));
    Matrix& X = std::get<1>(results[i]);
    Matrix& Y = std::get<2>(results[i]);
    for (size_t xi = 0; xi < X.getRows(); ++xi) xs[xi].push_back(X[xi][0]);
    for (size_t yi = 0; yi < Y.getRows(); ++yi) ys[yi].push_back(Y[yi][0]);
  }
}

GLFWwindow* Plot::setWindow() {
  GLFWwindow* window =
      glfwCreateWindow(1280, 720, "Plot results", nullptr, nullptr);
  if (window == nullptr) return NULL;

  glfwMaximizeWindow(window);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  // io.Fonts->AddFontFromFileTTF(FONT_PATH, 32.0f);

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  return window;
}

void Plot::plotResults() {
  glfwSetErrorCallback(glfwerror__callback);
  if (!glfwInit()) {
    exit(1);
  }

  GLFWwindow* window = setWindow();
  if (window == nullptr) {
    exit(1);
  }

  ImGui_ImplOpenGL3_Init("#version 130");

  {
    initFrame();
    ImPlot::CreateContext();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({1500, 1000});
    ImGui::Begin("Graphic");
    ImGui::Columns(2, "X and Y");
    for (const auto& [i, x] : xs) plotSingle(t, x, Xstring[i], ImVec2(750, 450));
    ImGui::NextColumn();
    for (const auto& [i, y] : ys) plotSingle(t, y, Ystring[i], ImVec2(750, 450));
    ImGui::Columns(1);
    ImGui::End();
    renderWindow(window);
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    initFrame();
    ImGui::Begin("Graphic");
    ImGui::Columns(2, "X and Y");
    for (const auto& [i, x] : xs) plotSingle(t, x, Xstring[i], ImVec2(750, 450));
    ImGui::NextColumn();
    for (const auto& [i, y] : ys) plotSingle(t, y, Ystring[i], ImVec2(750, 450));
    ImGui::Columns(1);
    ImGui::End();
    renderWindow(window);
  }

  closeWindow(window);
}

void Plot::initFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Plot::renderWindow(GLFWwindow* window) {
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwMakeContextCurrent(window);
  glfwSwapBuffers(window);
}

void Plot::closeWindow(GLFWwindow* window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Plot::plotSingle(std::vector<double> x, std::vector<double> y,
                      std::string label, ImVec2 size) {
  if (ImPlot::BeginPlot(label.c_str(), size)) {
    ImPlot::PlotLine(label.c_str(), x.data(), y.data(), x.size());
    ImPlot::EndPlot();
  }
}