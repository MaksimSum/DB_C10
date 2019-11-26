﻿#include <assert.h>
#include "isp_types.h"
#include "isp_reg.h"
#include "isp_hw.h"
#include "isp_hw_op.h"
#include "isp.h"
#include "isp_queue.h"
#include "mmp_isp.h"
#include "../../ith/it9070/ith_lcd.h"
#include <pthread.h>

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================
#define OUT_OF_RANGE(value, min, max)   (((value) < (min)) || ((max) < (value)))
#define GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt)    {   \
    if ((pISPctxt) == MMP_NULL)                             \
    {                                                       \
        (result) = ISP_ERR_NOT_INITIALIZE;                  \
        isp_msg_ex(ISP_MSG_TYPE_ERR, " NULL pointer !\n");  \
        goto end;                                           \
    }                                                       \
}

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
//=============================================================================
/**
 * Reference Count of Initialization
 */
//=============================================================================
static MMP_UINT32 RefCount = 0;

//=============================================================================
/**
 * Prevention data of race condition for ISP core.
 */
//=============================================================================
static pthread_mutex_t ISP_CORE_0_MUTEX      = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ISP_CORE_1_MUTEX      = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ISP_CORE_POWER_MUTEX  = PTHREAD_MUTEX_INITIALIZER;

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * ISP context initialization.
 *
 * @param ispCore  indicate which ISP core will run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API first when it want to use ISP API.
 */
//=============================================================================
ISP_RESULT
mmpIspInitialize(
    ISP_DEVICE   *ptDev,
    MMP_ISP_CORE ispCore)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = NULL;

    assert(ptDev);

    if ((*ptDev) == MMP_NULL)
        *ptDev = isp_Malloc(sizeof(ISP_CONTEXT));

    if ((*ptDev) == MMP_NULL)
    {
        result = ISP_ERR_CONTEXT_ALLOC_FAIL;
        goto end;
    }

    pISPctxt          = (ISP_CONTEXT *)*ptDev;
    pISPctxt->ispCore = ispCore;

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        pthread_mutex_lock(&ISP_CORE_0_MUTEX);
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        pthread_mutex_lock(&ISP_CORE_1_MUTEX);

    result = ISP_WaitEngineIdle(pISPctxt);
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);

        if (pISPctxt->ispCore == MMP_ISP_CORE_0)
            pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
        else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
            pthread_mutex_unlock(&ISP_CORE_1_MUTEX);

        goto end;
    }

    //ISP_ResetEngine(pISPctxt->ispCore);
    pthread_mutex_lock(&ISP_CORE_POWER_MUTEX);
    RefCount++;
    if (RefCount == 1)
        ISP_PowerUp();
    pthread_mutex_unlock(&ISP_CORE_POWER_MUTEX);

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        pthread_mutex_unlock(&ISP_CORE_1_MUTEX);

    ISP_CmdSelect(pISPctxt, &gtIspHwOpMMIO);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * ISP powerdown.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving ISP module.
 */
//=============================================================================
ISP_RESULT
mmpIspPowerdown(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    //
    // Disable ISP engine
    //
    ISP_PowerDown();

    //
    // Disable video flip and enable HW flip
    //
    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO)
    {
        isp_LcdEnableVideoFlip(MMP_FALSE);
    }

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * ISP resetengine.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving ISP module.
 */
