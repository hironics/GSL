/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <math.h>
#include <gsl_math.h>
#include <gsl_errno.h>
#include "gsl_sf_gamma.h"
#include "gsl_sf_trig.h"
#include "gsl_sf_legendre.h"



/* Logarithm of normalization factor, Log[N(ell,lambda)].
 * N(ell,lambda) = Product[ lambda^2 + n^2, {n,0,ell} ]
 *               = |Gamma(ell + 1 + I lambda)|^2  lambda sinh(Pi lambda) / Pi
 */
static
int
legendre_H3d_lnnorm(const int ell, const double lambda, double * result)
{
  double abs_lam = fabs(lambda);

  if(abs_lam == 0.0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else {
    double lg_r;
    double lg_theta;
    double ln_sinh;
    gsl_sf_lngamma_complex_impl(ell+1.0, lambda, &lg_r, &lg_theta);
    gsl_sf_lnsinh_impl(M_PI * abs_lam, &ln_sinh);
    *result = log(abs_lam) + ln_sinh + 2.0*lg_r;
    return GSL_SUCCESS;
  }
}


/* Evaluate legendre_H3d(ell+1)/legendre_H3d(ell)
 * by continued fraction.
 */
static
int
legendre_H3d_CF1(const int ell, const double lambda, const double coth_eta, double * result)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 5000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = sqrt(lambda*lambda + (ell+1.0)*(ell+1.0));
  double b1 = (2.0*ell + 3.0) * coth_eta;
  double An = b1*Anm1 + a1*Anm2;
  double Bn = b1*Bnm1 + a1*Bnm2;
  double an, bn;
  double fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = (lambda*lambda + ((double)ell + n)*((double)ell + n));
    bn = 2.0*ell + 2.0*n + 1.0;
    An = bn*Anm1 + an*Anm2;
    Bn = bn*Bnm1 + an*Bnm2;

    if(fabs(An) > RECUR_BIG || fabs(Bn) > RECUR_BIG) {
      An /= RECUR_BIG;
      Bn /= RECUR_BIG;
      Anm1 /= RECUR_BIG;
      Bnm1 /= RECUR_BIG;
      Anm2 /= RECUR_BIG;
      Bnm2 /= RECUR_BIG;
    }

    old_fn = fn;
    fn = An/Bn;
    del = old_fn/fn;
    
    if(fabs(del - 1.0) < 10.0*GSL_MACH_EPS) break;
  }

  *result = -fn; /* note overall sign */

  if(n == maxiter)
    return GSL_EMAXITER;
  else
    return GSL_SUCCESS;
}



