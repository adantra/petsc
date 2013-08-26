
#include <petsc-private/viewerimpl.h>   /*I  "petscsys.h"  */
#include <petscviewersaws.h>

#undef __FUNCT__
#define __FUNCT__ "PetscViewerSAWsOpen"
/*@C
    PetscViewerSAWsOpen - Opens an SAWs memory snooper PetscViewer.

    Collective on MPI_Comm

    Input Parameters:
.   comm - the MPI communicator

    Output Parameter:
.   lab - the PetscViewer

    Options Database Keys:
+   -ams_port <port number> - port number where you are running SAWs client
.   -xxx_view ams - publish the object xxx
-   -xxx_saws_block - blocks the program at the end of a critical point (for KSP and SNES it is the end of a solve) until
                    the user unblocks the the problem with an external tool that access the object with the AMS

    Level: advanced

    Fortran Note:
    This routine is not supported in Fortran.


    Notes:
    Unlike other viewers that only access the object being viewed on the call to XXXView(object,viewer) the SAWs viewer allows
    one to view the object asynchronously as the program continues to run. One can remove SAWs access to the object with a call to
    PetscObjectSAWsViewOff().

    Information about the SAWs is available via http://www.mcs.anl.gov/SAWs.

   Concepts: AMS
   Concepts: Argonne Memory Snooper
   Concepts: Asynchronous Memory Snooper

.seealso: PetscViewerDestroy(), PetscViewerStringSPrintf(), PETSC_VIEWER_SAWS_(), PetscObjectSAWsBlock(),
          PetscObjectSAWsViewOff(), PetscObjectSAWsTakeAccess(), PetscObjectSAWsGrantAccess()

@*/
PetscErrorCode PetscViewerSAWsOpen(MPI_Comm comm,PetscViewer *lab)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscViewerCreate(comm,lab);CHKERRQ(ierr);
  ierr = PetscViewerSetType(*lab,PETSCVIEWERSAWS);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscObjectViewSAWs"
/*@C
   PetscObjectViewSAWs - View the base portion of any object with an SAWs viewer

   Collective on PetscObject

   Input Parameters:
+  obj - the Petsc variable
         Thus must be cast with a (PetscObject), for example,
         PetscObjectSetName((PetscObject)mat,name);
-  viewer - the SAWs viewer

   Level: advanced

   Concepts: publishing object

.seealso: PetscObjectSetName(), PetscObjectSAWsViewOff()

@*/
PetscErrorCode  PetscObjectViewSAWs(PetscObject obj,PetscViewer viewer)
{
  PetscErrorCode ierr;
  char           dir[1024];

  PetscFunctionBegin;
  PetscValidHeader(obj,1);
  if (obj->amsmem) PetscFunctionReturn(0);
  obj->amsmem = PETSC_TRUE;
  ierr = PetscObjectName(obj);CHKERRQ(ierr);

  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/Class",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->class_name,1,SAWs_READ,SAWs_STRING));
  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/Type",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->type_name,1,SAWs_READ,SAWs_STRING));
  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/Id",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->id,1,SAWs_READ,SAWs_INT));
  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/ParentID",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->parentid,1,SAWs_READ,SAWs_INT));
  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/Publish_Block",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->amspublishblock,1,SAWs_READ,SAWs_BOOLEAN));
  ierr = PetscSNPrintf(dir,1024,"/PETSc/Objects/%s/Block",obj->name);CHKERRQ(ierr);
  PetscStackCallSAWs(SAWs_Register,(dir,&obj->amsblock,1,SAWs_WRITE,SAWs_BOOLEAN));
  PetscFunctionReturn(0);
}