//=============================================================================
ISP_RESULT
mmpIspResetEngine(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    //
    // Reset ISP engine
    //
    ISP_ResetEngine(pISPctxt->ispCore);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * ISP terminate.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving ISP module.
 */
//=============================================================================
ISP_RESULT
mmpIspTerminate(
    ISP_DEVICE *ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)*ptDev;

    if (ptDev == MMP_NULL || pISPctxt == MMP_NULL)
    {
        return result;
    }

    //
    // Disable video flip and enable HW flip
    //
    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO)
    {
        isp_LcdEnableVideoFlip(MMP_FALSE);
    }

    isp_Free(pISPctxt);
    *ptDev = MMP_NULL;

    //ISP_ResetEngine(pISPctxt->ispCore);
    pthread_mutex_lock(&ISP_CORE_POWER_MUTEX);
    RefCount--;
    if (RefCount == 0)
    {
        ISP_PowerDown();
    }
    else if (RefCount == -1)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, "RefCount err 0x%x !\n", RefCount);
        RefCount = 0;
    }
    pthread_mutex_unlock(&ISP_CORE_POWER_MUTEX);

    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set or change ISP multiple output.
 *
 * @param coreInfo  indicate which core setting will run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetCore(
    ISP_DEVICE        ptDev,
    MMP_ISP_CORE_INFO *coreInfo)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    ISP_ContextInitialize(pISPctxt, coreInfo);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set or change ISP mode.
 *
 * @param ispMode  indicate which mode will run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetMode(
    ISP_DEVICE   ptDev,
    MMP_ISP_MODE ispMode)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    /*if ((pISPctxt->ispMode != MMP_ISP_MODE_NONE) &&
        (pISPctxt->ispMode != ispMode))
    {
        // Reset the context.
        result = mmpIspInitialize(&ptDev);  // ... 需要重新 reset isp h/w 嗎？
        if (result)
        {
            isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
            goto end;
        }
    }*/

    pISPctxt->ispMode = ispMode;

    //
    //set isp engine block/line mode
    //
    switch (pISPctxt->ispMode)
    {
    case MMP_ISP_MODE_PLAY_VIDEO:
        pISPctxt->EngineMode.EnableBlockMode      = MMP_FALSE;
        pISPctxt->EngineMode.EnableReadMemoryMode = MMP_TRUE;
        pISPctxt->EngineMode.EnableJPEGDECODE     = MMP_FALSE;
        pISPctxt->EngineMode.EnableRawDataMode    = MMP_FALSE;
        //isp_LcdEnableVideoFlip(MMP_TRUE);
        break;

    // case MMP_ISP_MODE_MOTION_JPEG:
    //     pISPctxt->EngineMode.EnableBlockMode = MMP_FALSE;
    //     pISPctxt->EngineMode.EnableJPEGDECODE = MMP_TRUE;
    //     pISPctxt->EngineMode.EnableRawDataMode = MMP_FALSE;
    //     break;

    case MMP_ISP_MODE_SHOW_IMAGE:
        pISPctxt->EngineMode.EnableBlockMode      = MMP_FALSE;
        pISPctxt->EngineMode.EnableReadMemoryMode = MMP_TRUE;
        pISPctxt->EngineMode.EnableJPEGDECODE     = MMP_TRUE;
        pISPctxt->EngineMode.EnableRawDataMode    = MMP_FALSE;
        break;

    // case MMP_ISP_MODE_JPEG_RESIZE:
    //     pISPctxt->EngineMode.EnableBlockMode = MMP_FALSE;
    //     pISPctxt->EngineMode.EnableJPEGDECODE = MMP_TRUE;
    //     pISPctxt->EngineMode.EnableRawDataMode = MMP_FALSE;
    //     break;

    case MMP_ISP_MODE_TRANSFORM:
    case MMP_ISP_MODE_NONE:
        pISPctxt->EngineMode.EnableBlockMode      = MMP_FALSE;
        pISPctxt->EngineMode.EnableReadMemoryMode = MMP_TRUE;
        pISPctxt->EngineMode.EnableJPEGDECODE     = MMP_FALSE;
        pISPctxt->EngineMode.EnableRawDataMode    = MMP_FALSE;
        break;

    case MMP_ISP_MODE_SOFTWARE_HANDSHAKE:
        pISPctxt->EngineMode.EnableBlockMode      = MMP_FALSE;
        pISPctxt->EngineMode.EnableReadMemoryMode = MMP_TRUE;
        pISPctxt->EngineMode.EnableJPEGDECODE     = MMP_FALSE;
        pISPctxt->EngineMode.EnableRawDataMode    = MMP_TRUE;
        break;
    }

    ISP_SetColorMatrix(pISPctxt);

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_EngineMode;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable ISP capability.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP capability.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnable(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (cap)
    {
    case MMP_ISP_DEINTERLACE:
        pISPctxt->DeInterlace.Enable            = MMP_TRUE;
        pISPctxt->DeInterlace.UVRepeatMode      = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_LOWLEVELEDGE:
        pISPctxt->DeInterlace.EnLowLevelEdge    = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_FRAME_FUNCTION_0:
        pISPctxt->FrameFun0.Enable              = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun0;
        break;

    case MMP_ISP_FRAME_FUNCTION_1:
        pISPctxt->FrameFun1.Enable              = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun1;
        break;

    case MMP_ISP_CONST_DATA_BLEND_0:
        pISPctxt->FrameFun0.EnableBlendConst    = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun0;
        break;

    case MMP_ISP_CONST_DATA_BLEND_1:
        pISPctxt->FrameFun1.EnableBlendConst    = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun1;
        break;

    /*case MMP_ISP_SUBTITLE_0:
        pISPctxt->SubTitle0.Enable              = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_SubTitle0;
        break;

    case MMP_ISP_SUBTITLE_1:
        pISPctxt->SubTitle1.Enable              = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_SubTitle1;
        break;*/

    case MMP_ISP_CLIPPING0:
        pISPctxt->ClipFun0.Enable               = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip0Fun;
        break;

    case MMP_ISP_CLIPPING1:
        pISPctxt->ClipFun1.Enable               = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip1Fun;
        break;

    case MMP_ISP_CLIPPING2:
        pISPctxt->ClipFun2.Enable               = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip2Fun;
        break;

    case MMP_ISP_DISABLE_VIDEO_OUT:
        pISPctxt->OutInfo.DisbleVideoOut        = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_QFIRE_MODE:
        pISPctxt->OutInfo.EnableQueueFire       = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        pISPctxt->WorkFlags                    |= ISP_FLAGS_QUEUE_FIRE_INIT;
        break;

    case MMP_ISP_AUTO_SWAP_FIELD:
        pISPctxt->DeInterlace.EnAutoSwapField   = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_FIELD_SCALE_MODE:
        pISPctxt->OutInfo.EnableFieldScale      = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_ONFLY_WRITE_MEMORY:
        pISPctxt->OutInfo.EnableOnFlyWriteMem   = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_DOUBLE_FRAMERATE_MODE:
        pISPctxt->OutInfo.EnableDoubleFrameRate = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_NEGATIVE_EFFECT_MODE:
        pISPctxt->OutInfo.EnableNegative        = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_SOFTWARE_WRITE_FLIP_MODE:
        pISPctxt->OutInfo.EnableSWFlipMode      = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_OUTPUT_FIELD_MODE:
        pISPctxt->OutInfo.EnableOutputFieldMode = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_EngineMode;
        break;

    case MMP_ISP_KEEP_LAST_FIELD_MODE:
        pISPctxt->OutInfo.EnableKeepLastField   = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_PREVIEW_VIDEO_MODE:
        pISPctxt->OutInfo.EnableVideoPreview    = MMP_TRUE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_InputBuf;
        break;

    case MMP_ISP_UV_REPEAT_MODE:
        break;

    default:
        result = ISP_ERR_NO_MATCH_ENABLE_TYPE;
        break;
    }

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable ISP capability.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP capability.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisable(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (cap)
    {
    case MMP_ISP_DEINTERLACE:
        pISPctxt->DeInterlace.Enable            = MMP_FALSE;
        pISPctxt->DeInterlace.UVRepeatMode      = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_LOWLEVELEDGE:
        pISPctxt->DeInterlace.EnLowLevelEdge    = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_FRAME_FUNCTION_0:
        pISPctxt->FrameFun0.Enable              = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun0;
        break;

    case MMP_ISP_FRAME_FUNCTION_1:
        pISPctxt->FrameFun1.Enable              = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun1;
        break;

    case MMP_ISP_CONST_DATA_BLEND_0:
        pISPctxt->FrameFun0.EnableBlendConst    = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun0;
        break;

    case MMP_ISP_CONST_DATA_BLEND_1:
        pISPctxt->FrameFun1.EnableBlendConst    = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_FrameFun1;
        break;

    /*case MMP_ISP_SUBTITLE_0:
        pISPctxt->SubTitle0.Enable              = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_SubTitle0;
        break;

    case MMP_ISP_SUBTITLE_1:
        pISPctxt->SubTitle1.Enable              = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_SubTitle1;
        break;*/

    case MMP_ISP_CLIPPING0:
        pISPctxt->ClipFun0.Enable               = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip0Fun;
        break;

    case MMP_ISP_CLIPPING1:
        pISPctxt->ClipFun1.Enable               = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip1Fun;
        break;

    case MMP_ISP_CLIPPING2:
        pISPctxt->ClipFun2.Enable               = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_Clip2Fun;
        break;

    case MMP_ISP_DISABLE_VIDEO_OUT:
        pISPctxt->OutInfo.DisbleVideoOut        = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_QFIRE_MODE:
        pISPctxt->OutInfo.EnableQueueFire       = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        pISPctxt->WorkFlags                    &= ~ISP_FLAGS_QUEUE_FIRE_INIT;
        break;

    case MMP_ISP_ONFLY_WRITE_MEMORY:
        pISPctxt->OutInfo.EnableOnFlyWriteMem   = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_AUTO_SWAP_FIELD:
        pISPctxt->DeInterlace.EnAutoSwapField   = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;
        break;

    case MMP_ISP_FIELD_SCALE_MODE:
        pISPctxt->OutInfo.EnableFieldScale      = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_DOUBLE_FRAMERATE_MODE:
        pISPctxt->OutInfo.EnableDoubleFrameRate = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_NEGATIVE_EFFECT_MODE:
        pISPctxt->OutInfo.EnableNegative        = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_SOFTWARE_WRITE_FLIP_MODE:
        pISPctxt->OutInfo.EnableSWFlipMode      = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_OUTPUT_FIELD_MODE:
        pISPctxt->OutInfo.EnableOutputFieldMode = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_EngineMode;
        break;

    case MMP_ISP_KEEP_LAST_FIELD_MODE:
        pISPctxt->OutInfo.EnableKeepLastField   = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_OutParameter;
        break;

    case MMP_ISP_PREVIEW_VIDEO_MODE:
        pISPctxt->OutInfo.EnableVideoPreview    = MMP_FALSE;
        pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_InputBuf;
        break;

    case MMP_ISP_UV_REPEAT_MODE:
        break;

    default:
        result = ISP_ERR_NO_MATCH_ENABLE_TYPE;
        break;
    }

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Query ISP capability.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP capability.
 * @return MMP_TRUE if function enabled, MMP_FALSE if function disable.
 */
//=============================================================================
MMP_BOOL
mmpIspQuery(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap)
{
    MMP_BOOL    result    = MMP_FALSE;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (cap)
    {
    case MMP_ISP_DEINTERLACE:
        result = pISPctxt->DeInterlace.Enable;
        break;

    case MMP_ISP_LOWLEVELEDGE:
        result = pISPctxt->DeInterlace.EnLowLevelEdge;
        break;

    case MMP_ISP_FRAME_FUNCTION_0:
        result = pISPctxt->FrameFun0.Enable;
        break;

    case MMP_ISP_FRAME_FUNCTION_1:
        result = pISPctxt->FrameFun1.Enable;
        break;

    case MMP_ISP_CONST_DATA_BLEND_0:
        result = pISPctxt->FrameFun0.EnableBlendConst;
        break;

    case MMP_ISP_CONST_DATA_BLEND_1:
        result = pISPctxt->FrameFun1.EnableBlendConst;
        break;

    case MMP_ISP_SUBTITLE_0:
        result = pISPctxt->SubTitle0.Enable;
        break;

    case MMP_ISP_SUBTITLE_1:
        result = pISPctxt->SubTitle1.Enable;
        break;

    case MMP_ISP_CLIPPING0:
        result = pISPctxt->ClipFun0.Enable;
        break;

    case MMP_ISP_CLIPPING1:
        result = pISPctxt->ClipFun1.Enable;
        break;

    case MMP_ISP_CLIPPING2:
        result = pISPctxt->ClipFun2.Enable;
        break;

    case MMP_ISP_DISABLE_VIDEO_OUT:
        result = pISPctxt->OutInfo.DisbleVideoOut;
        break;

    case MMP_ISP_QFIRE_MODE:
        result = pISPctxt->OutInfo.EnableQueueFire;
        break;

    case MMP_ISP_AUTO_SWAP_FIELD:
        result = pISPctxt->DeInterlace.EnAutoSwapField;
        break;

    case MMP_ISP_FIELD_SCALE_MODE:
        result = pISPctxt->OutInfo.EnableFieldScale;
        break;

    case MMP_ISP_ONFLY_WRITE_MEMORY:
        result = pISPctxt->OutInfo.EnableOnFlyWriteMem;
        break;

    case MMP_ISP_DOUBLE_FRAMERATE_MODE:
        result = pISPctxt->OutInfo.EnableDoubleFrameRate;
        break;

    case MMP_ISP_NEGATIVE_EFFECT_MODE:
        result = pISPctxt->OutInfo.EnableNegative;
        break;

    case MMP_ISP_SOFTWARE_WRITE_FLIP_MODE:
        result = pISPctxt->OutInfo.EnableSWFlipMode;
        break;

    case MMP_ISP_OUTPUT_FIELD_MODE:
        result = pISPctxt->OutInfo.EnableOutputFieldMode;
        break;

    case MMP_ISP_CAP_ONFLY_MODE:
        result = pISPctxt->EngineMode.EnableCaptureOnFly;
        break;

    case MMP_ISP_LCD_ONFLY_MODE:
        result = pISPctxt->OutInfo.EnableLcdOnFly;
        break;

    case MMP_ISP_ISR_FIRE_MODE:
        result = pISPctxt->EnableISRFire;
        break;

    default:
        result = MMP_FALSE;
        break;
    }

end:
    return result;
}

//=============================================================================
/**
 * Set the value of the specified color control type.
 *
 * @param colorControl  Specifies a color control type.
 * @param value         The adjusted value related to specified color control.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Calling the mmpIspUpdate() method to active it.
 * @see MMP_ISP_COLOR_CONTROL
 */
//=============================================================================
ISP_RESULT
mmpIspSetColorCtrl(
    ISP_DEVICE         ptDev,
    MMP_ISP_COLOR_CTRL colorCtrl,
    MMP_INT            value)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (colorCtrl)
    {
    case MMP_ISP_HUE:
        if (OUT_OF_RANGE(value, 0, 359))
        {
            result = ISP_ERR_COLOR_CTRL_OUT_OF_RANGE;
            isp_msg_ex(ISP_MSG_TYPE_ERR, "Hue value (%d) out range !!", value);
        }
        else
            pISPctxt->hue = (MMP_INT16) value;
        break;

    case MMP_ISP_CONTRAST:
        if (OUT_OF_RANGE(value, -64, 63))
        {
            result = ISP_ERR_COLOR_CTRL_OUT_OF_RANGE;
            isp_msg_ex(ISP_MSG_TYPE_ERR, "Contrast value (%d) out range !!", value);
        }
        else
            pISPctxt->contrast = (MMP_INT16) value;
        break;

    case MMP_ISP_SATURATION:
        if (OUT_OF_RANGE(value, 0, 255))
        {
            result = ISP_ERR_COLOR_CTRL_OUT_OF_RANGE;
            isp_msg_ex(ISP_MSG_TYPE_ERR, "Saturation value (%d) out range !!", value);
        }
        else
            pISPctxt->saturation = (MMP_INT16) value;
        break;

    case MMP_ISP_BRIGHTNESS:
        if (OUT_OF_RANGE(value, -64, 63))
        {
            result = ISP_ERR_COLOR_CTRL_OUT_OF_RANGE;
            isp_msg_ex(ISP_MSG_TYPE_ERR, "Brightness value (%d) out range !!", value);
        }
        else
            pISPctxt->brightness = (MMP_INT16) value;
        break;

    default:
        result = ISP_ERR_UNSUPPORTED_COLOR_CTRL;
        isp_msg_ex(ISP_MSG_TYPE_ERR, "Wrong color type !!");
        break;
    }

#if 0
    if (result == ISP_SUCCESS)
    {
        ISP_SetColorCorrMatrix((void *)&pISPctxt->CCFun,
                               pISPctxt->hue,
                               pISPctxt->saturation,
                               pISPctxt->contrast,
                               pISPctxt->midPoint,
                               pISPctxt->colorEffect,
                               pISPctxt->brightness,
                               0);//pISPctxt->InInfo.EnableOnFlyOutYUV);
    }

    pISPctxt->UpdateFlags        |= ISP_FLAGS_UPDATE_CCMatrix;
    pISPctxt->OutInfo.EnableCCFun = MMP_TRUE;
    pISPctxt->UpdateFlags        |= ISP_FLAGS_UPDATE_OutParameter;
#endif

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Extend Destination.
 *
 * @param TopPixel    extend pixel of top.
 * @param DownPixel   extend pixel of down.
 * @param LeftPixel   extend pixel of left.
 * @param RightPixel  extend pixel of right.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetExtendDestination(
    ISP_DEVICE ptDev,
    MMP_UINT   LeftPixel,
    MMP_UINT   RightPixel,
    MMP_UINT   TopPixel,
    MMP_UINT   DownPixel)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->InInfo.DstExtedTop   = TopPixel;
    pISPctxt->InInfo.DstExtedDown  = DownPixel;
    pISPctxt->InInfo.DstExtedLeft  = LeftPixel;
    pISPctxt->InInfo.DstExtedRight = RightPixel;

    pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_InputBuf;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Extend Source.
 *
 * @param TopPixel    extend pixel of top.
 * @param DownPixel   extend pixel of down.
 * @param LeftPixel   extend pixel of left.
 * @param RightPixel  extend pixel of right.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetExtendSource(
    ISP_DEVICE ptDev,
    MMP_UINT   LeftPixel,
    MMP_UINT   RightPixel,
    MMP_UINT   TopPixel,
    MMP_UINT   DownPixel)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->InInfo.SrcExtedTop   = TopPixel;
    pISPctxt->InInfo.SrcExtedDown  = DownPixel;
    pISPctxt->InInfo.SrcExtedLeft  = LeftPixel;
    pISPctxt->InInfo.SrcExtedRight = RightPixel;

    pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_InputBuf;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set display window.
 *
 * @param vramAddr  base address of the destination buffer.
 * @param startX    starting X position at the output buffer.
 * @param startY    starting Y position at the output buffer.
 * @param width     width of content at the output buffer.
 * @param height    height of content at the output buffer.
 * @param pitch     pitch of the destination buffer.
 * @param format    format of the destination buffer.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetOutputWindow(
    ISP_DEVICE          ptDev,
    MMP_ISP_OUTPUT_INFO *outInfo)
{
    ISP_RESULT  result       = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt    = (ISP_CONTEXT *)ptDev;
    MMP_UINT32  offset       = 0;
    MMP_UINT16  bytePrePixel = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (outInfo == MMP_NULL)
    {
        result = ISP_ERR_INVALID_INPUT_PARAM;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " NULL pointer !\n");
        goto end;
    }

    if ((outInfo->width == 0) || (outInfo->height == 0))
    {
        result = ISP_ERR_INVALID_DISPLAY_WINDOW;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err outW = %d, outH = %d !\n", outInfo->width, outInfo->height);
        goto end;
    }

    mmpIspSetOutputFormat(ptDev, outInfo->format);

    switch (outInfo->format)
    {
    case MMP_ISP_OUT_DITHER565A:
    case MMP_ISP_OUT_DITHER565B:
    case MMP_ISP_OUT_DITHER444A:
    case MMP_ISP_OUT_DITHER444B:
    case MMP_ISP_OUT_RGB565:
        bytePrePixel                = 2;
        pISPctxt->OutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchRGB;
        pISPctxt->OutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->OutInfo.Height    = (MMP_UINT16)outInfo->height;

        offset                      = outInfo->startY * pISPctxt->OutInfo.PitchYRGB + outInfo->startX * bytePrePixel;

        pISPctxt->OutInfo.Addr0     = (MMP_UINT8 *)((MMP_UINT32)outInfo->addrRGB + offset);
        break;

    case MMP_ISP_OUT_RGB888:
        bytePrePixel                = 4;
        pISPctxt->OutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchRGB;
        pISPctxt->OutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->OutInfo.Height    = (MMP_UINT16)outInfo->height;

        offset                      = outInfo->startY * pISPctxt->OutInfo.PitchYRGB + outInfo->startX * bytePrePixel;

        pISPctxt->OutInfo.Addr0     = (MMP_UINT8 *)((MMP_UINT32)outInfo->addrRGB + offset);
        break;

    case MMP_ISP_OUT_YUYV:
    case MMP_ISP_OUT_YVYU:
    case MMP_ISP_OUT_UYVY:
    case MMP_ISP_OUT_VYUY:
        pISPctxt->OutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchY;
        pISPctxt->OutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->OutInfo.Height    = (MMP_UINT16)outInfo->height;

        pISPctxt->OutInfo.Addr0     = (MMP_UINT8 *)outInfo->addrY;
        break;

    case MMP_ISP_OUT_YUV420:
    case MMP_ISP_OUT_YUV422:
    case MMP_ISP_OUT_YUV444:
        pISPctxt->OutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchY;
        pISPctxt->OutInfo.PitchUV   = (MMP_UINT16)outInfo->pitchUv;
        pISPctxt->OutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->OutInfo.Height    = (MMP_UINT16)outInfo->height;

        pISPctxt->OutInfo.Addr0     = (MMP_UINT8 *)outInfo->addrY;
        pISPctxt->OutInfo.Addr1     = (MMP_UINT8 *)outInfo->addrU;
        pISPctxt->OutInfo.Addr2     = (MMP_UINT8 *)outInfo->addrV;
        break;

    case MMP_ISP_OUT_NV12:
    case MMP_ISP_OUT_NV21:
        pISPctxt->OutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchY;
        pISPctxt->OutInfo.PitchUV   = (MMP_UINT16)outInfo->pitchUv;
        pISPctxt->OutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->OutInfo.Height    = (MMP_UINT16)outInfo->height;

        pISPctxt->OutInfo.Addr0     = (MMP_UINT8 *)outInfo->addrY;
        pISPctxt->OutInfo.Addr1     = (MMP_UINT8 *)outInfo->addrU;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        goto end;
        break;
    }

    pISPctxt->OutInfo.EnableDoubleBuf = MMP_FALSE;
    pISPctxt->OutInfo.EnableTripleBuf = MMP_FALSE;

    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_OutParameter;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_OutBufInfo;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_OutAddress;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set preview display window.
 *
 * @param vramAddr  base address of the destination buffer.
 * @param startX    starting X position at the output buffer.
 * @param startY    starting Y position at the output buffer.
 * @param width     width of content at the output buffer.
 * @param height    height of content at the output buffer.
 * @param pitch     pitch of the destination buffer.
 * @param format    format of the destination buffer. only support RGB and YUV Packet.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetPVOutputWindow(
    ISP_DEVICE          ptDev,
    MMP_ISP_OUTPUT_INFO *outInfo)
{
    ISP_RESULT  result       = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt    = (ISP_CONTEXT *)ptDev;
    MMP_UINT32  offset       = 0;
    MMP_UINT16  bytePrePixel = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (outInfo == MMP_NULL)
    {
        result = ISP_ERR_INVALID_INPUT_PARAM;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " NULL pointer !\n");
        goto end;
    }

    if ((outInfo->width == 0) || (outInfo->height == 0))
    {
        result = ISP_ERR_INVALID_DISPLAY_WINDOW;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err outW = %d, outH = %d !\n", outInfo->width, outInfo->height);
        goto end;
    }

    mmpIspSetPVOutputFormat(ptDev, outInfo->format);

    switch (outInfo->format)
    {

    case MMP_ISP_OUT_DITHER565A:
    case MMP_ISP_OUT_DITHER565B:
    case MMP_ISP_OUT_DITHER444A:
    case MMP_ISP_OUT_DITHER444B:
    case MMP_ISP_OUT_RGB565:
        bytePrePixel                  = 2;
        pISPctxt->PVOutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchRGB;
        pISPctxt->PVOutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->PVOutInfo.Height    = (MMP_UINT16)outInfo->height;

        offset                        = outInfo->startY * pISPctxt->PVOutInfo.PitchYRGB + outInfo->startX * bytePrePixel;

        pISPctxt->PVOutInfo.Addr0     = (MMP_UINT8 *)((MMP_UINT32)outInfo->addrRGB + offset);
        break;

    case MMP_ISP_OUT_RGB888:
        bytePrePixel                  = 4;
        pISPctxt->PVOutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchRGB;
        pISPctxt->PVOutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->PVOutInfo.Height    = (MMP_UINT16)outInfo->height;

        offset                        = outInfo->startY * pISPctxt->PVOutInfo.PitchYRGB + outInfo->startX * bytePrePixel;

        pISPctxt->PVOutInfo.Addr0     = (MMP_UINT8 *)((MMP_UINT32)outInfo->addrRGB + offset);
        break;

    case MMP_ISP_OUT_YUYV:
    case MMP_ISP_OUT_YVYU:
    case MMP_ISP_OUT_UYVY:
    case MMP_ISP_OUT_VYUY:
        pISPctxt->PVOutInfo.PitchYRGB = (MMP_UINT16)outInfo->pitchY;
        pISPctxt->PVOutInfo.Width     = (MMP_UINT16)outInfo->width;
        pISPctxt->PVOutInfo.Height    = (MMP_UINT16)outInfo->height;

        pISPctxt->PVOutInfo.Addr0     = (MMP_UINT8 *)outInfo->addrY;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        goto end;
        break;
    }

    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_PVOutParameter;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_PVOutBufInfo;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_PVOutAddress;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set display window.
 *
 * @param startX  starting X position for display.
 * @param startY  starting Y position for display.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetDisplayWindow(
    ISP_DEVICE ptDev,
    MMP_INT    startX,
    MMP_INT    startY,
    MMP_UINT   width,
    MMP_UINT   height)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;
    MMP_UINT32  offset;
    MMP_UINT16  pitch;
    MMP_UINT16  bytePerPixel;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if ((width == 0) || (height == 0))
    {
        result = ISP_ERR_INVALID_DISPLAY_WINDOW;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err outW = %d, outH = %d !\n", width, height);
        goto end;
    }

#ifdef CFG_VP_TEST_MODULE_ENABLE
    pitch                             = 0;
    bytePerPixel                      = 0;
#else
    pitch                             = (MMP_UINT16)isp_LcdGetPitch();
    bytePerPixel                      = pitch / isp_LcdGetWidth();
#endif

    pISPctxt->OutInfo.PitchYRGB       = (MMP_UINT16)pitch;
    pISPctxt->OutInfo.Width           = (MMP_UINT16)width;
    pISPctxt->OutInfo.Height          = (MMP_UINT16)height;

    offset = startY * pISPctxt->OutInfo.PitchYRGB + startX * bytePerPixel;

    pISPctxt->OutInfo.Addr0           = (MMP_UINT8 *)(isp_LcdGetBaseAddr_A() + offset);
    pISPctxt->OutInfo.EnableDoubleBuf = MMP_FALSE;
    pISPctxt->OutInfo.EnableTripleBuf = MMP_FALSE;

    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO || pISPctxt->ispMode == MMP_ISP_MODE_MOTION_JPEG)
    {
        pISPctxt->OutInfo.Addr1           = (MMP_UINT8 *)(isp_LcdGetBaseAddr_B() + offset);
        pISPctxt->OutInfo.EnableDoubleBuf = MMP_TRUE;
        pISPctxt->OutInfo.EnableTripleBuf = MMP_FALSE;

        // Set LCD VideoFlip Mode
        isp_LcdEnableVideoFlip(MMP_TRUE);

#if defined(LCD_TRIPLE_BUFFER)
    #error "Lcd no LCD_PROPERTY_BASE_C"
        pISPctxt->OutInfo.Addr2           = (MMP_UINT8 *)(isp_LcdGetBaseAddr_C() + offset);
        pISPctxt->OutInfo.EnableDoubleBuf = MMP_FALSE;
        pISPctxt->OutInfo.EnableTripleBuf = MMP_TRUE;
#endif

        //Set Scale Output Size
        mmpIspSetVideoWindow(ptDev, 0, 0, width, height);
    }

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutParameter;
    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutBufInfo;
    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutAddress;

    pISPctxt->WorkFlags   |= ISP_FLAGS_FIRST_VIDEO_FRAME;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set display window.
 *
 * @param startX  starting X position for display.
 * @param startY  starting Y position for display.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetVideoWindow(
    ISP_DEVICE ptDev,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if ((width == 0) || (height == 0) ||
        (pISPctxt->OutInfo.Width  < (startX + width )) ||
        (pISPctxt->OutInfo.Height < (startY + height)))
    {
        result = ISP_ERR_INVALID_DISPLAY_WINDOW;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err param(x,y,w,h)=(%d, %d, %d, %d), out(w,h)=(%d, %d) !\n",
                   startX, startY, width, height,
                   pISPctxt->OutInfo.Width,
                   pISPctxt->OutInfo.Height);
        goto end;
    }

    pISPctxt->ScaleFun.DstPosX   = (MMP_UINT16)startX;
    pISPctxt->ScaleFun.DstPosY   = (MMP_UINT16)startY;
    pISPctxt->ScaleFun.DstWidth  = (MMP_UINT16)width;
    pISPctxt->ScaleFun.DstHeight = (MMP_UINT16)height;

    pISPctxt->UpdateFlags       |= ISP_FLAGS_UPDATE_ScaleParam;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set preview display window.
 *
 * @param startX  starting X position for display.
 * @param startY  starting Y position for display.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetPVVideoWindow(
    ISP_DEVICE ptDev,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if ((width == 0) || (height == 0) ||
        (pISPctxt->PVOutInfo.Width  < (startX + width )) ||
        (pISPctxt->PVOutInfo.Height < (startY + height)))
    {
        result = ISP_ERR_INVALID_DISPLAY_WINDOW;
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err param(x,y,w,h)=(%d, %d, %d, %d), out(w,h)=(%d, %d) !\n",
                   startX, startY, width, height,
                   pISPctxt->PVOutInfo.Width,
                   pISPctxt->PVOutInfo.Height);
        goto end;
    }

    pISPctxt->PVScaleFun.DstPosX   = (MMP_UINT16)startX;
    pISPctxt->PVScaleFun.DstPosY   = (MMP_UINT16)startY;
    pISPctxt->PVScaleFun.DstWidth  = (MMP_UINT16)width;
    pISPctxt->PVScaleFun.DstHeight = (MMP_UINT16)height;

    pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_PVScaleParam;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set isp output format.
 *
 * @param format  format of the destination buffer.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 **/
//=============================================================================
ISP_RESULT
mmpIspSetOutputFormat(
    ISP_DEVICE        ptDev,
    MMP_ISP_OUTFORMAT format)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (format)
    {
    case MMP_ISP_OUT_DITHER565A:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = Dither565;
        pISPctxt->OutInfo.DitherMode   = MMP_FALSE;
        break;

    case MMP_ISP_OUT_DITHER565B:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = Dither565;
        pISPctxt->OutInfo.DitherMode   = MMP_TRUE;
        break;

    case MMP_ISP_OUT_DITHER444A:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = Dither444;
        pISPctxt->OutInfo.DitherMode   = MMP_FALSE;
        break;

    case MMP_ISP_OUT_DITHER444B:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = Dither444;
        pISPctxt->OutInfo.DitherMode   = MMP_TRUE;
        break;

    case MMP_ISP_OUT_RGB565:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = NoDither565;
        break;

    case MMP_ISP_OUT_RGB888:
        pISPctxt->OutInfo.OutFormat    = RGBPacket;
        pISPctxt->OutInfo.RGBFormat    = NoDither888;
        break;

    case MMP_ISP_OUT_YUYV:
        pISPctxt->OutInfo.OutFormat    = YUVPacket;
        pISPctxt->OutInfo.PacketFormat = YUYV;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YVYU:
        pISPctxt->OutInfo.OutFormat    = YUVPacket;
        pISPctxt->OutInfo.PacketFormat = YVYU;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_UYVY:
        pISPctxt->OutInfo.OutFormat    = YUVPacket;
        pISPctxt->OutInfo.PacketFormat = UYVY;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_VYUY:
        pISPctxt->OutInfo.OutFormat    = YUVPacket;
        pISPctxt->OutInfo.PacketFormat = VYUY;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YUV422:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV422;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YUV420:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV420;
        pISPctxt->OutInfo.EnableNVMode = MMP_FALSE;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YUV444:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV444;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YUV422R:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV422R;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_NV12:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV420;
        pISPctxt->OutInfo.EnableNVMode = MMP_TRUE;
        pISPctxt->OutInfo.NVFormat     = NV12;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_NV21:
        pISPctxt->OutInfo.OutFormat    = YUVPlane;
        pISPctxt->OutInfo.PlaneFormat  = YUV420;
        pISPctxt->OutInfo.EnableNVMode = MMP_TRUE;
        pISPctxt->OutInfo.NVFormat     = NV21;
        pISPctxt->UpdateFlags         |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        break;
    }

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set preview output format of isp.
 *
 * @param format  format of the destination buffer. only support RGB and YUV Packet.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 **/
//=============================================================================
ISP_RESULT
mmpIspSetPVOutputFormat(
    ISP_DEVICE        ptDev,
    MMP_ISP_OUTFORMAT format)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (format)
    {

    case MMP_ISP_OUT_DITHER565A:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = Dither565;
        pISPctxt->PVOutInfo.DitherMode   = MMP_FALSE;
        break;

    case MMP_ISP_OUT_DITHER565B:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = Dither565;
        pISPctxt->PVOutInfo.DitherMode   = MMP_TRUE;
        break;

    case MMP_ISP_OUT_DITHER444A:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = Dither444;
        pISPctxt->PVOutInfo.DitherMode   = MMP_FALSE;
        break;

    case MMP_ISP_OUT_DITHER444B:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = Dither444;
        pISPctxt->PVOutInfo.DitherMode   = MMP_TRUE;
        break;

    case MMP_ISP_OUT_RGB565:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = NoDither565;
        break;

    case MMP_ISP_OUT_RGB888:
        pISPctxt->PVOutInfo.OutFormat    = RGBPacket;
        pISPctxt->PVOutInfo.RGBFormat    = NoDither888;
        break;

    case MMP_ISP_OUT_YUYV:
        pISPctxt->PVOutInfo.OutFormat    = YUVPacket;
        pISPctxt->PVOutInfo.PacketFormat = YUYV;
        pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_YVYU:
        pISPctxt->PVOutInfo.OutFormat    = YUVPacket;
        pISPctxt->PVOutInfo.PacketFormat = YVYU;
        pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_UYVY:
        pISPctxt->PVOutInfo.OutFormat    = YUVPacket;
        pISPctxt->PVOutInfo.PacketFormat = UYVY;
        pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    case MMP_ISP_OUT_VYUY:
        pISPctxt->PVOutInfo.OutFormat    = YUVPacket;
        pISPctxt->PVOutInfo.PacketFormat = VYUY;
        pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_RGBtoYUVMatrix;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        break;
    }

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_PVOutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * JPEG decoder use this interface to set the buffer information.
 *
 * @param data  share data between JPEG and DSC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspPlayImageProcess(
    ISP_DEVICE          ptDev,
    const MMP_ISP_SHARE *data)
{
    ISP_RESULT     result            = ISP_SUCCESS;
    ISP_CONTEXT    *pISPctxt         = (ISP_CONTEXT *)ptDev;
    ISP_INPUT_INFO *pInInfo;
    MMP_UINT16     Temp              = 0;
    MMP_BOOL       En_ISP_CORE_MUTEX = MMP_TRUE;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);
    pInInfo = &pISPctxt->InInfo;

    //
    // Set Input Address, Width, Height and Pitch
    //
    pInInfo->AddrY  = (MMP_UINT8 *)data->addrY;
    pInInfo->AddrV  = (MMP_UINT8 *)data->addrV;
    pInInfo->AddrU  = (MMP_UINT8 *)data->addrU;
    pInInfo->AddrYp = (MMP_UINT8 *)data->addrYp;

    // isp line base mode limit: width must be 2 alignment
    pInInfo->SrcWidth         = data->width;
    pInInfo->SrcHeight        = data->height;
    pInInfo->PitchY           = data->pitchY;
    pInInfo->PitchUV          = data->pitchUv;

#if !defined(CFG_VP_TEST_MODULE_ENABLE)
    pInInfo->SrcWidth         = (pInInfo->SrcWidth >> 1) << 1;
    pInInfo->SrcHeight        = (pInInfo->SrcHeight >> 1) << 1;
#endif

    pInInfo->SrcPosX          = pInInfo->SrcExtedLeft;
    pInInfo->SrcPosY          = pInInfo->SrcExtedTop;
    pInInfo->PanelWidth       = pInInfo->SrcWidth + pInInfo->SrcExtedLeft + pInInfo->SrcExtedRight;
    pInInfo->PanelHeight      = pInInfo->SrcHeight + pInInfo->SrcExtedTop + pInInfo->SrcExtedDown;

    pISPctxt->top_field_first = 1;

    pISPctxt->UpdateFlags    |= ISP_FLAGS_UPDATE_InputBuf;
    pISPctxt->UpdateFlags    |= ISP_FLAGS_UPDATE_InputAddr;

    //
    // Set Input Format
    //
    result = ISP_SetInputFormat(pISPctxt, data->format);
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
        goto end;
    }

    //
    //Set JPEG Slice Number
    //
    pISPctxt->EngineMode.TotalSliceNum    = data->sliceCount;
    pISPctxt->UpdateFlags                |= ISP_FLAGS_UPDATE_EngineMode;

    //Set SW Handshaking Number
    pISPctxt->EngineMode.WriteRawSliceNum = data->rawsliceCount;

    mmpIspDisable(ptDev, MMP_ISP_KEEP_LAST_FIELD_MODE);

    //
    //CMYK Color Format
    //
    if (data->isAdobe_CMYK)
        mmpIspEnable(ptDev, MMP_ISP_NEGATIVE_EFFECT_MODE);
    else
        mmpIspDisable(ptDev, MMP_ISP_NEGATIVE_EFFECT_MODE);

    //result = ISP_WaitEngineIdle();
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
        goto end;
    }

    /*if (pISPctxt->OutInfo.RotateType == Deg90 ||
        pISPctxt->OutInfo.RotateType == Deg270)
    {
        Temp                     = pISPctxt->OutInfo.Width;
        pISPctxt->OutInfo.Width  = pISPctxt->OutInfo.Height;
        pISPctxt->OutInfo.Height = Temp;
    }*/

    // Set Scale Output Size
    // mmpIspSetVideoWindow(ptDev, 0, 0, pISPctxt->OutInfo.Width, pISPctxt->OutInfo.Height);
