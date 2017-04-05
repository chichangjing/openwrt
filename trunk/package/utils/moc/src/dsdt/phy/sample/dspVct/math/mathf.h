#ifndef _MATH_H
#define _MATH_H

#ifndef _MCONF_H
typedef struct {
 float r;
 float i;
} cmplxf;

typedef struct {
 double r;
 double i;
} cmplx;
#endif

#ifdef mc6800
#include <bits/nan.h>
#include <bits/huge_val.h>
#include <float.h>
#endif

#if 1
/* Double precision constants */
#define M_E		2.7182818284590452354	/* e */
#define M_LOG2E		1.4426950408889634074	/* log_2 e */
#define M_LOG10E	0.43429448190325182765	/* log_10 e */
#define M_LN2		0.69314718055994530942	/* log_e 2 */
#define M_LN10		2.30258509299404568402	/* log_e 10 */
#define M_PI		3.14159265358979323846	/* pi */
#define M_PI_2		1.57079632679489661923	/* pi/2 */
#define M_PI_4		0.78539816339744830962	/* pi/4 */
#define M_1_PI		0.31830988618379067154	/* 1/pi */
#define M_2_PI		0.63661977236758134308	/* 2/pi */
#define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
/* Double precision routines */
extern double mvl_fmod(double, double);
extern double mvl_modf(double, double *);

