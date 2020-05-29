#include "parsethread.h"
#include "datacomingthread.h"

#include <QDebug>
#include <QTimer>

QMutex ParseThread::chartUpdateMutex;   //图列更新互斥锁

ParseThread::ParseThread(QObject *parent) :
    QThread(parent)
{
    relayPacketParameter.pout = sendBuf;
    cmdPacketParameter.pout  =sendBuf;
    sensorConnectedTable.clear();
    clearConnectedTable();

    checkConnectedTimer = new QTimer(parent);
    connect(checkConnectedTimer,SIGNAL(timeout()),this,SLOT(slot_checkConnectedTimerOut()));
    checkConnectedTimer->start(5000);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：getConnectedTable
 * 函数描述：读取传感器已连接的ID表
 * 输入参数：
 * **返回值：QList<uint16_t> 类型的ID表
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
QMap<uint16_t,bool> &ParseThread::getConnectedTable()
{
    return sensorConnectedTable;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_sendRelayPacket
 * 函数描述：发送转发数据槽函数
 * 输入参数：cmd-命令，len-转发数据长度，pdata-数据指针(只读)
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_sendRelayPacket(const uint8_t cmd, const uint32_t len, const uint8_t *pdata)
{
    relayPacketParameter.cmd = cmd;
    relayPacketParameter.relayLen = len;
    relayPacketParameter.pRelayData = pdata;
    createRelayPacket(&relayPacketParameter);
    emit sendData(relayPacketParameter.pout,relayPacketParameter.len);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_sendBTRelayPacket
 * 函数描述：发送蓝牙转发数据槽函数
 * 输入参数：len-转发数据长度，pdata-数据指针(只读)
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_sendBTRelayPacket(const uint8_t *pdata,const uint32_t len)
{
    relayPacketParameter.cmd = RELAY_BT_CODE;
    relayPacketParameter.relayLen = len;
    relayPacketParameter.pRelayData = pdata;
    createRelayPacket(&relayPacketParameter);
    emit sendData(relayPacketParameter.pout,relayPacketParameter.len);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_sendSensorRelayPacket
 * 函数描述：发送传感器转发数据槽函数
 * 输入参数：len-转发数据长度，pdata-数据指针(只读)
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_sendSensorRelayPacket(const uint8_t *pdata,const uint32_t len)
{
    relayPacketParameter.cmd = RELAY_SENSOR_CODE;
    relayPacketParameter.relayLen = len;
    relayPacketParameter.pRelayData = pdata;
    createRelayPacket(&relayPacketParameter);
    emit sendData(relayPacketParameter.pout,relayPacketParameter.len);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_sendCmdPacket
 * 函数描述：发送命令数据槽函数
 * 输入参数：cmd-命令，pdata-数据指针(只读)
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_sendCmdPacket(const uint8_t cmd, const uint32_t parameter)
{
    cmdPacketParameter.cmd  = cmd;
    cmdPacketParameter.parameter = parameter;
    createCmdPacket(&cmdPacketParameter);
    emit sendData(cmdPacketParameter.pout,cmdPacketParameter.len);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_parseData
 * 函数描述：解析指定数据槽函数
 * 输入参数：pdata-数据指针(只读)，len-解析数据的长度
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_parseData(const uint8_t *pdata, const uint32_t len)
{
    DataComingThread::comRecBuf2Mutex.lock();       //获取资源锁

    switch (pdata[0]) {
        case ACK_HEAD_CODE:
        {
            SYSAck_Packet ackPacket;

            memcpy(&ackPacket,pdata,sizeof(SYSAck_Packet));
            parseAckPacket(&ackPacket);
        }
            break;
        case RELAY_HEAD_CODE:
        {
            uint8_t cs;

            cs = calculatorCheckSum(&pdata[1],len-2);
            if(cs != pdata[len-1])
            {
                QString text = QString("校验和错误(%1H) != (%2H)!")
                        .arg(cs,2,16,QChar('0'))
                        .arg(pdata[len-1],2,16,QChar('0'));
                emit ack_signal(text);
                qDebug() << "len: " << pdata;
                DataComingThread::comRecBuf2Mutex.unlock();     //解锁
                return;
            }

            SYSRelay_Packet sysRelay_packet;

            memcpy(&sysRelay_packet,pdata,3);
            sysRelay_packet.cs = cs;
            sysRelay_packet.pData = &pdata[3];
            parseRelayPacket(&sysRelay_packet);
        }
            break;
        case SENSOR_DATA_HEAD_CODE:
        {
            uint16_t cs_16;

            cs_16 = calculatorCheckSum(&pdata[1],len-3);
            uint16_t packet_cs = pdata[len-1];
            packet_cs<<=8;
            packet_cs |= pdata[len-2];
            if(cs_16 != packet_cs)
            {
                QString text = QString("校验和错误(%1) != (%2)!")
                        .arg(cs_16,4,16,QChar('0'))
                        .arg(packet_cs,4,16,QChar('0'));
                emit ack_signal(text);
                qDebug() << "len: " << len;
                DataComingThread::comRecBuf2Mutex.unlock();     //解锁
                return;
            }

            SYSData_Packet sysData_Packet;

            sysData_Packet.cs = cs_16;
            sysData_Packet.pSensor = &pdata[2];
            sysData_Packet.sensor_num = pdata[1];
            parseSensorDataPacket(&sysData_Packet);
        }
            break;
        case DEV_DATA_HEAD_CODE:
        {
            uint8_t cs;

            cs = calculatorCheckSum(&pdata[1],len-2);
            if(cs != pdata[len-1])
            {
                QString text = QString("校验和错误(%1H) != (%2H)!")
                        .arg(cs,2,16,QChar('0'))
                        .arg(pdata[len-1],2,16,QChar('0'));
                emit ack_signal(text);
                qDebug() << "len: " << len;
                DataComingThread::comRecBuf2Mutex.unlock();     //解锁
                return;
            }

            SYSData2_Packet sysData2_packet;

            memcpy(&sysData2_packet,pdata,5);
            sysData2_packet.pdata = &pdata[5];
            sysData2_packet.cs = cs;
            parseSysDataPacket(&sysData2_packet);
        }
            break;
        default:
            break;
    }

    DataComingThread::comRecBuf2Mutex.unlock();     //解锁
}

void ParseThread::run()
{
    exec();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：parseAckPacket
 * 函数描述：解析应答包
 * 输入参数：p_Ackpacket-应答包结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::parseAckPacket(const SYSAck_Packet *p_Ackpacket)
{
    if((p_Ackpacket->addr&0xFF) != DEV_ID)
    {
        return;
    }
    QString ack_text;
    switch (p_Ackpacket->ack_code) {
    case 0xA5:  //设置成功
        ack_text = QString("设置成功！");
        break;
    case 0x00:  //不认可
        ack_text = QString("不认可设置！");
        break;
    case 0x01:  //非法命令
        ack_text = QString("非法命令！");
        break;
    case 0x02:  //非法参数
        ack_text = QString("非法参数！");
        break;
    case 0x03:  //校验和错误
        ack_text = QString("校验和错误！");
        break;

    default:
        ack_text = QString("未知回应码(%1)").arg(p_Ackpacket->ack_code,2,16,QChar('0'));
        break;
    }
    emit ack_signal(ack_text);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：parseRelayPacket
 * 函数描述：解析转发数据包
 * 输入参数：p_RelayPacket-转发包结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::parseRelayPacket(const SYSRelay_Packet *p_RelayPacket)
{
    switch (p_RelayPacket->cmd) {
    case 0x01:  //蓝牙命令
        parseBT_Packet(p_RelayPacket->pData);
        break;
    case 0x02:  //传感器数据帧命令

        break;
    default:
        break;
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：parseBT_Packet
 * 函数描述：解析蓝牙数据包
 * 输入参数：pdata-解析数据
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::parseBT_Packet(const uint8_t *pdata)
{
    QString bt_string((const char *)pdata);
    QString text_string;
    int type = 0;

    if(bt_string.contains("VERSION=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 1;
    }
    else if(bt_string.contains("MAC=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 2;
    }
    else if(bt_string.contains("MACS=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 3;
    }
    else if(bt_string.contains("NAME=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 4;
    }
    else if(bt_string.contains("NAMES=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 5;
    }
    else if(bt_string.contains("TYPE=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 6;
    }
    else if(bt_string.contains("PIN=",Qt::CaseInsensitive))
    {
        text_string = bt_string.split("=").at(1);
        type = 7;
    }

    if(type)
        emit btRelay_signal(type,text_string);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：parseSensorDataPacket
 * 函数描述：解析传感器数据包
 * 输入参数：p_sysData_Packet-传感器结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::parseSensorDataPacket(const SYSData_Packet *p_sysData_Packet)
{
    const SYSData_PerSensor_Packet *sensor_packet;
    uint32_t offset = 0;
    uint16_t sensor_id;
    int i,j;

    for(i = 0;i < p_sysData_Packet->sensor_num;i++)
    {
        sensor_id = p_sysData_Packet->pSensor[offset+1]-1;
        if(sensor_id > 3)
        {
            sensor_id-=4;
            sensor_id<<=8;
            sensor_id += p_sysData_Packet->pSensor[offset]+24;
        }
        else
        {
            sensor_id<<=8;
            sensor_id += p_sysData_Packet->pSensor[offset]-1;
        }
        sensorConnectedTable[sensor_id] = true;

        sensor_packet = (SYSData_PerSensor_Packet *)(&p_sysData_Packet->pSensor[offset]);
        for(j = 0;j < 8;j++)
        {
            //这是延时？？？
        }
        chartUpdateMutex.lock();
        emit chartData_signal(sensor_packet);
        chartUpdateMutex.unlock();
        offset = p_sysData_Packet->pSensor[offset+2]*2+3;
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：parseSysDataPacket
 * 函数描述：解析系统数据包
 * 输入参数：p_sysDataPacket-系统数据结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::parseSysDataPacket(const SYSData2_Packet *p_sysDataPacket)
{
    if(p_sysDataPacket->l_id != DEV_ID) return;

    switch (p_sysDataPacket->data_id) {
    case 0x01:  //电池电压标识符
    {
        uint16_t batter_vol;
        memcpy(&batter_vol,p_sysDataPacket->pdata,2);
        emit batteryVol_signal(batter_vol);
    }
        break;
    default:
        break;
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：clearConnectedTable
 * 函数描述：清除传感器连接ID表
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::clearConnectedTable()
{
    uint16_t i,j;
    uint16_t id;

    for(i = 0;i < 4;i++)
    {
        for(j = 0;j < 50;j++)
        {
            id = (i<<8)|j;
            sensorConnectedTable[id] = false;
        }
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_checkConnectedTimerOut
 * 函数描述：检查连接状态定时器溢出槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void ParseThread::slot_checkConnectedTimerOut()
{
    emit connectedTable(sensorConnectedTable);  //先发出再清除
    clearConnectedTable();
}