#if 0
    printf("I Y(%X)U(%X)V(%X)W(%d)H(%d)PY(%d)PUV(%d)\n",
           pInInfo->AddrY,
           pInInfo->AddrU,
           pInInfo->AddrV,
           pInInfo->SrcWidth,
           pInInfo->SrcHeight,
           pInInfo->PitchY,
           pInInfo->PitchUV);
    printf("F A(%X)W(%d)H(%d)P(%d)X(%d)Y(%d)E(%d)\n",
           pISPctxt->FrameFun0.Addr,
           pISPctxt->FrameFun0.Width,
           pISPctxt->FrameFun0.Height,
           pISPctxt->FrameFun0.Pitch,
           pISPctxt->FrameFun0.StartX,
           pISPctxt->FrameFun0.StartY,
           pISPctxt->FrameFun0.Enable);
    printf("O A(%X)W(%d)H(%d)P(%d)\n",
           pISPctxt->OutInfo.Addr0,
           pISPctxt->OutInfo.Width,
           pISPctxt->OutInfo.Height,
           pISPctxt->OutInfo.PitchYRGB);
#endif

    if (pISPctxt->WorkFlags & ISP_FLAGS_ENABLE_ISR_FIRE)
        En_ISP_CORE_MUTEX = MMP_FALSE;

    if (pISPctxt->WorkFlags & ISP_FLAGS_ENABLE_CMDQ)
        En_ISP_CORE_MUTEX = MMP_FALSE;

    if (pISPctxt->OutInfo.EnableLcdOnFly)
        if (!(pISPctxt->WorkFlags & ISP_FLAGS_LCD_ONFLY_INIT))
            En_ISP_CORE_MUTEX = MMP_FALSE;

    if (En_ISP_CORE_MUTEX)
    {
        if (pISPctxt->ispCore == MMP_ISP_CORE_0)
            pthread_mutex_lock(&ISP_CORE_0_MUTEX);
        else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
            pthread_mutex_lock(&ISP_CORE_1_MUTEX);

        result = ISP_WaitEngineIdle(pISPctxt);
        if (result)
        {
            isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);

            if (pISPctxt->ispCore == MMP_ISP_CORE_0)
                pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
            else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
                pthread_mutex_unlock(&ISP_CORE_1_MUTEX);

            goto end;
        }
    }

    result = ISP_Update(pISPctxt);
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);

        if (En_ISP_CORE_MUTEX)
        {
            if (pISPctxt->ispCore == MMP_ISP_CORE_0)
                pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
            else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
                pthread_mutex_unlock(&ISP_CORE_1_MUTEX);
        }

        goto end;
    }

    if (pISPctxt->OutInfo.EnableLcdOnFly)
    {
        if (pISPctxt->WorkFlags & ISP_FLAGS_LCD_ONFLY_INIT)
        {
            //ISP_LogReg(pISPctxt->ispCore);
            ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
            pISPctxt->WorkFlags &= ~ISP_FLAGS_LCD_ONFLY_INIT;
        }
        else
        {
            //ISP_LogReg(pISPctxt->ispCore);
            ISP_UpdateFire_Reg(pISPctxt, &pISPctxt->OutInfo);
        }
    }
    else
    {
        //ISP_LogReg(pISPctxt->ispCore);
        ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
    }

    if (En_ISP_CORE_MUTEX)
    {
        if (pISPctxt->ispCore == MMP_ISP_CORE_0)
            pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
        else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
            pthread_mutex_unlock(&ISP_CORE_1_MUTEX);
    }

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Query ISP input information.
 *
 * @param data  share data between MPEG and ISP.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspGetVideoDispInfo(
    ISP_DEVICE    ptDev,
    MMP_ISP_SHARE *data)
{
    ISP_RESULT     result    = ISP_SUCCESS;
    ISP_CONTEXT    *pISPctxt = (ISP_CONTEXT *)ptDev;
    ISP_INPUT_INFO *pInInfo;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);
    pInInfo       = &pISPctxt->InInfo;

    data->addrY   = (MMP_UINT32)pInInfo->AddrY;
    data->addrU   = (MMP_UINT32)pInInfo->AddrU;
    data->addrV   = (MMP_UINT32)pInInfo->AddrV;
    data->addrYp  = (MMP_UINT32)pInInfo->AddrYp;
    data->addrUp  = (MMP_UINT32)pInInfo->AddrUp;
    data->addrVp  = (MMP_UINT32)pInInfo->AddrVp;

    data->width   = pInInfo->SrcWidth;
    data->height  = pInInfo->SrcHeight;
    data->pitchY  = pInInfo->PitchY;
    data->pitchUv = pInInfo->PitchUV;

    switch (pInInfo->PlaneFormat)
    {
    case YUV420:
        data->format = MMP_ISP_IN_YUV420;
        break;

    case YUV422:
        data->format = MMP_ISP_IN_YUV422;
        break;

    default:
        result = ISP_ERR_INVALID_INPUT_PARAM;
        break;
    }

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * MPEG decoder use this interface to set the buffer information.
 *
 * @param data  share data between MPEG and ISP.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspPlayVideoProcess(
    ISP_DEVICE          ptDev,
    const MMP_ISP_SHARE *data)
{
    ISP_RESULT     result     = ISP_SUCCESS;
    ISP_CONTEXT    *pISPctxt  = (ISP_CONTEXT *)ptDev;
    ISP_INPUT_INFO *pInInfo;
    MMP_UINT32     status     = 0;
    MMP_UINT16     align_width, align_height;
    MMP_UINT16     DOWNSAMPLE = 2;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pInInfo     = &pISPctxt->InInfo;
    // isp limit: width must be 8 alignment
    align_width = (data->width >> 3) << 3;

    if (pISPctxt->OutInfo.EnableLcdOnFly &&
        !pISPctxt->OutInfo.EnableFieldScale &&
        pISPctxt->OutInfo.EnableVideoPreview && data->height > 16)
        align_height = ((data->height >> 1) << 1) >> DOWNSAMPLE;
    else
        align_height = (data->height >> 1) << 1;

    if (pInInfo->SrcWidth != align_width || pInInfo->SrcHeight != align_height)
    {
        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_InputBuf;
        pISPctxt->WorkFlags   |= ISP_FLAGS_FIRST_VIDEO_FRAME;
    }

    // update buffer address
    pInInfo->AddrY                = (MMP_UINT8 *)data->addrY;
    pInInfo->AddrV                = (MMP_UINT8 *)data->addrV;
    pInInfo->AddrU                = (MMP_UINT8 *)data->addrU;
    pInInfo->AddrYp               = (MMP_UINT8 *)data->addrYp;
    pInInfo->AddrVp               = (MMP_UINT8 *)data->addrVp;
    pInInfo->AddrUp               = (MMP_UINT8 *)data->addrUp;

    pISPctxt->Mpeg2TopBufferIndex = data->Mpeg2TopBufferIndex;
    pISPctxt->Mpeg2BotBufferIndex = data->Mpeg2BotBufferIndex;
    pISPctxt->top_field_first     = data->top_field_first;
    pISPctxt->Blank_Buffer_Index  = data->Blank_Buffer_Index;

    pISPctxt->UpdateFlags        |= ISP_FLAGS_UPDATE_InputAddr;
    pISPctxt->UpdateFlags        |= ISP_FLAGS_UPDATE_Mpeg2BufferIdx;

    // set width, height, pitch at the first frame
    if (pISPctxt->WorkFlags & ISP_FLAGS_FIRST_VIDEO_FRAME)
    {
        //
        // Set Input Format
        //
        result = ISP_SetInputFormat(pISPctxt, data->format);
        if (result)
        {
            isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
            goto end;
        }

        if (data->width == VIDEO_BLANK_WIDTH && data->height == VIDEO_BLANK_HEIGHT)
        {
            mmpIspEnable(ptDev, MMP_ISP_DISABLE_VIDEO_OUT);
        }
        else
        {
            mmpIspDisable(ptDev, MMP_ISP_DISABLE_VIDEO_OUT);
        }

        pInInfo->SrcWidth = data->width;

        if (pISPctxt->OutInfo.EnableLcdOnFly &&
            !pISPctxt->OutInfo.EnableFieldScale &&
            pISPctxt->OutInfo.EnableVideoPreview && data->height > VIDEO_BLANK_HEIGHT)
        {
            pInInfo->SrcHeight = ((data->height >> 1) << 1) >> DOWNSAMPLE;
            pInInfo->PitchY    = data->pitchY  << DOWNSAMPLE;
            pInInfo->PitchUV   = data->pitchUv << DOWNSAMPLE;

            mmpIspDisable(ptDev, MMP_ISP_SUBTITLE_0);
            mmpIspDisable(ptDev, MMP_ISP_SUBTITLE_1);
        }
        else
        {
            pInInfo->SrcHeight = (data->height >> 1) << 1;
            pInInfo->PitchY    = data->pitchY;
            pInInfo->PitchUV   = data->pitchUv;
        }

        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_InputBuf;

        // isp limit: width must be 8 alignment
        if (pInInfo->SrcWidth & 0x07)
        {
            pInInfo->SrcWidth = (pInInfo->SrcWidth >> 3) << 3;
            pInInfo->PitchY   = (pInInfo->PitchY   >> 3) << 3;
            pInInfo->PitchUV  = (pInInfo->PitchUV  >> 3) << 3;
        }
    }

    // subtitle
    pInInfo->SrcPosX     = pInInfo->SrcExtedLeft;
    pInInfo->SrcPosY     = pInInfo->SrcExtedTop;
    pInInfo->PanelWidth  = pInInfo->SrcWidth + pInInfo->SrcExtedLeft + pInInfo->SrcExtedRight;
    pInInfo->PanelHeight = pInInfo->SrcHeight + pInInfo->SrcExtedTop + pInInfo->SrcExtedDown;

#if 0
    if (pISPctxt->OutInfo.EnableFieldScale
        && (pInInfo->SrcWidth > VIDEO_BLANK_WIDTH && pInInfo->SrcHeight > VIDEO_BLANK_HEIGHT)
        && (pInInfo->SrcWidth != pISPctxt->OutInfo.Width
            || pInInfo->SrcHeight != pISPctxt->OutInfo.Height)
        && data->isProgressive
        && (!pISPctxt->OutInfo.EnableVideoPreview))
    {
        pISPctxt->OutInfo.EnableProgFieldMode = MMP_TRUE;
    }
    else
#endif
    pISPctxt->OutInfo.EnableProgFieldMode = MMP_FALSE;

    pISPctxt->UpdateFlags                |= ISP_FLAGS_UPDATE_InputBuf;

    if (pISPctxt->WorkFlags & ISP_FLAGS_FIRST_VIDEO_FRAME)
    {
        mmpIspDisable(ptDev, MMP_ISP_SUBTITLE_0);
        mmpIspDisable(ptDev, MMP_ISP_SUBTITLE_1);
    }

    if (ISP_WaitIspIdle(pISPctxt) != ISP_SUCCESS)
        goto end;

    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO && pISPctxt->OutInfo.EnableLcdOnFly == MMP_FALSE)
    {
        if (pISPctxt->OutInfo.EnableQueueFire)
        {
            if (pISPctxt->WorkFlags & ISP_FLAGS_QUEUE_FIRE_INIT)
            {
                mmpIspEnable(ptDev, MMP_ISP_SOFTWARE_WRITE_FLIP_MODE);
                isp_ReadHwReg(ITH_LCD_READ_STATUS2_REG, (MMP_UINT32 *)&status); // get Lcd flip index
                mmpIspSetSWWriteBufferNum(ptDev, !(status & 0x00001000));
                pISPctxt->WorkFlags &= ~ISP_FLAGS_QUEUE_FIRE_INIT;
            }
            else
            {
                mmpIspDisable(ptDev, MMP_ISP_SOFTWARE_WRITE_FLIP_MODE);
            }
        }
    }

    if (data->EnableKeepLastField
        && pISPctxt->OutInfo.EnableFieldScale
        && (!data->isProgressive))
        mmpIspEnable(ptDev, MMP_ISP_KEEP_LAST_FIELD_MODE);
    else
        mmpIspDisable(ptDev, MMP_ISP_KEEP_LAST_FIELD_MODE);

    pISPctxt->DeInterlace.EnSrcBottomFieldFirst = !(pISPctxt->top_field_first);
    if (!pISPctxt->OutInfo.EnableFieldScale)
    {
        pISPctxt->DeInterlace.EnDeinterBottomField = !pISPctxt->DeInterlace.EnSrcBottomFieldFirst;
        pISPctxt->UpdateFlags                     |= ISP_FLAGS_UPDATE_DeInterlaceParam;
    }

    result = ISP_Update(pISPctxt);
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
        goto end;
    }

    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO)
    {
        if (pISPctxt->OutInfo.EnableLcdOnFly)
        {
            if (pISPctxt->WorkFlags & ISP_FLAGS_LCD_ONFLY_INIT)
            {
                ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
                ISP_EnableLCD_OnFly();
                pISPctxt->WorkFlags &= ~ISP_FLAGS_LCD_ONFLY_INIT;
            }
            else
            {
                ISP_UpdateFire_Reg(pISPctxt, &pISPctxt->OutInfo);
            }
        }
        else
        {
            //ISP_LogReg(pISPctxt->ispCore);
            ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
        }
    }
    else
    {
        ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
    }

    if (pISPctxt->WorkFlags & ISP_FLAGS_FIRST_VIDEO_FRAME)
        pISPctxt->WorkFlags |= ISP_FLAGS_SECOND_VIDEO_FRAME;

    pISPctxt->WorkFlags &= ~ISP_FLAGS_FIRST_VIDEO_FRAME;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * MPEG decoder use this interface to set the buffer information.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspUpdateUIProcess(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;
    MMP_UINT16  status    = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (pISPctxt->OutInfo.EnableFieldScale)
        mmpIspEnable(ptDev, MMP_ISP_KEEP_LAST_FIELD_MODE);
    else
    {
        pISPctxt->DeInterlace.EnDeinterBottomField = !!(pISPctxt->DeInterlace.EnSrcBottomFieldFirst);
        pISPctxt->UpdateFlags                     |= ISP_FLAGS_UPDATE_DeInterlaceParam;
    }

    result = ISP_Update(pISPctxt);
    if (result)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err 0x%x !\n", result);
        goto end;
    }

    if (pISPctxt->ispMode == MMP_ISP_MODE_PLAY_VIDEO)
    {
        if (pISPctxt->OutInfo.EnableLcdOnFly)
        {
            if (pISPctxt->WorkFlags & ISP_FLAGS_LCD_ONFLY_INIT)
            {
                ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
                ISP_EnableLCD_OnFly();
                pISPctxt->WorkFlags &= ~ISP_FLAGS_LCD_ONFLY_INIT;
            }
            else
            {
                ISP_UpdateFire_Reg(pISPctxt, &pISPctxt->OutInfo);
            }
        }
        else
        {
            //ISP_LogReg(pISPctxt->ispCore);
            ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
        }
    }
    else
    {
        ISP_DriverFire_Reg(pISPctxt, &pISPctxt->OutInfo);
    }

    if (pISPctxt->WorkFlags & ISP_FLAGS_FIRST_VIDEO_FRAME)
        pISPctxt->WorkFlags |= ISP_FLAGS_SECOND_VIDEO_FRAME;

    pISPctxt->WorkFlags &= ~ISP_FLAGS_FIRST_VIDEO_FRAME;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Write Buffer information.
 *
 * @param wbInfo  destination address of Write Buffer
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetWriteBufferInfo(
    ISP_DEVICE                ptDev,
    MMP_ISP_WRITE_BUFFER_INFO *wbInfo)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->OutInfo.Addr0_1 = (MMP_UINT8 *)wbInfo->addrYRGB_1;
    pISPctxt->OutInfo.Addr1_1 = (MMP_UINT8 *)wbInfo->addrU_1;
    pISPctxt->OutInfo.Addr2_1 = (MMP_UINT8 *)wbInfo->addrV_1;
    pISPctxt->OutInfo.Addr0_2 = (MMP_UINT8 *)wbInfo->addrYRGB_2;
    pISPctxt->OutInfo.Addr1_2 = (MMP_UINT8 *)wbInfo->addrU_2;
    pISPctxt->OutInfo.Addr2_2 = (MMP_UINT8 *)wbInfo->addrV_2;
    pISPctxt->OutInfo.Addr0_3 = (MMP_UINT8 *)wbInfo->addrYRGB_3;
    pISPctxt->OutInfo.Addr1_3 = (MMP_UINT8 *)wbInfo->addrU_3;
    pISPctxt->OutInfo.Addr2_3 = (MMP_UINT8 *)wbInfo->addrV_3;
    pISPctxt->OutInfo.Addr0_4 = (MMP_UINT8 *)wbInfo->addrYRGB_4;
    pISPctxt->OutInfo.Addr1_4 = (MMP_UINT8 *)wbInfo->addrU_4;
    pISPctxt->OutInfo.Addr2_4 = (MMP_UINT8 *)wbInfo->addrV_4;

    pISPctxt->UpdateFlags      |= ISP_FLAGS_UPDATE_OutAddress;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Write Buffer amount of ISP Flip.
 *
 * @param bufIndex  Set the buffer amount, 00: BufferA 01: BufferB, 10: BufferC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetWriteBufferNum(
    ISP_DEVICE ptDev,
    MMP_UINT   bufIndex)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->OutInfo.WrFlipNum = bufIndex;
    pISPctxt->UpdateFlags      |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Write Buffer number of Software Flip.
 *
 * @param bufIndex  Set the current buffer, 00: BufferA 01: BufferB, 10: BufferC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetSWWriteBufferNum(
    ISP_DEVICE ptDev,
    MMP_UINT   bufIndex)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->OutInfo.SWWrFlipNum = bufIndex;
    pISPctxt->UpdateFlags        |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set frame function background image information & color key. (For Direct Assign VRAM address. Ex.2D input)
 *
 * @param baseAddr       base address of the background image buffer.
 * @param id             DSC capability for frame function identification.
 * @param startX         x position of the background image.
 * @param startY         y position of the background image.
 * @param width          width of the background image.
 * @param height         height of the background image.
 * @param colorKeyR      color key for R channel.
 * @param colorKeyG      color key for G channel.
 * @param colorKeyB      color key for B channel.
 * @param EnAlphaBlend   enable Alpha Blending function.
 * @param constantAlpha  constant Alpha Value.
 * @param format         format of the picture & color key.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable() mmpIspDisable()
 */
