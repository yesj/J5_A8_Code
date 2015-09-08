/*
 *  @file   GateAAMonitor.c
 *
 *  @brief      Gate based on Atomic Access Monitor HW.
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
#include <IGateMPSupport.h>
#include <_GateMP.h>
#include <IObject.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <Bitops.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Cache.h>

/* Module level headers */
#include <ti/syslink/utils/String.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/GateMonitor.h>
#include <GateAAMonitor.h>
#include <_Ipc.h>


#if defined (__cplusplus)
extern "C" {
#endif

#define GateAAMonitor_getLock ti_sdo_ipc_gates_GateAAMonitor_getLock__I
#define GateAAMonitor_SL2_RANGE_BASE 0x200000
#define GateAAMonitor_SL2_RANGE_MAX  0x2bffff
#define GateAAMonitor_CACHELINE_SIZE 64

/* =============================================================================
 * Macros
 * =============================================================================
 */

/* Macro to make a correct module magic number with refCount */
#define GATEAAMONITOR_MAKE_MAGICSTAMP(x) (  (GATEAAMONITOR_MODULEID << 12u)  \
                                           | (x))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for GateAAMonitor module state */
typedef struct GateAAMonitor_Module_State {
    Int32                 refCount;
    /* Reference count */
    GateAAMonitor_Config cfg;
    /* Current config values */
    GateAAMonitor_Config defaultCfg;
    /* default config values */
    GateAAMonitor_Params defInstParams;
    /* default instance paramters */
    UInt32                numMonitors;
    /*!< Maximum number of locks */
} GateAAMonitor_Module_State;

/* Structure defining internal object for the Gate Peterson.*/
struct GateAAMonitor_Object {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    UInt32 *              sharedAddr;
    /* Base address of lock registers */
    UInt                        monNum;
    UInt                        nested;
    IGateProvider_Handle        localGate;
};


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    GateAAMonitor_state
 *
 *  @brief  GateAAMonitor Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateAAMonitor_Module_State GateAAMonitor_state =
{
    .defaultCfg.defaultProtection  = GateAAMonitor_LocalProtect_INTERRUPT,
    .defaultCfg.numMonitors        = 32,
    .defInstParams.resourceId      = 0,
    .defInstParams.regionId        = 0,
    .defInstParams.sharedAddr      = NULL,
    .numMonitors                   = 32u,
};

/*!
 *  @var    GateAAMonitor_state
 *
 *  @brief  GateAAMonitor Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateAAMonitor_Module_State * GateAAMonitor_module = &GateAAMonitor_state;


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the GateAAMonitor module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to GateAAMonitor_setup filled in by
 *              the GateAAMonitor module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the GateAAMonitor module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         GateAAMonitor_setup
 */
Void
GateAAMonitor_getConfig(GateAAMonitor_Config * cfgParams)
{
    IArg key;
    GT_1trace(curTrace, GT_ENTER, "GateAAMonitor_getConfig", cfgParams);

    GT_assert(curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_getConfig",
                            GateAAMonitor_E_INVALIDARG,
                            "Argument of type (GateAAMonitor_Config *) passed "
                            "is null!");
    } else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();
        if (GateAAMonitor_module->refCount == 0) {
            Memory_copy((Ptr) cfgParams,
                        (Ptr) &GateAAMonitor_module->defaultCfg,
                        sizeof (GateAAMonitor_Config));
        } else {
            Memory_copy((Ptr) cfgParams,
                        (Ptr) &GateAAMonitor_module->cfg,
                        sizeof (GateAAMonitor_Config));
        }
        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "GateAAMonitor_getConfig");
}


/*!
 *  @brief      Setup the GateAAMonitor module.
 *
 *              This function sets up the GateAAMonitor module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GateAAMonitor_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GateAAMonitor_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional GateAAMonitor module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GateAAMonitor_destroy, GateAAMonitor_getConfig
 */
