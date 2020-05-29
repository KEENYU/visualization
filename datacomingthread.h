#ifndef DATACOMINGTHREAD_H
#define DATACOMINGTHREAD_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QMutex>
#include <QSemaphore>
#define buf1_size 1024
#define buf1_num 10
class QTimer;

class DataComingThread : public QThread
{
    Q_OBJECT

private:

    QTimer *comRecByteTimer;                            //虚拟串口接收每字节间隔定时器
    uint16_t comRecLittleCount;                         //USB每一次接收长度
    QSerialPort *serial;                                //串口指针
    uint8_t l_comRecBuf[buf1_num][buf1_size];                      //USB虚拟串口接受缓存
    uint16_t l_comRecCount;                             //缓存接收计数器
    uint8_t l2_comRecBuf[2048];                         //USB虚拟串口接受缓存
    uint16_t l2_comRecCount;                            //缓存接收计数器
    bool comOpen;                                       //串口打开标示位

private slots:
    void slot_comReadyRead();
    void slot_comRecByteTimeout();
    void slot_comError(QSerialPort::SerialPortError err);

public:
    DataComingThread(QObject *parent = Q_NULLPTR);
    ~DataComingThread();

    static QMutex comRecBuf2Mutex;                       //接收缓存互斥量

    bool getComStat();
public slots:

    void slot_comOnOffHandle(const bool stat,const QList<QVariant> &config_table);
    void slot_sendData(const uint8_t *pdata,const uint32_t len);
protected:
    void run();

signals:
    void recOnePacket(const uint8_t *pdata,const uint32_t len);
    void comClosed(const bool stat);
};

#endif // DATACOMINGTHREAD_H