//=============================================================================
ISP_RESULT
mmpIspSetFrameFunction(
    ISP_DEVICE       ptDev,
    MMP_ISP_CAPS     id,
    void             *vramAddr,
    MMP_UINT         startX,
    MMP_UINT         startY,
    MMP_UINT         width,
    MMP_UINT         height,
    MMP_UINT         pitch,
    MMP_UINT         colorKeyR,
    MMP_UINT         colorKeyG,
    MMP_UINT         colorKeyB,
    MMP_BOOL         EnAlphaBlend,
    MMP_UINT         constantAlpha,
    MMP_PIXEL_FORMAT format,
    MMP_UINT         uiBufferIndex)
{
    ISP_RESULT      result    = ISP_SUCCESS;
    ISP_CONTEXT     *pISPctxt = (ISP_CONTEXT *)ptDev;
    ISP_FRMFUN_CTRL *pIspFrameFunc;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (width < 16 || height < 16)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err, width(%d) < 16 or height(%d) < 16 !!", width, height);
        result = ISP_ERR_INVALID_PARAM;
        goto end;
    }

    if (id == MMP_ISP_FRAME_FUNCTION_0)
    {
        pIspFrameFunc          = &pISPctxt->FrameFun0;
        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_FrameFun0;
    }
    else if (id == MMP_ISP_FRAME_FUNCTION_1)
    {
        pIspFrameFunc          = &pISPctxt->FrameFun1;
        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_FrameFun1;
    }
    else
    {
        result = ISP_ERR_NULL_POINTER;
        goto end;
    }

    pIspFrameFunc->EnableRGB2YUV = pISPctxt->EnableYUVProcess;

    startX -= (startX & 0x1); // startX % 2;
    startY -= (startY & 0x1); // startY % 2;
    width  += (width  & 0x1); //  width % 2;
    height += (height & 0x1); // height % 2;

    switch (format)
    {
    case MMP_PIXEL_FORMAT_ARGB4444:
        pIspFrameFunc->Format = ARGB4444;
        break;

    case MMP_PIXEL_FORMAT_ARGB8888:
        pIspFrameFunc->Format = ARGB8888;
        break;

    case MMP_PIXEL_FORMAT_RGB565:
        pIspFrameFunc->Format = CARGB565;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        goto end;
        break;
    }

    pIspFrameFunc->ColorKeyR        = (MMP_UINT16) colorKeyR;
    pIspFrameFunc->ColorKeyG        = (MMP_UINT16) colorKeyG;
    pIspFrameFunc->ColorKeyB        = (MMP_UINT16) colorKeyB;
    pIspFrameFunc->EnableAlphaBlend = EnAlphaBlend;
    pIspFrameFunc->ConstantAlpha    = (MMP_UINT16) constantAlpha;
    pIspFrameFunc->StartX           = (MMP_UINT16) startX;
    pIspFrameFunc->StartY           = (MMP_UINT16) startY;
    pIspFrameFunc->Width            = (MMP_UINT16) width;
    pIspFrameFunc->Height           = (MMP_UINT16) height;

    pIspFrameFunc->Pitch            = (MMP_UINT16) pitch;
    pIspFrameFunc->Addr             = (MMP_UINT8 *) ((MMP_UINT) vramAddr);
    pIspFrameFunc->UIBufferIndex    = (MMP_UINT16)uiBufferIndex;
    pIspFrameFunc->EnableUiDec      = MMP_FALSE;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set ui decompress frame function background image information & color key. (For Direct Assign VRAM address. Ex.2D input)
 *
 * @param baseAddr       base address of the background image buffer.
 * @param id             DSC capability for frame function identification.
 * @param startX         x position of the background image.
 * @param startY         y position of the background image.
 * @param width          width of the background image.
 * @param height         height of the background image.
 * @param colorKeyR      color key for R channel.
 * @param colorKeyG      color key for G channel.
 * @param colorKeyB      color key for B channel.
 * @param EnAlphaBlend   enable Alpha Blending function.
 * @param constantAlpha  constant Alpha Value.
 * @param format         format of the picture & color key. only support RGB 888, RGB565.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable() mmpIspDisable()
 */
