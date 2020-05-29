#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "parsethread.h"
#include "threedimension.h"
#include "heatmapper.h"
#include "gradientpalette.h"
#include <QMainWindow>
#include <QMutex>
#include <QThreadPool>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QSerialPort>
#include <QWidget>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QStatusBar>
#include <QLabel>
#include <QIntValidator>
#include <QTimer>
#include <QMessageBox>
#include <QtDebug>
#include <QSound>
#include <QRadioButton>
#include <QSerialPortInfo>
#include <windows.h>
#include <QPen>
#include <QMap>
#include <QtCore/QRandomGenerator>
#include <QtCore/QtMath>
#include <QCursor>
#include <QKeyEvent>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QFormLayout>
#include "comconfigdialog.h"
#include "xmodemprotocol.h"
#include "btprotocol.h"
#include "sysprotocol.h"
#include "datacomingthread.h"
#include "parsethread.h"
#include "twodimension.h"


QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class HeatMapper;
class GradientPalette;
class QStatusBar;
class QLabel;
class QSound;
class DataComingThread;
class ParseThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

#define DEBUG_ON        0

#define CSS_COLOR_RED   "background-color: rgb(255, 0, 0);"
#define CSS_COLOR_GREEN   "background-color: rgb(0, 255, 0);"
#define CSS_COLOR_NORMAL   "background-color: rgb(246, 246, 246);"

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void slot_updateBatteryVol(const int val);
    void slot_updateSensorNum(const int num);
    void slot_updateAckText(const QString &text);
    void slot_updateChartData(const SYSData_PerSensor_Packet *sensorPacket);
    void slot_updateChart1Data(const SYSData_PerSensor_Packet *p_perSensorPacket);
    void slot_updateComStat(const bool new_stat);
    void slot_connectedTable(const QMap<uint16_t,bool> &table);

private slots:
    void slot_updateComconfigTable(QList<QVariant> &config);
    void slot_chart1SampleTimerOut();
    void slot_valShow(QPointF point,bool stat);
    void on_pushButton_chx_com_1_clicked();
    void on_comboBox_chx_com_1_currentIndexChanged(int index);
    void on_pushButton_sampleSet_1_clicked();
    void on_checkBox_com1_ch_1_stateChanged(int arg1);
    void on_checkBox_com1_ch_2_stateChanged(int arg1);
    void on_checkBox_com1_ch_3_stateChanged(int arg1);
    void on_checkBox_com1_ch_4_stateChanged(int arg1);
    void on_checkBox_com1_ch_5_stateChanged(int arg1);
    void on_checkBox_com1_ch_6_stateChanged(int arg1);
    void on_checkBox_com1_ch_7_stateChanged(int arg1);
    void on_checkBox_com1_ch_8_stateChanged(int arg1);
    void on_checkBox__chx_com_1_stateChanged(int arg1);
    void on_pushButton_chart_1_clicked();
    void on_pushButton_Clear_1_clicked();
    void on_action_COM_config_triggered();
    void on_action_ConnectMe_triggered();
    void on_checkBox_chart1_sample_clicked();
    void on_action_open_com_triggered();
    void on_action_us_triggered();
    void on_action_QT_triggered();
    void on_pushButton1_max_clicked();
    void on_menu_New_triggered();
    void removeAllDock();  //移除并隐藏所有dock
    void showDock(const QList<int>& index = QList<int>());  //显示dock窗口

    void on_action_Play_triggered();

    void on_action_Save_triggered();

    void on_menu_Open_triggered();

    void on_menu_Save_triggered();

    void on_mune_2D_triggered();

    void on_menu_3D_triggered();

    void on_menu_Graph_triggered();

    void on_menu_ColorBar_triggered();

    void on_menu_Com_Config_triggered();

    void on_menu_Sensor_Config_triggered();

    void on_menu_PressureCalibration_triggered();

private:
    QList<QDockWidget*> m_docks;                        //记录所有dockWidget的指针
    QList<QSplineSeries *> chart1_splineSeriesList;     //曲线序列1
    QList<QColor> chart1_colorList;                     //颜色序列1
    QChart *chart1;                                     //字符图列1
    QChartView *chartView1;                             //图例视图1
    QValueAxis *chart1_xAxis;                           //图例X坐标1
    QStatusBar *statusBar;                              //状态指针
    QLabel *labelBattery;                               //电池状态标签
    QTimer *chartSample_timer;                  //图例使用的采样定时器
    bool chart1_openGL;                                 //渲染标示位
    QLabel *devNumLabel;                                //设备数量的标签
    uint64_t chart1_xCount;                             //图例1X坐标轴计数器
    QPixmap activePixmap;                               //激活时的图像
    QPixmap noactivePixmap;                             //未激活的图像
    int oneSecondTimerID;                               //1秒定时ID号
    QList<QVariant> comConfigTable;                     //串口配置表
    QList<QVector<QPointF> > chart_data1;               //点阵序列表
    uint32_t chart1DataCnt;                             //数据计数器1
    bool chart1_max;                                    //最大化标示位
    QLabel *val_label;                                  //显示数值标签
    bool chart1_stop;                                   //图例1停止标示位

    QThread *dataComingThread;
    DataComingThread *dataComingObject;                 //数据接收处理线程
    QThread *parseThread;
    ParseThread *parseObject;                           //解析处理线程
    QThread *threeModifierThread;
    ThreeDimension *threeModifierObject;                 //3D图控制线程
    TwoDimension *twoDimensionView;

    QWidget *dockThreeView;
    QWidget *dockThreeConfigView;
    QWidget *sensorConfigView;

    void channelColorInit();
    void chartViewInit();
    void soundInit();
    void variablesInit();
    void uiViewInit();
    void statusBarInit();
    void dockInit();
    void twoDimentionInit();
    void threeDimentionInit();
    void threadInit();
    void sensorConfigInit();



    int getChart1Display(uint8_t *channel);

    QStringList updateComName();
    void generateSampleData(int type, int rowCount, int colCount);
    void initSampleData(int data, int rowCount, int colCount);
protected:
    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void comOnOff(const bool stat,const QList<QVariant> &table);
    void sendBTRelayData(const uint8_t *pdata,const uint32_t len);
    void sendSensorRelayData(const uint8_t *pdata,const uint32_t len);
};

#endif // MAINWINDOW_H
