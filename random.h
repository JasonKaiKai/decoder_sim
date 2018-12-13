#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
using namespace std;
#define INF 2147483648.0
    
/*----------------------------*/
/* random number generators   */
/*----------------------------*/
    
class Random {
  public: 
    Random(int seed) {initstate((unsigned int)seed, (char *)state, 128);}
    Random() {initstate((unsigned int)globalSeed++, (char *)state, 128);}
    static void SetGlobalSeed(int seed) {globalSeed = seed;}
    static int GetGlobalSeed() {return globalSeed;}
    virtual double operator++(int) = 0;
  protected:
    void restore() {setstate((char *)state);}
    double prob() {return random() / 2147483648.0;}
    long state[32];
  private:
    static int globalSeed;
};
    
class Prob: public Random {
/* return a uniform random number in [0,1] */
  public:
    double operator++(int);
    Prob(int s): Random(s) {}
    Prob(): Random() {}   
};
class Expon: public Random {
  public:
    double operator++(int);  
    double GetMean() {return mean;}
    void SetMean(double u) {mean =u;}
    Expon():Random() {}/* modify line */
    Expon(int s, double u):Random(s), mean(u) {}
    Expon(double u): Random(), mean(u) {}
    void Initialize(int,double);
  private:
    double mean;
};

class Pareto: public Random {
  public:
    double operator++(int);
    double GetLocation() {return location;}
    void SetLocation(double l) {location =l;}
    double GetShape() {return shape;}
    void SetShape(double s) {shape =s;}
    Pareto():Random() {}/* modify line */
    Pareto(int s, double l, double sh):Random(s), location(l), shape(sh) {}
    Pareto(double l, double sh): Random(), location(l), shape(sh) {}
  private:
    double location,shape;
};

class Erlang: public Random
{
  double x,y;
  public:
    Erlang(int s,double u,double v);  /*constructor1 with seed*/
    Erlang(double u,double v);   /*constructor2 without seed*/
    Erlang(){mean=1;var=1;k=1;a=1;}
    double operator++(int);  
    void SetMean(double u){ 
        if (u<=0)
           { printf("mean <=0, Error!");
             exit(0); }
        mean=u;
        x=(mean*mean)/var;
        y=ceil(x);
        if ((y-x)!=0)
           { printf("The stage (mean*mean/variance) is not an interger!\n");
             printf("  we will replace by its ceiling\n"); }
        k=(int)(y);
        a=mean/var;}
    void SetVar(double v){ 
        if (v<=0)
           { printf("variance <= 0, Error!");
             exit(0); }
        var=v;
        x=(mean*mean)/var;
        y=ceil(x);
        if ((y-x)!=0)
          { printf("The stage (mean*mean/variance) is not an interger!\n");
            printf("  we will replace by its ceiling.\n"); }
        k=(int)(y);
        a=mean/var;}
    double GetMean(){return mean;}
    double GetVar(){return var;}
    int GetStage(){return k;}
    double Geta(){return a;}
 
  private:
    double mean;
    double var;
    double a;
    int k;
};

class Uniform: public Random {
  public:
      double operator++(int);        /* return uniform random number [a,b] */
      double GetLB() {return lb;}
      double GetUB() {return ub;}
      void SetLB(double a) {lb = a;}
      void SetUB(double b) {ub = b;}
      Uniform(int s, double a, double b): Random(s), lb(a), ub(b) 
      {SetGlobalSeed(s);
       initstate(s, (char *)state, 128);}
      Uniform(double a, double b): Random(), lb(a), ub(b) {}
      Uniform(int s):Random(s){lb=0;ub=1;}
    private:
      double lb;
      double ub;
};

class Normal01: public Random {
  public: 
    double operator++(int);   /* return normal random number with mean */
    double GetMean() {return mean;}
    double GetStd() {return std;}
    Normal01(int s): Random(s),mean(0),std(1) {}
    Normal01(): Random(),mean(0),std(1) {}
  protected:

    double prob01();

  private:
    double mean;
    double std;
    static int FlipFlop;
    static double u1,u2;
};
class Gamma: public Random {
        double  _ln_gamma;
        double  *_pa;
public:
    Gamma(int s, double u, double v);
    Gamma(double u, double v);
    virtual ~Gamma() { delete[] _pa;}
    double GetMean() {return _mean;}
    double GetVar()  {return _var;}
    double GetShape() {return _alpha;}
    double GetScale() {return _beta;}
    void SetMean(double u);
    void SetVar(double v);
    double operator++(int);
protected:
        virtual double _getStdGammaRand1();     // for _alpha > 1.0
        virtual double _getStdGammaRand2();     // for _alpha < 1.0
        double  _alpha; // shape parameter
        double  _beta;  // scale paremeter
        double  _mean;  // mean
        double  _var;   // variance
};

class Geometric: public Random {
public:
/* the generated random number is from 1, Ross p.48 */
    /*Geometric(int s, double pp) {p=pp;q=1.0-p;mean=1/p;var=q/(p*p);}*/
    Geometric(double pp) {p=pp;q=1.0-p;mean=1/p;var=q/(p*p);}
    Geometric(int s, double a): Random(s), p(a),q(1.0-a)
      {SetGlobalSeed(s);mean=1/p;var=q/(p*p);
       initstate(s, (char *)state, 128);}
    /*Geometric(double a): Random(), p(a),q(1.0-a) {mean=1/p;var=q/(p*p);}*/
    double GetMean() {return mean;}
    double GetVar()  {return var;}
    void SetMean(double u) {mean=u;}
    void SetVar(double v) {var=v;}
    void SetP(double pp) {p=pp;q=1.0-p;mean=1/p;var=q/(p*p);}
    double operator++(int);
private:
    double mean;
    double var;
    double p,q;
};