//=============================================================================
ISP_RESULT
mmpIspSetUiDecFrameFunction(
    ISP_DEVICE       ptDev,
    MMP_ISP_CAPS     id,
    void             *vramAddr,
    MMP_UINT         startX,
    MMP_UINT         startY,
    MMP_UINT         width,
    MMP_UINT         height,
    MMP_UINT         pitch,
    MMP_UINT32       linebytes,
    MMP_UINT32       bitstreambytes,
    MMP_UINT         colorKeyR,
    MMP_UINT         colorKeyG,
    MMP_UINT         colorKeyB,
    MMP_BOOL         EnAlphaBlend,
    MMP_UINT         constantAlpha,
    MMP_PIXEL_FORMAT format,
    MMP_UINT         uiBufferIndex)
{
    ISP_RESULT      result          = ISP_SUCCESS;
    ISP_CONTEXT     *pISPctxt       = (ISP_CONTEXT *)ptDev;
    ISP_FRMFUN_CTRL *pIspFrameFunc;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (width < 16 || height < 16)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err, width(%d) < 16 or height(%d) < 16 !!", width, height);
        result = ISP_ERR_INVALID_PARAM;
        goto end;
    }

    if (linebytes > pitch)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err, linebyte(%d) > pitch(%d) !!", linebytes, pitch);
        result = ISP_ERR_INVALID_PARAM;
        goto end;
    }

    if (id == MMP_ISP_FRAME_FUNCTION_0)
    {
        pIspFrameFunc          = &pISPctxt->FrameFun0;
        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_FrameFun0;
    }
    else if (id == MMP_ISP_FRAME_FUNCTION_1)
    {
        pIspFrameFunc          = &pISPctxt->FrameFun1;
        pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_FrameFun1;
    }
    else
    {
        result = ISP_ERR_NULL_POINTER;
        goto end;
    }

    pIspFrameFunc->EnableRGB2YUV = (pISPctxt->EnableYUVProcess);

    width  += (width  & 0x1); // width % 2;
    height += (height & 0x1); // height % 2;

    switch (format)
    {
    case MMP_PIXEL_FORMAT_ARGB4444:
        pIspFrameFunc->Format = ARGB4444;
        break;

    case MMP_PIXEL_FORMAT_ARGB8888:
        pIspFrameFunc->Format = ARGB8888;
        break;

    case MMP_PIXEL_FORMAT_RGB565:
        pIspFrameFunc->Format = CARGB565;
        break;

    default:
        result = ISP_ERR_NO_MATCH_OUTPUT_FORMAT;
        goto end;
        break;
    }

    pIspFrameFunc->ColorKeyR        = (MMP_UINT16) colorKeyR;
    pIspFrameFunc->ColorKeyG        = (MMP_UINT16) colorKeyG;
    pIspFrameFunc->ColorKeyB        = (MMP_UINT16) colorKeyB;
    pIspFrameFunc->EnableAlphaBlend = EnAlphaBlend;
    pIspFrameFunc->ConstantAlpha    = (MMP_UINT16) constantAlpha;
    pIspFrameFunc->StartX           = (MMP_UINT16) startX;
    pIspFrameFunc->StartY           = (MMP_UINT16) startY;
    pIspFrameFunc->Width            = (MMP_UINT16) width;
    pIspFrameFunc->Height           = (MMP_UINT16) height;

    pIspFrameFunc->Pitch            = (MMP_UINT16) pitch;
    pIspFrameFunc->Addr             = (MMP_UINT8 *) ((MMP_UINT) vramAddr);
    pIspFrameFunc->UIBufferIndex    = (MMP_UINT16)uiBufferIndex;

    pIspFrameFunc->EnableUiDec      = MMP_TRUE;
    pIspFrameFunc->UiDecLineBytes   = linebytes;
    pIspFrameFunc->UiDecTotalBytes  = bitstreambytes;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set blend function of constant value.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetBlenConstData(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS id,
    MMP_BOOL     EnableGridData,
    MMP_UINT     GridPixelMode,
    MMP_UINT     ConstColorR0,
    MMP_UINT     ConstColorG0,
    MMP_UINT     ConstColorB0,
    MMP_UINT     ConstColorR1,
    MMP_UINT     ConstColorG1,
    MMP_UINT     ConstColorB1)
{
    ISP_RESULT          result          = ISP_SUCCESS;
    ISP_CONTEXT         *pISPctxt       = (ISP_CONTEXT *)ptDev;
    ISP_FRMFUN_CTRL     *pIspFrameFunc;
    ISP_FLAGS_UPDATE    UpdateFlag;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (id == MMP_ISP_FRAME_FUNCTION_0)
    {
        pIspFrameFunc   = &pISPctxt->FrameFun0;
        UpdateFlag      = ISP_FLAGS_UPDATE_FrameFun0;
    }
    else if (id == MMP_ISP_FRAME_FUNCTION_1)
    {
        pIspFrameFunc   = &pISPctxt->FrameFun1;
        UpdateFlag      = ISP_FLAGS_UPDATE_FrameFun1;
    }
    else
    {
        result = ISP_ERR_NULL_POINTER;
        goto end;
    }

    pIspFrameFunc->EnableGridConst = EnableGridData;
    pIspFrameFunc->GridDataMode = (MMP_UINT16)GridPixelMode;
    pIspFrameFunc->ConstColorR0 = (MMP_UINT16)ConstColorR0;
    pIspFrameFunc->ConstColorG0 = (MMP_UINT16)ConstColorG0;
    pIspFrameFunc->ConstColorB0 = (MMP_UINT16)ConstColorB0;
    pIspFrameFunc->ConstColorR1 = (MMP_UINT16)ConstColorR1;
    pIspFrameFunc->ConstColorG1 = (MMP_UINT16)ConstColorG1;
    pIspFrameFunc->ConstColorB1 = (MMP_UINT16)ConstColorB1;
    pISPctxt->UpdateFlags |= UpdateFlag;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set subtitle background image information.
 *
 * @param id        frame function 0 or 1.
 * @param baseAddr  base address of the background image buffer.
 * @param width     width of the background image.
 * @param height    height of the background image.
 * @param pitch     pitch of the background image.
 * @param format    format of the picture & color key. only support RGB 888, RGB565.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable() mmpIspDisable()
 */
//=============================================================================
ISP_RESULT
mmpIspSetSubTitle(
    ISP_DEVICE              ptDev,
    MMP_UINT                id,
    void                    *baseAddr,
    MMP_UINT                startX,
    MMP_UINT                startY,
    MMP_UINT                width,
    MMP_UINT                height,
    MMP_ISP_SUBTITLE_FORMAT format)
{
    ISP_RESULT        result        = ISP_SUCCESS;
    ISP_CONTEXT       *pISPctxt     = (ISP_CONTEXT *)ptDev;
    MMP_UINT16        SubRefWidth, SubRefHeight;
    ISP_SUBTITLE_CTRL *pIspSubTitle;
    ISP_FLAGS_UPDATE  UpdateFlag;

    static MMP_FLOAT  InvHCI;
    static MMP_FLOAT  InvVCI;
    static MMP_UINT16 PreSrcWidth  = 0;
    static MMP_UINT16 PreSrcHeight = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (pISPctxt->InInfo.SrcWidth <= PAL_WIDTH)
        SubRefWidth = PAL_WIDTH;
    else if (pISPctxt->InInfo.SrcWidth <= HD_WIDTH)
        SubRefWidth = HD_WIDTH;
    else
        SubRefWidth = FULLHD_WIDTH;

    if (pISPctxt->InInfo.SrcHeight <= PAL_HEIGHT)
        SubRefHeight = PAL_HEIGHT;
    else if (pISPctxt->InInfo.SrcHeight <= HD_HEIGHT)
        SubRefHeight = HD_HEIGHT;
    else
        SubRefHeight = FULLHD_HEIGHT;

    if (PreSrcWidth != pISPctxt->InInfo.SrcWidth)
        InvHCI = 1.0 / (MMP_FLOAT) (((MMP_INT) (16384.0f * SubRefWidth / (MMP_FLOAT)pISPctxt->InInfo.SrcWidth)) / 16384.0f);

    if (PreSrcHeight != pISPctxt->InInfo.SrcHeight)
        InvVCI = 1.0 / (MMP_FLOAT) (((MMP_INT) (16384.0f * SubRefHeight / (MMP_FLOAT)pISPctxt->InInfo.SrcHeight)) / 16384.0f);

    PreSrcWidth  = pISPctxt->InInfo.SrcWidth;
    PreSrcHeight = pISPctxt->InInfo.SrcHeight;

    if (id == 0)
    {
        pIspSubTitle = &pISPctxt->SubTitle0;
        UpdateFlag = ISP_FLAGS_UPDATE_SubTitle0;
    }
    else
    {
        pIspSubTitle = &pISPctxt->SubTitle1;
        UpdateFlag = ISP_FLAGS_UPDATE_SubTitle1;
    }

    switch (format)
    {
    case MMP_ISP_SUBTITLE_YUVT_6442:
        pIspSubTitle->Format = YUVT6442;
        break;

    case MMP_ISP_SUBTITLE_YUVT_4444:
        pIspSubTitle->Format = YUVT4444;
        break;

    case MMP_ISP_SUBTITLE_YUVT_6334:
        pIspSubTitle->Format = YUVT6334;
        break;

    case MMP_ISP_SUBTITLE_YUVT_5443:
        pIspSubTitle->Format = YUVT5443;
        break;
    }

    pIspSubTitle->StartX        = (MMP_UINT16)((MMP_FLOAT)startX * InvHCI) & 0xFFFE;
    pIspSubTitle->StartY        = (MMP_UINT16)((MMP_FLOAT)startY * InvVCI);
    pIspSubTitle->SrcWidth      = (MMP_UINT16)width;
    pIspSubTitle->SrcHeight     = (MMP_UINT16)height;
    pIspSubTitle->DstWidth      = (MMP_UINT16)((MMP_FLOAT)width  * InvHCI) & 0xFFFE;
    pIspSubTitle->DstHeight     = (MMP_UINT16)((MMP_FLOAT)height * InvVCI);
    pIspSubTitle->Pitch         = (MMP_UINT16)(width * 2);
    pIspSubTitle->Addr          = baseAddr;
    pIspSubTitle->EnableUiDec   = MMP_FALSE;

    pISPctxt->UpdateFlags |= UpdateFlag;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);
    return result;
}

