#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include "operations.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "glfw/include/GLFW/glfw3.h"

using namespace std;

/* ──────────────── ImVec2 helpers ─────────────── */
static inline ImVec2 operator+(ImVec2 a, ImVec2 b){ return {a.x+b.x, a.y+b.y}; }
static inline ImVec2 operator-(ImVec2 a, ImVec2 b){ return {a.x-b.x, a.y-b.y}; }

/* ─────────────── GLOBAL STATE ────────────────── */
static double xVal = 0.0, yVal = 0.0, result = 0.0;
static double area = 0.0, lengthP = 0.0;

static std::vector<double> dataSample;          // ← renamed
static char sampleBuf[2048] = {0};
static bool showPlot = false;

static int rows = 3, cols = 3;
static LinAlg::Matrix matrix = LinAlg::make(3,3);
static LinAlg::Matrix mRes;
static double detRes = 0;
static std::vector<double> eigVec;

/* ─────────────── DRAWING HELPERS ─────────────── */
void DrawShape2D(const char* title,int type,double p1,double p2)
{
    if(!ImGui::Begin(title,nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove)){
        ImGui::End(); return;
    }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 c = ImGui::GetCursorScreenPos() + ImVec2(100,100);
    const float k = 45.f;

    switch(type){
        case 0:{ float s=p1*k;
                 dl->AddRectFilled(c-ImVec2(s/2,s/2), c+ImVec2(s/2,s/2),
                                   IM_COL32(100,200,255,255)); }break;
        case 1:{ float sx=p1*k, sy=p2*k;
                 dl->AddRectFilled(c-ImVec2(sx/2,sy/2), c+ImVec2(sx/2,sy/2),
                                   IM_COL32(120,220,120,255)); }break;
        case 2:{ float bx=p1*k, h=p2*k;
                 ImVec2 A=c+ImVec2(-bx/2,h/2), B=c+ImVec2(bx/2,h/2), C=c-ImVec2(0,h/2);
                 dl->AddTriangleFilled(A,B,C, IM_COL32(240,200,80,255)); }break;
        case 3:{ float r=p1*k;
                 dl->AddCircleFilled(c,r, IM_COL32(255,180,180,255)); }break;
    }
    ImGui::Dummy(ImVec2(200,200));
    ImGui::End();
}

void DrawShape3D(const char* title,int type,double a,double h)
{
    if(!ImGui::Begin(title,nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove)){
        ImGui::End(); return;
    }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 o = ImGui::GetCursorScreenPos() + ImVec2(120,120);
    const float k = 35.f;

    if(type==0){                               // Cube
        float s=a*k;
        ImVec2 A=o, B=o+ImVec2(s,0), C=o+ImVec2(s,s), D=o+ImVec2(0,s);
        ImVec2 off=ImVec2(s*0.5f,-s*0.5f);
        ImVec2 A2=A+off,B2=B+off,C2=C+off,D2=D+off;
        dl->AddQuadFilled(A,B,C,D, IM_COL32(70,150,250,255));
        dl->AddQuadFilled(A2,B2,C2,D2, IM_COL32(90,170,255,220));
        dl->AddQuadFilled(B,B2,C2,C,  IM_COL32(60,130,230,255));
    }
    else if(type==1){                          // Pyramid
        float s=a*k, hh=h*k;
        ImVec2 A=o, B=o+ImVec2(s,0), C=o+ImVec2(s/2,s*0.866f),
               P=o+ImVec2(s/2,-hh);
        dl->AddTriangleFilled(A,B,C, IM_COL32(250,200,70,255));
        dl->AddTriangleFilled(A,B,P, IM_COL32(230,180,60,255));
        dl->AddTriangleFilled(B,C,P, IM_COL32(210,160,50,255));
        dl->AddTriangleFilled(C,A,P, IM_COL32(190,140,40,255));
    }
    else if(type==2){                          // Sphere
        float r=a*k;
        for(int i=0;i<12;++i){
            float rr = r*(1.f - i*0.04f);
            dl->AddCircle(o, rr,
                          IM_COL32(120+i*10,120+i*10,255, 150+i*8), 0, 3);
        }
        dl->AddCircleFilled(o,r*0.96f, IM_COL32(140,140,255,40));
    }
    ImGui::Dummy(ImVec2(240,240));
    ImGui::End();
}

/* ───────────── Regression Plot ──────────────── */
void ShowPlotWindow()
{
    if(!showPlot) return;
    if(!ImGui::Begin("Regression Plot",&showPlot,
                     ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove)){
        ImGui::End(); return;
    }
    if(dataSample.size()<2){ ImGui::Text("Need >= 2 points"); ImGui::End(); return; }

    double n=dataSample.size(), sx=0,sy=0,sxy=0,sx2=0;
    for(size_t i=0;i<n;++i){
        sx+=i; sy+=dataSample[i]; sxy+=i*dataSample[i]; sx2+=i*i;
    }
    double k=(n*sxy-sx*sy)/(n*sx2-sx*sx+1e-9);
    double b=(sy-k*sx)/n;

    ImVec2 sz(420,260); ImDrawList* dl=ImGui::GetWindowDrawList();
    ImVec2 p0=ImGui::GetCursorScreenPos(); dl->AddRect(p0,p0+sz,IM_COL32_WHITE);

    double ymin=*std::min_element(dataSample.begin(),dataSample.end());
    double ymax=*std::max_element(dataSample.begin(),dataSample.end());
    ymin=std::min(ymin,k*(n-1)+b); ymax=std::max(ymax,k*(n-1)+b);
    double yr=ymax-ymin+1e-9;

    auto map=[&](double ix,double y){
        float x=p0.x+50+float(ix/(n-1))*(sz.x-70);
        float yy=p0.y+20+float(1-(y-ymin)/yr)*(sz.y-60);
        return ImVec2(x,yy);
    };

    dl->AddLine(p0+ImVec2(50,20), p0+ImVec2(50,sz.y-40), IM_COL32_WHITE);
    dl->AddLine(p0+ImVec2(50,sz.y-40), p0+ImVec2(sz.x-20,sz.y-40), IM_COL32_WHITE);

    for(size_t i=0;i<n;++i)
        dl->AddCircleFilled(map(i,dataSample[i]),4, IM_COL32(255,100,100,255));

    dl->AddLine(map(0,b), map(n-1,k*(n-1)+b), IM_COL32(100,255,120,255),2);
    ImGui::Dummy(sz); ImGui::End();
}

/* ───────────── Sample Parser ──────────────── */
void ParseSample(){
    dataSample.clear();
    std::stringstream ss(sampleBuf);
    std::string token;
    while(std::getline(ss,token,',')){
        std::stringstream ss2(token);
        std::string num;
        while(std::getline(ss2,num,'\n')){
            if(!num.empty()) dataSample.push_back(std::stod(num));
        }
    }
}

/* ─────────────────────  MAIN  ────────────────── */
int main()
{
    if(!glfwInit()){ std::cerr<<"GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win=glfwCreateWindow(1800,1000,"ProMath",nullptr,nullptr);
    if(!win){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win); glfwSwapInterval(1);

    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(win,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    int section=0, current2D=-1, current3D=-1;
    double edge=1.0,height=1.0,radius=1.0;

    while(!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

        ImGui::Begin("ProMath Calculator");

        if(ImGui::Button("Calculator"))     section=0; ImGui::SameLine();
        if(ImGui::Button("Statistics"))     section=1; ImGui::SameLine();
        if(ImGui::Button("Linear Algebra")) section=2; ImGui::SameLine();
        if(ImGui::Button("2D Geometry"))    section=3; ImGui::SameLine();
        if(ImGui::Button("3D Geometry"))    section=4;

        ImGui::Separator();

        /* ---- 1. CALC ---- */
        if(section==0){
            ImGui::InputDouble("x",&xVal); ImGui::InputDouble("y",&yVal);
            if(ImGui::Button("x + y")){ result=BaseCalc::add(xVal,yVal); xVal=result; yVal=0; }
            ImGui::SameLine();
            if(ImGui::Button("x - y")){ result=BaseCalc::sub(xVal,yVal); xVal=result; yVal=0; }
            if(ImGui::Button("x * y")){ result=BaseCalc::mul(xVal,yVal); xVal=result; yVal=0; }
            ImGui::SameLine();
            if(ImGui::Button("x / y")){ result=BaseCalc::div(xVal,yVal); xVal=result; yVal=0; }
            if(ImGui::Button("x ^ y")){ result=AdvCalc::power(xVal,yVal); xVal=result; yVal=0; }
        }

        /* ---- 2. STATISTICS ---- */
        if(section==1){
            ImGui::InputTextMultiline("Sample (comma/newline)", sampleBuf, sizeof(sampleBuf),
                                      ImVec2(0,80));
            if(ImGui::Button("Parse sample")) ParseSample();
            ImGui::SameLine();
            if(ImGui::Button("Show plot"))    showPlot=true;

            if(!dataSample.empty()){
                if(ImGui::Button("Mean"))  result = MatStat::mean(dataSample);
                ImGui::SameLine(); if(ImGui::Button("MSE"))  result = MatStat::mse(dataSample);
                ImGui::SameLine(); if(ImGui::Button("RMSE")) result = MatStat::rmse(dataSample);
                ImGui::SameLine(); if(ImGui::Button("MAE"))  result = MatStat::mae(dataSample);
                ImGui::Text("N = %d",(int)dataSample.size());
            }
        }

        /* ---- 3. LINEAR ALGEBRA ---- */
        if(section==2)
        {
            ImGui::InputInt("Rows", &rows); ImGui::SameLine();
            ImGui::InputInt("Cols", &cols);
            rows = std::clamp(rows,1,6);
            cols = std::clamp(cols,1,6);

            /* auto-resize buffer to keep indices valid */
            if(rows != (int)matrix.size() || cols != (int)matrix[0].size())
                matrix = LinAlg::make(rows, cols);

            ImGui::Separator();

            /* editable cells */
            for(int i = 0; i < (int)matrix.size(); ++i)
            {
                for(int j = 0; j < (int)matrix[i].size(); ++j)
                {
                    ImGui::PushID(i*10 + j);
                    ImGui::SetNextItemWidth(55);
                    ImGui::InputDouble("", &matrix[i][j], 0, 0, "%.2f");
                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::NewLine();
            }

            /* square-only operations */
            if(matrix.size() == matrix[0].size())
            {
                if(ImGui::Button("Determinant"))  detRes = LinAlg::determinant(matrix);
                ImGui::SameLine();
                if(ImGui::Button("Transpose"))    mRes   = LinAlg::transpose(matrix);
                ImGui::SameLine();
                if(ImGui::Button("Adjugate"))     mRes   = LinAlg::adjugate(matrix);
                ImGui::SameLine();
                if(ImGui::Button("Eigen vector")) eigVec = LinAlg::eigenVector(matrix);
            }
            else
                ImGui::Text("Matrix must be square for det / adj / eigen");

            if(detRes != 0) ImGui::Text("det = %.4f", detRes);

            if(!mRes.empty())
            {
                ImGui::Text("Result matrix:");
                for(size_t i = 0; i < mRes.size(); ++i)
                {
                    for(size_t j = 0; j < mRes[i].size(); ++j)
                        ImGui::Text("%.2f ", mRes[i][j]), ImGui::SameLine();
                    ImGui::NewLine();
                }
            }

            if(!eigVec.empty())
                ImGui::Text("Eigen ≈ (%.3f, %.3f, %.3f)",
                            eigVec[0], eigVec[1], eigVec[2]);
        }

        /* ---- 4. 2D GEOMETRY ---- */
        if(section==3){
            ImGui::RadioButton("Square",&current2D,0); ImGui::SameLine();
            ImGui::RadioButton("Rectangle",&current2D,1); ImGui::SameLine();
            ImGui::RadioButton("Triangle",&current2D,2); ImGui::SameLine();
            ImGui::RadioButton("Circle",&current2D,3);

            if(current2D>=0){
                if(current2D==0){
                    ImGui::InputDouble("a",&xVal);
                    if(ImGui::Button("Area")) area=Square::area(xVal);
                    DrawShape2D("2D Preview",0,xVal,0);
                }
                if(current2D==1){
                    ImGui::InputDouble("a",&xVal); ImGui::InputDouble("b",&yVal);
                    if(ImGui::Button("Area")) area=Rectangle::area(xVal,yVal);
                    DrawShape2D("2D Preview",1,xVal,yVal);
                }
                if(current2D==2){
                    ImGui::InputDouble("base",&xVal); ImGui::InputDouble("h",&yVal);
                    if(ImGui::Button("Area")) area=Triangle::area(xVal,yVal);
                    DrawShape2D("2D Preview",2,xVal,yVal);
                }
                if(current2D==3){
                    ImGui::InputDouble("r",&xVal);
                    if(ImGui::Button("Area")) area=Circle::area(xVal);
                    ImGui::SameLine();
                    if(ImGui::Button("Perimeter")) lengthP=Circle::length(xVal);
                    DrawShape2D("2D Preview",3,xVal,0);
                }
            }
        }

        /* ---- 5. 3D GEOMETRY ---- */
        if(section==4){
            ImGui::RadioButton("Cube",&current3D,0); ImGui::SameLine();
            ImGui::RadioButton("Pyramid",&current3D,1); ImGui::SameLine();
            ImGui::RadioButton("Sphere",&current3D,2);

            if(current3D==0){
                ImGui::InputDouble("edge",&edge);
                if(ImGui::Button("Area"))   area=Cube::area(edge);
                ImGui::SameLine();
                if(ImGui::Button("Volume")) result=Cube::volume(edge);
                DrawShape3D("3D Preview",0,edge,0);
            }
            if(current3D==1){
                ImGui::InputDouble("base edge",&edge); ImGui::InputDouble("height",&height);
                double Sbase=Triangle::area(edge,edge*std::sqrt(3)/2);
                if(ImGui::Button("Area"))   area=Pyramid::area(Sbase,edge,3);
                ImGui::SameLine();
                if(ImGui::Button("Volume")) result=Pyramid::volume(Sbase,height);
                DrawShape3D("3D Preview",1,edge,height);
            }
            if(current3D==2){
                ImGui::InputDouble("radius",&radius);
                if(ImGui::Button("Area"))   area=Sphere::area(radius);
                ImGui::SameLine();
                if(ImGui::Button("Volume")) result=Sphere::volume(radius);
                DrawShape3D("3D Preview",2,radius,0);
            }
        }

        ImGui::Separator();
        ImGui::Text("Result  : %.6f", result);
        ImGui::Text("Area    : %.6f", area);
        ImGui::Text("Length  : %.6f", lengthP);

        ImGui::End();

        ShowPlotWindow();

        ImGui::Render();
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
