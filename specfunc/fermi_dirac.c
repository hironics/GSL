/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <math.h>
#include <gsl_math.h>
#include <gsl_errno.h>
#include "gsl_sf_chebyshev.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_pow_int.h"
#include "gsl_sf_zeta.h"
#include "gsl_sf_fermi_dirac.h"


#define locMAX(a,b)   ((a) > (b) ? (a) : (b))
#define locEPS        (1000.0*GSL_MACH_EPS)


/* Chebyshev fit for F_{1}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_1_a_data[22] = {
  1.8949340668482264365,
  0.7237719066890052793,
  0.1250000000000000000,
  0.0101065196435973942,
  0.0,
 -0.0000600615242174119,
  0.0,
  6.816528764623e-7,
  0.0,
 -9.5895779195e-9,
  0.0,
  1.515104135e-10,
  0.0,
 -2.5785616e-12,
  0.0,
  4.62270e-14,
  0.0,
 -8.612e-16,
  0.0,
  1.65e-17,
  0.0,
 -3.e-19
};
static struct gsl_sf_cheb_series fd_1_a_cs = {
  fd_1_a_data,
  21,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_1_b_data[22] = {
  10.409136795234611872,
  3.899445098225161947,
  0.513510935510521222,
  0.010618736770218426,
 -0.001584468020659694,
  0.000146139297161640,
 -1.408095734499e-6,
 -2.177993899484e-6,
  3.91423660640e-7,
 -2.3860262660e-8,
 -4.138309573e-9,
  1.283965236e-9,
 -1.39695990e-10,
 -4.907743e-12,
  4.399878e-12,
 -7.17291e-13,
  2.4320e-14,
  1.4230e-14,
 -3.446e-15,
  2.93e-16,
  3.7e-17,
 -1.6e-17
};
static struct gsl_sf_cheb_series fd_1_b_cs = {
  fd_1_b_data,
  21,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_1_c_data[23] = {
  56.78099449124299762,
  21.00718468237668011,
  2.24592457063193457,
  0.00173793640425994,
 -0.00058716468739423,
  0.00016306958492437,
 -0.00003817425583020,
  7.64527252009e-6,
 -1.31348500162e-6,
  1.9000646056e-7,
 -2.141328223e-8,
  1.23906372e-9,
  2.1848049e-10,
 -1.0134282e-10,
  2.484728e-11,
 -4.73067e-12,
  7.3555e-13,
 -8.740e-14,
  4.85e-15,
  1.23e-15,
 -5.6e-16,
  1.4e-16,
 -3.e-17
};
static struct gsl_sf_cheb_series fd_1_c_cs = {
  fd_1_c_data,
  22,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1}(x) / x^2
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 * x = 10(t+2)
 */
static double fd_1_d_data[30] = {
  1.0126626021151374442,
 -0.0063312525536433793,
  0.0024837319237084326,
 -0.0008764333697726109,
  0.0002913344438921266,
 -0.0000931877907705692,
  0.0000290151342040275,
 -8.8548707259955e-6,
  2.6603474114517e-6,
 -7.891415690452e-7,
  2.315730237195e-7,
 -6.73179452963e-8,
  1.94048035606e-8,
 -5.5507129189e-9,
  1.5766090896e-9,
 -4.449310875e-10,
  1.248292745e-10,
 -3.48392894e-11,
  9.6791550e-12,
 -2.6786240e-12,
  7.388852e-13,
 -2.032828e-13,
  5.58115e-14,
 -1.52987e-14,
  4.1886e-15,
 -1.1458e-15,
  3.132e-16,
 -8.56e-17,
  2.33e-17,
 -5.9e-18
};
static struct gsl_sf_cheb_series fd_1_d_cs = {
  fd_1_d_data,
  29,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1}(x) / x^2
 * 30 < x < Inf
 * -1 < t < 1
 * t = 60/x - 1
 * x = 60/(t+1)
 */
