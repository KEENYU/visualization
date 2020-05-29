#ifndef PARSETHREAD_H
#define PARSETHREAD_H

#include "sysprotocol.h"
#include "xmodemprotocol.h"

#include <QSemaphore>
#include <QMutex>
#include <QThread>
#include <QMap>
#include <QRunnable>

class QTimer;

class ParseThread : public QThread
{
    Q_OBJECT

public:
    ParseThread(QObject *parent = Q_NULLPTR);

    static QMutex chartUpdateMutex;

    QMap<uint16_t,bool> &getConnectedTable();
public slots:
    void slot_sendRelayPacket(const uint8_t cmd,const uint32_t len,const uint8_t *pdata);
    void slot_sendBTRelayPacket(const uint8_t *pdata,const uint32_t len);
    void slot_sendSensorRelayPacket(const uint8_t *pdata,const uint32_t len);
    void slot_sendCmdPacket(const uint8_t cmd,const uint32_t parameter);
    void slot_parseData(const uint8_t *pdata,const uint32_t len);

protected:
    void run();

signals:
    void ack_signal(const QString &text);
    void btRelay_signal(const int type,const QString &text);
    void chartData_signal(const SYSData_PerSensor_Packet*);
    void batteryVol_signal(const int val);
    void connectedTable(const QMap<uint16_t,bool> &table);
    void sendData(const uint8_t *pdata,const uint32_t len);

private:
    RelayPacketParameter relayPacketParameter;           //转发包
    CmdPacketParameter cmdPacketParameter;               //命令包
    uint8_t sendBuf[100];                                //发送数据缓存
    QMap<uint16_t,bool> sensorConnectedTable;            //传感器已连接状态表
    QTimer *checkConnectedTimer;                         //检查连接定时器


    void parseAckPacket(const SYSAck_Packet *p_Ackpacket);
    void parseRelayPacket(const SYSRelay_Packet *p_RelayPacket);
    void parseBT_Packet(const uint8_t *pdata);
    void parseSensorDataPacket(const SYSData_Packet *p_sysData_Packet);
    void parseSysDataPacket(const SYSData2_Packet *p_sysDataPacket);

    void clearConnectedTable();
private slots:
    void slot_checkConnectedTimerOut();
};

#endif // PARSETHREAD_H
