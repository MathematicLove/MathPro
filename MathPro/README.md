
- Base operations:
  - Sum
  - Minus
  - Multiply
  - Division

- Trigonometry:
  - Sin(x)
  - Cos(x)
  - Tan(x)
  - Ctg(x)

- ProOperations:
  - Sqrt(x)
  - pow(x, y)
  - Корень степени y: pow(x, 1/y)

- Math. Statistic:
  - Ввод выборки вручную через UI
  - E[X]
  - MSE
  - RMSE
  - MAE

- Linear Algebra (3×3):
  - Determinant
  - Transpose
  - Adjugate (алгебраические дополнения)
  - Eigen-vector (метод степенных приближений)

- Geometry 2D:
  - Triangle, Square, Rectangle, Circle
  - Area and Circle length

- Geometry 3D:
  - Cube, Pyramid
  - Area и Volume 

## To compile:

```bash
clang++ -std=c++17 \
  main.cpp \
  imgui/imgui.cpp \
  imgui/imgui_draw.cpp \
  imgui/imgui_tables.cpp \
  imgui/imgui_widgets.cpp \
  imgui/backends/imgui_impl_glfw.cpp \
  imgui/backends/imgui_impl_opengl3.cpp \
  -I./imgui \
  -I./imgui/backends \
  $(pkg-config --cflags --libs glfw3) \
  -framework OpenGL \
  -framework Cocoa \
  -framework IOKit \
  -framework CoreVideo \
  -o CalculatorApp
```
