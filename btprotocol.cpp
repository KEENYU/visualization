#include "btprotocol.h"
#include <string.h>
#include <QString>

uint32_t quaryVersion(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+VERSION",10);
    return 10;
}

uint32_t resetBtDevice(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+RST",6);
    return 6;
}

uint32_t disconnectDevice(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+DISC",7);
    return 7;
}


uint32_t quary_BLE_MAC(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+MAC",6);
    return 6;
}

uint32_t quary_SPP_MACS(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+MACS",7);
    return 7;
}

uint32_t quary_Baudrate(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+BAUD",7);
    return 7;
}

uint32_t set_Baudrate(uint8_t *pbuf,uint32_t offset,uint8_t baudrate_id)
{
    QString string = QString("AT+BAUD%1").arg(baudrate_id);
    std::string cfg_std_string = string.toStdString();
    const char *p_char_data = cfg_std_string.c_str();
    uint32_t len  = strlen(p_char_data);
    memcpy(&pbuf[offset],p_char_data,len);

    return len;
}

uint32_t quary_BLE_NAME(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+NAME",7);
    return 7;
}

uint32_t set_BEL_NAME(uint8_t *pbuf,uint32_t offset,const char *name)
{
    QString string = QString("AT+NAME%1").arg(name);
    std::string cfg_std_string = string.toStdString();
    const char *p_char_data = cfg_std_string.c_str();
    uint32_t len  = strlen(p_char_data);
    memcpy(&pbuf[offset],p_char_data,len);

    return len;
}

uint32_t quary_SPP_NAME(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+NAMES",7);
    return 7;
}

uint32_t set_SPP_NAMES(uint8_t *pbuf,uint32_t offset,const char *name)
{
    QString string = QString("AT+NAMES%1").arg(name);
    std::string cfg_std_string = string.toStdString();
    const char *p_char_data = cfg_std_string.c_str();
    uint32_t len  = strlen(p_char_data);
    memcpy(&pbuf[offset],p_char_data,len);

    return len;
}

uint32_t quary_Type(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+TYPE",7);
    return 7;
}

uint32_t set_Type(uint8_t *pbuf,uint32_t offset,uint8_t type_id)
{
    QString string = QString("AT+TYPE%1").arg(type_id);
    std::string cfg_std_string = string.toStdString();
    const char *p_char_data = cfg_std_string.c_str();
    uint32_t len  = strlen(p_char_data);
    memcpy(&pbuf[offset],p_char_data,len);

    return len;
}

uint32_t quary_PIN(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+PIN",6);
    return 6;
}

uint32_t set_PIN(uint8_t *pbuf,uint32_t offset,uint32_t pw)
{
    QString string = QString("AT+TYPE%1").arg(pw,4,10,QChar('0'));
    std::string cfg_std_string = string.toStdString();
    const char *p_char_data = cfg_std_string.c_str();
    uint32_t len  = strlen(p_char_data);
    memcpy(&pbuf[offset],p_char_data,len);

    return len;
}

uint32_t setToDefault(uint8_t *pbuf,uint32_t offset)
{
    memcpy(&pbuf[offset],"AT+DEFAULT",10);
    return 10;
}

