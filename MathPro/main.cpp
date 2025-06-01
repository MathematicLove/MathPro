#include <iostream>
#include <list>
#include <vector>
#include "operations.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "glfw/include/GLFW/glfw3.h"

using namespace std;

// Sample data for statistics
list<double> X = {1, 3, 5, 2, 4, 6, 2, 1, 4, 6, 7, 5, 9};

int main(int argc, char** argv) {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(2560, 1440, "ProMath", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    int current_section = 0;
    double a = 0.0, b = 0.0, result = 0.0, area = 0.0, length = 0.0;
    
    BaseOperations baseOp;
    ProOperations proOp;
    MatStat mStat;
    Triangle triangle;
    Circle circle;
    Square square;
    Rectangle rectangle;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Calculator");

        if (ImGui::Button("1) Base calculator"))      current_section = 1;
        ImGui::SameLine();
        if (ImGui::Button("2) Math. Statistic"))     current_section = 2;
        ImGui::SameLine();
        if (ImGui::Button("3) Pro operations"))      current_section = 3;
        ImGui::SameLine();
        if (ImGui::Button("4) Linear algebra"))      current_section = 4;
        ImGui::SameLine();
        if (ImGui::Button("5) Geometry operations")) current_section = 5;

        ImGui::Separator();

        if (current_section >= 1 && current_section <= 5) {
            ImGui::InputDouble("x", &a);
            if (current_section != 2)
                ImGui::InputDouble("y", &b);
        }

        switch (current_section) {
            case 1: // Base calculator
                if (ImGui::Button("x + y"))      result = baseOp.add(a, b);
                ImGui::SameLine();
                if (ImGui::Button("x - y"))      result = baseOp.minus(a, b);
                if (ImGui::Button("x * y"))      result = baseOp.mult(a, b);
                ImGui::SameLine();
                if (ImGui::Button("x / y"))      result = baseOp.div(a, b);
                break;

            case 2: // Math. Statistic
                if (ImGui::Button("Avg of X"))   result = mStat.viborka(X);
                ImGui::SameLine();
                if (ImGui::Button("MSE"))        result = mStat.MSE(X);
                if (ImGui::Button("RMSE"))       result = mStat.RMSE(X);
                ImGui::SameLine();
                if (ImGui::Button("MAE"))        result = mStat.MAE(X);
                break;

            case 3: // Pro operations
                if (ImGui::Button("x^y"))        result = proOp.inDeg(a, b);
                ImGui::SameLine();
                if (ImGui::Button("sqrt(x)"))    result = proOp.inSqrt(a);
                if (ImGui::Button("y-th root"))  result = proOp.specSqrt(a, b);
                break;

            case 4: // Linear algebra (placeholder)
                ImGui::Text("Linear algebra functions placeholder");
                break;
                
            case 5:
                ImGui::Text("Geometry");
                if(ImGui::Button("Squere"))       area = square.area(a);
                if(ImGui::Button("Rectangle"))    area = rectangle.area(a, b);
                if(ImGui::Button("Triangle"))     area = triangle.area(a, b);
                if(ImGui::Button("Circle"))       area = circle.area(a);
                if(ImGui::Button("Circle lenght")) length = circle.length(a);
                break;

            default:
                ImGui::Text("Select a section");
        }

        // Display result
        ImGui::Separator();
        ImGui::Text("Result: %.6f", result);
        ImGui::Text("Area: %.6f", area);
        ImGui::Text("Length: %.6f", length);

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
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
