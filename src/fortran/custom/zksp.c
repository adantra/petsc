
#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: zksp.c,v 1.34 1999/10/01 21:23:14 bsmith Exp balay $";
#endif

#include "src/fortran/custom/zpetsc.h"
#include "ksp.h"

#ifdef PETSC_HAVE_FORTRAN_CAPS
#define kspregisterdestroy_       KSPREGISTERDESTROY
#define kspdestroy_               KSPDESTROY
#define ksplgmonitordestroy_      KSPLGMONITORDESTROY
#define ksplgmonitorcreate_       KSPLGMONITORCREATE
#define kspgetrhs_                KSPGETRHS
#define kspgetsolution_           KSPGETSOLUTION
#define kspgetpc_                 KSPGETPC
#define kspsetmonitor_            KSPSETMONITOR
#define kspsetconvergencetest_    KSPSETCONVERGENCETEST
#define kspcreate_                KSPCREATE
#define kspsetoptionsprefix_      KSPSETOPTIONSPREFIX
#define kspappendoptionsprefix_   KSPAPPENDOPTIONSPREFIX
#define kspgettype_               KSPGETTYPE
#define kspgetpreconditionerside_ KSPGETPRECONDITIONERSIDE
#define kspbuildsolution_         KSPBUILDSOLUTION
#define kspsettype_               KSPSETTYPE           
#define kspgetresidualhistory_    KSPGETRESIDUALHISTORY
#define kspgetoptionsprefix_      KSPGETOPTIONSPREFIX
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define kspgetresidualhistory_    kspgetresidualhistory
#define kspsettype_               kspsettype
#define kspregisterdestroy_       kspregisterdestroy
#define kspdestroy_               kspdestroy
#define ksplgmonitordestroy_      ksplgmonitordestroy
#define ksplgmonitorcreate_       ksplgmonitorcreate
#define kspgetrhs_                kspgetrhs
#define kspgetsolution_           kspgetsolution
#define kspgetpc_                 kspgetpc
#define kspsetmonitor_            kspsetmonitor
#define kspsetconvergencetest_    kspsetconvergencetest
#define kspcreate_                kspcreate
#define kspsetoptionsprefix_      kspsetoptionsprefix
#define kspappendoptionsprefix_   kspappendoptionsprefix
#define kspgettype_               kspgettype
#define kspgetpreconditionerside_ kspgetpreconditionerside
#define kspbuildsolution_         kspbuildsolution
#define kspgetoptionsprefix_      kspgetoptionsprefix
#endif

EXTERN_C_BEGIN

void PETSC_STDCALL kspgetresidualhistory_(KSP *ksp,int *na,int *__ierr)
{
  *__ierr = KSPGetResidualHistory(*ksp, PETSC_NULL, na);
}

void PETSC_STDCALL kspsettype_(KSP *ksp,CHAR type PETSC_MIXED_LEN(len), int *__ierr PETSC_END_LEN(len) )
{
  char *t;

  FIXCHAR(type,len,t);
  *__ierr = KSPSetType(*ksp,t);
  FREECHAR(type,t);
}

void PETSC_STDCALL kspgettype_(KSP *ksp,CHAR name PETSC_MIXED_LEN(len),int *__ierr PETSC_END_LEN(len) )
{
  char *tname;

  *__ierr = KSPGetType(*ksp,&tname);if (*__ierr) return;
#if defined(USES_CPTOFCD)
  {
    char *t = _fcdtocp(name); int len1 = _fcdlen(name);
    *__ierr = PetscStrncpy(t,tname,len1); 
  }
#else
  *__ierr = PetscStrncpy(name,tname,len);
#endif
}

void PETSC_STDCALL kspgetpreconditionerside_(KSP *ksp,PCSide *side, int *__ierr ){
*__ierr = KSPGetPreconditionerSide(*ksp,side );
}

void PETSC_STDCALL kspsetoptionsprefix_(KSP *ksp,CHAR prefix PETSC_MIXED_LEN(len), 
                                        int *__ierr PETSC_END_LEN(len) )
{
  char *t;

  FIXCHAR(prefix,len,t);
  *__ierr = KSPSetOptionsPrefix(*ksp,t);
  FREECHAR(prefix,t);
}

