#include <unistd.h>
#include "iic/mmp_iic.h"
#include "capture_module/gc0328.h"
#include "capture_module.h"

//=============================================================================
//                Constant Definition
//=============================================================================
#define SENSOR_ID           0x9D
#define POWER_MANAGEMENT    0xFC
#define RESET_MASK          (1 << 7)

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
typedef struct _REGPAIR
{
    uint8_t     addr;
    uint16_t    value;
} REGPAIR;

typedef struct GC0328CaptureModuleDriverStruct
{
    CaptureModuleDriverStruct base;
} GC0328CaptureModuleDriverStruct;

//=============================================================================
//                Global Data Definition
//=============================================================================
static uint8_t              GC0328_IICADDR      = 0x42 >> 1;
static MMP_BOOL             gtGC0328InitDone    = MMP_FALSE;

/* 32-Lead LFCSP , page 108*/
static REGPAIR              INPUT_REG[]         =
{
    //GC0328 initial setting
    {0xfe, 0x80},
    {0xfe, 0x80},
    {0xfc, 0x16},
    {0xfc, 0x16},
    {0xfc, 0x16},
    {0xfc, 0x16},
    {0xf1, 0x00},
    {0xf2, 0x00},
    {0xfe, 0x00},
    {0x4f, 0x00},
    {0x03, 0x00},
    {0x04, 0xc0},
    {0x42, 0x00},
    {0x77, 0x5a},
    {0x78, 0x40},
    {0x79, 0x56},

    {0xfe, 0x00},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x09, 0x00},
    {0x0a, 0x00},
    {0x0b, 0x00},
    {0x0c, 0x00},
    {0x16, 0x00},
    {0x17, 0x14},
    {0x18, 0x0e},
    {0x19, 0x06},

    {0x1b, 0x48},   //gc0308 0x26
    {0x1f, 0xC8},   //gc0308 0x0d
    {0x20, 0x01},
    {0x21, 0x78},
    {0x22, 0xb0},
    {0x23, 0x04},   // 0x06 20140519 lanking GC0328C
    {0x24, 0x33},   // 0x11  Benson (sync_drv:16ma, data driver:4ma , pclk driver:8ma (likegc0308)) //0x32
    {0x26, 0x00},
    {0x50, 0x01},   //crop mode

    //global gain for range
    {0x70, 0x45},

    /////////////banding/////////////
    {0x05, 0x02},   //hb
    {0x06, 0x2c},   //
    {0x07, 0x00},   //vb
    {0x08, 0xb8},   //
    {0xfe, 0x01},   //
    {0x29, 0x00},   //anti-flicker step [11:8]
    {0x2a, 0x60},   //anti-flicker step [7:0]
    {0x2b, 0x02},   //exp level 0  14.28fps
    {0x2c, 0xa0},   //
    {0x2d, 0x03},   //exp level 1  12.50fps
    {0x2e, 0x00},   //
    {0x2f, 0x03},   //exp level 2  10.00fps
    {0x30, 0xc0},   //
    {0x31, 0x05},   //exp level 3  7.14fps
    {0x32, 0x40},   //
    {0xfe, 0x00},   //

    ///////////////AWB//////////////
    {0xfe, 0x01},
    {0x50, 0x00},
    {0x4f, 0x00},
    {0x4c, 0x01},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4d, 0x30},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x40},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x50},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x60},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x70},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4f, 0x01},
    {0x50, 0x88},
    {0xfe, 0x00},

    //////////// BLK//////////////////////
    {0xfe, 0x00},
    {0x27, 0xb7},
    {0x28, 0x7F},
    {0x29, 0x20},
    {0x33, 0x20},
    {0x34, 0x20},
    {0x35, 0x20},
    {0x36, 0x20},
    {0x32, 0x08},
    {0x3b, 0x00},
    {0x3c, 0x00},
    {0x3d, 0x00},
    {0x3e, 0x00},
    {0x47, 0x00},
    {0x48, 0x00},

    //////////// block enable/////////////
    {0x40, 0x7f},
    {0x41, 0x26},
    {0x42, 0xfb},
    {0x44, 0x22},   //yuv //Benson 0x02
    {0x45, 0x00},
    {0x46, 0x03},   //Benson 0x02
    {0x4f, 0x01},
    {0x4b, 0x01},
    {0x50, 0x01},

    /////DN & EEINTP/////
    {0x7e, 0x0a},
    {0x7f, 0x03},
    {0x80, 0x27},  //  20140915
    {0x81, 0x15},
    {0x82, 0x90},
    {0x82, 0x90},
    {0x83, 0x02},
    {0x84, 0x23},  // 0x22 20140915
    {0x90, 0x2c},
    {0x92, 0x02},
    {0x94, 0x02},
    {0x95, 0x35},

    ////////////YCP///////////
    {0xd1, 0x24},   // 0x30 for front
    {0xd2, 0x24},   // 0x30 for front
    {0xd3, 0x40},
    {0xdd, 0xd3},
    {0xde, 0x38},
    {0xe4, 0x88},
    {0xe5, 0x40},
    {0xd7, 0x0e},

    ///////////rgb gamma ////////////
    {0xfe, 0x00},
    {0xbf, 0x0e},
    {0xc0, 0x1c},
    {0xc1, 0x34},
    {0xc2, 0x48},
    {0xc3, 0x5a},
    {0xc4, 0x6e},
    {0xc5, 0x80},
    {0xc6, 0x9c},
    {0xc7, 0xb4},
    {0xc8, 0xc7},
    {0xc9, 0xd7},
    {0xca, 0xe3},
    {0xcb, 0xed},
    {0xcc, 0xf2},
    {0xcd, 0xf8},
    {0xce, 0xfd},
    {0xcf, 0xff},

    /////////////Y gamma//////////
    {0xfe, 0x00},
    {0x63, 0x00},
    {0x64, 0x05},
    {0x65, 0x0b},
    {0x66, 0x19},
    {0x67, 0x2e},
    {0x68, 0x40},
    {0x69, 0x54},
    {0x6a, 0x66},
    {0x6b, 0x86},
    {0x6c, 0xa7},
    {0x6d, 0xc6},
    {0x6e, 0xe4},
    {0x6f, 0xff},

    //////////////ASDE/////////////
    {0xfe, 0x01},
    {0x18, 0x02},
    {0xfe, 0x00},
    {0x97, 0x30},
    {0x98, 0x00},
    {0x9b, 0x60},
    {0x9c, 0x60},
    {0xa4, 0x50},
    {0xa8, 0x80},
    {0xaa, 0x40},
    {0xa2, 0x23},
    {0xad, 0x28},

    //////////////abs///////////
    {0xfe, 0x01},
    {0x9c, 0x00},
    {0x9e, 0xc0},
    {0x9f, 0x40},

    ////////////// AEC////////////
    {0xfe, 0x01},
    {0x08, 0xa0},
    {0x09, 0xe8},
    {0x10, 0x08},
    {0x11, 0x21},
    {0x12, 0x11},
    {0x13, 0x45},
    {0x15, 0xfc},
    {0x18, 0x02},
    {0x21, 0xf0},
    {0x22, 0x60},
    {0x23, 0x30},
    {0x25, 0x00},
    {0x24, 0x14},
    {0x3d, 0x80},
    {0x3e, 0x40},

    ////////////////AWB///////////
    {0xfe, 0x01},
    {0x51, 0x88},
    {0x52, 0x12},
    {0x53, 0x80},
    {0x54, 0x60},
    {0x55, 0x01},
    {0x56, 0x02},
    {0x58, 0x00},
    {0x5b, 0x02},
    {0x5e, 0xa4},
    {0x5f, 0x8a},
    {0x61, 0xdc},
    {0x62, 0xdc},
    {0x70, 0xfc},
    {0x71, 0x10},
    {0x72, 0x30},
    {0x73, 0x0b},
    {0x74, 0x0b},
    {0x75, 0x01},
    {0x76, 0x00},
    {0x77, 0x40},
    {0x78, 0x70},
    {0x79, 0x00},
    {0x7b, 0x00},
    {0x7c, 0x71},
    {0x7d, 0x00},
    {0x80, 0x70},
    {0x81, 0x58},
    {0x82, 0x98},
    {0x83, 0x60},
    {0x84, 0x58},
    {0x85, 0x50},
    {0xfe, 0x00},

    ////////////////LSC////////////////
    {0xfe, 0x01},
    {0xc0, 0x10},
    {0xc1, 0x0c},
    {0xc2, 0x0a},
    {0xc6, 0x0e},
    {0xc7, 0x0b},
    {0xc8, 0x0a},
    {0xba, 0x26},
    {0xbb, 0x1c},
    {0xbc, 0x1d},
    {0xb4, 0x23},
    {0xb5, 0x1c},
    {0xb6, 0x1a},
    {0xc3, 0x00},
    {0xc4, 0x00},
    {0xc5, 0x00},
    {0xc9, 0x00},
    {0xca, 0x00},
    {0xcb, 0x00},
    {0xbd, 0x00},
    {0xbe, 0x00},
    {0xbf, 0x00},
    {0xb7, 0x07},
    {0xb8, 0x05},
    {0xb9, 0x05},
    {0xa8, 0x07},
    {0xa9, 0x06},
    {0xaa, 0x00},
    {0xab, 0x04},
    {0xac, 0x00},
    {0xad, 0x02},
    {0xae, 0x0d},
    {0xaf, 0x05},
    {0xb0, 0x00},
    {0xb1, 0x07},
    {0xb2, 0x03},
    {0xb3, 0x00},
    {0xa4, 0x00},
    {0xa5, 0x00},
    {0xa6, 0x00},
    {0xa7, 0x00},
    {0xa1, 0x3c},
    {0xa2, 0x50},
    {0xfe, 0x00},

    ///////////////CCT ///////////
    {0xb1, 0x12},
    {0xb2, 0xf5},
    {0xb3, 0xfe},
    {0xb4, 0xe0},
    {0xb5, 0x15},
    {0xb6, 0xc8},

    /*   /////skin CC for front //////
       (0xb1 , 0x00);
       (0xb2 , 0x00);
       (0xb3 , 0x00);
       (0xb4 , 0xf0);
       (0xb5 , 0x00);
       (0xb6 , 0x00);
     */

    ///////////////AWB////////////////
    {0xfe, 0x01},
    {0x50, 0x00},
    {0xfe, 0x01},
    {0x4f, 0x00},
    {0x4c, 0x01},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4d, 0x34},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x02},
    {0x4e, 0x02},
    {0x4d, 0x44},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x53},
    {0x4e, 0x00},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4e, 0x04},
    {0x4d, 0x65},
    {0x4e, 0x04},
    {0x4d, 0x73},
    {0x4e, 0x20},
    {0x4d, 0x83},
    {0x4e, 0x20},
    {0x4f, 0x01},
    {0x50, 0x88},
    /////////output////////
    {0xfa, 0x00},  //Benson add  for pclk
    {0xfe, 0x00},
    {0xf1, 0x07},
    {0xf2, 0x01},
};

