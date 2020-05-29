#include "xmodemprotocol.h"

uint64_t calculatorCheckSum(const uint8_t *pdata, uint64_t len)
{
    uint64_t sum = 0;

    while(len--)
    {
        sum += *pdata++;
    }

    return sum;
}

void fileDataProtocolCreate(uint8_t *pbuf,uint64_t *len,uint16_t id,uint8_t package_id)
{

    pbuf[XM_HEAD_OFFSET] = XM_FILE_DATA_HEAD_CODE;
    pbuf[XM_LOW_ADDR_OFFSET] = id;
    pbuf[XM_HIGHT_ADDR_OFFSET] = id>>8;
    pbuf[XM_PACKAGE_ID_OFFSET] = package_id;
    pbuf[XM_PACKAGE_FID_OFFSET] = (uint8_t)(~package_id);
    uint16_t cs = calculatorCheckSum(&pbuf[XM_DATA_OFFSET],*len);
    pbuf[*len+5] = cs;
    pbuf[*len+5+1] = cs>>8;

    *len = *len + 7;
}

void fileInfoProtocolCreate(uint8_t *pbuf,uint64_t *len,uint16_t id,uint8_t package_id)
{

    pbuf[XM_HEAD_OFFSET] = XM_FILE_INFO_HEAD_CODE;
    pbuf[XM_LOW_ADDR_OFFSET] = id;
    pbuf[XM_HIGHT_ADDR_OFFSET] = id>>8;
    pbuf[XM_PACKAGE_ID_OFFSET] = package_id;
    pbuf[XM_PACKAGE_FID_OFFSET] = (uint8_t)(~package_id);
    uint16_t cs = calculatorCheckSum(&pbuf[XM_DATA_OFFSET],*len);
    pbuf[*len+5] = cs;
    pbuf[*len+5+1] = cs>>8;

    *len = *len + 7;
}