void PETSC_STDCALL kspappendoptionsprefix_(KSP *ksp,CHAR prefix PETSC_MIXED_LEN(len),
                                           int *__ierr PETSC_END_LEN(len) )
{
  char *t;

  FIXCHAR(prefix,len,t);
  *__ierr = KSPAppendOptionsPrefix(*ksp,t);
  FREECHAR(prefix,t);
}

void PETSC_STDCALL kspcreate_(MPI_Comm *comm,KSP *ksp, int *__ierr ){
  *__ierr = KSPCreate((MPI_Comm)PetscToPointerComm( *comm ),ksp);
}

static int (*f2)(KSP*,int*,double*,void*,int*);
static int ourtest(KSP ksp,int i,double d,void* ctx)
{
  int ierr = 0;
  (*f2)(&ksp,&i,&d,ctx,&ierr);CHKERRQ(ierr);
  return 0;
}
void PETSC_STDCALL kspsetconvergencetest_(KSP *ksp,
      int (*converge)(KSP*,int*,double*,void*,int*),void *cctx, int *__ierr)
{
  f2 = converge;
  *__ierr = KSPSetConvergenceTest(*ksp,ourtest,cctx);
}

static int (*f1)(KSP*,int*,double*,void*,int*);
static int ourmonitor(KSP ksp,int i,double d,void* ctx)
{
  int ierr = 0;
  (*f1)(&ksp,&i,&d,ctx,&ierr);CHKERRQ(ierr);
  return 0;
}
void PETSC_STDCALL kspsetmonitor_(KSP *ksp,int (*monitor)(KSP*,int*,double*,void*,int*),
                    void *mctx, int (*monitordestroy)(void *),int *__ierr )
{
  f1 = monitor;
  *__ierr = KSPSetMonitor(*ksp,ourmonitor,mctx,0);
}

void PETSC_STDCALL kspgetpc_(KSP *ksp,PC *B, int *__ierr )
{
  *__ierr = KSPGetPC(*ksp,B);
}

void PETSC_STDCALL kspgetsolution_(KSP *ksp,Vec *v, int *__ierr )
{
  *__ierr = KSPGetSolution(*ksp,v);
}

void PETSC_STDCALL kspgetrhs_(KSP *ksp,Vec *r, int *__ierr )
{
  *__ierr = KSPGetRhs(*ksp,r);
}

/*
   Possible bleeds memory but cannot be helped.
*/
void PETSC_STDCALL ksplgmonitorcreate_(CHAR host PETSC_MIXED_LEN(len1),
                    CHAR label PETSC_MIXED_LEN(len2),int *x,int *y,int *m,int *n,DrawLG *ctx,
                    int *__ierr PETSC_END_LEN(len1) PETSC_END_LEN(len2) )
{
  char   *t1,*t2;

  FIXCHAR(host,len1,t1);
  FIXCHAR(label,len2,t2);
  *__ierr = KSPLGMonitorCreate(t1,t2,*x,*y,*m,*n,ctx);
}

void PETSC_STDCALL ksplgmonitordestroy_(DrawLG *ctx, int *__ierr )
{
  *__ierr = KSPLGMonitorDestroy(*ctx);
}

void PETSC_STDCALL kspdestroy_(KSP *ksp, int *__ierr )
{
  *__ierr = KSPDestroy(*ksp);
}

void PETSC_STDCALL kspregisterdestroy_(int* __ierr)
{
  *__ierr = KSPRegisterDestroy();
}

void PETSC_STDCALL kspbuildsolution_(KSP *ctx,Vec *v,Vec *V, int *__ierr )
{
  *__ierr = KSPBuildSolution(*ctx,*v,V);
}

void PETSC_STDCALL kspbuildresidual_(KSP *ctx,Vec *t,Vec *v,Vec *V, int *__ierr )
{
  *__ierr = KSPBuildResidual(*ctx,*t,*v,V);
}

void PETSC_STDCALL kspgetoptionsprefix_(KSP *ksp, CHAR prefix PETSC_MIXED_LEN(len),
                    int *__ierr PETSC_END_LEN(len) )
{
  char *tname;

  *__ierr = KSPGetOptionsPrefix(*ksp,&tname);
#if defined(USES_CPTOFCD)
  {
    char *t = _fcdtocp(prefix); int len1 = _fcdlen(prefix);
    *__ierr = PetscStrncpy(t,tname,len1); if (*__ierr) return;
  }
#else
  *__ierr = PetscStrncpy(prefix,tname,len); if (*__ierr) return;
#endif
}
EXTERN_C_END