static double fd_1_e_data[10] = {
  1.0013707783890401683,
  0.0009138522593601060,
  0.0002284630648400133,
 -1.57e-17,
 -1.27e-17,
 -9.7e-18,
 -6.9e-18,
 -4.6e-18,
 -2.9e-18,
 -1.7e-18
};
static struct gsl_sf_cheb_series fd_1_e_cs = {
  fd_1_e_data,
  9,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1/2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_half_a_data[23] = {
  1.7177138871306189157,
  0.6192579515822668460,
  0.0932802275119206269,
  0.0047094853246636182,
 -0.0004243667967864481,
 -0.0000452569787686193,
  5.2426509519168e-6,
  6.387648249080e-7,
 -8.05777004848e-8,
 -1.04290272415e-8,
  1.3769478010e-9,
  1.847190359e-10,
 -2.51061890e-11,
 -3.4497818e-12,
  4.784373e-13,
  6.68828e-14,
 -9.4147e-15,
 -1.3333e-15,
  1.898e-16,
  2.72e-17,
 -3.9e-18,
 -6.e-19,
  1.e-19
};
static struct gsl_sf_cheb_series fd_half_a_cs = {
  fd_half_a_data,
  22,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1/2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_half_b_data[20] = {
  7.651013792074984027,
  2.475545606866155737,
  0.218335982672476128,
 -0.007730591500584980,
 -0.000217443383867318,
  0.000147663980681359,
 -0.000021586361321527,
  8.07712735394e-7,
  3.28858050706e-7,
 -7.9474330632e-8,
  6.940207234e-9,
  6.75594681e-10,
 -3.10200490e-10,
  4.2677233e-11,
 -2.1696e-14,
 -1.170245e-12,
  2.34757e-13,
 -1.4139e-14,
 -3.864e-15,
  1.202e-15
};
static struct gsl_sf_cheb_series fd_half_b_cs = {
  fd_half_b_data,
  19,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1/2}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_half_c_data[23] = {
  29.584339348839816528,
  8.808344283250615592,
  0.503771641883577308,
 -0.021540694914550443,
  0.002143341709406890,
 -0.000257365680646579,
  0.000027933539372803,
 -1.678525030167e-6,
 -2.78100117693e-7,
  1.35218065147e-7,
 -3.3740425009e-8,
  6.474834942e-9,
 -1.009678978e-9,
  1.20057555e-10,
 -6.636314e-12,
 -1.710566e-12,
  7.75069e-13,
 -1.97973e-13,
  3.9414e-14,
 -6.374e-15,
  7.77e-16,
 -4.0e-17,
 -1.4e-17
};
static struct gsl_sf_cheb_series fd_half_c_cs = {
  fd_half_c_data,
  22,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{1/2}(x) / x^(3/2)
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 */
static double fd_half_d_data[30] = {
  1.5116909434145508537,
 -0.0036043405371630468,
  0.0014207743256393359,
 -0.0005045399052400260,
  0.0001690758006957347,
 -0.0000546305872688307,
  0.0000172223228484571,
 -5.3352603788706e-6,
  1.6315287543662e-6,
 -4.939021084898e-7,
  1.482515450316e-7,
 -4.41552276226e-8,
  1.30503160961e-8,
 -3.8262599802e-9,
  1.1123226976e-9,
 -3.204765534e-10,
  9.14870489e-11,
 -2.58778946e-11,
  7.2550731e-12,
 -2.0172226e-12,
  5.566891e-13,
 -1.526247e-13,
  4.16121e-14,
 -1.12933e-14,
  3.0537e-15,
 -8.234e-16,
  2.215e-16,
 -5.95e-17,
  1.59e-17,
 -4.0e-18
};
static struct gsl_sf_cheb_series fd_half_d_cs = {
  fd_half_d_data,
  29,
  -1, 1,
  (double *)0,
  (double *)0
};



/* Chebyshev fit for F_{3/2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_3half_a_data[20] = {
  2.0404775940601704976,
  0.8122168298093491444,
  0.1536371165644008069,
  0.0156174323847845125,
  0.0005943427879290297,
 -0.0000429609447738365,
 -3.8246452994606e-6,
  3.802306180287e-7,
  4.05746157593e-8,
 -4.5530360159e-9,
 -5.306873139e-10,
  6.37297268e-11,
  7.8403674e-12,
 -9.840241e-13,
 -1.255952e-13,
  1.62617e-14,
  2.1318e-15,
 -2.825e-16,
 -3.78e-17,
  5.1e-18
};
static struct gsl_sf_cheb_series fd_3half_a_cs = {
  fd_3half_a_data,
  19,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{3/2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_3half_b_data[22] = {
  13.403206654624176674,
  5.574508357051880924,
  0.931228574387527769,
  0.054638356514085862,
 -0.001477172902737439,
 -0.000029378553381869,
  0.000018357033493246,
 -2.348059218454e-6,
  8.3173787440e-8,
  2.6826486956e-8,
 -6.011244398e-9,
  4.94345981e-10,
  3.9557340e-11,
 -1.7894930e-11,
  2.348972e-12,
 -1.2823e-14,
 -5.4192e-14,
  1.0527e-14,
 -6.39e-16,
 -1.47e-16,
  4.5e-17,
 -5.e-18
};
static struct gsl_sf_cheb_series fd_3half_b_cs = {
  fd_3half_b_data,
  21,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{3/2}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_3half_c_data[21] = {
  101.03685253378877642,
  43.62085156043435883,
  6.62241373362387453,
  0.25081415008708521,
 -0.00798124846271395,
  0.00063462245101023,
 -0.00006392178890410,
  6.04535131939e-6,
 -3.4007683037e-7,
 -4.072661545e-8,
  1.931148453e-8,
 -4.46328355e-9,
  7.9434717e-10,
 -1.1573569e-10,
  1.304658e-11,
 -7.4114e-13,
 -1.4181e-13,
  6.491e-14,
 -1.597e-14,
  3.05e-15,
 -4.8e-16
};
static struct gsl_sf_cheb_series fd_3half_c_cs = {
  fd_3half_c_data,
  20,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Chebyshev fit for F_{3/2}(x) / x^(5/2)
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 */
static double fd_3half_d_data[25] = {
  0.6160645215171852381,
 -0.0071239478492671463,
  0.0027906866139659846,
 -0.0009829521424317718,
  0.0003260229808519545,
 -0.0001040160912910890,
  0.0000322931223232439,
 -9.8243506588102e-6,
  2.9420132351277e-6,
 -8.699154670418e-7,
  2.545460071999e-7,
 -7.38305056331e-8,
  2.12545670310e-8,
 -6.0796532462e-9,
  1.7294556741e-9,
 -4.896540687e-10,
  1.380786037e-10,
 -3.88057305e-11,
  1.08753212e-11,
 -3.0407308e-12,
  8.485626e-13,
 -2.364275e-13,
  6.57636e-14,
 -1.81807e-14,
  4.6884e-15
};
static struct gsl_sf_cheb_series fd_3half_d_cs = {
  fd_3half_d_data,
  24,
  -1, 1,
  (double *)0,
  (double *)0
};


/* Goano's modification of the Levin-u implementation from TOMS-602.
 */
static
int
fd_whiz(const double term, const int iterm,
        double * qnum, double * qden,
        double * result, double * s)
{
  if(iterm == 0) *s = 0.0;

  *s += term;

  qden[iterm] = 1.0/(term*(iterm+1.0)*(iterm+1.0));
  qnum[iterm] = *s * qden[iterm];

  if(iterm > 0) {
    double factor = 1.0;
    double ratio  = iterm/(iterm+1.0);
    int j;
    for(j=iterm-1; j>=0; j--) {
      double c = factor * (j+1.0) / (iterm+1.0);
      factor *= ratio;
      qden[j] = qden[j+1] - c * qden[j];
      qnum[j] = qnum[j+1] - c * qnum[j];
    }
  }

  *result = qnum[0] / qden[0];
  return GSL_SUCCESS;
}


/* Handle case of integer j <= -2.
 */
static
int
fd_nint(const int j, const double x, double * result)
{
  const int nmax = 100;
  double qcoeff[nmax+1];

  if(j >= -1) {
    *result = 0.0;
    return GSL_ESANITY;
  }
  else if(j < -(nmax+1)) {
    *result = 0.0;
    return GSL_EUNIMPL;
  }
  else {
    double a, p, f;
    int i, k;
    int n = -(j+1);

    qcoeff[1] = 1.0;

    for(k=2; k<=n; k++) {
      qcoeff[k] = -qcoeff[k-1];
      for(i=k-1; i>=2; i--) {
        qcoeff[i] = i*qcoeff[i] - (k-(i-1))*qcoeff[i-1];
      }
    }

    if(x >= 0.0) {
      a = exp(-x);
      f = qcoeff[1];
      for(i=2; i<=n; i++) {
        f = f*a + qcoeff[i];
      }
    }
    else {
      a = exp(x);
      f = qcoeff[n];
      for(i=n-1; i>=1; i--) {
        f = f*a + qcoeff[i];
      }
    }

    p = gsl_sf_pow_int(1.0+a, j);
    *result = f*a*p;
    return GSL_SUCCESS;
  }
}


/* x < 0
 */
static
int
fd_neg(const double j, const double x, double * result)
{
  const int itmax = 100;
  double qnum[itmax+1], qden[itmax+1];

  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_SUCCESS;
  }
  else if(x < -1.0 && x < -fabs(j+1.0)) {
    /* Simple series implementation. Avoid the
     * complexity and extra work of the series
     * acceleration method below.
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100; n++) {
      double rat = (n-1.0)/n;
      double p   = pow(rat, j+1.0);
      term *= -ex * p;
      sum  += term;
      if(fabs(term/sum) < GSL_MACH_EPS) break;
    }
    *result = sum;
    return GSL_SUCCESS;
  }
  else {
    double s;
    double xn = x;
    double ex  = -exp(x);
    double enx = -ex;
    double f = 0.0;
    int jterm;
    for(jterm=0; jterm<=itmax; jterm++) {
      double p = pow(jterm+1.0, j+1.0);
      double f_previous = f;
      double term = enx/p;
      fd_whiz(term, jterm, qnum, qden, &f, &s);
      xn += x;
      if(fabs(f-f_previous) < fabs(f)*10.0*GSL_MACH_EPS || xn < GSL_LOG_DBL_MIN) break;
      enx *= ex;
    }

    *result = f;

    if(jterm == itmax)
      return GSL_EMAXITER;
    else
      return GSL_SUCCESS;
  }
}


/* asymptotic expansion
 * j + 2.0 > 0.0
 */
static
int
fd_asymp(const double j, const double x, double * result)
{
  const int j_integer = ( fabs(j - floor(j+0.5)) < 100.0*GSL_MACH_EPS );
  const int itmax = 200;
  double lg;
  int stat_lg = gsl_sf_lngamma_impl(j + 2.0, &lg);
  double seqn = 0.5;
  double xm2  = (1.0/x)/x;
  double xgam = 1.0;
  double add = DBL_MAX;
  double fneg;
  double exp_arg;
  int stat_fneg;
  int stat_ser;
  int stat_e;
  int n;
  for(n=1; n<=itmax; n++) {
    double add_previous = add;
    double eta;
    gsl_sf_eta_int_impl(2*n, &eta);
    xgam = xgam * xm2 * (j + 1.0 - (2*n-2)) * (j + 1.0 - (2*n-1));
    add  = eta * xgam;
    if(!j_integer && fabs(add) > fabs(add_previous)) break;
    if( j_integer && 2*n > j) break;
    if(fabs(add/seqn) < GSL_MACH_EPS) break;
    seqn += add;
  }
  stat_ser = ( fabs(add) > locEPS*fabs(seqn) ? GSL_ELOSS : GSL_SUCCESS );

  stat_fneg = fd_neg(j, -x, &fneg);
  stat_e    = gsl_sf_exp_impl((j+1.0)*log(x) - lg, &exp_arg);
  *result = cos(j*M_PI) * fneg + 2.0 * seqn * exp_arg;
  return GSL_ERROR_SELECT_4(stat_e, stat_ser, stat_fneg, stat_lg);
}


/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

/* [Goano, TOMS-745, (4)] */
int gsl_sf_fermi_dirac_m1_impl(const double x, double * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else if( x < 0.0) {
    double ex = exp(x);
    *result = ex/(1.0+ex);
    return GSL_SUCCESS;
  }
  else {
    *result = 1.0/(1.0 + exp(-x));
    return GSL_SUCCESS;
  }
}


/* [Goano, TOMS-745, (3)] */
int gsl_sf_fermi_dirac_0_impl(const double x, double * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else if(x < -5.0) {
    double ex  = exp(x);
    double ser = 1.0 - ex*(0.5 - ex*(1.0/3.0 - ex*(1.0/4.0 - ex*(1.0/5.0 - ex/6.0))));
    *result = ex * ser;
    return GSL_SUCCESS;
  }
  else if(x < 10.0) {
    *result = log(1.0 + exp(x));
    return GSL_SUCCESS;
  }
  else {
    double ex = exp(-x);
    *result = x + ex * (1.0 - 0.5*ex + ex*ex/3.0 - ex*ex*ex/4.0);
    return GSL_SUCCESS;
  }
}


int gsl_sf_fermi_dirac_1_impl(const double x, double * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * rat;
      sum  += term;
      if(fabs(term/sum) < GSL_MACH_EPS) break;
    }
    *result = sum;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    *result = gsl_sf_cheb_eval(&fd_1_a_cs, x);
    return GSL_SUCCESS;
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_1_b_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_1_c_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 30.0) {
    double t = 0.1*x - 2.0;
    double c = gsl_sf_cheb_eval(&fd_1_d_cs, t);
    *result  = c * x*x;
    return GSL_SUCCESS;
  }
  else if(x < 1.0/GSL_SQRT_MACH_EPS) {
    double t = 60.0/x - 1.0;
    double c = gsl_sf_cheb_eval(&fd_1_e_cs, t);
    *result  = c * x*x;
    return GSL_SUCCESS;
  }
  else if(x < GSL_SQRT_DBL_MAX) {
    *result = 0.5 * x*x;
    return GSL_SUCCESS;
  }
  else {
    *result = 0.0;
    return GSL_EOVRFLW;
  }
}


int gsl_sf_fermi_dirac_int_impl(const int j, const double x, double * result)
{
  if(j == 0) {
    return gsl_sf_fermi_dirac_0_impl(x, result);
  }
  else if(j == -1) {
    return gsl_sf_fermi_dirac_m1_impl(x, result);
  }
  else if(j < 0) {
    return fd_nint(j, x, result);
  }
  else if(x <= 0.0) {
    return fd_neg(j, x, result);
  }
  else {
    double k_div = -log10(10.0*GSL_MACH_EPS);
    double a1    = 2.0*k_div - j*(2.0+k_div/10.0);
    double a2    = sqrt(fabs((2.0*k_div - 1.0 - j)*(2.0*k_div - j)));
    double a     = locMAX(a1, a2);
    double xasymp = locMAX(j-1.0, a);
    double fasymp;
    int stat_asymp = fd_asymp(j, x, &fasymp);
  }
}


int gsl_sf_fermi_dirac_half_impl(const double x, double * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * sqrt(rat);
      sum  += term;
      if(fabs(term/sum) < GSL_MACH_EPS) break;
    }
    *result = sum;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    *result = gsl_sf_cheb_eval(&fd_half_a_cs, x);
    return GSL_SUCCESS;
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_half_b_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_half_c_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 30.0) {
    double t = 0.1*x - 2.0;
    double c = gsl_sf_cheb_eval(&fd_half_d_cs, t);
    *result  = c * x*sqrt(x);
    return GSL_SUCCESS;
  }
  else {
    return fd_asymp(0.5, x, result);
  }
}


