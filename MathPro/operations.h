#pragma once
#include <cmath>
#include <vector>
#include <array>
#include <numeric>
#include <cstdlib>

/* ─────────────────── 2-D & 3-D GEOMETRY ────────────────────── */

class Pyramid {
public:
    static double area(double Sbase, double Hside, int Nfaces = 3) {
        return Nfaces * Hside + Sbase;               // ∑ боковых + осн.
    }
    static double volume(double Sbase, double h) {
        return Sbase * h / 3.0;                      // V = S·h / 3
    }
};

class Cube {
public:
    static double area(double a)   { return 6.0 * a * a; }
    static double volume(double a) { return a * a * a;   }
};

class Sphere {
public:
    static double area(double r)   { return 4.0 * M_PI * r * r; }
    static double volume(double r) { return 4.0 / 3.0 * M_PI * r * r * r; }
};

/* ───────────────────── BASIC ARITHMETIC ─────────────────────── */

struct BaseCalc {
    static double add(double x, double y) { return x + y; }
    static double sub(double x, double y) { return x - y; }
    static double mul(double x, double y) { return x * y; }
    static double div(double x, double y) { return y == 0 ? 0 : x / y; }
};

struct AdvCalc {
    static double power(double x, double y) { return std::pow(x, y); }
    static double sqrt(double x)            { return std::sqrt(x);   }
    static double root(double x, double n)  { return std::pow(x, 1.0 / n); }
};

/* ───────────────────── TRIGONOMETRY ─────────────────────────── */

struct Trig {
    static double sin(double x) { return std::sin(x); }
    static double cos(double x) { return std::cos(x); }
    static double tan(double x) { return std::tan(x); }
    static double ctg(double x) { return std::cos(x) / std::sin(x); }
};

/* ───────────────────  PLANE FIGURES (2-D) ───────────────────── */

struct Circle    { static double area(double r){ return M_PI*r*r;      }
                   static double length(double r){ return 2*M_PI*r;  } };
struct Square    { static double area(double a){ return a*a;          } };
struct Rectangle { static double area(double a,double b){ return a*b; } };
struct Triangle  { static double area(double b,double h){ return 0.5*b*h; } };

/* ───────────────── DESCRIPTIVE STATISTICS ───────────────────── */

struct MatStat {
    static double mean(const std::vector<double>& v) {
        if (v.empty()) return 0;
        return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    }
    static double mse (const std::vector<double>& v){
        if (v.empty()) return 0;
        double m = mean(v), s = 0;
        for (double x: v) s += (x - m)*(x - m);
        return s / v.size();
    }
    static double rmse(const std::vector<double>& v){ return std::sqrt(mse(v)); }
    static double mae (const std::vector<double>& v){
        if (v.empty()) return 0;
        double m = mean(v), s = 0;
        for (double x: v) s += std::abs(x - m);
        return s / v.size();
    }
};

/* ───────────────  LINEAR ALGEBRA (dynamic)  ─────────────────── */

class LinAlg{
public:
    using Matrix = std::vector<std::vector<double>>;

    static Matrix make(int r,int c){ return Matrix(r,std::vector<double>(c,0.0)); }

    static Matrix transpose(const Matrix& A){
        int r=A.size(), c=A[0].size(); Matrix T=make(c,r);
        for(int i=0;i<r;++i) for(int j=0;j<c;++j) T[j][i]=A[i][j]; return T;
    }

    static double determinant(const Matrix& A){
        int n=A.size(); if(n!=int(A[0].size())) return 0;
        if(n==1) return A[0][0];
        if(n==2) return A[0][0]*A[1][1]-A[0][1]*A[1][0];
        double det=0;
        for(int col=0;col<n;++col){
            Matrix sub=make(n-1,n-1);
            for(int i=1;i<n;++i){ int sj=0;
                for(int j=0;j<n;++j){ if(j==col) continue; sub[i-1][sj++]=A[i][j]; }}
            det += ((col%2)?-1:1)*A[0][col]*determinant(sub);
        }
        return det;
    }

    static Matrix cofactor(const Matrix& A){
        int n=A.size(); Matrix C=make(n,n);
        for(int i=0;i<n;++i) for(int j=0;j<n;++j){
            Matrix sub=make(n-1,n-1); int ri=0;
            for(int r=0;r<n;++r){ if(r==i) continue; int cj=0;
                for(int c=0;c<n;++c){ if(c==j) continue; sub[ri][cj++]=A[r][c]; } ++ri; }
            C[i][j]=(((i+j)%2)?-1:1)*determinant(sub);
        }
        return C;
    }

    static Matrix adjugate(const Matrix& A){ return transpose(cofactor(A)); }   // ← ДОБАВЛЕНО

    static std::vector<double> eigenVector(const Matrix& A,int iters=60){
        int n=A.size(); std::vector<double> v(n,1.0);
        for(int k=0;k<iters;++k){
            std::vector<double> v2(n,0.0);
            for(int i=0;i<n;++i) for(int j=0;j<n;++j) v2[i]+=A[i][j]*v[j];
            double norm=0; for(double x:v2) norm+=x*x; norm=std::sqrt(norm);
            for(int i=0;i<n;++i) v[i]=v2[i]/norm;
        }
        return v;
    }
};
