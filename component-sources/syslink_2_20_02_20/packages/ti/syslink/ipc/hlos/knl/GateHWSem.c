/*
 *  @file   GateHWSem.c
 *
 *  @brief      Gate based on Hardware Semaphore.
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
#include <ti/syslink/Std.h>

/* Utilities & OSAL headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/inc/IGateMPSupport.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>

/* Module level headers */
#include <ti/syslink/utils/String.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/GateSem.h>
#include <ti/syslink/inc/GateHWSem.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */

/* Macro to make a correct module magic number with refCount */
#define GATEHWSEM_MAKE_MAGICSTAMP(x) (  (GATEHWSEM_MODULEID << 12u)  \
                                           | (x))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for GateHWSem module state */
typedef struct GateHWSem_Module_State {
    Int32                 refCount;
    /* Reference count */
    GateHWSem_Config cfg;
    /* Current config values */
    GateHWSem_Config defaultCfg;
    /* default config values */
    GateHWSem_Params defInstParams;
    /* default instance paramters */
    UInt32 *              baseAddr;
    /* Base address of semaphore registers */
    UInt32                numSems;
    /*!< Maximum number of semaphores */
} GateHWSem_Module_State;

/* Structure defining internal object for the Gate Peterson.*/
struct GateHWSem_Object {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    UInt                        semNum;
    UInt                        nested;
    IGateProvider_Handle        localGate;
};


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    GateHWSem_state
 *
 *  @brief  GateHWSem Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateHWSem_Module_State GateHWSem_state =
{
    .defaultCfg.defaultProtection  = GateHWSem_LocalProtect_INTERRUPT,
    .defInstParams.resourceId      = 0,
    .defInstParams.regionId        = 0,
    .defInstParams.sharedAddr      = NULL,
    .numSems                      = 32u, /* C6474 has 32 semaphores */
};

/*!
 *  @var    GateHWSem_state
 *
 *  @brief  GateHWSem Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateHWSem_Module_State * GateHWSem_module = &GateHWSem_state;

/*
 *  ======== GateHWSem_Module_startup ========
 *  release the HW semaphore if the core owns it.
 */
Int GateHWSem_Module_startup(GateHWSem_Object *obj)
{
    Int i;
    volatile UInt32 *baseAddr = (volatile UInt32 *)GateHWSem_module->baseAddr;

    /* releases the HW semaphore if the core owns it otherwise its a nop */
    for (i = 0; i < GateHWSem_module->numSems; i++) {
        baseAddr[i] = 1;
    }

    return (GateHWSem_S_SUCCESS);
}

/*
 *  ======== GateHWSem_postInit ========
 *  Function to be called during
 *  1. module startup to complete the initialization of all static instances
 *  2. instance_init to complete the initialization of a dynamic instance
 *
 *  Main purpose is to initialize hardware semaphroe
 */
Void GateHWSem_postInit(GateHWSem_Object *obj)
{
    volatile UInt32 *baseAddr = (volatile UInt32 *)GateHWSem_module->baseAddr;

    /* Reset the hardware semaphore */
    baseAddr[obj->semNum] = 1;
}

/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the GateHWSem module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to GateHWSem_setup filled in by
 *              the GateHWSem module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the GateHWSem module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         GateHWSem_setup
 */
Void
GateHWSem_getConfig (GateHWSem_Config * cfgParams)
{
        IArg key;
    GT_1trace (curTrace, GT_ENTER, "GateHWSem_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_getConfig",
                             GateHWSem_E_INVALIDARG,
                             "Argument of type (GateHWSem_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                key = Gate_enterSystem ();
        if (GateHWSem_module->refCount == 0) {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GateHWSem_module->defaultCfg,
                         sizeof (GateHWSem_Config));
        }
        else {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GateHWSem_module->cfg,
                         sizeof (GateHWSem_Config));
        }
                Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "GateHWSem_getConfig");
}


/*!
 *  @brief      Setup the GateHWSem module.
 *
 *              This function sets up the GateHWSem module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GateHWSem_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GateHWSem_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional GateHWSem module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GateHWSem_destroy, GateHWSem_getConfig
 */
