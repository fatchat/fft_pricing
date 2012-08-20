#include <cmath>
#include <algorithm>
#include <vector>

#include "pricing/bermudanpricing.h"
#include "csutils/globals.h"

double BermudanPricing::callpayoff(double y)
{ return std::max(0.0, exp(y) - 1.0); }
  
double BermudanPricing::putpayoff(double y)
{ return std::max(0.0, 1.0 - exp(y)); }

CMP BermudanPricing:charfunction(double u, double r,
				 double sigma, double time, int M)
{
  const double delta = time / M;
  const double a = r - 0.5 * sigma*sigma;
  const CMP factor = delta * (c_I * a * u
			      - 0.5 * u*u * sigma*sigma);
  return exp(factor);
}
   
std::vector<double>
BermudanPricing:bermudanpricing(double S, double K, double r, double sigma,
				double time, double a, int M, int N)
{
  const double delta = time/M;
  const double L = 10.;
  // this enters the picture when using the FFT, L=10 should work.
  std::vector<double> csi;
  const double deltacsi = 2 * PI / L;
  for(int j = 0; j < csi.size(); ++j) {
    csi[j] = (j - 0.5 * N) * deltacsi;
  }

  std::vector<double> x;
  const double deltax = L / N;
  for(int j = 0; j < x.size(); ++j) {
    x[j] = (j - 0.5 * N) * deltax;
  }

  std::vector<double> V(N);
  for(int j = 0; j < V.size(); ++j) {
    V[j] = callpayoff(x[j]);  // putpayoff for a bermudan put
  }
  
  for(int i = M; i > 0; i--){
    
    std::vector<double> v(N);
    for(int j = 0; j < v.size(); ++j) {
      v[j] = exp(a * x[j]) * V[j];
      // pick a <-1 for calls, a > 0 for puts
      // we damp the function to make it integrable
    }

    std::vector<double> vmod(N);
    vmod[0] = 0.5 * v[0];
    vmod[N-1] = pow(-1.0,N-1) * 0.5 * v[N-1];
    for(int j = 1; j < vmod.size() - 1; ++j){
      vmod[j] = pow(-1.0,j) * v[j];
    }

    std::vector<CMP> F1 = FFTW_BACKWARD(vmod);
    // in the terminology of the Fastest Fourier transform in the West

    std::vector<CMP> F2;
    for(int j = 0; j < F2.size(); ++j) {
      F2[j] = F1[j] * charfunction(-csi[j] + c_I*a);
    }
    std::vector<CMP> F3 = FFTW_FORWARD(F2)/N;//every entry is divided by N
    std::vector<CMP> F4;
    for(int j = 0; j < F4.size(); ++j) {
      F4[j] = (exp(-r * delta) * exp(-a * x[j]) * pow(-1.0,j) * F3[j]).real();
      //it should be real stuff, the imaginary part should be quite small
    }                                                            
    std::vector<double> V(N);//the vector V changes at every step in the loop
    for(int j = 0; j < V.size(); ++j){
      V[j]= max(F4[j], callpayoff(x[j]));
    }
  }
 
  std::vector<double> v(N);
  //the case i=0 is different as the function returns F4
  for(int j = 0; j < v.size(); ++j){
    v[j] = exp(a * x[j]) * V[j];
  }

  std::vector<double> vmod(N);
  vmod[0]= 0.5 * v[0];
  vmod[N-1] = pow(-1.0,N-1) * 0.5 * v[N-1];
  for(int j=1;j<vmod.size()-1;++j){
    vmod[j]=pow(-1.0,j)*v[j];
  };
  
  std::vector<CMP> F1 = FFTW_BACKWARD(vmod);
  std::vector<CMP> F2;
  for(int j=0;j<F2.size();++j){
    F2[j]=F1[j]*charfunction(-csi[j]+ c_I*a);
  };
  std::vector<CMP> F3 = FFTW_FORWARD(F2)/N;//every entry is divided by N
  std::vector<CMP> F4;
  for(int j=0;j<F4.size();++j){
    F4[j]=(exp(-r * delta) * exp(-a * x[j])* pow(-1.0,j)*F3[j]).real();
  }     //here is the only difference with the loop


  return K*F4;//every element is multiplied by K; note F4[j]=F4(x[j]) where x[j]=log(S)-log(K)
}; //so K is chosen before hand, x can take any value x[j] whilst S=K*exp(x[j]) (i.e. we do not input S into the
//program). The option value will then be K*F4 for the appropriate choice of x[j]. Note that x[N/2]=0, so the case
//S=K is always covered.  