//=============================================================================
/**
 * Set ui decompress subtitle background image information.
 *
 * @param id        frame function 0 or 1.
 * @param baseAddr  base address of the background image buffer.
 * @param width     width of the background image.
 * @param height    height of the background image.
 * @param pitch     pitch of the background image.
 * @param format    format of the picture & color key. only support RGB 888, RGB565.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable() mmpIspDisable()
 */
//=============================================================================
ISP_RESULT
mmpIspSetUiDecSubTitle(
    ISP_DEVICE              ptDev,
    MMP_UINT                id,
    void                    *baseAddr,
    MMP_UINT                startX,
    MMP_UINT                startY,
    MMP_UINT                width,
    MMP_UINT                height,
    MMP_UINT32              linebytes,
    MMP_UINT32              bitstreambytes,
    MMP_ISP_SUBTITLE_FORMAT format)
{
    ISP_RESULT        result         = ISP_SUCCESS;
    ISP_CONTEXT       *pISPctxt      = (ISP_CONTEXT *)ptDev;
    ISP_SUBTITLE_CTRL *pIspSubTitle;
    ISP_FLAGS_UPDATE  UpdateFlag;
    MMP_UINT16        SubRefWidth;

    static MMP_FLOAT  InvHCI;
    static MMP_UINT16 PreSrcWidth = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (linebytes > width * 2)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err, linebyte(%d) > pitch(%d) !!", linebytes, width * 2);
        result = ISP_ERR_INVALID_PARAM;
        goto end;
    }

    if (pISPctxt->InInfo.SrcWidth <= PAL_WIDTH)
        SubRefWidth = PAL_WIDTH;
    else if (pISPctxt->InInfo.SrcWidth <= HD_WIDTH)
        SubRefWidth = HD_WIDTH;
    else
        SubRefWidth = FULLHD_WIDTH;

    if (PreSrcWidth != pISPctxt->InInfo.SrcWidth)
        InvHCI = 1.0 / (MMP_FLOAT) (((MMP_INT) (16384.0f * SubRefWidth / (MMP_FLOAT)pISPctxt->InInfo.SrcWidth)) / 16384.0f);

    PreSrcWidth = pISPctxt->InInfo.SrcWidth;

    if (id == 0)
    {
        pIspSubTitle = &pISPctxt->SubTitle0;
        UpdateFlag = ISP_FLAGS_UPDATE_SubTitle0;
    }
    else
    {
        pIspSubTitle = &pISPctxt->SubTitle1;
        UpdateFlag = ISP_FLAGS_UPDATE_SubTitle1;
    }

    switch (format)
    {
    case MMP_ISP_SUBTITLE_YUVT_6442:
        pIspSubTitle->Format = YUVT6442;
        break;

    case MMP_ISP_SUBTITLE_YUVT_4444:
        pIspSubTitle->Format = YUVT4444;
        break;

    case MMP_ISP_SUBTITLE_YUVT_6334:
        pIspSubTitle->Format = YUVT6334;
        break;

    case MMP_ISP_SUBTITLE_YUVT_5443:
        pIspSubTitle->Format = YUVT5443;
        break;
    }

    pIspSubTitle->StartX          = (MMP_UINT16) ((MMP_FLOAT)startX * InvHCI) & 0xFFFE;
    pIspSubTitle->StartY          = (MMP_UINT16) startY;
    pIspSubTitle->SrcWidth        = (MMP_UINT16) width;
    pIspSubTitle->SrcHeight       = (MMP_UINT16) height;
    pIspSubTitle->DstWidth        = (MMP_UINT16) ((MMP_FLOAT)width * InvHCI) & 0xFFFE;
    pIspSubTitle->DstHeight       = (MMP_UINT16) height;
    pIspSubTitle->Pitch           = (MMP_UINT16) (width * 2);
    pIspSubTitle->Addr            = baseAddr;

    pIspSubTitle->EnableUiDec     = MMP_TRUE;
    pIspSubTitle->UiDecLineBytes  = linebytes;
    pIspSubTitle->UiDecTotalBytes = bitstreambytes;

    pISPctxt->UpdateFlags         |= UpdateFlag;