static CAP_TIMINFO_TABLE    GC0328_TABLE [] = {
                                                                                                                                    //Index, HActive, VActive,  Rate,            FrameRate,                 Hpor,   Vpor,  HStar,     HEnd,   VStar1,   VEnd1,  VStar2,   VEnd2,
    {0,     640,    480,        3000, CAP_FRAMERATE_30HZ,         0,      0,      0,        0,        0,      0,   0,       0   },  //480i60     // Benson
};

//=============================================================================
//                Private Function Definition
//=============================================================================
static uint8_t _GC0328_ReadI2c_Byte(uint8_t RegAddr)
{
    uint8_t     data;
    MMP_RESULT  flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicReceiveData(IIC_PORT_0, IIC_MASTER_MODE, GC0328_IICADDR, &RegAddr, 1, &data, sizeof(data))))
    {
        printf("GC0328 I2C Read error, reg = %02x\n", RegAddr);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return data;
}

static MMP_RESULT _GC0328_WriteI2c_Byte(uint8_t RegAddr, uint8_t data)
{
    MMP_RESULT flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicSendData(IIC_PORT_0, IIC_MASTER_MODE, GC0328_IICADDR, RegAddr, &data, sizeof(data))))
    {
        printf("GC0328 I2c write error, reg = %02x val =%02x\n", RegAddr, data);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return flag;
}

