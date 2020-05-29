#include "datacomingthread.h"

#include <QTimer>
#include <QVariant>
#include <QSerialPortInfo>
#include <QDebug>

//串口2级缓存互斥锁
QMutex DataComingThread::comRecBuf2Mutex;                                 //接收缓存互斥量

DataComingThread::DataComingThread(QObject *parent) :
    QThread(parent)
{
    comOpen = false;
    l_comRecCount = 0;

    serial = new QSerialPort(parent);
    connect(serial,SIGNAL(readyRead()),this,SLOT(slot_comReadyRead()),Qt::DirectConnection);
    qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
    connect(serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(slot_comError(QSerialPort::SerialPortError)),Qt::DirectConnection);

    comRecByteTimer = new QTimer(parent);
    connect(comRecByteTimer,SIGNAL(timeout()),this,SLOT(slot_comRecByteTimeout()),Qt::DirectConnection);
    comRecByteTimer->setInterval(3);
}

DataComingThread::~DataComingThread()
{

}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_comReadyRead
 * 函数描述：读取数据槽函数，对于串口1个字节会执行一次，USB虚拟串口根据设备缓存执行一次
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void DataComingThread::slot_comReadyRead()
{
    if(!l_comRecCount)
    {
        comRecLittleCount = serial->read((char *)&l_comRecBuf[l_comRecCount],2048-l_comRecCount); //数据追加写入buf1
        if((l_comRecBuf[0][0]==0xA7)||(l_comRecBuf[0][0]==0xA6)||(l_comRecBuf[0][0]==0x5A)) //如果不是这些开头就说明数据不可用
        {
            l_comRecCount += comRecLittleCount;     //接收数量增加
            comRecByteTimer->start();               //启动定时器
        }
    }
    else
    {
        comRecLittleCount = serial->read((char *)&l_comRecBuf[l_comRecCount],2048-l_comRecCount);
        l_comRecCount += comRecLittleCount;     //接收数量增加
        comRecByteTimer->start();               //启动定时器
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_comRecByteTimeout
 * 函数描述：接收串口字节间隔溢出槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void DataComingThread::slot_comRecByteTimeout()
{
    comRecByteTimer->stop();        //先停止

    /*
     * 加一个buf1部分区域的指针锁（未）
     * */
    comRecBuf2Mutex.lock();          //获取互斥锁
    memcpy(l2_comRecBuf,l_comRecBuf,l_comRecCount);
    l2_comRecCount = l_comRecCount;
    l_comRecCount = 0;
    emit recOnePacket(l2_comRecBuf,l2_comRecCount);
    serial->clear(QSerialPort::Input);
    comRecBuf2Mutex.unlock();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_comError
 * 函数描述：串口错误处理槽函数
 * 输入参数：err-错误参数
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void DataComingThread::slot_comError(QSerialPort::SerialPortError err)
{
    int err_no = err;

    if(err_no)
    {
        comOpen = false;
        emit comClosed(true);
        serial->close();
    }
}

/******************************************************************
 * 作   者：PQP
 * 函数名称：getComStat
 * 函数描述：获取串口的状态
 * 输入参数：
 * **返回值：true-串口打开，false-串口已关闭
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
bool DataComingThread::getComStat()
{
    return comOpen;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_comOnOffHandle
 * 函数描述：串口开关处理槽函数
 * 输入参数：stat-开关状态，config_table-打开串口需要的配置表(除了第0个是QString类型外都是int型)
 *         依次为：串口名称、波特率、数据位、奇偶效验位、停止位、硬件控制位
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void DataComingThread::slot_comOnOffHandle(const bool stat,const QList<QVariant> &config_table)
{
    if(stat)
    {
        bool ret;
        serial->setPortName(config_table.at(0).toString());
        ret = serial->open(QIODevice::ReadWrite);
        if(ret)
        {
            serial->setBaudRate(config_table.at(1).toInt());
            serial->setDataBits(static_cast<QSerialPort::DataBits>(config_table.at(2).toInt()));
            serial->setParity(static_cast<QSerialPort::Parity>(config_table.at(3).toInt()));
            serial->setStopBits(static_cast<QSerialPort::StopBits>(config_table.at(4).toInt()));
            serial->setFlowControl(static_cast<QSerialPort::FlowControl>(config_table.at(5).toInt()));
            comOpen = true;
            serial->clear();    //清除接收到的所有缓存
        }
        else
        {
            comOpen = false;
        }
    }
    else
    {
        comOpen = false;
        serial->close();    //先关闭
    }
    emit comClosed(!comOpen);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_sendData
 * 函数描述：使用串口发送数据
 * 输入参数：pdata-发送数据指针，len-发送数据长度
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void DataComingThread::slot_sendData(const uint8_t *pdata, const uint32_t len)
{
    serial->write((const char *)pdata,len);
}

void DataComingThread::run()
{
    exec();
}