Int32
GateHWSem_setup (const GateHWSem_Config * cfg)
{
    Int32                 status = GateHWSem_S_SUCCESS;
    GateHWSem_Config tmpCfg;
        IArg                  key;

    GT_1trace (curTrace, GT_ENTER, "GateHWSem_setup", cfg);

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
        key = Gate_enterSystem ();
    if (GateHWSem_module->refCount > 1) {
        status = GateHWSem_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "GateHWSem Module already initialized!");
                GateHWSem_module->refCount++;
                Gate_leaveSystem (key);
    }
    else {
        GateHWSem_module->refCount++;
        Gate_leaveSystem (key);
        if (cfg == NULL) {
            GateHWSem_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        /* Copy the cfg */
        Memory_copy ((Ptr) &GateHWSem_module->cfg,
                     (Ptr) cfg,
                     sizeof (GateHWSem_Config));
        GateHWSem_module->baseAddr = (Ptr)cfg->baseAddr;
        GateHWSem_module->numSems = cfg->numSems;
    }

    GT_1trace (curTrace, GT_LEAVE, "GateHWSem_setup", status);

    /*! @retval GateHWSem_S_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to destroy the GateHWSem module.
 *
 *  @sa         GateHWSem_setup
 */
Int32
GateHWSem_destroy (Void)
{
    Int32 status = GateHWSem_S_SUCCESS;
        IArg  key;

    GT_0trace (curTrace, GT_ENTER, "GateHWSem_destroy");

        key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateHWSem_module->refCount < 1) {
                Gate_leaveSystem (key);
        /*! @retval GateHWSem_E_INVALIDSTATE Module was not initialized */
        status = GateHWSem_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (--GateHWSem_module->refCount == 1) {
                        Gate_leaveSystem (key);
            /* Clear cfg area */
            Memory_set ((Ptr) &GateHWSem_module->cfg,
                        0,
                        sizeof (GateHWSem_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "GateHWSem_destroy", status);

    /*! @retval GateHWSem_S_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 */
UInt32 GateHWSem_getNumInstances(Void)
{
    return (GateHWSem_module->numSems);
}

/* Initialize the locks */
Void GateHWSem_locksinit()
{
    UInt32  i;

    for (i = 0; i < GateHWSem_module->numSems; i++) {
        GateHWSem_module->baseAddr[i] = 0;
    }
}

/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      handle  If specified as NULL, default parameters are returned.
 *                      If not NULL, the parameters as configured for this
 *                      instance are returned.
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateHWSem_create
 */
Void
GateHWSem_Params_init (GateHWSem_Params * params)
{
        IArg key;

    GT_1trace (curTrace, GT_ENTER, "GateHWSem_Params_init", params);

    GT_assert (curTrace, (params != NULL));

    key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateHWSem_module->refCount < 1) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_Params_init",
                             GateHWSem_E_INVALIDSTATE,
                             "Module was not initialized!");

        Gate_leaveSystem (key);
    }
    else if (params == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_Params_init",
                             GateHWSem_E_INVALIDARG,
                             "Argument of type (GateHWSem_Params *) is "
                             "NULL!");
        Gate_leaveSystem (key);
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Gate_leaveSystem (key);
            Memory_copy (params,
                         &(GateHWSem_state.defInstParams),
                         sizeof (GateHWSem_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "GateHWSem_Params_init");
}


/*
 *  ======== GateHWSem_Instance_init ========
 */
Int GateHWSem_Instance_init (      GateHWSem_Object *obj,
                                        IGateMPSupport_LocalProtect localProtect,
                                  const GateHWSem_Params *params)
{
    Int32 status = 0;

    IGateProvider_ObjectInitializer (obj, GateHWSem);

    /* Assert that params->resourceId is valid */
    if (params->resourceId >= GateHWSem_module->numSems) {
        status = 1;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_Instance_init",
                             GateHWSem_E_FAIL,
                             "params->resourceId >= GateHWSem_numSems!");
    }

    /* Create the local gate */
    obj->localGate = GateMP_createLocal(localProtect);
    if (obj->localGate == NULL) {
        status = 2;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSem_Instance_init",
                             GateHWSem_E_FAIL,
                             "GateMP_createLocal failed!");
    }

    obj->semNum = params->resourceId;
    obj->nested   = 0;

    /* Check for dynamic open */
    if (!params->openFlag) {
        /* Initialize the HW Semaphore */
        GateHWSem_postInit(obj);
    }

    return (0);
}

/*
 *  ======== GateHWSem_Instance_finalize ========
 */
Void GateHWSem_Instance_finalize(GateHWSem_Object *obj, Int status)
{
}

/*
 *  ======== GateHWSem_enter ========
 */
IArg GateHWSem_enter(GateHWSem_Object *obj)
{
    volatile UInt32 *baseAddr = (volatile UInt32 *)
                                             GateHWSem_module->baseAddr;
    IArg key;

    key = IGateProvider_enter(obj->localGate);

    /* If the gate object has already been entered, return the nested value */
    obj->nested++;
    if (obj->nested > 1) {
        return (key);
    }

    /* Enter the semphore */
    while (baseAddr[obj->semNum] != 1) {
    }

    return (key);
}

Void GateHWSem_setReserved(UInt32 lockNum)
{
    /* Currently doesn't support reserved locks */
    Osal_printf ("WARN:GateHWSem_setReserved() not implemented!");
}

/*
 *  ======== GateHWSem_leave ========
 */
Void GateHWSem_leave(GateHWSem_Object *obj, IArg key)
{
    volatile UInt32 *baseAddr = (volatile UInt32 *)
                                            GateHWSem_module->baseAddr;

    obj->nested--;

    /* Leave the semaphore if the leave() is not nested */
    if (obj->nested == 0) {
        baseAddr[obj->semNum] = 1;
    }

    IGateProvider_leave(obj->localGate, key);
}

/*
 *  ======== GateHWSem_getResourceId ========
 */
Bits32 GateHWSem_getResourceId(GateHWSem_Object *obj)
{
    return (obj->semNum);
}

/*
 *  ======== GateHWSem_sharedMemReq ========
 */
SizeT GateHWSem_sharedMemReq(const IGateMPSupport_Params *params)
{
    return (0);
}

/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */
/*
 *  ======== GateHWSem_getReservedMask ========
 */
Bits32 *GateHWSem_getReservedMask()
{
    /* This gate doesn't allow reserving resources */
    return NULL;
}

/*
 *  ======== GateHWSem_query ========
 */
Bool GateHWSem_query(Int qual)
{
    Bool rc;

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
            /* Depends on gate proxy? */
            rc = TRUE;
            break;

        case IGateProvider_Q_PREEMPTING:
            /* Depends on gate proxy? */
            rc = TRUE;
            break;
        default:
            rc = FALSE;
            break;
    }
    return (rc);
}

/* Override the IObject interface to define craete and delete APIs */
IOBJECT_CREATE1 (GateHWSem, IGateMPSupport_LocalProtect);
