/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 110 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPIX_Iscan */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPIX_Iscan = PMPIX_Iscan
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPIX_Iscan  MPIX_Iscan
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPIX_Iscan as PMPIX_Iscan
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPIX_Iscan
#define MPIX_Iscan PMPIX_Iscan

/* any non-MPI functions go here, especially non-static ones */

#undef FUNCNAME
#define FUNCNAME MPIR_Iscan_impl
#undef FCNAME
#define FCNAME MPIU_QUOTE(FUNCNAME)
int MPIR_Iscan_impl(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPID_Comm *comm_ptr, MPI_Request *request)
{
    int mpi_errno = MPI_SUCCESS;

    if (comm_ptr->coll_fns != NULL && comm_ptr->coll_fns->Iscan != NULL) {
        mpi_errno = comm_ptr->coll_fns->Iscan(sendbuf, recvbuf, count, datatype, op, comm_ptr, request);
        if (mpi_errno) MPIU_ERR_POP(mpi_errno);
    }
    else {
        MPID_Abort(comm_ptr, MPI_ERR_OTHER, 1, "default version of MPIX_Iscan not yet implemented");
        MPIU_Assertp(0); /* never get here */
        /* TODO implement the following functions and uncomment this code: */
#if 0
        if (comm_ptr->comm_kind == MPID_INTRACOMM) {
            /* intracommunicator */
            mpi_errno = MPIR_Iscan_intra(sendbuf, recvbuf, recvcount, datatype, op, comm_ptr);
            if (mpi_errno) MPIU_ERR_POP(mpi_errno);
        }
        else {
            /* intercommunicator */
            mpi_errno = MPIR_Iscan_inter(sendbuf, recvbuf, recvcount, datatype, op, comm_ptr);
            if (mpi_errno) MPIU_ERR_POP(mpi_errno);
        }
#endif
    }

fn_exit:
    return mpi_errno;
fn_fail:
    goto fn_exit;
}

#endif /* MPICH_MPI_FROM_PMPI */

#undef FUNCNAME
#define FUNCNAME MPI_Allreduce
#undef FCNAME
#define FCNAME MPIU_QUOTE(FUNCNAME)
/*@
MPIX_Iscan - XXX description here

Input Parameters:
+ sendbuf - starting address of the send buffer (choice)
. count - number of elements in input buffer (non-negative integer)
. datatype - data type of elements of input buffer (handle)
. op - operation (handle)
- comm - communicator (handle)

Output Parameters:
+ recvbuf - starting address of the receive buffer (choice)
- request - communication request (handle)

.N ThreadSafe

.N Fortran

.N Errors
@*/
int MPIX_Iscan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
    int mpi_errno = MPI_SUCCESS;
    MPID_Comm *comm_ptr = NULL;
    MPID_MPI_STATE_DECL(MPID_STATE_MPIX_ISCAN);

    MPIU_THREAD_CS_ENTER(ALLFUNC,);
    MPID_MPI_FUNC_ENTER(MPID_STATE_MPIX_ISCAN);

    /* Validate parameters, especially handles needing to be converted */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS
        {
            MPIR_ERRTEST_DATATYPE(datatype, "datatype", mpi_errno);
            MPIR_ERRTEST_OP(op, mpi_errno);
            MPIR_ERRTEST_COMM(comm, mpi_errno);

            /* TODO more checks may be appropriate */
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
        }
        MPID_END_ERROR_CHECKS
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* Convert MPI object handles to object pointers */
    MPID_Comm_get_ptr(comm, comm_ptr);

    /* Validate parameters and objects (post conversion) */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS
        {
            if (HANDLE_GET_KIND(datatype) != HANDLE_KIND_BUILTIN) {
                MPID_Datatype *datatype_ptr = NULL;
                MPID_Datatype_get_ptr(datatype, datatype_ptr);
                MPID_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                MPID_Datatype_committed_ptr(datatype_ptr, mpi_errno);
            }

            if (HANDLE_GET_KIND(op) != HANDLE_KIND_BUILTIN) {
                MPID_Op *op_ptr = NULL;
                MPID_Op_get_ptr(op, op_ptr);
                MPID_Op_valid_ptr(op_ptr, mpi_errno);
            }
            else if (HANDLE_GET_KIND(op) == HANDLE_KIND_BUILTIN) {
                mpi_errno = ( * MPIR_Op_check_dtype_table[op%16 - 1] )(datatype);
            }

            MPID_Comm_valid_ptr(comm_ptr, mpi_errno);
            MPIR_ERRTEST_ARGNULL(request,"request", mpi_errno);
            /* TODO more checks may be appropriate (counts, in_place, buffer aliasing, etc) */
            if (mpi_errno != MPI_SUCCESS) goto fn_fail;
        }
        MPID_END_ERROR_CHECKS
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ...  */

    mpi_errno = MPIR_Iscan_impl(sendbuf, recvbuf, count, datatype, op, comm_ptr, request);
    if (mpi_errno) MPIU_ERR_POP(mpi_errno);

    /* ... end of body of routine ... */

fn_exit:
    MPID_MPI_FUNC_EXIT(MPID_STATE_MPIX_ISCAN);
    MPIU_THREAD_CS_EXIT(ALLFUNC,);
    return mpi_errno;

fn_fail:
    /* --BEGIN ERROR HANDLING-- */
#   ifdef HAVE_ERROR_CHECKING
    {
        mpi_errno = MPIR_Err_create_code(
            mpi_errno, MPIR_ERR_RECOVERABLE, FCNAME, __LINE__, MPI_ERR_OTHER,
            "**mpix_iscan", "**mpix_iscan %p %p %d %D %O %C %p", sendbuf, recvbuf, count, datatype, op, comm, request);
    }
#   endif
    mpi_errno = MPIR_Err_return_comm(comm_ptr, FCNAME, mpi_errno);
    goto fn_exit;
    /* --END ERROR HANDLING-- */
    goto fn_exit;
}