extern double mvl_acosh ( double x );
extern int mvl_airy ( double x, double *ai, double *aip, double *bi, double *bip );
extern double mvl_asin ( double x );
extern double mvl_acos ( double x );
extern double mvl_asinh ( double xx );
extern double mvl_atan ( double x );
extern double mvl_atan2 ( double y, double x );
extern double mvl_atanh ( double x );
extern double mvl_bdtrc ( int k, int n, double p );
extern double mvl_bdtr ( int k, int n, double p );
extern double mvl_bdtri ( int k, int n, double y );
extern double mvl_beta ( double a, double b );
extern double mvl_lbeta ( double a, double b );
extern double mvl_btdtr ( double a, double b, double x );
extern double mvl_cbrt ( double x );
extern double mvl_chbevl ( double x, double array[], int n );
extern double mvl_chdtrc ( double df, double x );
extern double mvl_chdtr ( double df, double x );
extern double mvl_chdtri ( double df, double y );
extern void mvl_clog ( cmplx *z, cmplx *w );
extern void mvl_cexp ( cmplx *z, cmplx *w );
extern void mvl_csin ( cmplx *z, cmplx *w );
extern void mvl_ccos ( cmplx *z, cmplx *w );
extern void mvl_ctan ( cmplx *z, cmplx *w );
extern void mvl_ccot ( cmplx *z, cmplx *w );
extern void mvl_casin ( cmplx *z, cmplx *w );
extern void mvl_cacos ( cmplx *z, cmplx *w );
extern void mvl_catan ( cmplx *z, cmplx *w );
extern void mvl_csinh ( cmplx *z, cmplx *w );
extern void mvl_casinh ( cmplx *z, cmplx *w );
extern void mvl_ccosh ( cmplx *z, cmplx *w );
extern void mvl_cacosh ( cmplx *z, cmplx *w );
extern void mvl_ctanh ( cmplx *z, cmplx *w );
extern void mvl_catanh ( cmplx *z, cmplx *w );
extern void mvl_cpow ( cmplx *a, cmplx *z, cmplx *w );
extern void mvl_cadd ( cmplx *a, cmplx *b, cmplx *c );
extern void mvl_csub ( cmplx *a, cmplx *b, cmplx *c );
extern void mvl_cmul ( cmplx *a, cmplx *b, cmplx *c );
extern void mvl_cdiv ( cmplx *a, cmplx *b, cmplx *c );
extern void mvl_cmov ( void *a, void *b );
extern void mvl_cneg ( cmplx *a );
extern double mvl_cabs ( cmplx *z );
extern void mvl_csqrt ( cmplx *z, cmplx *w );
extern double mvl_hypot ( double x, double y );
extern double mvl_cosh ( double x );
extern double mvl_dawsn ( double xx );
extern int mvl_drand ( double *a );
extern double mvl_ei ( double x );
extern double mvl_ellie ( double phi, double m );
extern double mvl_ellik ( double phi, double m );
extern double mvl_ellpe ( double x );
extern int mvl_ellpj ( double u, double m, double *sn, double *cn, double *dn, double *ph );
extern double mvl_ellpk ( double x );
extern double mvl_exp ( double x );
extern double mvl_exp10 ( double x );
extern double mvl_exp2 ( double x );
extern double mvl_expn ( int n, double x );
extern double mvl_expx2 ( double x, int sign );
extern double mvl_fabs ( double x );
extern double mvl_fac ( int i );
extern double mvl_fdtrc ( int ia, int ib, double x );
extern double mvl_fdtr ( int ia, int ib, double x );
extern double mvl_fdtri ( int ia, int ib, double y );
extern int mvl_fresnl ( double xxa, double *ssa, double *cca );
extern double mvl_gamma ( double x );
extern double mvl_lgam ( double x );
extern double mvl_gdtr ( double a, double b, double x );
extern double mvl_gdtrc ( double a, double b, double x );
extern double mvl_hyp2f1 ( double a, double b, double c, double x );
extern double mvl_hyperg ( double a, double b, double x );
extern double mvl_hyp2f0 ( double a, double b, double x, int type, double *err );
extern double mvl_i0 ( double x );
extern double mvl_i0e ( double x );
extern double mvl_i1 ( double x );
extern double mvl_i1e ( double x );
extern double mvl_igami ( double, double );
extern double mvl_incbet ( double aa, double bb, double xx );
extern double mvl_incbi ( double aa, double bb, double yy0 );
extern double mvl_igamc ( double a, double x );
extern double mvl_igam ( double a, double x );
extern int mvl_signbit ( double x );
extern int mvl_isnan ( double x );
extern int mvl_isfinite ( double x );
extern double mvl_iv ( double v, double x );
extern double mvl_j0 ( double x );
extern double mvl_y0 ( double x );
extern double mvl_j1 ( double x );
extern double mvl_y1 ( double x );
extern double mvl_jn ( int n, double x );
extern double mvl_jv ( double n, double x );
extern double mvl_k0 ( double x );
extern double mvl_k0e ( double x );
extern double mvl_k1 ( double x );
extern double mvl_k1e ( double x );
extern double mvl_kn ( int nn, double x );
extern double mvl_smirnov ( int n, double e );
extern double mvl_kolmogorov ( double y );
extern double mvl_smirnovi ( int n, double p );
extern double mvl_kolmogi ( double p );
extern double mvl_log ( double x );
extern double mvl_log2 ( double x );
extern double mvl_log10 ( double x );
extern long lrand ( void );
extern double mvl_nbdtrc ( int k, int n, double p );
extern double mvl_nbdtr ( int k, int n, double p );
extern double mvl_nbdtri ( int k, int n, double p );
extern double mvl_ndtr ( double a );
extern double mvl_erfc ( double a );
extern double mvl_erf ( double x );
extern double mvl_ndtri ( double );
extern double mvl_pdtrc ( int k, double m );
extern double mvl_pdtr ( int k, double m );
extern double mvl_pdtri ( int k, double y );
extern double mvl_plancki ( double w, double T );
extern double mvl_planckc ( double w, double T );
extern double mvl_planckd ( double w, double T );
extern double mvl_planckw ( double T );
extern double mvl_polevl ( double x, double coef[], int N );
extern double mvl_p1evl ( double x, double coef[], int N );
extern void mvl_polatn ( double num[], double den[], double ans[], int nn );
extern void mvl_polsqt ( double pol[], double ans[], int nn );
extern void mvl_polsin ( double x[], double y[], int nn );
extern void mvl_polcos ( double x[], double y[], int nn );
extern double mvl_polylog ( int n, double x );
extern void mvl_polini ( int maxdeg );
extern void mvl_polprt ( double a[], int na, int d );
extern void mvl_polclr ( double *a, int n );
extern void mvl_polmov ( double *a, int na, double *b );
extern void mvl_polmul ( double a[], int na, double b[], int nb, double c[] );
extern void mvl_poladd ( double a[], int na, double b[], int nb, double c[] );
extern void mvl_polsub ( double a[], int na, double b[], int nb, double c[] );
extern int mvl_poldiv ( double a[], int na, double b[], int nb, double c[] );
extern void mvl_polsbt ( double a[], int na, double b[], int nb, double c[] );
extern double mvl_poleva ( double a[], int na, double x );
extern double mvl_pow ( double x, double y );
extern double mvl_powi ( double x, int nn );
extern double mvl_psi ( double x );
extern double mvl_rgamma ( double x );
extern double mvl_round ( double x );
extern int mvl_shichi ( double x, double *si, double *ci );
extern int mvl_sici ( double x, double *si, double *ci );
extern double mvl_mvl_sin ( double x );
extern double mvl_cos ( double x );
extern double mvl_radian ( double d, double m, double s );
extern double mvl_sindg ( double x );
extern double mvl_cosdg ( double x );
extern double mvl_sinh ( double x );
extern double mvl_spence ( double x );
extern double mvl_stdtr ( int k, double t );
extern double mvl_stdtri ( int k, double p );
extern double mvl_onef2 ( double a, double b, double c, double x, double *err );
extern double mvl_threef0 ( double a, double b, double c, double x, double *err );
extern double mvl_struve ( double v, double x );
extern double mvl_yv ( double v, double x );
extern double mvl_tan ( double x );
extern double mvl_cot ( double x );
extern double mvl_tandg ( double x );
extern double mvl_cotdg ( double x );
extern double mvl_tanh ( double x );
extern double mvl_log1p ( double x );
extern double mvl_expm1 ( double x );
extern double mvl_cosm1 ( double x );
extern double mvl_yn ( int n, double x );
extern double mvl_zeta ( double x, double q );
extern double mvl_zetac ( double x );
extern double mvl_sqrt ( double x );
extern double mvl_ceil ( double x );
extern double mvl_floor ( double x );
extern double mvl_frexp ( double x, int *pw2 );
extern double mvl_ldexp ( double x, int pw2 );
extern int mvl_sprec ( void );
extern int mvl_dprec ( void );
extern int mvl_ldprec ( void );
extern int mvl_mtherr ( char *name, int code );
#else
/* Single precision constants */
#define M_Ef		2.7182818284f		/* e */
#define M_LOG2Ef	1.4426950408f		/* log_2 e */
#define M_LOG10Ef	0.43429448190f		/* log_10 e */
#define M_LN2f		0.69314718055f		/* log_e 2 */
#define M_LN10f		2.3025850929f		/* log_e 10 */
#define M_PIf 		3.1415926535f		/* pi */
#define M_PI_2f		1.5707963267f		/* pi/2 */
#define M_PI_4f		0.78539816339f		/* pi/4 */
#define M_1_PIf		0.31830988618f		/* 1/pi */
#define M_2_PIf		0.63661977236f		/* 2/pi */
#define M_2_SQRTPIf	1.1283791670f		/* 2/sqrt(pi) */
#define M_SQRT2f	1.4142135623f		/* sqrt(2) */
#define M_SQRT1_2f	0.70710678118f		/* 1/sqrt(2) */