int gsl_sf_fermi_dirac_3half_impl(const double x, double * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * rat * sqrt(rat);
      sum  += term;
      if(fabs(term/sum) < GSL_MACH_EPS) break;
    }
    *result = sum;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    *result = gsl_sf_cheb_eval(&fd_3half_a_cs, x);
    return GSL_SUCCESS;
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_3half_b_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    *result  = gsl_sf_cheb_eval(&fd_3half_c_cs, t);
    return GSL_SUCCESS;
  }
  else if(x < 30.0) {
    double t = 0.1*x - 2.0;
    double c = gsl_sf_cheb_eval(&fd_3half_d_cs, t);
    *result  = c * x*x*sqrt(x);
    return GSL_SUCCESS;
  }
  else {
    return fd_asymp(1.5, x, result);
  }
}

/* [Goano, TOMS-745, p. 222] */
int gsl_sf_fermi_dirac_inc_0_impl(const double x, const double b, double * result)
{
  if(b < 0.0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else {
    double arg = b - x;
    double f0;
    int status = gsl_sf_fermi_dirac_0_impl(arg, &f0);
    *result = f0 - arg;
    return status;
  }
}



/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Error Handling *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_fermi_dirac_m1_e(const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_m1_impl(x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_m1_e", status);
  }
  return status;
}


int gsl_sf_fermi_dirac_0_e(const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_0_impl(x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_0_e", status);
  }
  return status;
}


int gsl_sf_fermi_dirac_1_e(const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_1_impl(x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_1_e", status);
  }
  return status;
}


int gsl_sf_fermi_dirac_int_e(const int j, const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_int_impl(j, x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_int_e", status);
  }
  return status;
}


int gsl_sf_fermi_dirac_half_e(const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_half_impl(x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_half_e", status);
  }
  return status;
}


int gsl_sf_fermi_dirac_3half_e(const double x, double * result)
{
  int status = gsl_sf_fermi_dirac_3half_impl(x, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_3half_e", status);
  }
  return status;
}


int 
gsl_sf_fermi_dirac_inc_0_e(const double x, const double b, double * result)
{
  int status = gsl_sf_fermi_dirac_inc_0_impl(x, b, result);
  if(status != GSL_SUCCESS){
    GSL_ERROR("gsl_sf_fermi_dirac_inc_0_e", status);
  }
  return status;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_fermi_dirac_m1(const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_m1_impl(x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_m1", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_0(const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_0_impl(x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_0", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_1(const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_1_impl(x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_1", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_int(const int j, const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_int_impl(j, x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_int", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_half(const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_half_impl(x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_half", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_3half(const double x)
{
  double y;
  int status = gsl_sf_fermi_dirac_3half_impl(x, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_3half", status);
  }
  return y;
}


double gsl_sf_fermi_dirac_inc_0(const double x, const double b)
{
  double y;
  int status = gsl_sf_fermi_dirac_inc_0_impl(x, b, &y);
  if(status != GSL_SUCCESS){
    GSL_WARNING("gsl_sf_fermi_dirac_inc_0", status);
  }
  return y;
}