/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_legendre_H3d_0_impl(const double lambda, const double eta, double * result)
{
  if(eta < 0.0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else if(eta == 0.0 || lambda == 0.0) {
    *result = 1.0;
    return GSL_SUCCESS;
  }
  else {
    if(eta + log(fabs(lambda)) > -GSL_LOG_DBL_MIN) {
      *result = 0.0;
      return GSL_EUNDRFLW;
    }
    else if(eta > -0.5*GSL_LOG_MACH_EPS) {
      *result = 2.0 * sin(lambda*eta)/lambda * exp(-eta);
      return GSL_SUCCESS;
    }
    else {
      *result = sin(lambda*eta)/(lambda*sinh(eta));
      return GSL_SUCCESS;
    }
  }
}


int
gsl_sf_legendre_H3d_1_impl(const double lambda, const double eta, double * result)
{
  const double xi    = fabs(eta*lambda);
  const double lsqp1 = lambda*lambda + 1.0;

  if(eta < 0.0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else if(eta == 0.0 || lambda == 0.0) {
    *result = 0.0;
    return GSL_SUCCESS;
  }
  else if(xi < GSL_ROOT5_MACH_EPS && eta < GSL_ROOT5_MACH_EPS) {
    double etasq = eta*eta;
    double xisq  = xi*xi;
    double term1 = (etasq + xisq)/3.0;
    double term2 = -(2.0*etasq*etasq + 5.0*etasq*xisq + 3.0*xisq*xisq)/90.0;
    double sinh_term = 1.0 - eta*eta/6.0 * (1.0 - 7.0/60.0*eta*eta);
    *result = sinh_term/sqrt(lsqp1) * (term1 + term2) / eta;
    return GSL_SUCCESS;
  }
  else if(1) {
    double sin_term;     /*  sin(xi)/xi     */
    double cos_term;     /*  cos(xi)        */
    double coth_term;    /*  eta/tanh(eta)  */
    double sinh_term;    /*  eta/sinh(eta)  */
    if(xi < GSL_ROOT5_MACH_EPS) {
      sin_term = 1.0 - xi*xi/6.0 * (1.0 - xi*xi/20.0);
      cos_term = 1.0 - 0.5*xi*xi * (1.0 - xi*xi/12.0);
    }
    else {
      sin_term = sin(xi)/xi;
      cos_term = cos(xi);
    }
    if(eta < GSL_ROOT5_MACH_EPS) {
      coth_term = 1.0 + eta*eta/3.0 * (1.0 - eta*eta/15.0);
      sinh_term = 1.0 - eta*eta/6.0 * (1.0 - 7.0/60.0*eta*eta);
    }
    else {
      coth_term = eta/tanh(eta);
      sinh_term = eta/sinh(eta);
    }
    *result = sinh_term/sqrt(lsqp1) * (sin_term*coth_term - cos_term) /eta;
    return GSL_SUCCESS;
  }
  else {
    double sin_term;     /*  sin(xi)/xi            */
    double cos_term;     /*  sin(xi)/xi - cos(xi)  */
    double coth_term;    /*  coth(eta) - 1/eta     */
    double sinh_term;    /*  eta/sinh(eta)         */
    if(xi < GSL_ROOT5_MACH_EPS) {
      sin_term = 1.0 - xi*xi/6.0 * (1.0 - 0.05*xi*xi);
      cos_term = 1.0/3.0 * xi*xi * (1.0 - xi*xi/10.0*(1.0 - xi*xi/28.0));
    }
    else {
      sin_term = sin(xi)/xi;
      cos_term = sin_term - cos(xi);
    }
    if(eta < GSL_ROOT5_MACH_EPS) {
      coth_term = 1.0/3.0 * eta * (1.0 - eta*eta/15.0 * (1.0 - 2.0*eta*eta/21.0));
      sinh_term = 1.0 - eta*eta/6.0 * (1.0 - 7.0/60.0*eta*eta);
    }
    else {
      coth_term = 1.0/tanh(eta) - 1.0/eta;
      sinh_term = eta/sinh(eta);
    }
    *result = sinh_term/sqrt(lsqp1) * (coth_term * sin_term - cos_term/eta);
    return GSL_SUCCESS;
  }
}


int
gsl_sf_legendre_H3d_impl(const int ell, const double lambda, const double eta, double * result)
{
  if(eta == 0.0) {
    if(ell == 0)
      *result = 1.0;
    else
      *result = 0.0;
    return GSL_SUCCESS;
  }
  else if(ell == 0) {
    return gsl_sf_legendre_H3d_0_impl(lambda, eta, result);
  }
  else if(ell == 1) {
    return gsl_sf_legendre_H3d_1_impl(lambda, eta, result);
  }
  else {
    const double coth_eta = 1.0/tanh(eta);
    double rH;
    int stat_CF1 = legendre_H3d_CF1(ell, lambda, coth_eta, &rH);
    double Hlm1;
    double Hl    = GSL_SQRT_DBL_MIN;
    double Hlp1  = rH * Hl;
    int lp;
    for(lp=ell; lp>0; lp--) {
      double root_term_0 = sqrt(lambda*lambda + (double)lp*lp);
      double root_term_1 = sqrt(lambda*lambda + (lp+1.0)*(lp+1.0));
      Hlm1 = -((2.0*lp + 1.0)*coth_eta*Hl + root_term_1 * Hlp1)/root_term_0;
      Hlp1 = Hl;
      Hl   = Hlm1;
    }
    
    if(fabs(Hlm1) > fabs(Hl)) {
      double H0;
      int stat_H0 = gsl_sf_legendre_H3d_0_impl(lambda, eta, &H0);
      *result = GSL_SQRT_DBL_MIN/Hlm1 * H0;
      return GSL_SUCCESS;
    }
    else {
      double H1;
      int stat_H1 = gsl_sf_legendre_H3d_1_impl(lambda, eta, &H1);
      *result = rH*GSL_SQRT_DBL_MIN/Hl * H1;
      return GSL_SUCCESS;
    }
  }
}



int
gsl_sf_hyper_array_impl(int lmax, double lambda, double x, double * result, double * harvest)
{
  double X = 1./tanh(x);
  double y2, y1, y0;
  int ell;

/*
  gsl_sf_hyper_0_impl(lambda, x, &y2);
  gsl_sf_hyper_1_impl(lambda, x, &y1);
*/
  harvest[0] = y2;
  harvest[1] = y1;

  for(ell=2; ell<=lmax; ell++) {
    double a = sqrt(lambda*lambda + ell*ell);
    double b = sqrt(lambda*lambda + (ell-1)*(ell-1));
    y0 = ((2*ell-1)*X*y1 - b*y2) / a;
    y2 = y1;
    y1 = y0;
    harvest[ell] = y0;
  }

  *result = y0;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Error Handling *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_legendre_H3d_0_e(const double lambda, const double eta, double * result)
{
  int status = gsl_sf_legendre_H3d_0_impl(lambda, eta, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_legendre_H3d_0_e", status);
  }
  return status;
}

int
gsl_sf_legendre_H3d_1_e(const double lambda, const double eta, double * result)
{
  int status = gsl_sf_legendre_H3d_1_impl(lambda, eta, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_legendre_H3d_1_e", status);
  }
  return status;
}



/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double
gsl_sf_legendre_H3d_0(const double lambda, const double eta)
{
  double y;
  int status = gsl_sf_legendre_H3d_0_impl(lambda, eta, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_legendre_H3d_0", status);
  }
  return y;
}

double
gsl_sf_legendre_H3d_1(const double lambda, const double eta)
{
  double y;
  int status = gsl_sf_legendre_H3d_1_impl(lambda, eta, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_legendre_H3d_1", status);
  }
  return y;
}
