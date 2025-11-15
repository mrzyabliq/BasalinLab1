#ifndef PLOT_H
#define PLOT_H
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include "Matrix.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

using output = std::vector<std::tuple<double, Matrix, Matrix>>;
class Plot {
 private:
  std::vector<double> t;
  std::map<int, std::vector<double>> xs;
  std::map<int, std::vector<double>> ys;
  std::vector<std::string> Xstring;
  std::vector<std::string> Ystring;

  void initFrame();
  void renderWindow(GLFWwindow* window);
  void closeWindow(GLFWwindow* window);
  GLFWwindow* setWindow();
  void plotSingle(std::vector<double> x, std::vector<double> y,
                  std::string label, ImVec2 size);

 public:
  Plot(output results, std::vector<std::string> Xstring,
       std::vector<std::string> Ystring);
  void plotResults();
};

#endif  // PLOT_H