/* Single precision routines */
extern float mvl_acosf ( float x );
extern float mvl_acoshf ( float xx );
extern int mvl_airyf ( float xx, float *ai, float *aip, float *bi, float *bip );
extern float mvl_asinf ( float xx );
extern float mvl_asinhf ( float xx );
extern float mvl_atan2f ( float y, float x );
extern float mvl_atanf ( float xx );
extern float mvl_atanhf ( float xx );
extern float mvl_bdtrcf ( int k, int n, float pp );
extern float mvl_bdtrf ( int k, int n, float pp );
extern float mvl_bdtrif ( int k, int n, float yy );
extern float mvl_betaf ( float aa, float bb );
extern float mvl_cabsf ( cmplxf *z );
extern void mvl_cacosf ( cmplxf *z, cmplxf *w );
extern void mvl_caddf ( cmplxf *a, cmplxf *b, cmplxf *c );
extern void mvl_casinf ( cmplxf *z, cmplxf *w );
extern void mvl_catanf ( cmplxf *z, cmplxf *w );
extern float mvl_cbrtf ( float xx );
extern void mvl_cchshf ( float xx, float *c, float *s );
extern void mvl_ccosf ( cmplxf *z, cmplxf *w );
extern void mvl_ccotf ( cmplxf *z, cmplxf *w );
extern void mvl_cdivf ( cmplxf *a, cmplxf *b, cmplxf *c );
extern float mvl_ceilf ( float x );
extern void mvl_cexpf ( cmplxf *z, cmplxf *w );
extern float mvl_chbevlf ( float x, float *array, int n );
extern float mvl_chdtrcf ( float dff, float xx );
extern float mvl_chdtrf ( float dff, float xx );
extern float mvl_chdtrif ( float dff, float yy );
extern void mvl_clogf ( cmplxf *z, cmplxf *w );
extern void mvl_cmovf ( short *a, short *b );
extern void mvl_cmulf ( cmplxf *a, cmplxf *b, cmplxf *c );
extern void mvl_cnegf ( cmplxf *a );
extern float mvl_cosdgf ( float xx );
extern float mvl_cosf ( float xx );
extern float mvl_coshf ( float xx );
extern float mvl_cotdgf ( float x );
extern float mvl_cotf ( float x );
extern void mvl_csinf ( cmplxf *z, cmplxf *w );
extern void mvl_csqrtf ( cmplxf *z, cmplxf *w );
extern void mvl_csubf ( cmplxf *a, cmplxf *b, cmplxf *c );
extern void mvl_ctanf ( cmplxf *z, cmplxf *w );
extern float mvl_ctansf ( cmplxf *z );
extern float mvl_dawsnf ( float xxx );
extern int mvl_dprec ( void );
extern float mvl_ellief ( float phia, float ma );
extern float mvl_ellikf ( float phia, float ma );
extern float mvl_ellpef ( float xx );
extern int mvl_ellpjf ( float uu, float mm, float *sn, float *cn, float *dn, float *ph );
extern float mvl_ellpkf ( float xx );
extern float mvl_erfcf ( float aa );
extern float mvl_erff ( float xx );
extern float mvl_exp10f ( float xx );
extern float mvl_exp2f ( float xx );
extern float mvl_expf ( float xx );
extern float mvl_expnf ( int n, float xx );
extern float mvl_facf ( int i );
extern float mvl_fdtrcf ( int ia, int ib, float xx );
extern float mvl_fdtrf ( int ia, int ib, int xx );
extern float mvl_fdtrif ( int ia, int ib, float yy );
extern float mvl_floorf ( float x );
extern void mvl_fresnlf ( float xxa, float *ssa, float *cca );
extern float mvl_frexpf ( float x, int *pw2 );
extern float mvl_gammaf ( float xx );
extern float mvl_gdtrcf ( float aa, float bb, float xx );
extern float mvl_gdtrf ( float aa, float bb, float xx );
extern float mvl_hyp2f0f ( float aa, float bb, float xx, int type, float *err );
extern float mvl_hyp2f1f ( float aa, float bb, float cc, float xx );
extern float mvl_hypergf ( float aa, float bb, float xx );
extern float mvl_i0ef ( float x );
extern float mvl_i0f ( float x );
extern float mvl_i1ef ( float xx );
extern float mvl_i1f ( float xx );
extern float mvl_igamcf ( float aa, float xx );
extern float mvl_igamf ( float aa, float xx );
extern float mvl_igamif ( float aa, float yy0 );
extern float mvl_incbetf ( float aaa, float bbb, float xxx );
extern float mvl_incbif ( float aaa, float bbb, float yyy0 );
extern float mvl_incbpsf ( float aa, float bb, float xx );
extern float mvl_ivf ( float v, float x );
extern float mvl_j0f ( float xx );
extern float mvl_j1f ( float xx );
extern float mvl_jnf ( int n, float xx );
extern float mvl_jvf ( float nn, float xx );
extern float mvl_k0ef ( float xx );
extern float mvl_k0f ( float xx );
extern float mvl_k1ef ( float xx );
extern float mvl_k1f ( float xx );
extern float mvl_knf ( int nnn, float xx );
extern float mvl_ldexpf ( float x, int pw2 );
extern int mvl_ldprec ( void );
extern float mvl_lgamf ( float xx );
extern float mvl_log10f ( float xx );
extern float mvl_log2f ( float xx );
extern float mvl_logf ( float xx );
extern int mvl_mtherr ( char *name, int code );
extern float mvl_nbdtrcf ( int k, int n, float pp );
extern float mvl_nbdtrf ( int k, int n, float pp );
extern float mvl_ndtrf ( float aa );
extern float mvl_ndtrif ( float yy0 );
extern float mvl_onef2f ( float aa, float bb, float cc, float xx, float *err );
extern float mvl_p1evlf ( float xx, float *coef, int N );
extern float mvl_pdtrcf ( int k, float mm );
extern float mvl_pdtrf ( int k, float mm );
extern float mvl_pdtrif ( int k, float yy );
extern void mvl_poladdf ( float a[], int na, float b[], int nb, float c[] );
extern void mvl_polclrf ( float *a, int n );
extern int mvl_poldivf ( float a[], int na, float b[], int nb, float c[] );
extern float mvl_polevaf ( float *a, int na, float xx );
extern float mvl_polevlf ( float xx, float *coef, int N );
extern void mvl_polinif ( int maxdeg );
extern void mvl_polmovf ( float *a, int na, float *b );
extern void mvl_polmulf ( float a[], int na, float b[], int nb, float c[] );
extern void mvl_polprtf ( float *a, int na, int d );
extern void mvl_polsbtf ( float a[], int na, float b[], int nb, float c[] );
extern void mvl_polsubf ( float a[], int na, float b[], int nb, float c[] );
extern float mvl_powf ( float x, float y );
extern float mvl_powif ( float x, int nn );
extern float mvl_psif ( float xx );
extern float mvl_redupif ( float xx );
extern float mvl_rgammaf ( float xx );
extern int mvl_shichif ( float xx, float *si, float *ci );
extern int mvl_sicif ( float xx, float *si, float *ci );
extern float mvl_sindgf ( float xx );
extern float mvl_sinf ( float xx );
extern float mvl_sinhf ( float xx );
extern float mvl_spencef ( float xx );
extern int mvl_sprec ( void );
extern float mvl_sqrtf ( float xx );
extern float mvl_stdtrf ( int k, float tt );
extern float mvl_struvef ( float vv, float xx );
extern float mvl_tandgf ( float x );
extern float mvl_tanf ( float x );
extern float mvl_tanhf ( float xx );
extern float mvl_threef0f ( float aa, float bb, float cc, float xx, float *err );
extern float mvl_y0f ( float xx );
extern float mvl_y1f ( float xx );
extern float mvl_ynf ( int nn, float xx );
extern float mvl_yvf ( float vv, float xx );
extern float mvl_zetacf ( float xx );
extern float mvl_zetaf ( float xx, float qq );
#endif


#define rint(x) ((double) ((int) ((x) + 0.5)))

#endif
