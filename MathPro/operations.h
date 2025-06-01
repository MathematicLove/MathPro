#pragma once
#include <cmath>
#include <list>

using namespace std;

class BaseOperations{
public:
    double add(double x, double y){
        return x + y;
    }
    
    double mult(double x, double y){
        return x * y;
    }
    
    double div(double x, double y){
        return x/y;
    }
    
    double minus(double x, double y){
        return x-y;
    }
};

class ProOperations{
public:
    double inDeg(double x, double y){
        return pow(x, y);
    }
    
    double inSqrt(double x){
        return sqrt(x);
    }
    
    double specSqrt(double x, double y){
        return pow(x, (1/y));
    }

};

class Trigonometry{
public:
    double sinus(double x){
        return sin(x);
    }
    
    double cosinus(double x){
        return cos(x);
    }
    
    double tangens(double x){
        return tan(x);
    }
    
    double cotangens(double x){
        return cos(x)/sin(x);
    }
};

    class Circle{
    public:
        double area(double x){
            return M_PI*pow(x,2);
        }
        
        double length(double x){
            return (2*M_PI*x);
        }
    };
    class Triangle{
    public:
        double area(double x, double y){
            return 1/2*x*y;
        }
    };
    class Square{
    public:
        double area(double x){
            return pow(x,2);
        }
        };
    class Rectangle{
    public:
        double area(double x, double y){
            return x*y;
        }
    };

class MatStat{
    double sum;
public:
    double viborka(list<double> data){
        for(double i: data){
            sum += i++;
        }
        return sum/data.size();
    }
    
    double MSE(list<double> data){
        double powValue = 0;
        double dataSize = data.size();
        for(double i: data){
            sum += i++;
        }
        double avg = sum/dataSize;
        for(double j: data){
            powValue = pow(j-avg, 2);
        }
        return (1/dataSize)*powValue;
    }
    
    double RMSE(list<double> data){
        double powValue = 0;
        double dataSize = data.size();
        for(double i: data){
            sum += i++;
        }
        double avg = sum/dataSize;
        for(double j: data){
            powValue = pow(j-avg, 2);
        }
        return sqrt((1/dataSize)*powValue);
    }
    
    double MAE(list<double> data){
        double powValue = 0;
        double dataSize = data.size();
        for(double i: data){
            sum += i++;
        }
        double avg = sum/dataSize;
        for(double j: data){
            powValue = pow(j-avg, 2);
        }
        return abs((1/dataSize)*powValue);
    }
};

class LinearAlgebra{
public:
    double matrix[3][3][3] = {{1,2,3},
                             {4,5,6},
                             {7,8,9}};
    double determinant(double Matrix[3][3][3]){
        //to do
        return 0;
    }
    
    double transponated(double Matrix[3][3][3]){
        //to do
        return 0;
    }
    
    double inDeg(double Matrix[3][3][3]){
        //to do
        return 0;
    }
};
