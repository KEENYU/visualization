#ifndef XMODEMPROTOCOL_H
#define XMODEMPROTOCOL_H

#include <stdint.h>

#define XM_HEAD_OFFSET          0
#define XM_LOW_ADDR_OFFSET      1
#define XM_HIGHT_ADDR_OFFSET    2
#define XM_PACKAGE_ID_OFFSET    3
#define XM_PACKAGE_FID_OFFSET   4
#define XM_DATA_OFFSET          5

#define XM_FILE_INFO_HEAD_CODE    0x01
#define XM_FILE_DATA_HEAD_CODE    0x02

#define XM_FILE_INFO_PACHAGE_SIZE   128
#define XM_FILE_DATA_PACKAGE_SIZE   1024

#define XM_ABORT_CODE           0x18
#define XM_RST_CODE             0x19

typedef enum  //枚举，第一个默认为0，后续递增
{
  check_type_sum,
  check_type_crc16
}xm_checkType;

extern "C"
{
    uint64_t calculatorCheckSum(const uint8_t *pdata,uint64_t len);
    void fileInfoProtocolCreate(uint8_t *pbuf,uint64_t *len,uint16_t id,uint8_t package_id);
    void fileDataProtocolCreate(uint8_t *pbuf,uint64_t *len,uint16_t id,uint8_t package_id);
    void sendAbort();
}



#endif // XMODEMPROTOCOL_H
