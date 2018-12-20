#include "random.h" 
#include <time.h>
int Random::globalSeed=(unsigned)time(NULL);
int Normal01::FlipFlop=0;
double Normal01::u1=0;
double Normal01::u2=0;
#define TWOPI 6.28318530718


double Prob::operator++(int)
{
    restore();
    return prob();
}

double Uniform::operator++(int)
{
    restore();
    return(lb + (ub - lb) * prob());
}

void Expon::Initialize(int s,double u)
{
   SetGlobalSeed(s);
   initstate(s, (char *)state, 128);
   mean=u; 
}
 
double Expon::operator++(int)
{
    // restore(); marked by �ا�
    return (-mean * log(prob()));
}

double Pareto::operator++(int)
{
    restore();
    return (location/(pow(1-prob(),1/shape)));
} 

Erlang :: Erlang(int s,double u,double v):Random(s),mean(u),var(v)
{
    double x,y;

    SetGlobalSeed(s);
    initstate(s, (char *)state, 128);
    if (v<=0)
      { printf("variance <= 0, Error!");
        exit(0); }
    if (u<=0)
      { printf("mean <=0, Error!");
        exit(0); }
    mean=u;
    var=v;
    x=(mean*mean)/var;
    y=ceil(x);
    if ((y-x)!=0)
       { printf("The stage (mean*mean/variance) is not an interger!\n");
         printf("  we will replace by its ceiling\n"); }
    k=(int)(y);
    a=mean/var;/*lambda*/
}

/* k is interger, a > 0 */
/* m=k/a, v=k/(a*a) */

Erlang :: Erlang(double u,double v): Random(),mean(u),var(v)
  {
    double x,y;
  
    if (v<=0)
      { printf("variance <= 0, Error!");
        exit(0); }
    if (u<=0)
      { printf("mean <=0, Error!");
        exit(0); }
    mean=u;
    var=v;
    x=(mean*mean)/var;/*alpha*/
    y=ceil(x);
    if ((y-x)!=0)
       { printf("The stage (mean*mean/variance) is not an interger!\n");
         printf("  we will replace by its ceiling\n"); }
    k=(int)(y);
    a=mean/var;

    /*k = int(u*u/v+0.5);
    k = (k > 0)? k:1;
    a =double(int(k/u));*/
  }

double Erlang :: operator++(int)
  {
    restore(); 
   // double temp1=0.0;
    double prod = 1.0;
    double tt1=0.0;
    int i,jj;
 
    /* prior code 
    for (register int i=0;i < k;i++)
      prod *= prob();

    return -log(prod)/a; */

    /*if(k<=100){ 
               for (register int i=0;i < k;i++)
                   prod *= prob();
    
               return -log(prod)/a;}
    else {
           for (register int i=0;i < 100;i++)
                   prod *= prob(); 
           tt1=log(prod);
           for (register int i=100;i < k;i++)
                   tt1=tt1+log(prob());
           return -tt1/a; 
         }*/ /* version 2*/

    /* version 3*/ 
    if(k<=100){
                for (register int i=0;i < k;i++)
                   prod *= prob();

                return -log(prod)/a;}
    else {
            jj=(int)(floor((double)k/100.0));
            for(i=0;i<jj;i++)
            {
               prod=1.0;
               for (register int j=0;j < 100;j++)
                   prod *= prob();
               tt1=tt1+log(prod);
            }
            prod=1.0;
            for (register int j=0;j < (k-100*jj);j++)
                   prod *= prob();
            tt1=tt1+log(prod);
            return -tt1/a;
          }  
} 

double Normal01::prob01(void)
{
    FlipFlop=!FlipFlop;
    if (FlipFlop)
    {
       u1=prob();
       u2=prob();
       return(sqrt(-2.0 * log(u1)) * sin(TWOPI * u2));
    }
    else
    {
       return(sqrt(-2.0 * log(u1)) * cos(TWOPI * u2));
    }
}

double Normal01::operator++(int)
{
    restore();
    return prob01();
}

double lnGamma(double x)
{
        if (x < 1.0) {
                double  z = 3.14159265359 * (1.0-x);
                return log(z/sin(z))-lnGamma(2.0-x); }
        else {
                static double cof[6]={  76.18009173, -86.50532033,
                                        24.01409822, -1.231739516,
                                        0.120858003e-2, -0.536382e-5 };

                double xx = x - 1.0, tmp = xx + 5.5, ser = 1.0;
                tmp -= (xx + 0.5) * log(tmp);
                int     j;
                for (j = 0; j <= 5; j++) {
                        xx += 1.0;
                        ser += cof[j] / xx;
                }
                return -tmp + log(2.50662827465*ser);
        }
} /* end of InGamma */

