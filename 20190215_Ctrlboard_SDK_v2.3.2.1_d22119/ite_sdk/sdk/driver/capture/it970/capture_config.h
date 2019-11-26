#ifndef __CAP_CONFIG_H__
#define __CAP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Constant Definition
//=============================================================================

//#define CAP_USE_COLOR_EFFECT

#ifdef CFG_SENSOR_ENABLE
#define SENSOR_DEV
#else
#endif

/*define use how many capture max = 2*/
#define CAP_DEVICE_ID_MAX 2
/*define use how many memory buffer */
#define CAPTURE_MEM_BUF_COUNT 6
//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif