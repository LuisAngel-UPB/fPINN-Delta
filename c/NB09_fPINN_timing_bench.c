/* NB09_fPINN_timing_bench.c - repeated timing of the fixed-cost kernels: loop timing (R updates per repetition, NREP repetitions),
   loop overhead (same input handling, no kernel), and per-batch latency distribution (batches of B updates).
   Build: gcc -O2 -std=gnu99 NB09_fPINN_timing_bench.c -o bench -lm ; Run: ./bench [R NREP] (default 1e6 31) */
#include "NB07_fPINN_memkernels_lib.h"
#include <string.h>
static long R = 1000000L; static int NREP = 31;
#define B 100
#define NB 20000
static int cmp(const void *x, const void *y){ double a=*(const double*)x, b=*(const double*)y; return (a>b)-(a<b); }
static double qs_(double *v, int n, double p){ qsort(v, n, sizeof(double), cmp); double r=p*(n-1); int i=(int)r; return i+1<n ? v[i]+(r-i)*(v[i+1]-v[i]) : v[n-1]; }
static double *U; static volatile double vsink;
typedef void (*upd)(void*, const double*, double*);
static void none_upd(void *s, const double *q, double *D){ (void)s; D[0]=q[0]; D[1]=q[1]; D[2]=q[2]; }
static void run(const char *name, void *st, upd f, void (*reinit)(void*), int size){
  double t[64], D[3], q[3], sink=0; long i; int r;
  for (r=0; r<NREP; r++){ if (reinit) reinit(st);
    for (i=0;i<20000;i++){ q[0]=q[1]=q[2]=U[i]; f(st,q,D); }            /* warm-up */
    double t0=now_s(); for (i=0;i<R;i++){ q[0]=U[i]; q[1]=U[i]; q[2]=U[i]; f(st,q,D); sink+=D[0]; } t[r]=1e9*(now_s()-t0)/R; }
  double mn=t[0]; for (r=1;r<NREP;r++) if (t[r]<mn) mn=t[r];
  double med=qs_(t,NREP,.5), p25=qs_(t,NREP,.25), p75=qs_(t,NREP,.75), mx=t[NREP-1];
  /* per-batch latency */
  static double bt[NB]; long k=0; if (reinit) reinit(st);
  for (int b=0;b<NB;b++){ double t0=now_s(); for (int j=0;j<B;j++,k++){ long ii=k%R; q[0]=U[ii]; q[1]=U[ii]; q[2]=U[ii]; f(st,q,D); sink+=D[0]; } bt[b]=1e9*(now_s()-t0)/B; }
  double b50=qs_(bt,NB,.5), b99=qs_(bt,NB,.99), b999=qs_(bt,NB,.999), bmx=bt[NB-1];
  vsink+=sink;
  printf("%s,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", name, size, mn, p25, med, p75, mx, b50, b99, b999, bmx);
}
static double cglob, aglob=0.5, hglob=0.002;
static Oust OO; static int om; static double owb;
static void oust_re(void *s){ oust_init((Oust*)s, aglob, om, owb, 1e3, hglob); }
static Soe SS; static int sk; static void soe_re(void *s){ soe_init((Soe*)s, &SOES[sk], cglob); }
static Win WW; static int wn; static void win_re(void *s){ Win *W=(Win*)s; long i; W->c=cglob; W->a=aglob; W->Nw=wn; W->tail=1; W->n=0; W->started=0; for(i=0;i<=W->Nw;i++) W->bw[i]=l1w(aglob,i);
  double q[3], D[3]; for(i=0;i<5000;i++){ q[0]=q[1]=q[2]=1e-3*sin(1e-3*i); win_update(W,q,D);} }
int main(int argc, char **argv){
  if (argc > 2) { R = atol(argv[1]); NREP = atoi(argv[2]); if (NREP > 64) NREP = 64; }
  struct timespec res; clock_getres(CLOCK_MONOTONIC,&res);
  long i; U=malloc(sizeof(double)*R); for(i=0;i<R;i++) U[i]=1e-3*sin(1e-3*i);
  cglob=pow(hglob,-aglob)/tgamma(2-aglob);
  double t0=now_s(); for(i=0;i<R;i++) vsink+=now_s(); double tcall=1e9*(now_s()-t0)/R;
  printf("# clock_getres(CLOCK_MONOTONIC) = %ld ns; cost of one clock_gettime call = %.1f ns\n", res.tv_nsec, tcall);
  printf("method,size,min_ns,p25_ns,median_ns,p75_ns,max_ns,batch%d_p50_ns,batch_p99_ns,batch_p999_ns,batch_max_ns\n", B);
  run("loop overhead (no kernel)", NULL, none_upd, NULL, 0);
  int ms[]={5,7,9,11}; double wbs[]={1e-2,1e-2,1e-4,1e-4}; char nm[64];
  for(int k=0;k<4;k++){ om=ms[k]; owb=wbs[k]; sprintf(nm,"Oustaloup %d [%g; 1e3]",om,owb); run(nm,&OO,(upd)oust_update,oust_re,om); }
  for(sk=0;sk<N_SOE;sk++){ sprintf(nm,"SOE %s [1 h]",SOES[sk].label); run(nm,&SS,(upd)soe_update,soe_re,SOES[sk].nm); }
  int ws[]={250,1000}; for(int w=0;w<2;w++){ wn=ws[w]; sprintf(nm,"window %g s + tail",wn*hglob); run(nm,&WW,(upd)win_update,win_re,wn); }
  return 0; }