Int32
GateAAMonitor_setup(const GateAAMonitor_Config * cfg)
{
    Int32                 status = GateAAMonitor_S_SUCCESS;
    GateAAMonitor_Config  tmpCfg;
    IArg                  key;

    GT_1trace(curTrace, GT_ENTER, "GateAAMonitor_setup", cfg);

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    key = Gate_enterSystem ();
    if (GateAAMonitor_module->refCount > 1) {
        status = GateAAMonitor_S_ALREADYSETUP;
        GT_0trace(curTrace,
                  GT_2CLASS,
                  "GateAAMonitor Module already initialized!");
        GateAAMonitor_module->refCount++;
        Gate_leaveSystem (key);
    }
    else {
        GateAAMonitor_module->refCount++;
        Gate_leaveSystem (key);
        if (cfg == NULL) {
            GateAAMonitor_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        /* Copy the cfg */
        Memory_copy((Ptr) &GateAAMonitor_module->cfg,
                    (Ptr) cfg,
                    sizeof (GateAAMonitor_Config));
        GateAAMonitor_module->numMonitors = cfg->numMonitors;
    }

    GT_1trace (curTrace, GT_LEAVE, "GateAAMonitor_setup", status);

    /*! @retval GateAAMonitor_S_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to destroy the GateAAMonitor module.
 *
 *  @sa         GateAAMonitor_setup
 */
Int32
GateAAMonitor_destroy(Void)
{
    Int32 status = GateAAMonitor_S_SUCCESS;
    IArg  key;

    GT_0trace (curTrace, GT_ENTER, "GateAAMonitor_destroy");

    key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateAAMonitor_module->refCount < 1) {
        Gate_leaveSystem(key);
        /*! @retval GateAAMonitor_E_INVALIDSTATE Module was not initialized */
        status = GateAAMonitor_E_INVALIDSTATE;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_destroy",
                            status,
                            "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (--GateAAMonitor_module->refCount == 1) {
            Gate_leaveSystem (key);
            /* Clear cfg area */
            Memory_set((Ptr) &GateAAMonitor_module->cfg,
                       0,
                       sizeof (GateAAMonitor_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace(curTrace, GT_LEAVE, "GateAAMonitor_destroy", status);

    /*! @retval GateAAMonitor_S_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 */
UInt32 GateAAMonitor_getNumInstances()
{
    return (GateAAMonitor_module->numMonitors);
}

/* Initialize the locks */
Void GateAAMonitor_locksinit()
{
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
 *  @sa         GateAAMonitor_create
 */
Void
GateAAMonitor_Params_init(GateAAMonitor_Params * params)
{
    IArg key;

    GT_1trace(curTrace, GT_ENTER, "GateAAMonitor_Params_init", params);

    GT_assert(curTrace, (params != NULL));

    key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateAAMonitor_module->refCount < 1) {
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_Params_init",
                            GateAAMonitor_E_INVALIDSTATE,
                            "Module was not initialized!");
        Gate_leaveSystem (key);
    }
    else if (params == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_Params_init",
                            GateAAMonitor_E_INVALIDARG,
                            "Argument of type (GateAAMonitor_Params *) is "
                            "NULL!");
        Gate_leaveSystem(key);
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
         Gate_leaveSystem(key);
         Memory_copy(params,
                     &(GateAAMonitor_state.defInstParams),
                     sizeof (GateAAMonitor_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace(curTrace, GT_LEAVE, "GateAAMonitor_Params_init");
}


/*
 *  ======== GateAAMonitor_Instance_init ========
 */
Int GateAAMonitor_Instance_init(GateAAMonitor_Object *obj,
                                IGateMPSupport_LocalProtect localProtect,
                                const GateAAMonitor_Params *params)
{
    Int32 status = 0;

    GT_1trace(curTrace, GT_ENTER, "GateAAMonitor_Instance_init", params);

    IGateProvider_ObjectInitializer(obj, GateAAMonitor);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params->sharedAddr == NULL) {
        status = 1;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_Instance_init",
                            GateAAMonitor_E_FAIL,
                            "params->sharedAddr passed is NULL!");
    } else if (params->resourceId >= GateAAMonitor_module->numMonitors) {
        /* Assert that params->resourceId is valid */
        status = 2;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_Instance_init",
                            GateAAMonitor_E_FAIL,
                            "params->resourceId >= GateAAMonitor_numMonitors!");
    } else if ((params->sharedAddr < (Ptr)GateAAMonitor_SL2_RANGE_BASE) ||
               (params->sharedAddr >  (Ptr)GateAAMonitor_SL2_RANGE_MAX)) {
        status = 3;
        GT_setFailureReason(curTrace,
                            GT_4CLASS,
                            "GateAAMonitor_Instance_init",
                            GateAAMonitor_E_FAIL,
                            "GateAAMonitor address should be in SL2 memory!");
    } else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Create the local gate */
        obj->localGate = GateMP_createLocal(localProtect);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->localGate == NULL) {
            status = 5;
            GT_setFailureReason(curTrace,
                                GT_4CLASS,
                                "GateAAMonitor_Instance_init",
                                GateAAMonitor_E_FAIL,
                                "GateMP_createLocal failed!");
        } else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->sharedAddr = (Ptr)_Ipc_roundup(params->sharedAddr,
                        GateAAMonitor_CACHELINE_SIZE);
            obj->nested     = 0;
            obj->monNum = params->resourceId;
            if (!params->openFlag) {
                /* Creating */
            *(obj->sharedAddr)= 0;
                Cache_wbInv((Ptr)obj->sharedAddr, GateAAMonitor_CACHELINE_SIZE,
                    Cache_Type_ALL, TRUE);
            } else
                Cache_inv((Ptr)obj->sharedAddr, GateAAMonitor_CACHELINE_SIZE,
                    Cache_Type_ALL, TRUE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace(curTrace, GT_LEAVE, "GateAAMonitor_Instance_init");

    return (0);
}

/*
 *  ======== GateAAMonitor_Instance_finalize ========
 */
Void GateAAMonitor_Instance_finalize(GateAAMonitor_Object *obj, Int status)
{
}

/*
 *  ======== GateAAMonitor_enter ========
 */
IArg GateAAMonitor_enter(GateAAMonitor_Object *obj)
{
    IArg key;

    key = IGateProvider_enter(obj->localGate);

    /* If the gate object has already been entered, return the nested value */
    obj->nested++;
    if (obj->nested > 1) {
        return (key);
    }

    /* Enter the Monitor */
    GateAAMonitor_getLock((Ptr)(obj->sharedAddr));

    return (key);
}

Void GateAAMonitor_setReserved(UInt32 lockNum)
{
    /* Currently doesn't support reserved locks */
    Osal_printf ("WARN:GateAAMonitor_setReserved() not implemented!");
}


/*
 *  ======== GateAAMonitor_leave ========
 */
Void GateAAMonitor_leave(GateAAMonitor_Object *obj, IArg key)
{
    UInt32 *baseAddr = (UInt32 *)obj->sharedAddr;
    enum Cache_Mode mode;

    obj->nested--;

    if (obj->nested == 0) {
        mode = Cache_setMode(Cache_Type_L1D, Cache_Mode_FREEZE);

        /* Leave the critical region by setting address value to zero */
        *baseAddr = 0;
        Cache_setMode(Cache_Type_L1D, mode);
    }

    IGateProvider_leave(obj->localGate, key);
}

/*
 *  ======== GateAAMonitor_getResourceId ========
 */
Bits32 GateAAMonitor_getResourceId(GateAAMonitor_Object *obj)
{
    return (obj->monNum);
}

/*
 *  ======== GateAAMonitor_sharedMemReq ========
 */
SizeT GateAAMonitor_sharedMemReq(const IGateMPSupport_Params *params)
{

    SizeT memReq;

    memReq = (SizeT)_Ipc_roundup(GateAAMonitor_CACHELINE_SIZE,
                         SharedRegion_getCacheLineSize(0));

    return(memReq);
}

/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */
/*
 *  ======== GateAAMonitor_getReservedMask ========
 */
Bits32 *GateAAMonitor_getReservedMask()
{
    /* This gate doesn't allow reserving resources */
    return NULL;
}

/*
 *  ======== GateAAMonitor_query ========
 */
Bool GateAAMonitor_query(Int qual)
{
    Bool rc;

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
            /* Depends on gate proxy? */
            rc = FALSE;
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
IOBJECT_CREATE1 (GateAAMonitor, IGateMPSupport_LocalProtect);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
