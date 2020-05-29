#ifndef SYSPROTOCOL_H
#define SYSPROTOCOL_H

#include <stdint.h>

//头部标识码定义
#define ACK_HEAD_CODE           0xA5
#define RELAY_HEAD_CODE         0xA6
#define SENSOR_DATA_HEAD_CODE   0xA7
#define DEV_DATA_HEAD_CODE      0xA8
//命令帧各项偏移定义
#define CMD_L_ID_OFFSET         0x01
#define CMD_H_ID_OFFSET         0x02
#define CMD_CMD_OFFSET          0x03
#define CMD_DATA_OFFSET         0x04
//命令帧定义
#define CMD_SET_UP_CYCLE        0x01
#define CMD_SET_BAUD            0x02
#define CMD_SET_UP_NUM          0x03
#define CMD_SET_BT_SW           0x04
#define CMD_SET_FILTER_TYPE     0x05
#define CMD_SET_UP__SW          0x06
#define CMD_RESET               0x07
//转发帧各项偏移定义
#define REALY_CMD_OFFSET        0x01
#define RELAY_LEN_OFFSET        0x02
#define RELAY_DATA_OFFSET       0x03
//转发帧定义
#define RELAY_BT_CODE           0x01
#define RELAY_SENSOR_CODE       0x02

//传感器数量偏移定义
#define SENSOR_NUM_OFFSET  0x01
//设备各项偏移定义
#define DEV_L_ID_OFFSET     0x01
#define DEV_H_OFFSET        0x02
#define DEV_DATA_ID_OFFSET  0x03
#define DEV_DATA_LEN_OFFSET 0x04
#define DEV_DATA_OFFSET     0x05
//设备帧定义
#define DEV_BAT_CODE        0x01

#define DEV_ID              0x01

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    uint8_t head1;
    uint8_t head2;
    uint8_t addr_L;
    uint8_t addr_H;
    uint8_t cmd;
    uint32_t para;
    uint16_t cs;
}SensorCmd_Packet;		//传感器命令包

typedef struct
{
    uint8_t head;
    uint8_t addr_L;
    uint8_t addr_H;
    uint8_t cmd;
    uint8_t channels;
    uint16_t *pChannelData;
    uint16_t cs;
}SensorData_Packet;		//传感器数据包

typedef struct
{
    uint8_t head;
    uint8_t addr_L;
    uint8_t addr_H;
    uint8_t ack_code;
}SensorAck_Packet;		//传感器应答包

typedef struct
{
    uint8_t head;
    uint8_t addr_L;
    uint8_t addr_H;
    uint8_t cmd;
    uint32_t para;
    uint16_t cs;
}SYSCmd_Packet;				//采集卡命令报

typedef struct
{
    uint8_t head;
    uint8_t cmd;
    uint8_t len;
    const uint8_t *pData;
    uint8_t cs;
}SYSRelay_Packet;			//采集卡转发包

typedef struct
{
    uint16_t id;
    uint8_t channels;
    uint16_t data[12];
}SYSData_PerSensor_Packet;	//采集卡每一传感器数据包

typedef struct
{
    uint8_t head;
    uint8_t sensor_num;
    const uint8_t *pSensor;
    uint16_t cs;
}SYSData_Packet;				//采集卡数据包

typedef struct
{
    uint8_t head;
    uint8_t l_id;
    uint8_t h_id;
    uint8_t data_id;
    uint8_t data_len;
    const uint8_t *pdata;
    uint8_t cs;
}SYSData2_Packet;

typedef struct
{
    uint8_t head;
    uint16_t addr;
    uint8_t ack_code;
}SYSAck_Packet;					//采集卡应答包

struct CmdPacketParameter
{
    uint8_t cmd;
    uint32_t parameter;
    uint32_t len;
    uint8_t *pout;
};

struct RelayPacketParameter
{
    uint8_t cmd;
    uint8_t relayLen;
    const uint8_t *pRelayData;
    uint32_t len;
    uint8_t *pout;
};

#pragma pack(pop)

extern "C"
{

    void createCmdPacket(CmdPacketParameter *p_cmdPacketParameter);
    void CreateUpdateCycle(CmdPacketParameter *p_cmdPacketParameter);
    void CreateUpdateBaudrate(CmdPacketParameter *p_cmdPacketParameter);
    void CreateUpdateSensorNumber(CmdPacketParameter *p_cmdPacketParameter);
    void CreateSetBtSwitch(CmdPacketParameter *p_cmdPacketParameter);
    void CreateSetFilterType(CmdPacketParameter *p_cmdPacketParameter);
    void CreateUpdateSwitch(CmdPacketParameter *p_cmdPacketParameter);
    void CreateRst(CmdPacketParameter *p_cmdPacketParameter);
    void createRelayPacket(RelayPacketParameter *pRelayPacketPar);
    void CreateBtRelay(RelayPacketParameter *pRelayPacketPar);
    void CreateSensorRelay(RelayPacketParameter *pRelayPacketPar);

}

#endif // SYSPROTOCOL_H
