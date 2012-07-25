/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpiimpl.h"
#include "rma.h"

/* -- Begin Profiling Symbol Block for routine MPI_Win_allocate_shared */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPIX_Win_allocate_shared = PMPIX_Win_allocate_shared
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPIX_Win_allocate_shared  MPIX_Win_allocate_shared
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPIX_Win_allocate_shared as PMPIX_Win_allocate_shared
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPIX_Win_allocate_shared
#define MPIX_Win_allocate_shared PMPIX_Win_allocate_shared

#endif

#undef FUNCNAME
#define FUNCNAME MPIX_Win_allocate_shared
#undef FCNAME
#define FCNAME MPIU_QUOTE(FUNCNAME)

/*@
   MPIX_Win_allocate_shared - Create an MPI Window object for one-sided communication
   and allocate memory at each process.

   Input Parameters:
. size - size of window in bytes (nonnegative integer)
. info - info argument (handle)
- comm - communicator (handle)

  Output Parameter:
. baseptr - initial address of window (choice)
- win - window object returned by the call (handle)

.N ThreadSafe
.N Fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_ARG
.N MPI_ERR_COMM
.N MPI_ERR_INFO
.N MPI_ERR_OTHER
.N MPI_ERR_SIZE
@*/
int MPIX_Win_allocate_shared(MPI_Aint size, MPI_Info info, MPI_Comm comm,
                             void *baseptr, MPI_Win *win)
{
    int mpi_errno = MPI_SUCCESS;
    MPID_Win *win_ptr = NULL;
    MPID_Comm *comm_ptr = NULL;
    MPID_Info *info_ptr = NULL;
    MPID_MPI_STATE_DECL(MPID_STATE_MPIX_WIN_ALLOCATE_SHARED);

    MPIR_ERRTEST_INITIALIZED_ORDIE();
    
    MPIU_THREAD_CS_ENTER(ALLFUNC,);
    MPID_MPI_RMA_FUNC_ENTER(MPID_STATE_MPIX_WIN_ALLOCATE_SHARED);

    /* Validate parameters, especially handles needing to be converted */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
	    MPIR_ERRTEST_COMM(comm, mpi_errno);
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
	}
        MPID_END_ERROR_CHECKS;
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* Convert MPI object handles to object pointers */
    MPID_Comm_get_ptr( comm, comm_ptr );
    MPID_Info_get_ptr( info, info_ptr );

    /* Validate parameters and objects (post conversion) */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            /* Validate pointers */
	    MPID_Comm_valid_ptr( comm_ptr, mpi_errno );
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
            if (size < 0)
                mpi_errno = MPIR_Err_create_code( MPI_SUCCESS,
						  MPIR_ERR_RECOVERABLE,
						  FCNAME, __LINE__,
						  MPI_ERR_SIZE,
                                                  "**rmasize",
                                                  "**rmasize %d", size);
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
        }
        MPID_END_ERROR_CHECKS;
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ...  */
    
    mpi_errno = MPID_Win_allocate_shared(size, info_ptr, comm_ptr, baseptr, &win_ptr);
    if (mpi_errno != MPI_SUCCESS) goto fn_fail;

    /* Initialize a few fields that have specific defaults */
    win_ptr->name[0]    = 0;
    win_ptr->errhandler = 0;
    win_ptr->lockRank   = -1;

    /* return the handle of the window object to the user */
    MPIU_OBJ_PUBLISH_HANDLE(*win, win_ptr->handle);

    /* ... end of body of routine ... */

  fn_exit:
    MPID_MPI_RMA_FUNC_EXIT(MPID_STATE_MPIX_WIN_ALLOCATE_SHARED);
    MPIU_THREAD_CS_EXIT(ALLFUNC,);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLING-- */
#   ifdef HAVE_ERROR_CHECKING
    {
	mpi_errno = MPIR_Err_create_code(
	    mpi_errno, MPIR_ERR_RECOVERABLE, FCNAME, __LINE__, MPI_ERR_OTHER, "**mpi_win_allocate_shared",
	    "**mpi_win_allocate_shared %d %I %C %p %p", size, info, comm, baseptr, win);
    }
#   endif
    mpi_errno = MPIR_Err_return_comm( comm_ptr, FCNAME, mpi_errno );
    goto fn_exit;
    /* --END ERROR HANDLING-- */
}