static MMP_RESULT _GC0328_WriteI2c_ByteMask(uint8_t RegAddr, uint8_t data, uint8_t mask)
{
    uint8_t     value;
    MMP_RESULT  flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicReceiveData(IIC_PORT_0, IIC_MASTER_MODE, GC0328_IICADDR, &RegAddr, 1, &value, 1)))
    {
        printf("GC0328 I2C Read error, reg = %02x\n", RegAddr);
        mmpIicGenStop(IIC_PORT_0);
    }

    value = ((value & ~mask) | (data & mask));

    if (0 != (flag = mmpIicSendData(IIC_PORT_0, IIC_MASTER_MODE, GC0328_IICADDR, RegAddr, &value, 1)))
    {
        printf("GC0328 I2c write error, reg = %02x val =%02x\n", RegAddr, value);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return flag;
}

static void _GC0328_SWReset()
{
    _GC0328_WriteI2c_Byte(0xFE, 0x80);
}

static void _Set_GC0328_Input_Setting(void)
{
    uint16_t i;

    for (i = 0; i < (sizeof(INPUT_REG) / sizeof(REGPAIR)); i++)
    {
        _GC0328_WriteI2c_Byte(INPUT_REG[i].addr, INPUT_REG[i].value);
    }
}

static MMP_BOOL _GC0328_ReadSensorID(void)
{
    uint8_t SenSorID;

    SenSorID = _GC0328_ReadI2c_Byte(0xF0);
    if (SenSorID != SENSOR_ID)
    {
        printf("GC0328 Get Wrong SensorID ,SenSorID = 0x%x\n", SenSorID);
        return MMP_FALSE;
    }
    else
        return MMP_TRUE;
}