Gamma::Gamma(int s,double u,double v)
{
       if (u<=0)
            { printf("mean<=0, error! \n");  exit(0); }
        if (v<=0)
            { printf("variance=0, error! \n");  exit(0); }
        _mean  = u;
        _var = v;
        _beta = _var/_mean;
        _alpha = (_mean*_mean)/_var;
        _ln_gamma = lnGamma(_alpha) + log(_beta);
        _pa = 0;
        if (_alpha > 1.0) {
                        _pa = new double[4];
                        _pa[0] = 1.0 / sqrt(2.0*_alpha - 1.0);
                        _pa[1] = _alpha - log(4.0);
                        _pa[2] = _alpha + 1 / _pa[0];
                        _pa[3] = 1 + log(4.5);
        }
}

Gamma :: Gamma(double u,double v)
{
       if (u<=0)
            { printf("mean<=0, error! \n");  exit(0); }
        if (v<=0)
            { printf("variance=0, error! \n");  exit(0); }
        _mean  = u;
        _var = v;
        _beta = _var/_mean;
        _alpha = (_mean*_mean)/_var;
        _ln_gamma = lnGamma(_alpha) + log(_beta);
        _pa = 0;
        if (_alpha > 1.0) {
                        _pa = new double[4];
                        _pa[0] = 1.0 / sqrt(2.0*_alpha - 1.0);
                        _pa[1] = _alpha - log(4.0);
                        _pa[2] = _alpha + 1 / _pa[0];
                        _pa[3] = 1 + log(4.5);
        }
} /* end Gamma */

void Gamma::SetMean(double u)  {
     if (u<=0)
        { printf("mean<=0, error! \n");  exit(0); }
     _mean  = u;
     _beta = _var/_mean;
     _alpha = (_mean*_mean)/_var;
     _ln_gamma = lnGamma(_alpha) + log(_beta);
     _pa = 0;
     if (_alpha > 1.0) {
        _pa = new double[4];
        _pa[0] = 1.0 / sqrt(2.0*_alpha - 1.0);
        _pa[1] = _alpha - log(4.0);
        _pa[2] = _alpha + 1 / _pa[0];
        _pa[3] = 1 + log(4.5); }
} /* end of SetMean */

void Gamma::SetVar(double v) {
     if (v<=0)
        { printf("variance<=0, error! \n");  exit(0); }
     _var  = v;
     _beta = _var/_mean;
     _alpha = (_mean*_mean)/_var;
     _ln_gamma = lnGamma(_alpha) + log(_beta);
     _pa = 0;
     if (_alpha > 1.0) {
        _pa = new double[4];
        _pa[0] = 1.0 / sqrt(2.0*_alpha - 1.0);
        _pa[1] = _alpha - log(4.0);
        _pa[2] = _alpha + 1 / _pa[0];
        _pa[3] = 1 + log(4.5); }
} /* end of SerVar */

double Gamma::operator++(int)
{
        /* restore(); */
                if (_alpha == 1.0)      // equal to exponential dst.
                        { return (-_mean*log(prob())); }
                else if (_alpha > 1.0)
                        { return _getStdGammaRand1(); }
                else
                        { return _getStdGammaRand2(); }
}

double Gamma::_getStdGammaRand1()
{
        double  u1, u2, v, y, z, w, t;
        while (1) {
                /* u1 = Random::get();  u2 = Random::get(); */
                u1=prob();
                u2=prob();
                v = _pa[0] * log(u1 / (1.0 - u1));
                y = _alpha * exp(v);
                z = u1 * u1 * u2;
                w = _pa[1] + _pa[2] * v - y;
                t = w + _pa[3] - 4.5 * z;
                if (t >= 0 || w >= log(z))
                        return y*_beta;
        }
}

double Gamma::_getStdGammaRand2()
{
        static double   exp_1 = exp(1.0);
        double  b;
        b = (exp_1 + _alpha) / exp_1;

        double  u1, p, u2, y;
        while (1) {
                /* u1 = Random::get(); */
        u1=prob();
                p = b * u1;
                if (p <= 1) {
                        y = pow(p, (1/_alpha));
                        /* u2 = Random::get(); */
            u2=prob();
                        if (u2 <= exp(-y))
                                return (y*_beta);
                } else {
                        y = -log((b-p)/_alpha);
                        /* u2 = Random::get(); */
            u2=prob();
                        if (u2 <= pow(y, (_alpha-1)))
                                return (y*_beta);
                }
        }
}
double Geometric::operator++(int)
{
    restore();
    return ((int)(floor(log(prob())/log(q)))+1);
}