end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set clipping window (only for display).
 *
 * @param id      DSC capability for clipping function identification.
 * @param type    Specify the clip type. Clip inside or outside.
 * @param startX  x coordinate of clipping window.
 * @param startY  y coordinate of clipping window.
 * @param width   width of clipping window.
 * @param height  height of clipping window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetClipWindow(
    ISP_DEVICE        ptDev,
    MMP_ISP_CAPS      id,
    MMP_ISP_CLIP_TYPE type,
    MMP_UINT          startX,
    MMP_UINT          startY,
    MMP_UINT          width,
    MMP_UINT          height)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;
    ISP_CLIP_FN_CTRL  *pClipFun;
    ISP_FLAGS_UPDATE  UpdateFlag;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (width < 16 || height < 16)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, " err, width(%d) < 16 or height(%d) < 16 !!", width, height);
        result = ISP_ERR_INVALID_PARAM;
        goto end;
    }

    switch (id)
    {
    case MMP_ISP_CLIPPING0:
        pClipFun = &pISPctxt->ClipFun0;
        UpdateFlag = ISP_FLAGS_UPDATE_Clip0Fun;
        break;

    case MMP_ISP_CLIPPING1:
        pClipFun = &pISPctxt->ClipFun1;
        UpdateFlag = ISP_FLAGS_UPDATE_Clip1Fun;
        break;

    case MMP_ISP_CLIPPING2:
        pClipFun = &pISPctxt->ClipFun2;
        UpdateFlag = ISP_FLAGS_UPDATE_Clip2Fun;
        break;

    default:
        goto end;
    }

    pClipFun->Format = (type == MMP_ISP_CLIP_INSIDE) ? ClipInside : ClipOutside;
    pClipFun->StartX = (MMP_UINT16)startX;
    pClipFun->StartY = (MMP_UINT16)startY;
    pClipFun->Width  = (MMP_UINT16)width;
    pClipFun->Height = (MMP_UINT16)height;

    pISPctxt->UpdateFlags |= UpdateFlag;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Rotate image +90, -90, 180, flip, or mirror. It will apply to display.
 *
 * @param type  Specify the rotation type.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetDisplayRotateType(
    ISP_DEVICE          ptDev,
    MMP_ISP_ROTATE_TYPE type)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    switch (type)
    {
    case MMP_ISP_ROTATE_0:      pISPctxt->OutInfo.RotateType = Deg0;     break;
    case MMP_ISP_ROTATE_90:     pISPctxt->OutInfo.RotateType = Deg90;    break;
    case MMP_ISP_ROTATE_270:    pISPctxt->OutInfo.RotateType = Deg270;   break;
    case MMP_ISP_ROTATE_180:    pISPctxt->OutInfo.RotateType = Deg180;   break;
    case MMP_ISP_ROTATE_MIRROR: pISPctxt->OutInfo.RotateType = Mirror;   break;
    case MMP_ISP_ROTATE_FLIP:   pISPctxt->OutInfo.RotateType = Flip;     break;
    }

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set image information in OnFly mode.
 *
 * @param baseAddr  base address of the image buffer.
 * @param startX    x coordinate of clipping window.
 * @param startY    y coordinate of clipping window.
 * @param width     width of clipping window.
 * @param height    height of clipping window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable() mmpIspDisable()
 */
