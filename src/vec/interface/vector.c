
#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: vector.c,v 1.128 1998/04/13 17:26:33 bsmith Exp bsmith $";
#endif
/*
     Provides the interface functions for all vector operations.
   These are the vector functions the user calls.
*/
#include "src/vec/vecimpl.h"    /*I "vec.h" I*/

#undef __FUNC__  
#define __FUNC__ "VecSetBlockSize"
/*@
   VecSetBlockSize - Sets the blocksize for future calls to VecSetValuesBlocked()
      and VecSetValuesBlockedLocal().

   Input Parameter:
.  v - the vector
.  bs - the blocksize

   Collective on Vec

   Notes:
     All vectors obtained by VecDuplicate() inherit the same blocksize

.seealso: VecSetValuesBlocked(); VecSetLocalToGlobalMappingBlocked()

.keywords: block size, vectors
@*/
int VecSetBlockSize(Vec v,int bs)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VEC_COOKIE); 
  if (v->N % bs) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,1,"Vector length not divisible by blocksize");
  if (v->n % bs) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,1,"Local vector length not divisible by blocksize");
  
  v->bs = bs;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecValid"
/*@
   VecValid - Checks whether a vector object is valid.

   Input Parameter:
.  v - the object to check

   Output Parameter:
   flg - flag indicating vector status, either
$     PETSC_TRUE if vector is valid;
$     PETSC_FALSE otherwise.

   Not Collective

.keywords: vector, valid
@*/
int VecValid(Vec v,PetscTruth *flg)
{
  PetscFunctionBegin;
  PetscValidIntPointer(flg);
  if (!v)                           *flg = PETSC_FALSE;
  else if (v->cookie != VEC_COOKIE) *flg = PETSC_FALSE;
  else                              *flg = PETSC_TRUE;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDot"
/*@
   VecDot - Computes the vector dot product.

   Input Parameters:
.  x, y - the vectors

   Output Parameter:
.  alpha - the dot product

   Collective on Vec

   Notes for Users of Complex Numbers:
   For complex vectors, VecDot() computes 
$      val = (x,y) = y^H x,
   where y^H denotes the conjugate transpose of y.

   Use VecTDot() for the indefinite form
$      val = (x,y) = y^T x,
   where y^T denotes the transpose of y.

.keywords: vector, dot product, inner product

.seealso: VecMDot(), VecTDot()
@*/
int VecDot(Vec x, Vec y, Scalar *val)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PetscCheckSameType(x,y);
  PLogEventBegin(VEC_Dot,x,y,0,0);
  ierr = (*x->ops->dot)(x,y,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_Dot,x,y,0,0);
  /*
     The next block is for incremental debugging
  */
  if (PetscCompare) {
    int flag;
    ierr = MPI_Comm_compare(PETSC_COMM_WORLD,x->comm,&flag);CHKERRQ(ierr);
    if (flag != MPI_UNEQUAL) {
      ierr = PetscCompareScalar(*val);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecNorm"
/*@
   VecNorm  - Computes the vector norm.

   Input Parameters:
.  x - the vector
.  type - one of NORM_1, NORM_2, NORM_INFINITY
          NORM_1_AND_2 computes both norms and stores them
          in a two element array.

   Output Parameter:
.  val - the norm 

   Collective on Vec

.keywords: vector, norm

.seealso: VecDot(), VecTDot()

@*/
int VecNorm(Vec x,NormType type,double *val)  
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PLogEventBegin(VEC_Norm,x,0,0,0);
  ierr = (*x->ops->norm)(x,type,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_Norm,x,0,0,0);
  /*
     The next block is for incremental debugging
  */
  if (PetscCompare) {
    int flag;
    ierr = MPI_Comm_compare(PETSC_COMM_WORLD,x->comm,&flag);CHKERRQ(ierr);
    if (flag != MPI_UNEQUAL) {
      ierr = PetscCompareDouble(*val);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecMax"
/*@
   VecMax - Determines the maximum vector component and its location.

   Input Parameter:
.  x - the vector

   Output Parameters:
.  val - the maximum component
.  p - the location of val

   Collective on Vec

   Notes:
   Returns the value PETSC_MIN and p = -1 if the vector is of length 0.

.keywords: vector, maximum

.seealso: VecNorm(), VecMin()
@*/
int VecMax(Vec x,int *p,double *val)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PLogEventBegin(VEC_Max,x,0,0,0);
  ierr = (*x->ops->max)(x,p,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_Max,x,0,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecMin"
/*@
   VecMin - Determines the minimum vector component and its location.

   Input Parameters:
.  x - the vector

   Output Parameter:
.  val - the minimum component
.  p - the location of val

   Collective on Vec

   Notes: Returns the value PETSC_MAX and p = -1 if the vector is of length 0.

.keywords: vector, minimum

.seealso: VecMax()
@*/
int VecMin(Vec x,int *p,double *val)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PLogEventBegin(VEC_Min,x,0,0,0);
  ierr = (*x->ops->min)(x,p,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_Min,x,0,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecTDot"
/*@
   VecTDot - Computes an indefinite vector dot product. That is, this
   routine does NOT use the complex conjugate.

   Input Parameters:
.  x, y - the vectors

   Output Parameter:
.  val - the dot product

   Collective on Vec

   Notes for Users of Complex Numbers:
   For complex vectors, VecTDot() computes the indefinite form
$      val = (x,y) = y^T x,
   where y^T denotes the transpose of y.

   Use VecDot() for the inner product
$      val = (x,y) = y^H x,
   where y^H denotes the conjugate transpose of y.

.keywords: vector, dot product, inner product

.seealso: VecDot(), VecMTDot()
@*/
int VecTDot(Vec x,Vec y,Scalar *val) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PetscCheckSameType(x,y);
  PLogEventBegin(VEC_TDot,x,y,0,0);
  ierr = (*x->ops->tdot)(x,y,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_TDot,x,y,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecScale"
/*@
   VecScale - Scales a vector. 

   Input Parameters:
.  x - the vector
.  alpha - the scalar

   Output Parameter:
.  x - the scaled vector

   Collective on Vec

   Note:
   For a vector with n components, VecScale() computes 
$      x[i] = alpha * x[i], for i=1,...,n.

.keywords: vector, scale
@*/
int VecScale(Scalar *alpha,Vec x)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PLogEventBegin(VEC_Scale,x,0,0,0);
  ierr = (*x->ops->scale)(alpha,x); CHKERRQ(ierr);
  PLogEventEnd(VEC_Scale,x,0,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecCopy"
/*@
   VecCopy - Copies a vector. 

   Input Parameter:
.  x - the vector

   Output Parameter:
.  y - the copy

   Collective on Vec

.keywords: vector, copy

.seealso: VecDuplicate()
@*/
int VecCopy(Vec x,Vec y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PLogEventBegin(VEC_Copy,x,y,0,0);
  ierr = (*x->ops->copy)(x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_Copy,x,y,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSet"
/*@
   VecSet - Sets all components of a vector to a scalar. 

   Input Parameters:
.  alpha - the scalar
.  x  - the vector

   Output Parameter:
.  x  - the vector

   Collective on Vec

   Note:
   For a vector with n components, VecSet() computes
$      x[i] = alpha, for i=1,...,n.

.keywords: vector, set
@*/
int VecSet(Scalar *alpha,Vec x) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PLogEventBegin(VEC_Set,x,0,0,0);
  ierr = (*x->ops->set)(alpha,x); CHKERRQ(ierr);
  PLogEventEnd(VEC_Set,x,0,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecSetRandom"
/*@C
   VecSetRandom - Sets all components of a vector to random numbers.

   Input Parameters:
.  rctx - the random number context, formed by PetscRandomCreate()
.  x  - the vector

   Output Parameter:
.  x  - the vector

   Collective on Vec

   Example of Usage:
$    PetscRandomCreate(PETSC_COMM_WORLD,RANDOM_DEFAULT,&rctx);
$    VecSetRandom(rctx,x);
$    PetscRandomDestroy(rctx);

.keywords: vector, set, random

.seealso: VecSet(), VecSetValues(), PetscRandomCreate(), PetscRandomDestroy()
@*/
int VecSetRandom(PetscRandom rctx,Vec x) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(rctx,PETSCRANDOM_COOKIE);
  PLogEventBegin(VEC_SetRandom,x,rctx,0,0);
  ierr = (*x->ops->setrandom)(rctx,x); CHKERRQ(ierr);
  PLogEventEnd(VEC_SetRandom,x,rctx,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecAXPY"
/*@
   VecAXPY - Computes y = alpha x + y. 

   Input Parameters:
.  alpha - the scalar
.  x, y  - the vectors

   Output Parameter:
.  y - output vector

   Collective on Vec

.keywords: vector, saxpy

.seealso: VecAYPX(), VecMAXPY(), VecWAXPY()
@*/
int VecAXPY(Scalar *alpha,Vec x,Vec y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PLogEventBegin(VEC_AXPY,x,y,0,0);
  ierr = (*x->ops->axpy)(alpha,x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_AXPY,x,y,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecAXPBY"
/*@
   VecAXPBY - Computes y = alpha x + beta y. 

   Input Parameters:
.  alpha,beta - the scalars
.  x, y  - the vectors

   Output Parameter:
.  y - output vector

   Collective on Vec

.keywords: vector, saxpy

.seealso: VecAYPX(), VecMAXPY(), VecWAXPY(), VecAXPY()
@*/
int VecAXPBY(Scalar *alpha,Scalar *beta,Vec x,Vec y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PetscValidScalarPointer(beta);
  PLogEventBegin(VEC_AXPY,x,y,0,0);
  ierr = (*x->ops->axpby)(alpha,beta,x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_AXPY,x,y,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecAYPX"
/*@
   VecAYPX - Computes y = x + alpha y.

   Input Parameters:
.  alpha - the scalar
.  x, y  - the vectors

   Output Parameter:
.  y - output vector

   Collective on Vec

.keywords: vector, saypx

.seealso: VecAXPY(), VecWAXPY()
@*/
int VecAYPX(Scalar *alpha,Vec x,Vec y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PLogEventBegin(VEC_AYPX,x,y,0,0);
  ierr =  (*x->ops->aypx)(alpha,x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_AYPX,x,y,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecSwap"
/*@
   VecSwap - Swaps the vectors x and y.

   Input Parameters:
.  x, y  - the vectors

   Collective on Vec

.keywords: vector, swap
@*/
int VecSwap(Vec x,Vec y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);  
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscCheckSameType(x,y);
  PLogEventBegin(VEC_Swap,x,y,0,0);
  ierr = (*x->ops->swap)(x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_Swap,x,y,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecWAXPY"
/*@
   VecWAXPY - Computes w = alpha x + y.

   Input Parameters:
.  alpha - the scalar
.  x, y  - the vectors

   Output Parameter:
.  w - the result

   Collective on Vec

.keywords: vector, waxpy

.seealso: VecAXPY(), VecAYPX()
@*/
int VecWAXPY(Scalar *alpha,Vec x,Vec y,Vec w)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidHeaderSpecific(w,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PetscCheckSameType(x,y); PetscCheckSameType(y,w);
  PLogEventBegin(VEC_WAXPY,x,y,w,0);
  ierr =  (*x->ops->waxpy)(alpha,x,y,w); CHKERRQ(ierr);
  PLogEventEnd(VEC_WAXPY,x,y,w,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecPointwiseMult"
/*@
   VecPointwiseMult - Computes the componentwise multiplication w = x*y.

   Input Parameters:
.  x, y  - the vectors

   Output Parameter:
.  w - the result

   Collective on Vec

.keywords: vector, multiply, componentwise, pointwise

.seealso: VecPointwiseDivide()
@*/
int VecPointwiseMult(Vec x,Vec y,Vec w)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidHeaderSpecific(w,VEC_COOKIE);
  PLogEventBegin(VEC_PMult,x,y,w,0);
  ierr = (*x->ops->pointwisemult)(x,y,w); CHKERRQ(ierr);
  PLogEventEnd(VEC_PMult,x,y,w,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecPointwiseDivide"
/*@
   VecPointwiseDivide - Computes the componentwise division w = x/y.

   Input Parameters:
.  x, y  - the vectors

   Output Parameter:
.  w - the result

   Collective on Vec

.keywords: vector, divide, componentwise, pointwise

.seealso: VecPointwiseMult()
@*/
int VecPointwiseDivide(Vec x,Vec y,Vec w)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(y,VEC_COOKIE);
  PetscValidHeaderSpecific(w,VEC_COOKIE);
  ierr = (*x->ops->pointwisedivide)(x,y,w);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDuplicate"
/*@C
   VecDuplicate - Creates a new vector of the same type as an existing vector.

   Input Parameters:
.  v - a vector to mimic

   Output Parameter:
.  newv - location to put new vector

   Collective on Vec

   Notes:
   VecDuplicate() does not copy the vector, but rather allocates storage
   for the new vector.  Use VecCopy() to copy a vector.

   Use VecDestroy() to free the space. Use VecDuplicateVecs() to get several
   vectors. 

.keywords: vector, duplicate, create

.seealso: VecDestroy(), VecDuplicateVecs(), VecCreate(), VecCopy()
@*/
int VecDuplicate(Vec v,Vec *newv) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VEC_COOKIE);
  PetscValidPointer(newv);
  ierr = (*v->ops->duplicate)(v,newv);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDestroy"
/*@C
   VecDestroy - Destroys a vector.

   Input Parameters:
.  v  - the vector

   Collective on Vec

.keywords: vector, destroy

.seealso: VecDuplicate()
@*/
int VecDestroy(Vec v)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VEC_COOKIE);
  if (--v->refct > 0) PetscFunctionReturn(0);

  ierr = (*v->ops->destroy)(v);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDuplicateVecs"
/*@C
   VecDuplicateVecs - Creates several vectors of the same type as an existing vector.

   Input Parameters:
.  m - the number of vectors to obtain
.  v - a vector to mimic

   Output Parameter:
.  V - location to put pointer to array of vectors

   Collective on Vec

   Notes:
   Use VecDestroyVecs() to free the space. Use VecDuplicate() to form a single
   vector.

   Fortran Note:
   The Fortran interface is slightly different from that given below, it 
   requires one to pass in V a Vec (integer) array of size at least m.
   See the Fortran chapter of the users manual and petsc/src/vec/examples for details.

.keywords: vector, duplicate

.seealso:  VecDestroyVecs(), VecDuplicate(), VecCreate(), VecDuplicateVecsF90()
@*/
int VecDuplicateVecs(Vec v,int m,Vec **V)  
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VEC_COOKIE);
  PetscValidPointer(V);
  ierr = (*v->ops->duplicatevecs)( v, m,V );CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDestroyVecs"
/*@C
   VecDestroyVecs - Frees a block of vectors obtained with VecDuplicateVecs().

   Input Parameters:
.  vv - pointer to array of vector pointers
.  m - the number of vectors previously obtained

   Collective on Vec

   Fortran Note:
   The Fortran interface is slightly different from that given below.
   See the Fortran chapter of the users manual and 
   petsc/src/vec/examples for details.

.keywords: vector, destroy

.seealso: VecDuplicateVecs(), VecDestroyVecsF90()
@*/
int VecDestroyVecs(Vec *vv,int m)
{
  int ierr;

  PetscFunctionBegin;
  if (!vv) SETERRQ(PETSC_ERR_ARG_BADPTR,0,"Null vectors");
  PetscValidHeaderSpecific(*vv,VEC_COOKIE);
  ierr = (*(*vv)->ops->destroyvecs)( vv, m );CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetValues"
/*@
   VecSetValues - Inserts or adds values into certain locations of a vector. 

   Input Parameters:
.  x - vector to insert in
.  ni - number of elements to add
.  ix - indices where to add
.  y - array of values
.  iora - either INSERT_VALUES or ADD_VALUES

   Not Collective

   Notes: 
   x[ix[i]] = y[i], for i=0,...,ni-1.

   Notes:
   Calls to VecSetValues() with the INSERT_VALUES and ADD_VALUES 
   options cannot be mixed without intervening calls to the assembly
   routines.

   These values may be cached, so VecAssemblyBegin() and VecAssemblyEnd() 
   MUST be called after all calls to VecSetValues() have been completed.

   VecSetValues() uses 0-based indices in Fortran as well as in C.

.keywords: vector, set, values

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValuesLocal(),
           VecSetValue(), VecSetValuesBlocked()
@*/
int VecSetValues(Vec x,int ni,int *ix,Scalar *y,InsertMode iora) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(ix);
  PetscValidScalarPointer(y);
  PLogEventBegin(VEC_SetValues,x,0,0,0);
  ierr = (*x->ops->setvalues)( x, ni,ix, y,iora ); CHKERRQ(ierr);
  PLogEventEnd(VEC_SetValues,x,0,0,0);  
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetValuesBlocked"
/*@
   VecSetValuesBlocked - Inserts or adds blocks of values into certain locations of a vector. 

   Input Parameters:
.  x - vector to insert in
.  ni - number of blocks to add
.  ix - indices where to add in block count, rather than element count
.  y - array of values
.  iora - either INSERT_VALUES or ADD_VALUES

   Not Collective

   Notes: 
   x[ix[bs*i]+j] = y[bs*i+j], for j=0,...,bs, for i=0,...,ni-1. where bs was set with 
   VecSetBlockSize()

   Notes:
   Calls to VecSetValues() with the INSERT_VALUES and ADD_VALUES 
   options cannot be mixed without intervening calls to the assembly
   routines.

   These values may be cached, so VecAssemblyBegin() and VecAssemblyEnd() 
   MUST be called after all calls to VecSetValues() have been completed.

   VecSetValues() uses 0-based indices in Fortran as well as in C.

.keywords: vector, set, values

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValuesLocal(),
           VecSetValue()
@*/
int VecSetValuesBlocked(Vec x,int ni,int *ix,Scalar *y,InsertMode iora) 
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(ix);
  PetscValidScalarPointer(y);
  PLogEventBegin(VEC_SetValues,x,0,0,0);
  ierr = (*x->ops->setvaluesblocked)( x, ni,ix, y,iora ); CHKERRQ(ierr);
  PLogEventEnd(VEC_SetValues,x,0,0,0);  
  PetscFunctionReturn(0);
}

/*MC
   VecSetValue - Set a single entry into a vector.

   Input Parameters:
.  v - the vector
.  row - the row location of the entry
.  value - the value to insert
.  mode - either INSERT_VALUES or ADD_VALUES

   Synopsis:
   void VecSetValue(Vec v,int row,Scalar value, InsertMode mode);

   Notes: For efficiency one should use VecSetValues() and set 
several or many values simultaneously.

.seealso: VecSetValues()
M*/

#undef __FUNC__  
#define __FUNC__ "VecSetLocalToGlobalMapping"
/*@
   VecSetLocalToGlobalMapping - Sets a local numbering to global numbering used
   by the routine VecSetValuesLocal() to allow users to insert vector entries
   using a local (per-processor) numbering.

   Input Parameters:
.  x - vector
.  mapping - mapping created with ISLocalToGlobalMappingCreate() or ISLocalToGlobalMappingCreateIS()

   Collective on Vec

   Notes: 
   All vectors obtained with VecDuplicate() from this vector inherit the same mapping.

.keywords: vector, set, values, local ordering

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValues(), VecSetValuesLocal(),
           VecSetLocalToGlobalMappingBlocked(), VecSetValuesBlockedLocal()
@*/
int VecSetLocalToGlobalMapping(Vec x, ISLocalToGlobalMapping mapping)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(mapping,IS_LTOGM_COOKIE);

  if (x->mapping) {
    SETERRQ(PETSC_ERR_ARG_WRONGSTATE,0,"Mapping already set for vector");
  }

  x->mapping = mapping;
  PetscObjectReference((PetscObject)mapping);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetLocalToGlobalMappingBlocked"
/*@
   VecSetLocalToGlobalMappingBlocked - Sets a local numbering to global numbering used
   by the routine VecSetValuesBlockedLocal() to allow users to insert vector entries
   using a local (per-processor) numbering.

   Input Parameters:
.  x - vector
.  mapping - mapping created with ISLocalToGlobalMappingCreate() or ISLocalToGlobalMappingCreateIS()

   Collective on Vec

   Notes: 
   All vectors obtained with VecDuplicate() from this vector inherit the same mapping.

.keywords: vector, set, values, local ordering

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValues(), VecSetValuesLocal(),
           VecSetLocalToGlobalMapping(), VecSetValuesBlockedLocal()
@*/
int VecSetLocalToGlobalMappingBlocked(Vec x, ISLocalToGlobalMapping mapping)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(mapping,IS_LTOGM_COOKIE);

  if (x->bmapping) {
    SETERRQ(PETSC_ERR_ARG_WRONGSTATE,0,"Mapping already set for vector");
  }
  x->bmapping = mapping;
  PetscObjectReference((PetscObject)mapping);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetValuesLocal"
/*@
   VecSetValuesLocal - Inserts or adds values into certain locations of a vector,
   using a local ordering of the nodes. 

   Input Parameters:
.  x - vector to insert in
.  ni - number of elements to add
.  ix - indices where to add
.  y - array of values
.  iora - either INSERT_VALUES or ADD_VALUES

   Not Collective

   Notes: 
   x[ix[i]] = y[i], for i=0,...,ni-1.

   Notes:
   Calls to VecSetValues() with the INSERT_VALUES and ADD_VALUES 
   options cannot be mixed without intervening calls to the assembly
   routines.

   These values may be cached, so VecAssemblyBegin() and VecAssemblyEnd() 
   MUST be called after all calls to VecSetValuesLocal() have been completed.

   VecSetValuesLocal() uses 0-based indices in Fortran as well as in C.

.keywords: vector, set, values, local ordering

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValues(), VecSetLocalToGlobalMapping(),
           VecSetValuesBlockedLocal()
@*/
int VecSetValuesLocal(Vec x,int ni,int *ix,Scalar *y,InsertMode iora) 
{
  int ierr,lixp[128],*lix = lixp;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(ix);
  PetscValidScalarPointer(y);
  if (!x->mapping) {
    SETERRQ(PETSC_ERR_ARG_WRONGSTATE,0,"Local to global never set with VecSetLocalToGlobalMapping()");
  }
  if (ni > 128) {
    lix = (int *) PetscMalloc( ni*sizeof(int) );CHKPTRQ(lix);
  }

  PLogEventBegin(VEC_SetValues,x,0,0,0);
  ierr = ISLocalToGlobalMappingApply(x->mapping,ni,ix,lix); CHKERRQ(ierr);
  ierr = (*x->ops->setvalues)( x,ni,lix, y,iora ); CHKERRQ(ierr);
  PLogEventEnd(VEC_SetValues,x,0,0,0);  
  if (ni > 128) {
    PetscFree(lix);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetValuesBlockedLocal"
/*@
   VecSetValuesBlockedLocal - Inserts or adds values into certain locations of a vector,
   using a local ordering of the nodes. 

   Input Parameters:
.  x - vector to insert in
.  ni - number of blocks to add
.  ix - indices where to add in block count, not element count
.  y - array of values
.  iora - either INSERT_VALUES or ADD_VALUES

   Not Collective

   Notes: 
   x[bs*ix[i]+j] = y[bs*i+j], for j=0,..bs-1, for i=0,...,ni-1.
   Where bs is set with VecSetBlockSize()

   Notes:
   Calls to VecSetValues() with the INSERT_VALUES and ADD_VALUES 
   options cannot be mixed without intervening calls to the assembly
   routines.

   These values may be cached, so VecAssemblyBegin() and VecAssemblyEnd() 
   MUST be called after all calls to VecSetValuesLocal() have been completed.

   VecSetValuesLocal() uses 0-based indices in Fortran as well as in C.

.keywords: vector, set, values, local ordering

.seealso:  VecAssemblyBegin(), VecAssemblyEnd(), VecSetValues(), VecSetLocalToGlobalMapping(),
           VecSetLocalToGlobalMappingBlocked()
@*/
int VecSetValuesBlockedLocal(Vec x,int ni,int *ix,Scalar *y,InsertMode iora) 
{
  int ierr,lixp[128],*lix = lixp;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(ix);
  PetscValidScalarPointer(y);
  if (!x->bmapping) {
    SETERRQ(PETSC_ERR_ARG_WRONGSTATE,0,"Local to global never set with VecSetLocalToGlobalMappingBlocked()");
  }
  if (ni > 128) {
    lix = (int *) PetscMalloc( ni*sizeof(int) );CHKPTRQ(lix);
  }

  PLogEventBegin(VEC_SetValues,x,0,0,0);
  ierr = ISLocalToGlobalMappingApply(x->mapping,ni,ix,lix); CHKERRQ(ierr);
  ierr = (*x->ops->setvaluesblocked)( x,ni,lix, y,iora ); CHKERRQ(ierr);
  PLogEventEnd(VEC_SetValues,x,0,0,0);  
  if (ni > 128) {
    PetscFree(lix);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecAssemblyBegin"
/*@
   VecAssemblyBegin - Begins assembling the vector.  This routine should
   be called after completing all calls to VecSetValues().

   Input Parameter:
.  vec - the vector

   Collective on Vec

.keywords: vector, begin, assembly, assemble

.seealso: VecAssemblyEnd(), VecSetValues()
@*/
int VecAssemblyBegin(Vec vec)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(vec,VEC_COOKIE);
  PLogEventBegin(VEC_AssemblyBegin,vec,0,0,0);
  if (vec->ops->assemblybegin) {
    ierr = (*vec->ops->assemblybegin)(vec); CHKERRQ(ierr);
  }
  PLogEventEnd(VEC_AssemblyBegin,vec,0,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecAssemblyEnd"
/*@
   VecAssemblyEnd - Completes assembling the vector.  This routine should
   be called after VecAssemblyBegin().

   Input Parameter:
.  vec - the vector

   Collective on Vec

.keywords: vector, end, assembly, assemble

.seealso: VecAssemblyBegin(), VecSetValues()
@*/
int VecAssemblyEnd(Vec vec)
{
  int ierr,flg;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(vec,VEC_COOKIE);
  PLogEventBegin(VEC_AssemblyEnd,vec,0,0,0);
  if (vec->ops->assemblyend) {
    ierr = (*vec->ops->assemblyend)(vec); CHKERRQ(ierr);
  }
  PLogEventEnd(VEC_AssemblyEnd,vec,0,0,0);
  ierr = OptionsHasName(PETSC_NULL,"-vec_view",&flg); CHKERRQ(ierr);
  if (flg) {
    ierr = VecView(vec,VIEWER_STDOUT_(vec->comm)); CHKERRQ(ierr);
  }
  ierr = OptionsHasName(PETSC_NULL,"-vec_view_matlab",&flg); CHKERRQ(ierr);
  if (flg) {
    ierr = ViewerPushFormat(VIEWER_STDOUT_(vec->comm),VIEWER_FORMAT_ASCII_MATLAB,"V");CHKERRQ(ierr);
    ierr = VecView(vec,VIEWER_STDOUT_(vec->comm)); CHKERRQ(ierr);
    ierr = ViewerPopFormat(VIEWER_STDOUT_(vec->comm));CHKERRQ(ierr);
  }
  ierr = OptionsHasName(PETSC_NULL,"-vec_view_draw",&flg); CHKERRQ(ierr);
  if (flg) {
    ierr = VecView(vec,VIEWER_DRAWX_(vec->comm)); CHKERRQ(ierr);
    ierr = ViewerFlush(VIEWER_DRAWX_(vec->comm)); CHKERRQ(ierr);
  }
  ierr = OptionsHasName(PETSC_NULL,"-vec_view_draw_lg",&flg); CHKERRQ(ierr);
  if (flg) {
    ierr = ViewerSetFormat(VIEWER_DRAWX_(vec->comm),VIEWER_FORMAT_DRAW_LG,0); CHKERRQ(ierr);
    ierr = VecView(vec,VIEWER_DRAWX_(vec->comm)); CHKERRQ(ierr);
    ierr = ViewerFlush(VIEWER_DRAWX_(vec->comm)); CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecMTDot"
/*@C
   VecMTDot - Computes indefinite vector multiple dot products. 
   That is, it does NOT use the complex conjugate.

   Input Parameters:
.  nv - number of vectors
.  x - one vector
.  y - array of vectors.  Note that vectors are pointers

   Output Parameter:
.  val - array of the dot products

   Collective on Vec

   Notes for Users of Complex Numbers:
   For complex vectors, VecMTDot() computes the indefinite form
$      val = (x,y) = y^T x,
   where y^T denotes the transpose of y.

   Use VecMDot() for the inner product
$      val = (x,y) = y^H x,
   where y^H denotes the conjugate transpose of y.

.keywords: vector, dot product, inner product, non-Hermitian, multiple

.seealso: VecMDot(), VecTDot()
@*/
int VecMTDot(int nv,Vec x,Vec *y,Scalar *val)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(*y,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PetscCheckSameType(x,*y);
  PLogEventBegin(VEC_MTDot,x,*y,0,0);
  ierr = (*x->ops->mtdot)(nv,x,y,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_MTDot,x,*y,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecMDot"
/*@C
   VecMDot - Computes vector multiple dot products. 

   Input Parameters:
.  nv - number of vectors
.  x - one vector
.  y - array of vectors. 

   Output Parameter:
.  val - array of the dot products

   Collective on Vec

   Notes for Users of Complex Numbers:
   For complex vectors, VecMDot() computes 
$      val = (x,y) = y^H x,
   where y^H denotes the conjugate transpose of y.

   Use VecMTDot() for the indefinite form
$      val = (x,y) = y^T x,
   where y^T denotes the transpose of y.

.keywords: vector, dot product, inner product, multiple

.seealso: VecMTDot(), VecDot()
@*/
int VecMDot(int nv,Vec x,Vec *y,Scalar *val)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE); 
  PetscValidHeaderSpecific(*y,VEC_COOKIE);
  PetscValidScalarPointer(val);
  PetscCheckSameType(x,*y);
  PLogEventBegin(VEC_MDot,x,*y,0,0);
  ierr = (*x->ops->mdot)(nv,x,y,val); CHKERRQ(ierr);
  PLogEventEnd(VEC_MDot,x,*y,0,0);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecMAXPY"
/*@C
   VecMAXPY - Computes x = x + sum alpha[j] y[j]

   Input Parameters:
.  nv - number of scalars and x-vectors
.  alpha - array of scalars
.  x  - one vector
.  y  - array of vectors

   Output Parameter:
.  y  - array of vectors

   Collective on Vec

.keywords: vector, saxpy, maxpy, multiple

.seealso: VecAXPY(), VecWAXPY(), VecAYPX()
@*/
int  VecMAXPY(int nv,Scalar *alpha,Vec x,Vec *y)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidHeaderSpecific(*y,VEC_COOKIE);
  PetscValidScalarPointer(alpha);
  PetscCheckSameType(x,*y);
  PLogEventBegin(VEC_MAXPY,x,*y,0,0);
  ierr = (*x->ops->maxpy)(nv,alpha,x,y); CHKERRQ(ierr);
  PLogEventEnd(VEC_MAXPY,x,*y,0,0);
  PetscFunctionReturn(0);
} 

#undef __FUNC__  
#define __FUNC__ "VecGetArray"
/*@C
   VecGetArray - Returns a pointer to vector data. For default PETSc
   vectors, VecGetArray() returns a pointer to the local data array. Otherwise,
   this routine is implementation dependent. You MUST call VecRestoreArray() 
   when you no longer need access to the array.

   Input Parameter:
.  x - the vector

   Output Parameter:
.  a - location to put pointer to the array

   Not Collective

   Fortran Note:
   The Fortran interface is slightly different from that given below.
   See the Fortran chapter of the users manual and 
   petsc/src/vec/examples for details.

.keywords: vector, get, array

.seealso: VecRestoreArray(), VecGetArrays(), VecGetArrayF90(), VecPlaceArray()
@*/
int VecGetArray(Vec x,Scalar **a)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidPointer(a);
  ierr = (*x->ops->getarray)(x,a);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecGetArrays" 
/*@C
   VecGetArrays - Returns a pointer to the arrays in a set of vectors
   that were created by a call to VecDuplicateVecs().  You MUST call
   VecRestoreArrays() when you no longer need access to the array.

   Input Parameter:
.  x - the vectors
.  n - the number of vectors

   Output Parameter:
.  a - location to put pointer to the array

   Not Collective

   Fortran Note:
   This routine is not supported in Fortran.

.keywords: vector, get, arrays

.seealso: VecGetArray(), VecRestoreArrays()
@*/
int VecGetArrays(Vec *x,int n,Scalar ***a)
{
  int    i,ierr;
  Scalar **q;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(*x,VEC_COOKIE);
  PetscValidPointer(a);
  if (n <= 0) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,0,"Must get at least one array");
  q = (Scalar **) PetscMalloc(n*sizeof(Scalar*)); CHKPTRQ(q);
  for (i=0; i<n; ++i) {
    ierr = VecGetArray(x[i],&q[i]); CHKERRQ(ierr);
  }
  *a = q;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecRestoreArrays"
/*@C
   VecRestoreArrays - Restores a group of vectors after VecGetArrays()
   has been called.

   Input Parameters:
.  x - the vector
.  n - the number of vectors
.  a - location of pointer to arrays obtained from VecGetArrays()

  Not Collective

   Fortran Note:
   This routine is not supported in Fortran.

.keywords: vector, restore, arrays

.seealso: VecGetArrays(), VecRestoreArray()
@*/
int VecRestoreArrays(Vec *x,int n,Scalar ***a)
{
  int    i,ierr;
  Scalar **q = *a;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(*x,VEC_COOKIE);
  PetscValidPointer(a);
  q = *a;
  for(i=0;i<n;++i) {
    ierr = VecRestoreArray(x[i],&q[i]); CHKERRQ(ierr);
  }
  PetscFree(q);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecRestoreArray"
/*@C
   VecRestoreArray - Restores a vector after VecGetArray() has been called.

   Input Parameters:
.  x - the vector
.  a - location of pointer to array obtained from VecGetArray()

   Not Collective

   Fortran Note:
   The Fortran interface is slightly different from that given below.
   See the users manual and petsc/src/vec/examples for details.

.keywords: vector, restore, array

.seealso: VecGetArray(), VecRestoreArays(), VecRestoreArrayF90(), VecPlaceArray()
@*/
int VecRestoreArray(Vec x,Scalar **a)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidPointer(a);
  if (x->ops->restorearray) {
    ierr = (*x->ops->getarray)(x,a);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecView"
/*@C
   VecView - Views a vector object. 

   Input Parameters:
.  v - the vector
.  viewer - an optional visualization context

   Collective on Vec unless Viewer is VIEWER_STDOUT_SELF

   Notes:
   The available visualization contexts include
$     VIEWER_STDOUT_SELF - standard output (default)
$     VIEWER_STDOUT_WORLD - synchronized standard
$       output where only the first processor opens
$       the file.  All other processors send their 
$       data to the first processor to print. 
$     VIEWER_DRAWX_WORLD - graphical display of vector

   The user can open alternative vistualization contexts with
$    ViewerFileOpenASCII() - output vector to a specified file
$    ViewerFileOpenBinary() - output in binary to a
$         specified file; corresponding input uses VecLoad()
$    ViewerDrawOpenX() - output vector to an X window display
$    ViewerMatlabOpen() - output vector to Matlab viewer

.keywords: Vec, view, visualize, output, print, write, draw

.seealso: ViewerFileOpenASCII(), ViewerDrawOpenX(), DrawLGCreate(),
          ViewerMatlabOpen(), ViewerFileOpenBinary(), VecLoad()
@*/
int VecView(Vec v,Viewer viewer)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VEC_COOKIE);
  if (!viewer) {viewer = VIEWER_STDOUT_SELF;}
  else { PetscValidHeaderSpecific(viewer,VIEWER_COOKIE);}
  ierr = (*v->ops->view)(v,viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecGetSize"
/*@
   VecGetSize - Returns the global number of elements of the vector.

   Input Parameter:
.  x - the vector

   Output Parameters:
.  size - the global length of the vector

   Not Collective

.keywords: vector, get, size, global, dimension

.seealso: VecGetLocalSize()
@*/
int VecGetSize(Vec x,int *size)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(size);
  ierr = (*x->ops->getsize)(x,size);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecGetLocalSize"
/*@
   VecGetLocalSize - Returns the number of elements of the vector stored 
   in local memory. This routine may be implementation dependent, so use 
   with care.

   Input Parameter:
.  x - the vector

   Output Parameter:
.  size - the length of the local piece of the vector

   Not Collective

.keywords: vector, get, dimension, size, local

.seealso: VecGetSize()
@*/
int VecGetLocalSize(Vec x,int *size)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(size);
  ierr = (*x->ops->getlocalsize)(x,size);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecGetOwnershipRange"
/*@
   VecGetOwnershipRange - Returns the range of indices owned by 
   this processor, assuming that the vectors are laid out with the
   first n1 elements on the first processor, next n2 elements on the
   second, etc.  For certain parallel layouts this range may not be 
   well defined. 

   Input Parameter:
.  x - the vector

   Output Parameters:
.  low - the first local element
.  high - one more than the last local element

   Not Collective

  Note: The high argument is one more then the last element stored locally.

.keywords: vector, get, range, ownership
@*/
int VecGetOwnershipRange(Vec x,int *low,int *high)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  PetscValidIntPointer(low);
  PetscValidIntPointer(high);
  ierr = (*x->ops->getownershiprange)(x,low,high);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecSetOption"
/*@
   VecSetOption - Allows one to set options for a vectors behavior.

   Input Parameter:
.  x - the vector
.  op - the option

   Collective on Vec

  Note: Currently the only option supported is
$ VEC_IGNORE_OFF_PROC_ENTRIES which causes VecSetValues() to ignore 
    entries destined to be stored on a seperate processor.

.keywords: vector, options
@*/
int VecSetOption(Vec x,VecOption op)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(x,VEC_COOKIE);
  if (x->ops->setoption) {
    ierr = (*x->ops->setoption)(x,op); CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDuplicateVecs_Default"
/* Default routines for obtaining and releasing; */
/* may be used by any implementation */
int VecDuplicateVecs_Default(Vec w,int m,Vec **V )
{
  int  i,ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(w,VEC_COOKIE);
  PetscValidPointer(V);
  if (m <= 0) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,0,"m must be > 0");
  *V = (Vec *) PetscMalloc( m * sizeof(Vec *)); CHKPTRQ(*V);
  for (i=0; i<m; i++) {ierr = VecDuplicate(w,*V+i); CHKERRQ(ierr);}
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "VecDestroyVecs_Default"
int VecDestroyVecs_Default( Vec *v, int m )
{
  int i,ierr;

  PetscFunctionBegin;
  PetscValidPointer(v);
  if (m <= 0) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,0,"m must be > 0");
  for (i=0; i<m; i++) {ierr = VecDestroy(v[i]); CHKERRQ(ierr);}
  PetscFree( v );
  PetscFunctionReturn(0);
}

/*MC
    VecGetArrayF90 - Accesses a vector array from Fortran90. For default PETSc
    vectors, VecGetArrayF90() returns a pointer to the local data array. Otherwise,
    this routine is implementation dependent. You MUST call VecRestoreArrayF90() 
    when you no longer need access to the array.

    Input Parameter:
.   x - vector

    Output Parameters:
.   xx_v - the Fortran90 pointer to the array
.   ierr - error code

    Synopsis:
    VecGetArrayF90(Vec x,{Scalar, pointer :: xx_v(:)},integer ierr)

    Example of Usage: 
$    Scalar, pointer :: xx_v(:)
$    ....
$    call VecGetArrayF90(x,xx_v,ierr)
$    a = xx_v(3)
$    call VecRestoreArrayF90(x,xx_v,ierr)

    Notes:
    Currently only supported using the NAG F90 compiler.

.seealso:  VecRestoreArrayF90(), VecGetArray(), VecRestoreArray()

.keywords:  vector, array, f90
M*/

/*MC
    VecRestoreArrayF90 - Restores a vector to a usable state after a call to
    VecGetArrayF90().

    Input Parameters:
.   x - vector
.   xx_v - the Fortran90 pointer to the array

    Output Parameter:
.   ierr - error code

    Synopsis:
    VecRestoreArrayF90(Vec x,{Scalar, pointer :: xx_v(:)},integer ierr)

    Example of Usage: 
$    Scalar, pointer :: xx_v(:)
$    ....
$    call VecGetArrayF90(x,xx_v,ierr)
$    a = xx_v(3)
$    call VecRestoreArrayF90(x,xx_v,ierr)
   
    Notes:
    Currently only supported using the NAG F90 compiler.

.seealso:  VecGetArrayF90(), VecGetArray(), VecRestoreArray()

.keywords:  vector, array, f90
M*/

/*MC
    VecDuplicateVecsF90 - Creates several vectors of the same type as an existing vector
    and makes them accessible via a Fortran90 pointer.

    Input Parameters:
.   x - a vector to mimic
.   n - the number of vectors to obtain

    Output Parameters:
.   y - Fortran90 pointer to the array of vectors
.   ierr - error code

    Synopsis:
    VecGetArrayF90(Vec x,int n,{Scalar, pointer :: y(:)},integer ierr)

    Example of Usage: 
$    Vec x
$    Vec, pointer :: y(:)
$    ....
$    call VecDuplicateVecsF90(x,2,y,ierr)
$    call VecSet(alpha,y(2),ierr)
$    call VecSet(alpha,y(2),ierr)
$    ....
$    call VecDestroyVecsF90(y,2,ierr)

    Notes:
    Currently only supported using the NAG F90 compiler.

    Use VecDestroyVecsF90() to free the space.

.seealso:  VecDestroyVecsF90(), VecDuplicateVecs()

.keywords:  vector, duplicate, f90
M*/

/*MC
    VecDestroyVecsF90 - Frees a block of vectors obtained with VecDuplicateVecsF90().

    Input Parameters:
.   x - pointer to array of vector pointers
.   n - the number of vectors previously obtained

    Output Parameter:
.   ierr - error code

    Synopsis:
    VecDestroyVecsF90({Scalar, pointer :: x(:)},integer n,integer ierr)

    Notes:
    Currently only supported using the NAG F90 compiler.

.seealso:  VecDestroyVecs(), VecDuplicateVecsF90()

.keywords:  vector, destroy, f90
M*/