//=============================================================================
//                Public Function Definition
//=============================================================================

void GC0328_PowerDown(
    MMP_BOOL enable)
{
    //When PDBP is set to 1, setting the PWRDWN bit switches the GC0328 to a chip-wide power-down mode.
    if (enable)
    {
        gtGC0328InitDone = MMP_FALSE;
        _GC0328_WriteI2c_Byte(POWER_MANAGEMENT, 0x00);
    }
    else
        _GC0328_WriteI2c_Byte(POWER_MANAGEMENT, 0x16);
}

void GC0328Initialize(void)
{
    MMP_BOOL    bSensorID;
    uint16_t    data;
    printf("GC0328Initial here\n");

    gtGC0328InitDone    = MMP_FALSE;
    bSensorID           = _GC0328_ReadSensorID();
    if (!bSensorID)
    {
        printf("GC0328Initial fail!\n");
        return 0;
    }

    _GC0328_SWReset();
    usleep(1000 * 10);
    _Set_GC0328_Input_Setting();
    gtGC0328InitDone = MMP_TRUE;
}

void GC0328Terminate(void)
{
    printf("GC0328Terminate\n");
    GC0328_PowerDown(MMP_TRUE);
}

void GC0328OutputPinTriState(unsigned char flag)
{
}

unsigned char GC0328IsSignalStable(void)
{
    return true;
}

void GC0328GetProperty(CAP_GET_PROPERTY *pGetProperty)
{
    uint16_t i;
    pGetProperty->GetTopFieldPolarity   = MMP_FALSE;
    pGetProperty->GetHeight             = 480;
    pGetProperty->GetWidth              = 640;
    pGetProperty->Rate                  = 3000;
    pGetProperty->GetModuleIsInterlace  = 0;

    for (i = 0; i < (sizeof(GC0328_TABLE) / sizeof(CAP_TIMINFO_TABLE)); ++i)
    {
        if ((pGetProperty->GetWidth == GC0328_TABLE[i].HActive) && (pGetProperty->GetHeight == GC0328_TABLE[i].VActive) && pGetProperty->Rate == GC0328_TABLE[i].Rate)
        {
            pGetProperty->HPolarity         = GC0328_TABLE[i].HPolarity;
            pGetProperty->VPolarity         = GC0328_TABLE[i].VPolarity;
            pGetProperty->FrameRate         = GC0328_TABLE[i].FrameRate;
            pGetProperty->matchResolution   = MMP_TRUE;

            pGetProperty->HStar             = GC0328_TABLE[i].HStar;
            pGetProperty->HEnd              = GC0328_TABLE[i].HEnd;
            pGetProperty->VStar1            = GC0328_TABLE[i].VStar1;
            pGetProperty->VEnd1             = GC0328_TABLE[i].VEnd1;
            pGetProperty->VStar2            = GC0328_TABLE[i].VStar2;
            pGetProperty->VEnd2             = GC0328_TABLE[i].VEnd2;
        }
    }
}

void GC0328PowerDown(unsigned char enable)
{
    GC0328_PowerDown(enable);
}

void GC0328ForCaptureDriverSetting(CAP_CONTEXT *Capctxt)
{
    /* Input Format default Setting */
    Capctxt->Interleave = Progressive;

    /* Input Data Format Setting */
    // 8bit bus
    Capctxt->YUV422Format               = CAP_IN_YUV422_YUYV;
    Capctxt->EnDEMode                   = MMP_TRUE;
    Capctxt->input_protocol             = BT_601;

    Capctxt->input_video_source.HSYNCDE = MMP_TRUE;
}

static void GC0328CaptureModuleDriver_Destory(CaptureModuleDriver base)
{
    GC0328CaptureModuleDriver self = (GC0328CaptureModuleDriver)base;
    if (self)
        free(self);
}

static CaptureModuleDriverInterfaceStruct interface =
{
    GC0328Initialize,
    GC0328Terminate,
    GC0328OutputPinTriState,
    GC0328IsSignalStable,
    GC0328GetProperty,
    GC0328PowerDown,
    GC0328ForCaptureDriverSetting,
    GC0328CaptureModuleDriver_Destory
};

CaptureModuleDriver GC0328CaptureModuleDriver_Create()
{
    GC0328CaptureModuleDriver self = calloc(1, sizeof(GC0328CaptureModuleDriverStruct));
    self->base.vtable   = &interface;
    self->base.type     = "GC0328";
    return (GC0328CaptureModuleDriver)self;
}