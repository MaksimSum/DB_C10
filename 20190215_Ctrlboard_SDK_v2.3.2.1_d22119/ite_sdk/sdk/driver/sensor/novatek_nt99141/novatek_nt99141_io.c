#include "ite/ith.h"
#include "ite/mmp_types.h"
#include "iic/iic.h"
#include "iic/mmp_iic.h"
#include "sensor/novatek_nt99141_io.h"    
#include "sensor/sensor_gpio_config.h"

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
static uint8_t   NOVATEK_IICADDR = 0x54 >> 1;

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================
uint8_t 
NOVATEK_ReadI2C_8Bit(
    uint16_t RegAddr)
{
    uint8_t     dbuf[256];
    uint8_t*    pdbuf = dbuf;
    uint8_t     value;	
	int     	result = 0;
	char        portname[4];
	ITPI2cInfo  evt;
	int         gMasterDev = 0;

	sprintf(portname, ":i2c%d", SN1_IIC_PORT);	
	gMasterDev = open(portname, 0);
	
    *pdbuf++ = (uint8_t)((RegAddr&0xff00)>>8);
    *pdbuf++ = (uint8_t)(RegAddr&0x00ff);

	evt.slaveAddress   = NOVATEK_IICADDR;		//�ﱵ�˸msalve address
	evt.cmdBuffer	   = dbuf;				   //���ǰe��slave�˸m��data buffer(�]�tcmooand)
	evt.cmdBufferSize	= 2;					//�ǰedata size,��쬰byte
	evt.dataBuffer	   = pdbuf; 			   //������slave�˸m��data buffer
	evt.dataBufferSize	= 1;					//����data size,��쬰byte

	result = read(gMasterDev, &evt, 1);
    
    value = (dbuf[2] & 0xFF); 
    
    printf("0x%04x = 0x%02x %d\n", RegAddr, value, value);           
      
    return value;
}

int 
NOVATEK_WriteI2C_8Bit(
    uint16_t RegAddr,
    uint8_t  data)
{
    uint8_t     dbuf[256];
    uint8_t*    pdbuf = dbuf;
    int      	result = 0;
	char        portname[4];
	ITPI2cInfo  evt;
	int         gMasterDev = 0;

	sprintf(portname, ":i2c%d", SN1_IIC_PORT);
	gMasterDev = open(portname, 0);
	
    *pdbuf++ = (uint8_t)((RegAddr&0xff00)>>8);
    *pdbuf++ = (uint8_t)(RegAddr&0x00ff);
    *pdbuf = (uint8_t)(data);

	evt.slaveAddress   = NOVATEK_IICADDR;		//�ﱵ�˸msalve address
	evt.cmdBuffer	   = dbuf;				   //���ǰe��slave�˸m��data buffer(�]�tcmooand)
	evt.cmdBufferSize  = 3; 				   //�ǰedata size,��쬰byte
    if(0 != (result = write(gMasterDev, &evt, 1)))
    {
        printf("NOVATEK_WriteI2C_8Bit I2c Write Error, reg=%04x val=%02x\n", RegAddr, data);
    }        
    
    return result;
}
