#ifndef BTPROTOCOL_H
#define BTPROTOCOL_H

#include <stdint.h>



extern "C"
{
    uint32_t quaryVersion(uint8_t *pbuf,uint32_t offset);
    uint32_t resetBtDevice(uint8_t *pbuf,uint32_t offset);
    uint32_t disconnectDevice(uint8_t *pbuf,uint32_t offset);
    uint32_t quary_BLE_MAC(uint8_t *pbuf,uint32_t offset);
    uint32_t quary_SPP_MACS(uint8_t *pbuf,uint32_t offset);
    uint32_t quary_Baudrate(uint8_t *pbuf,uint32_t offset);
    uint32_t set_Baudrate(uint8_t *pbuf,uint32_t offset,uint8_t baudrate_id);
    uint32_t quary_BLE_NAME(uint8_t *pbuf,uint32_t offset);
    uint32_t set_BEL_NAME(uint8_t *pbuf,uint32_t offset,const char *name);
    uint32_t quary_SPP_NAME(uint8_t *pbuf,uint32_t offset);
    uint32_t set_SPP_NAMES(uint8_t *pbuf,uint32_t offset,const char *name);
    uint32_t quary_Type(uint8_t *pbuf,uint32_t offset);
    uint32_t set_Type(uint8_t *pbuf,uint32_t offset,uint8_t type_id);
    uint32_t quary_PIN(uint8_t *pbuf,uint32_t offset);
    uint32_t set_PIN(uint8_t *pbuf,uint32_t offset,uint32_t pw);
    uint32_t setToDefault(uint8_t *pbuf,uint32_t offset);
}

#endif // BTPROTOCOL_H
