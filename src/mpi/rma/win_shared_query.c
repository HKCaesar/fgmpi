/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpiimpl.h"
#include "rma.h"

/* -- Begin Profiling Symbol Block for routine MPIX_Win_shared_query */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPIX_Win_shared_query = PMPIX_Win_shared_query
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPIX_Win_shared_query  MPIX_Win_shared_query
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPIX_Win_shared_query as PMPIX_Win_shared_query
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPIX_Win_shared_query
#define MPIX_Win_shared_query PMPIX_Win_shared_query

#endif

#undef FUNCNAME
#define FUNCNAME MPIX_Win_shared_query

/*@
   MPIX_Win_shared_query - Query the size and base pointer for a patch of a
   shared memory window

 Input Parameters:
+ rank - target rank
- win - window object used for communication (handle) 

 Output Parameters:
+ size - size of the segment at the given rank
- baseptr - base pointer in the calling process' address space of the shared
  segment belonging to the target rank.

.N ThreadSafe

.N Fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_ARG
.N MPI_ERR_RANK
.N MPI_ERR_WIN
@*/
int MPIX_Win_shared_query(MPI_Win win, int rank, MPI_Aint *size, void *baseptr)
{
    static const char FCNAME[] = "MPIX_Win_shared_query";
    int mpi_errno = MPI_SUCCESS;
    MPID_Win *win_ptr = NULL;
    MPID_MPI_STATE_DECL(MPID_STATE_MPIX_WIN_SHARED_QUERY);

    MPIR_ERRTEST_INITIALIZED_ORDIE();
    
    MPIU_THREAD_CS_ENTER(ALLFUNC,);
    MPID_MPI_RMA_FUNC_ENTER(MPID_STATE_MPIX_WIN_SHARED_QUERY);

    /* Validate parameters, especially handles needing to be converted */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_WIN(win, mpi_errno);
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
        }
        MPID_END_ERROR_CHECKS;
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* Convert MPI object handles to object pointers */
    MPID_Win_get_ptr( win, win_ptr );

    /* Validate parameters and objects (post conversion) */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPID_Comm * comm_ptr;

            /* Validate win_ptr */
            MPID_Win_valid_ptr( win_ptr, mpi_errno );
            if (mpi_errno) goto fn_fail;

            MPIR_ERRTEST_ARGNULL(size, "size", mpi_errno);
            MPIR_ERRTEST_ARGNULL(baseptr, "baseptr", mpi_errno);

            if (mpi_errno) goto fn_fail;

            comm_ptr = win_ptr->comm_ptr;
            MPIR_ERRTEST_SEND_RANK(comm_ptr, rank, mpi_errno);

            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
        }
        MPID_END_ERROR_CHECKS;
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ...  */
    
    mpi_errno = MPIU_RMA_CALL(win_ptr,
                              Win_shared_query(win_ptr, rank, size, baseptr));
    if (mpi_errno != MPI_SUCCESS) goto fn_fail;

    /* ... end of body of routine ... */

  fn_exit:
    MPID_MPI_RMA_FUNC_EXIT(MPID_STATE_MPIX_WIN_SHARED_QUERY);
    MPIU_THREAD_CS_EXIT(ALLFUNC,);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLING-- */
#   ifdef HAVE_ERROR_CHECKING
    {
        mpi_errno = MPIR_Err_create_code(
            mpi_errno, MPIR_ERR_RECOVERABLE, FCNAME, __LINE__, MPI_ERR_OTHER, "**mpix_win_shared_query", "**mpix_win_shared_query %W %d %p %p",
            win, rank, size, baseptr);
    }
#   endif
    mpi_errno = MPIR_Err_return_win( win_ptr, FCNAME, mpi_errno );
    goto fn_exit;
    /* --END ERROR HANDLING-- */
}
