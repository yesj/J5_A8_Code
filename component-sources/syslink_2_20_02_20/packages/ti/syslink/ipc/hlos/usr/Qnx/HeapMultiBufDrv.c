/*
 *  @file   HeapMultiBufDrv.c
 *
 *  @brief      OS-specific implementation of HeapMultiBuf driver for Qnx
 *
 *
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */



/* Standard headers */
#include  <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module specific header files */
//#include <Heap.h>
#include <HeapMultiBuf.h>
#include <HeapMultiBufDrvDefs.h>

/* Linux specific header files */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for HeapMultiBuf.
 */
#define HEAPMULTIBUF_DRIVER_NAME     "/dev/syslinkipc/HeapMultiBuf"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for HeapMultiBuf in this process.
 */
static Int32 HeapMultiBufDrv_handle = 0;

/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 HeapMultiBufDrv_refCount = 0;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the HeapMultiBuf driver.
 *
 *  @sa     HeapMultiBufDrv_close
 */
Int
HeapMultiBufDrv_open (Void)
{
    //Int status      = HEAPMULTIBUF_SUCCESS;
    Int status      = 0;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "HeapMultiBufDrv_open");

#if 0
    if (HeapMultiBufDrv_refCount == 0) {

        HeapMultiBufDrv_handle = open (HEAPMULTIBUF_DRIVER_NAME,
                                  O_SYNC | O_RDWR);
        if (HeapMultiBufDrv_handle < 0) {
            perror (HEAPMULTIBUF_DRIVER_NAME);
            /*! @retval HEAPMULTIBUF_E_OSFAILURE Failed to open
             *          HeapMultiBuf driver with OS
             */
            status = HEAPMULTIBUF_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMultiBufDrv_open",
                                 status,
                                 "Failed to open HeapMultiBuf driver with OS!");
        }
        else {
            osStatus = fcntl (HeapMultiBufDrv_handle, F_SETFD, FD_CLOEXEC);
            if (osStatus != 0) {
                /*! @retval HEAPMULTIBUF_E_OSFAILURE
                 *          Failed to set file descriptor flags
                 */
                status = HEAPMULTIBUF_E_OSFAILURE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapMultiBufDrv_open",
                                     status,
                                     "Failed to set file descriptor flags!");
            }
            else {
                /* TBD: Protection for refCount. */
                HeapMultiBufDrv_refCount++;
            }
        }
    }
    else {
        HeapMultiBufDrv_refCount++;
    }

#endif
    GT_1trace (curTrace, GT_LEAVE, "HeapMultiBufDrv_open", status);

    /*! @retval HEAPMULTIBUF_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief  Function to close the HeapMultiBuf driver.
 *
 *  @sa     HeapMultiBufDrv_open
 */
Int
HeapMultiBufDrv_close (Void)
{
    //Int status      = HEAPMULTIBUF_SUCCESS;
    Int status      = 0;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "HeapMultiBufDrv_close");

#if 0
    /* TBD: Protection for refCount. */
    HeapMultiBufDrv_refCount--;
    if (HeapMultiBufDrv_refCount == 0) {
        osStatus = close (HeapMultiBufDrv_handle);
        if (osStatus != 0) {
            perror ("HeapMultiBuf driver close: ");
            /*! @retval HEAPMULTIBUF_E_OSFAILURE
             *          Failed to open HeapMultiBuf driver with OS
             */
            status = HEAPMULTIBUF_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMultiBufDrv_close",
                                 status,
                                 "Failed to close HeapMultiBuf driver with OS!");
        }
        else {
            HeapMultiBufDrv_handle = 0;
        }
    }

#endif
    GT_1trace (curTrace, GT_LEAVE, "HeapMultiBufDrv_close", status);

    /*! @retval HEAPMULTIBUF_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief  Function to invoke the APIs through ioctl.
 *
 *  @param  cmd     Command for driver ioctl
 *  @param  args    Arguments for the ioctl command
 *
 *  @sa
 */
Int
HeapMultiBufDrv_ioctl (UInt32 cmd, Ptr args)
{
    //Int status      = HEAPMULTIBUF_SUCCESS;
    Int status      = 0;
    int osStatus    = 0;

#if 0
    GT_2trace (curTrace, GT_ENTER, "HeapMultiBufDrv_ioctl", cmd, args);

    GT_assert (curTrace, (HeapMultiBufDrv_refCount > 0));

    osStatus = ioctl (HeapMultiBufDrv_handle, cmd, args);
    if (osStatus < 0) {
        /*! @retval HEAPMULTIBUF_E_OSFAILURE Driver ioctl failed */
        status = HEAPMULTIBUF_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMultiBufDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((HeapMultiBufDrv_CmdArgs *) args)->apiStatus;
    }

#endif
    GT_1trace (curTrace, GT_LEAVE, "HeapMultiBufDrv_ioctl", status);

    /*! @retval HEAPMULTIBUF_SUCCESS Operation successfully completed. */
    return status;
}



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