//=============================================================================
ISP_RESULT
mmpIspSetOnFlyWriteMemFunction(
    ISP_DEVICE ptDev,
    void       *vramAddr,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->OutInfo.Addr0   = vramAddr;
    pISPctxt->ClipFun2.Enable = MMP_TRUE;

    mmpIspSetClipWindow(ptDev, 2, MMP_ISP_CLIP_OUTSIDE, startX, startY, width, height);

    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_Clip2Fun;
    pISPctxt->UpdateFlags |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable LCD OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableLCDOnFlyMode(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    result = isp_LcdWaitChangeMode();
    if (result != 0)
    {
        isp_msg_ex(ISP_MSG_TYPE_ERR, "LCD_WaitChangeMode error\n");
        goto end;
    }

    pISPctxt->WorkFlags |= ISP_FLAGS_ENABLE_ONFLY_UPDATEFLAG;

    //Wait ISP Queue Idle
    //IspQ_WaitIdle(0x00E0, 0x00E0);

    pISPctxt->OutInfo.EnableLcdOnFly = MMP_TRUE;
    pISPctxt->EnableYUVProcess       = MMP_FALSE;
    pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_OutParameter;
    pISPctxt->WorkFlags             |= ISP_FLAGS_LCD_ONFLY_INIT;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable LCD OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableLCDOnFlyMode(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    isp_LcdEnableVideoFlip(MMP_FALSE);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_ENABLE_ONFLY_UPDATEFLAG);

    //Wait ISP Queue Idle
    //IspQ_WaitIdle(0x00E0, 0x00E0);

    //Wait ISP Update idle
    //ISP_WaitISPChangeIdle();

    //ISP_DisableLCD_OnFly();

    //Wait Lcd Change Mode
    isp_LcdWaitChangeMode();

    pISPctxt->OutInfo.EnableLcdOnFly = MMP_FALSE;
    pISPctxt->EnableYUVProcess       = MMP_FALSE;
    pISPctxt->UpdateFlags           |= ISP_FLAGS_UPDATE_OutParameter;
    pISPctxt->WorkFlags             &= (~ISP_FLAGS_LCD_ONFLY_INIT);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable Capture OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableCAPOnFlyMode(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags |= ISP_FLAGS_CAP_ONFLY_INIT;

    pISPctxt->EngineMode.EnableCaptureOnFly   = MMP_TRUE;
    pISPctxt->EngineMode.EnableReadMemoryMode = MMP_FALSE;
    pISPctxt->UpdateFlags                    |= ISP_FLAGS_UPDATE_EngineMode;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable Capture OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableCAPOnFlyMode(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_CAP_ONFLY_INIT);

    pISPctxt->EngineMode.EnableCaptureOnFly   = MMP_FALSE;
    pISPctxt->EngineMode.EnableReadMemoryMode = MMP_TRUE;
    pISPctxt->UpdateFlags                    |= ISP_FLAGS_UPDATE_EngineMode;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Wait ISP Engine Idle.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspWaitEngineIdle(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        pthread_mutex_lock(&ISP_CORE_0_MUTEX);
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        pthread_mutex_lock(&ISP_CORE_1_MUTEX);

    //ISP_LogReg(pISPctxt->ispCore);
    result = ISP_WaitEngineIdle(pISPctxt);

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        pthread_mutex_unlock(&ISP_CORE_1_MUTEX);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Query ISP Engine Idle.
 *
 * @return MMP_TRUE if engine is idle, MMP_FALSE otherwise.
 */
//=============================================================================
MMP_BOOL
mmpIspIsEngineIdle(
    ISP_DEVICE ptDev)
{
    ISP_CONTEXT *pISPctxt         = (ISP_CONTEXT *)ptDev;
    MMP_UINT32  status            = 0;
    MMP_BOOL    result            = MMP_TRUE;
    MMP_BOOL    En_ISP_CORE_MUTEX = MMP_TRUE;

    assert(ptDev);

    //
    //  IS ISP engine idle?   0x6FC VP0 D[5] , D[10]  0: idle, 1: busy
    //                              VP1 D[21], D[26]  0: idle, 1: busy
    //

    if (pISPctxt->WorkFlags & ISP_FLAGS_ENABLE_ISR_FIRE)
        En_ISP_CORE_MUTEX = MMP_FALSE;

    if (pISPctxt->OutInfo.EnableLcdOnFly)
        if (!(pISPctxt->WorkFlags & ISP_FLAGS_LCD_ONFLY_INIT))
            En_ISP_CORE_MUTEX = MMP_FALSE;

    if (En_ISP_CORE_MUTEX)
    {
        if (pISPctxt->ispCore == MMP_ISP_CORE_0)
            pthread_mutex_lock(&ISP_CORE_0_MUTEX);
        else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
            pthread_mutex_lock(&ISP_CORE_1_MUTEX);
    }

    isp_ReadHwReg(ISP_REG_ISP_ENGINE_STATUS, (MMP_UINT32 *)&status);
    //isp_ReadHwReg(REG_ADDR_TRANSFER(pISPctxt->ispCore, ISP_REG_ISP_ENGINE_STATUS), (MMP_UINT32 *)&status);

    if (En_ISP_CORE_MUTEX)
    {
        if (pISPctxt->ispCore == MMP_ISP_CORE_0)
            pthread_mutex_unlock(&ISP_CORE_0_MUTEX);
        else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
            pthread_mutex_unlock(&ISP_CORE_1_MUTEX);
    }

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        result = (status >> 5) & 1 | (status >> 10) & 1;
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        result = (status >> 21) & 1 | (status >> 26) & 1;

    return !result;
}

//=============================================================================
/**
 * Wait ISP Change Idle.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspWaitChangeIdle(
    void)
{
    return ISP_WaitISPChangeIdle();
}

//=============================================================================
/**
 * Wait ISP Slice Buffer Empty.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspWaitRawSliceBufferEmpty(
    void)
{
    return ISP_WaitRawSliceBufferEmpty();
}

//=============================================================================
/**
 * Driver Write Raw Data to Slice Buffer.
 *
 * This function is used in BMP image decoder for partially scaling the large
 * bmp image to a small destination surface.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspWriteDataToSliceBuf(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    //ISP_LogReg(pISPctxt->ispCore);
    ISP_WirteRawSliceFire_Reg(pISPctxt, &pISPctxt->OutInfo);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Wait ISP UI Buffer Change.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspWaitUIBufferChange(
    ISP_DEVICE ptDev,
    MMP_UINT   uiBufferIndex)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;
    MMP_UINT16  timeOut   = 0;
    MMP_UINT32  bufIdx    = 0;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    do
    {
        isp_ReadHwReg(ISP_REG_ISP_UIBUFFER_STATUS, (MMP_UINT32 *)&bufIdx);
        //isp_ReadHwReg(REG_ADDR_TRANSFER(pISPctxt->ispCore, ISP_REG_ISP_UIBUFFER_STATUS), (MMP_UINT32 *)&bufIdx);

        //For Debug
        isp_msg(0, "bufIdx(%d)(%d)\n", bufIdx, uiBufferIndex);

        isp_sleep(20);
        if (++timeOut > 10)
        {
            result = ISP_ERR_NOT_IDLE;
            goto end;
        }
    } while (bufIdx == (MMP_UINT8)uiBufferIndex);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Register ISP IRQ.
 *
 * @param isphandler  The callback function to handle ISP Interrupt.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpISPRegisterIRQ(
    ISP_DEVICE  ptDev,
    ISP_handler isphandler)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

#ifndef WIN32
    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
    {
        ithIntrDisableIrq(ITH_INTR_ISP_CORE0);
        ithIntrRegisterHandlerIrq(ITH_INTR_ISP_CORE0, isphandler, NULL);
        ithIntrSetTriggerModeIrq(ITH_INTR_ISP_CORE0, ITH_INTR_EDGE);
        ithIntrSetTriggerLevelIrq(ITH_INTR_ISP_CORE0, ITH_INTR_HIGH_RISING);
        ithIntrEnableIrq(ITH_INTR_ISP_CORE0);
    }
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
    {
        ithIntrDisableIrq(ITH_INTR_ISP_CORE1);
        ithIntrRegisterHandlerIrq(ITH_INTR_ISP_CORE1, isphandler, NULL);
        ithIntrSetTriggerModeIrq(ITH_INTR_ISP_CORE1, ITH_INTR_EDGE);
        ithIntrSetTriggerLevelIrq(ITH_INTR_ISP_CORE1, ITH_INTR_HIGH_RISING);
        ithIntrEnableIrq(ITH_INTR_ISP_CORE1);
    }
    else
        result = ISP_ERR_INVALID_PARAM;
#endif

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable ISP CMDQ.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableCMDQ(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags |= ISP_FLAGS_ENABLE_CMDQ;

    ISP_CmdSelect(pISPctxt, &gtIspHwOpCmdQ);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable ISP CMDQ.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableCMDQ(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_ENABLE_CMDQ);

    ISP_CmdSelect(pISPctxt, &gtIspHwOpMMIO);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableInterrupt(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags |= ISP_FLAGS_INTERRUPT_INIT;

    //Wait ISP Queue Idle
    //IspQ_WaitIdle(0x00E0, 0x00E0);

    pISPctxt->OutInfo.EnableInterrupt = MMP_TRUE;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableInterrupt(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_INTERRUPT_INIT);

    //Wait ISP Queue Idle
    //IspQ_WaitIdle(0x00E0, 0x00E0);

    //Wait ISP Update idle
    //ISP_WaitISPChangeIdle();

    pISPctxt->OutInfo.EnableInterrupt = MMP_FALSE;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Clear ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspClearInterrupt(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    if (pISPctxt->ispCore == MMP_ISP_CORE_0)
        isp_WriteHwRegMask(ISP_REG_SET500, (MMP_UINT32)(
                 ((1 & ISP_BIT_ISP_CLEAR_INTERRUPT_EN) << ISP_SHT_ISP_CLEAR_INTERRUPT_EN) |
                 ((0 & ISP_BIT_DRIVER_FIRE_EN)         << ISP_SHT_DRIVER_FIRE_EN)), (MMP_UINT32)(
                 (1 << ISP_SHT_ISP_CLEAR_INTERRUPT_EN) |
                 (1 << ISP_SHT_DRIVER_FIRE_EN)));
    else if (pISPctxt->ispCore == MMP_ISP_CORE_1)
        isp_WriteHwRegMask(ISP_REG_SET500 + ISP_CORE_1_BASE, (MMP_UINT32)(
                 ((1 & ISP_BIT_ISP_CLEAR_INTERRUPT_EN) << ISP_SHT_ISP_CLEAR_INTERRUPT_EN) |
                 ((0 & ISP_BIT_DRIVER_FIRE_EN)         << ISP_SHT_DRIVER_FIRE_EN)), (MMP_UINT32)(
                 (1 << ISP_SHT_ISP_CLEAR_INTERRUPT_EN) |
                 (1 << ISP_SHT_DRIVER_FIRE_EN)));
    else
        result = ISP_ERR_INVALID_PARAM;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable ISP Motion Detection.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableMotionDetection(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags |= ISP_FLAGS_ENABLE_MOTION_DETECTION;

    pISPctxt->DeInterlace.EnMotionDetection = MMP_TRUE;
    pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable ISP Motion Detection.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableMotionDetection(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_ENABLE_MOTION_DETECTION);

    pISPctxt->DeInterlace.EnMotionDetection = MMP_FALSE;
    pISPctxt->UpdateFlags                  |= ISP_FLAGS_UPDATE_DeInterlaceParam;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set deinterlace type. Only support YUV 420 input.
 *
 * @param type  Specify the deinterlace type.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetDeinterlaceType(
    ISP_DEVICE               ptDev,
    MMP_ISP_DEINTERLACE_TYPE type)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->DeInterlace.DeinterMode = type;
    pISPctxt->UpdateFlags            |= ISP_FLAGS_UPDATE_DeInterlaceParam;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Enable ISP ISR Fire.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableISRFire(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->EnableISRFire = MMP_TRUE;
    pISPctxt->WorkFlags    |= ISP_FLAGS_ENABLE_ISR_FIRE;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable ISP ISR Fire.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableISRFire(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->EnableISRFire = MMP_FALSE;
    pISPctxt->WorkFlags    &= (~ISP_FLAGS_ENABLE_ISR_FIRE);

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Set Slice amount of H.264 Low Latency.
 *
 * @param sliceCount  Set the slice amount between H.264 and ISP.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspSetLowLatencySlice(
    ISP_DEVICE ptDev,
    MMP_UINT   sliceCount)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->OutInfo.LOWLATENCYSliceNum = sliceCount;
    pISPctxt->UpdateFlags               |= ISP_FLAGS_UPDATE_OutParameter;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}


//=============================================================================
/**
 * Enable H.264 Low Latency.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspEnableLowLatency(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags |= ISP_FLAGS_ENABLE_LOW_LATENCY;

    pISPctxt->EngineMode.EnableLOWLATENCY = MMP_TRUE;
    pISPctxt->UpdateFlags                |= ISP_FLAGS_UPDATE_EngineMode;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * Disable H.264 Low Latency.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_RESULT
mmpIspDisableLowLatency(
    ISP_DEVICE ptDev)
{
    ISP_RESULT  result    = ISP_SUCCESS;
    ISP_CONTEXT *pISPctxt = (ISP_CONTEXT *)ptDev;

    GOTO_END_IF_NOT_INITIALIZE(result, pISPctxt);

    pISPctxt->WorkFlags &= (~ISP_FLAGS_ENABLE_LOW_LATENCY);

    pISPctxt->EngineMode.EnableLOWLATENCY = MMP_FALSE;
    pISPctxt->UpdateFlags                |= ISP_FLAGS_UPDATE_EngineMode;

end:
    if (result)
        isp_msg_ex(ISP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}
