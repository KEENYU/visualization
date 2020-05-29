#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("压力曲线界面"));
    variablesInit();
    chartViewInit();
    uiViewInit();
    statusBarInit();

    sensorConfigInit();
    twoDimentionInit();   //2D窗口初始化
    threeDimentionInit();      //3D窗口以及控制面板初始化

    dockInit();  //各模块的浮动窗口初始化
    threadInit();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateBatteryVol
 * 函数描述：更新电池电压槽函数
 * 输入参数：val-adc转换值
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateBatteryVol(int val)
{
    double vol = val * 0.001611328;     // 6.6/4096
    labelBattery->setText(tr("电池电压：%1V").arg(vol,3,'g',2,QChar('0')));
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateChartData
 * 函数描述：更新图例槽函数
 * 输入参数：sensorPacket-传感器数据结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateChartData(const SYSData_PerSensor_Packet *sensorPacket)
{
    uint8_t h_id = sensorPacket->id>>8;
    uint8_t l_id = sensorPacket->id;

    if(h_id > 4)
    {
        h_id-=4;
        l_id+=25;
    }
    if(ui->checkBox__chx_com_1->isChecked()&&(h_id == 1))
    {
        if((ui->comboBox_ch_Sensor_1->currentIndex()+1) == l_id)
        {
            slot_updateChart1Data(sensorPacket);
        }
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateSensorNum
 * 函数描述：更新已连接的传感器数量槽函数
 * 输入参数：num-连接的数量
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateSensorNum(int num)
{
    devNumLabel->setText(tr("传感器：%1个").arg(num,3,10,QChar('0')));
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateAckText
 * 函数描述：更新应答内容槽函数
 * 输入参数：text-内容
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateAckText(const QString &text)
{
    statusBar->showMessage(text,3000);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateChart1Data
 * 函数描述：更新图例1槽函数
 * 输入参数：p_perSensorPacket-传感器数据结构指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateChart1Data(const SYSData_PerSensor_Packet *p_perSensorPacket)
{
    if(chart1_stop) return;
    QPointF point;
    int16_t val;

    point.setX(chart1_xCount);
    for(int i = 0;i < 8;i++)
    {
        val = p_perSensorPacket->data[i]<<8;
        val |= p_perSensorPacket->data[i]>>8;
        point.setY(val*0.2441406);
        chart1_splineSeriesList[i]->append(point);
        //chart1_scatterSeries[i]->append(point);
    }

    chart1_xCount++;
    if(chart1_xCount >= 100)
    {
        chart1_xAxis->setRange(chart1_xAxis->min()+1,chart1_xAxis->max()+1);
        for(int i = 0;i < 8;i++)
        {
            chart1_splineSeriesList[i]->remove(0);      //删除第一个点
            //chart1_scatterSeries[i]->remove(0);
        }

        if(chart1_xCount >= 10000)
        {
            on_pushButton_Clear_1_clicked();
        }
    }
    //    for(int i = 0;i < chart_data1.count();i++)
    //    {
    //        chart_data1[i][chart1DataCnt] = QPointF(chart1DataCnt,p_perSensorPacket->data[i]);
    //    }
    //    chart1DataCnt++;
    //    if(chart1DataCnt >= 100)
    //    {
    //        chart1DataCnt = 0;
    //    }

}

/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateComStat
 * 函数描述：串口状态更新槽函数
 * 输入参数：new_stat-新的状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateComStat(bool com_close)
{
    if(com_close == true)
    {
        ui->action_open_com->setText(tr("打开串口"));
        ui->action_open_com->setIcon(QIcon(noactivePixmap));
    }
    else
    {
        ui->action_open_com->setText(tr("关闭串口"));
        ui->action_open_com->setIcon(QIcon(activePixmap));
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：channelColorInit
 * 函数描述：显示的通道颜色初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::channelColorInit()
{
    QPalette palette =ui->line_chx_com_1->palette();
    palette.setColor(QPalette::WindowText,chart1_colorList.at(ui->comboBox_chx_com_1->currentIndex()));
    ui->line_chx_com_1->setPalette(palette);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：chartViewInit
 * 函数描述：图列和视图初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::chartViewInit()
{
    int i;

    for(i = 0;i < 8;i++)
    {
        QSplineSeries *splineSeries = new QSplineSeries;
        connect(splineSeries,SIGNAL(hovered(QPointF,bool)),this,SLOT(slot_valShow(QPointF,bool)));
        splineSeries->setName(tr("通道%1").arg(i+1));
        splineSeries->setColor(chart1_colorList.at(i));  //给每条曲线添色
        chart1_splineSeriesList.append(splineSeries);
        //        QScatterSeries *scatterSeries = new QScatterSeries;
        //        scatterSeries->setMarkerSize(6);
        //        chart1_scatterSeries.append(scatterSeries);
    }

    chart1 = new QChart();
    chart1->legend()->setAlignment(Qt::AlignRight);
    //chart1->legend()->hide();
    //chart1->setTitle(tr("通道1压力曲线图"));
    chart1_xAxis = new QValueAxis;
    //chart1_xAxis->setFormat("ss.zzz");
    chart1_xAxis->setLabelsColor(Qt::black);
    //chart1_xAxis->setRange(QDateTime::currentDateTime(),QDateTime::currentDateTime().addSecs(10));
    chart1_xAxis->setRange(0,100);
    chart1_xAxis->setTickCount(100/5+1);
    chart1_xAxis->setLabelFormat("%.2g");
    chart1_xAxis->setTitleText(tr("10-2ms"));
    QValueAxis *chart1_yAxis = new QValueAxis;
    chart1_yAxis->setTitleText(tr("10-3pf"));
    chart1_yAxis->setLabelFormat("%.2g");
    chart1_yAxis->setLabelsColor(Qt::blue);
    chart1_yAxis->setRange(-7450,-6950);    //35200,36800
    chart1_yAxis->setTickCount(500/100+1);

    //    chart1->createDefaultAxes();
    //    chart1->axisX()->setRange(0,100);
    //    chart1->axisY()->setRange(0,512);
    for(i = 0;i < chart1_splineSeriesList.size();i++)
    {
        chart1->addSeries(chart1_splineSeriesList.at(i));
        chart1->setAxisX(chart1_xAxis,chart1_splineSeriesList.at(i));
        chart1->setAxisY(chart1_yAxis,chart1_splineSeriesList.at(i));

        //        chart1->addSeries(chart1_scatterSeries.at(i));
        //        chart1->setAxisX(chart1_xAxis,chart1_scatterSeries.at(i));
        //        chart1->setAxisY(chart1_yAxis,chart1_scatterSeries.at(i));
    }
    chartView1  = new QChartView(chart1);
    chartView1->setRubberBand(QChartView::RectangleRubberBand);  //橡皮筋设置
    chartView1->setRenderHint(QPainter::Antialiasing);
    chartView1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    chartView1->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    chartView1->installEventFilter(this);

    //chartSample_timer.append(new QTimer(this));
    chartSample_timer = new QTimer(this);
    connect(chartSample_timer,SIGNAL(timeout()),this,SLOT(slot_chart1SampleTimerOut()));

    //generateSampleData(1, 8, 100);
    initSampleData(-7200, 8, 50);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：variablesInit
 * 函数描述：变量初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::variablesInit()
{
    chart1_openGL = false;  //GPU渲染标示位1
    chart1_xCount = 0;
    chart1DataCnt = 0;
    chart1_max = false;
    chart1_stop = false;

    QIcon icon(":/green.png");
    activePixmap = icon.pixmap(24,24,QIcon::Active,QIcon::On);
    noactivePixmap = icon.pixmap(24,24,QIcon::Disabled,QIcon::Off);

    QStringList color_list;
    color_list << "red" << "green" << "blue" << "yellow" << "black" << "magenta" << "cyan" << "darkCyan";
    for(int i = 0;i < color_list.size();i++)
    {
        chart1_colorList.append(QColor(color_list.at(i)));
    }

    oneSecondTimerID = startTimer(1000);
    comConfigTable.append(updateComName().last());
    comConfigTable.append("115200");
    comConfigTable.append("8");
    comConfigTable.append("0");
    comConfigTable.append("0");
    comConfigTable.append("0");

    val_label = new QLabel(this);
    val_label->hide();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：uiViewInit
 * 函数描述：UI界面视图初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::uiViewInit()
{
    ui->comboBox_ch_Sensor_1->clear();
    for(int i = 0;i < 50;i++)       //添加了50个
    {
        ui->comboBox_ch_Sensor_1->addItem(noactivePixmap,QString("%1号传感器").arg(i+1));
    }
    channelColorInit();

    ui->action_open_com->setIcon(QIcon(noactivePixmap));

    ui->lineEdit_chx_com_1->setText("5");
    QIntValidator *sampleIntValidator = new QIntValidator(1,20); //范围验证
    ui->lineEdit_chx_com_1->setValidator(sampleIntValidator);

    int sample_val = ui->lineEdit_chx_com_1->text().toInt();
    chartSample_timer->setInterval(1000/sample_val);

    on_pushButton_chart_1_clicked();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：statusBarInit
 * 函数描述：状态栏初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::statusBarInit()
{
    labelBattery = new QLabel;
    slot_updateBatteryVol(0);

    devNumLabel = new QLabel;
    slot_updateSensorNum(0);

    statusBar = ui->statusBar;
    statusBar->setFixedHeight(16);
    statusBar->addPermanentWidget(labelBattery);
    statusBar->addPermanentWidget(devNumLabel);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：dockInit
 * 函数描述：浮动窗口初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::dockInit(){
    //MainWindow初始最大化
    //setWindowState(Qt::WindowMaximized);

    //removeAllDock();

    //去除MainWindow的中间窗口
    QWidget* p = takeCentralWidget();
    if(p)
        delete p;
    //允许嵌套dock
    setDockNestingEnabled(true);

    //配置串口窗口
    addDockWidget(Qt::TopDockWidgetArea,ui->dockUsrtConfig);

    //配置传感器窗口
    QDockWidget *dockSensor = new QDockWidget("配置传感器",this);
    addDockWidget(Qt::TopDockWidgetArea,dockSensor);
    dockSensor->setWidget(sensorConfigView);

    //压力校验窗口
    QDockWidget *dockCheck = new QDockWidget("压力校验",this);
    addDockWidget(Qt::BottomDockWidgetArea,dockCheck);

    //2D窗口
    QDockWidget *dockTwoDimension = new QDockWidget("2D图",this);
    addDockWidget(Qt::TopDockWidgetArea,dockTwoDimension);
    dockTwoDimension->setWidget(twoDimensionView);

    //ColorBar窗口
    QDockWidget *dockColorBar = new QDockWidget("ColorBar",this);
    addDockWidget(Qt::TopDockWidgetArea,dockColorBar);

    //3D控制窗口
    QDockWidget *dockThreeDimensionConfig = new QDockWidget("3D图控制面板",this);
    addDockWidget(Qt::TopDockWidgetArea,dockThreeDimensionConfig);
    dockThreeDimensionConfig->setWidget(dockThreeConfigView);

    //通道曲线图窗口
    QDockWidget *dockCurve = new QDockWidget("通道曲线图",this);
    addDockWidget(Qt::BottomDockWidgetArea,dockCurve);
    dockCurve->setWidget(chartView1);

    //初始化窗口大小
    //ui->dockChannel->setFixedWidth(328);
    //ui->dockUsrtConfig->setFixedWidth(328);
    //dockSensor->setFixedWidth(328);
    //dockColorBar->setFixedWidth(180);
    //dockThreeDimensionConfig->setFixedWidth(180);
    ui->dockChannel->setMaximumWidth(328);
    ui->dockChannel->setMinimumWidth(327);
    ui->dockUsrtConfig->setMaximumWidth(328);
    ui->dockUsrtConfig->setMinimumWidth(327);
    dockThreeDimensionConfig->setMaximumWidth(180);


    //记录所有的dock指针
    m_docks.append(ui->dockChannel);
    m_docks.append(ui->dockUsrtConfig);
    m_docks.append(dockSensor);
    m_docks.append(dockCheck);
    m_docks.append(dockCurve);
    m_docks.append(dockTwoDimension);
    m_docks.append(dockColorBar);

    splitDockWidget(ui->dockUsrtConfig,dockTwoDimension,Qt::Horizontal);
    splitDockWidget(ui->dockUsrtConfig,ui->dockChannel,Qt::Vertical);
    splitDockWidget(dockTwoDimension,dockCurve,Qt::Vertical);

    //3D窗口
    QDockWidget *dockThreeDimension = new QDockWidget("3D图",this);
    addDockWidget(Qt::TopDockWidgetArea,dockThreeDimension);
    dockThreeDimension->setWidget(dockThreeView);
    m_docks.append(dockThreeDimension);
    m_docks.append(dockThreeDimensionConfig);
    splitDockWidget(dockTwoDimension,dockThreeDimension,Qt::Horizontal);
    dockThreeDimension->setMinimumHeight(150);

    tabifyDockWidget(ui->dockUsrtConfig,dockSensor);
    ui->dockUsrtConfig->raise();
    tabifyDockWidget(ui->dockChannel,dockCheck);
    ui->dockChannel->raise();
    tabifyDockWidget(dockThreeDimensionConfig,dockColorBar);
    dockThreeDimensionConfig->raise();
    //showDock();
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：sensorConfigInit
 * 函数描述：传感器配置初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/27
 * ***************************************************************/
void MainWindow::sensorConfigInit()
{
    sensorConfigView = new QWidget();
    QLabel *sensorTypeLabel = new QLabel("传感器型号：");
    QLabel *sensorSAreaLabel = new QLabel("传感器面积：");

    QComboBox *sensorType = new QComboBox();
    sensorType->addItem(QStringLiteral("1通道"));
    sensorType->addItem(QStringLiteral("25通道"));

    QComboBox *sensorSArea = new QComboBox();
    sensorSArea->addItem(QStringLiteral("面积1"));
    sensorSArea->addItem(QStringLiteral("面积2"));

    QPushButton  *sensoeComfigOk = new QPushButton("确定");
    QPushButton  *sensoeComfigNo = new QPushButton("取消");

    QFormLayout *FormLayout = new QFormLayout(sensorConfigView);
    FormLayout->addRow(sensorTypeLabel,sensorType);
    FormLayout->addRow(sensorSAreaLabel,sensorSArea);
    FormLayout->addRow(sensoeComfigOk,sensoeComfigNo);
    FormLayout->setSpacing(20);

    //layout->setRowWrapPolicy(QFormLayout::WrapLongRows);//该参数通常适用于小屏幕中，当标签和文本框在本行显示不全时，文本框会显示在下一行，使得标签独占一行
    FormLayout->setLabelAlignment(Qt::AlignLeft);//设置标签的对齐方式
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：twoDimentionInit
 * 函数描述：二维窗口初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::twoDimentionInit()
{
    twoDimensionView = new TwoDimension();
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：threeDimentionInit
 * 函数描述：三维窗口初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::threeDimentionInit()
{
    Q3DSurface *graph = new Q3DSurface();
    dockThreeView = QWidget::createWindowContainer(graph);
    //! [0]

    if (!graph->hasContext()) { //已被初始化
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }

    //QSize screenSize = graph->size();
    //dockThreeView->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    //dockThreeView->setMaximumSize(screenSize);
    dockThreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dockThreeView->setFocusPolicy(Qt::StrongFocus);

    //! [1]
    dockThreeConfigView = new QWidget();
    QHBoxLayout *hLayout = new QHBoxLayout(dockThreeConfigView);
    QVBoxLayout *vLayout = new QVBoxLayout();
    //    hLayout->addWidget(dockThreeView, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);
    //! [1]

    dockThreeConfigView->setWindowTitle(QStringLiteral("Surface example"));

    QGroupBox *modelGroupBox = new QGroupBox(QStringLiteral("Model"));

    QRadioButton *sqrtSinModelRB = new QRadioButton(dockThreeConfigView);
    sqrtSinModelRB->setText(QStringLiteral("Sqrt && Sin"));
    sqrtSinModelRB->setChecked(false);

    QRadioButton *heightMapModelRB = new QRadioButton(dockThreeConfigView);
    heightMapModelRB->setText(QStringLiteral("Height Map"));
    heightMapModelRB->setChecked(false);

    QVBoxLayout *modelVBox = new QVBoxLayout;
    modelVBox->addWidget(sqrtSinModelRB);
    modelVBox->addWidget(heightMapModelRB);
    modelGroupBox->setLayout(modelVBox);

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));

    QRadioButton *modeNoneRB = new QRadioButton(dockThreeConfigView);
    modeNoneRB->setText(QStringLiteral("No selection"));
    modeNoneRB->setChecked(false);

    QRadioButton *modeItemRB = new QRadioButton(dockThreeConfigView);
    modeItemRB->setText(QStringLiteral("Item"));
    modeItemRB->setChecked(false);

    QRadioButton *modeSliceRowRB = new QRadioButton(dockThreeConfigView);
    modeSliceRowRB->setText(QStringLiteral("Row Slice"));
    modeSliceRowRB->setChecked(false);

    QRadioButton *modeSliceColumnRB = new QRadioButton(dockThreeConfigView);
    modeSliceColumnRB->setText(QStringLiteral("Column Slice"));
    modeSliceColumnRB->setChecked(false);

    QVBoxLayout *selectionVBox = new QVBoxLayout;
    selectionVBox->addWidget(modeNoneRB);
    selectionVBox->addWidget(modeItemRB);
    selectionVBox->addWidget(modeSliceRowRB);
    selectionVBox->addWidget(modeSliceColumnRB);
    selectionGroupBox->setLayout(selectionVBox);

    QSlider *axisMinSliderX = new QSlider(Qt::Horizontal, dockThreeConfigView);
    axisMinSliderX->setMinimum(0);
    axisMinSliderX->setTickInterval(1);
    axisMinSliderX->setEnabled(true);
    QSlider *axisMaxSliderX = new QSlider(Qt::Horizontal, dockThreeConfigView);
    axisMaxSliderX->setMinimum(1);
    axisMaxSliderX->setTickInterval(1);
    axisMaxSliderX->setEnabled(true);
    QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal, dockThreeConfigView);
    axisMinSliderZ->setMinimum(0);
    axisMinSliderZ->setTickInterval(1);
    axisMinSliderZ->setEnabled(true);
    QSlider *axisMaxSliderZ = new QSlider(Qt::Horizontal, dockThreeConfigView);
    axisMaxSliderZ->setMinimum(1);
    axisMaxSliderZ->setTickInterval(1);
    axisMaxSliderZ->setEnabled(true);

    QComboBox *themeList = new QComboBox(dockThreeConfigView);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));

    QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(1.0, Qt::black);
    grBtoY.setColorAt(0.67, Qt::blue);
    grBtoY.setColorAt(0.33, Qt::red);
    grBtoY.setColorAt(0.0, Qt::yellow);
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(dockThreeConfigView);
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.0, Qt::darkGreen);
    grGtoR.setColorAt(0.5, Qt::yellow);
    grGtoR.setColorAt(0.2, Qt::red);
    grGtoR.setColorAt(0.0, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientGtoRPB = new QPushButton(dockThreeConfigView);
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));

    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorGroupBox->setLayout(colorHBox);

    vLayout->addWidget(modelGroupBox);
    vLayout->addWidget(selectionGroupBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Column range")));
    vLayout->addWidget(axisMinSliderX);
    vLayout->addWidget(axisMaxSliderX);
    vLayout->addWidget(new QLabel(QStringLiteral("Row range")));
    vLayout->addWidget(axisMinSliderZ);
    vLayout->addWidget(axisMaxSliderZ);
    vLayout->addWidget(new QLabel(QStringLiteral("Theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(colorGroupBox);

    threeModifierObject = new ThreeDimension(graph);
    threeModifierThread = new QThread();
    threeModifierObject->moveToThread(threeModifierThread);

    qRegisterMetaType<QList<QColor>>("QList<QColor>");
    qRegisterMetaType<Q3DTheme::ColorStyle>("Q3DTheme::ColorStyle");
    qRegisterMetaType<QList<QLinearGradient>>("QList<QLinearGradient>");
    qRegisterMetaType<QLinearGradient>("QLinearGradient");
    qRegisterMetaType<Q3DTheme::Theme>("Q3DTheme::Theme");
    qRegisterMetaType<QAbstract3DGraph::SelectionFlags>("QAbstract3DGraph::SelectionFlags");

    connect(threeModifierThread,&QThread::finished,threeModifierThread,&QObject::deleteLater);
    connect(threeModifierThread,&QThread::finished,threeModifierObject,&QObject::deleteLater);

    //QObject::connect(heightMapModelRB, &QRadioButton::toggled,threeModifierObject, &ThreeDimension::enableHeightMapModel);
    connect(sqrtSinModelRB, &QRadioButton::toggled,threeModifierObject, &ThreeDimension::enableSqrtSinModel);
    connect(modeNoneRB, &QRadioButton::toggled,threeModifierObject, &ThreeDimension::toggleModeNone);
    connect(modeItemRB,  &QRadioButton::toggled,threeModifierObject, &ThreeDimension::toggleModeItem);
    connect(modeSliceRowRB,  &QRadioButton::toggled,threeModifierObject, &ThreeDimension::toggleModeSliceRow);
    connect(modeSliceColumnRB,  &QRadioButton::toggled,threeModifierObject, &ThreeDimension::toggleModeSliceColumn);
    connect(axisMinSliderX, &QSlider::valueChanged,threeModifierObject, &ThreeDimension::adjustXMin);
    connect(axisMaxSliderX, &QSlider::valueChanged,threeModifierObject, &ThreeDimension::adjustXMax);
    connect(axisMinSliderZ, &QSlider::valueChanged,threeModifierObject, &ThreeDimension::adjustZMin);
    connect(axisMaxSliderZ, &QSlider::valueChanged,threeModifierObject, &ThreeDimension::adjustZMax);
    connect(themeList, SIGNAL(currentIndexChanged(int)),threeModifierObject, SLOT(changeTheme(int)));
    connect(gradientBtoYPB, &QPushButton::pressed,threeModifierObject, &ThreeDimension::setBlackToYellowGradient);
    connect(gradientGtoRPB, &QPushButton::pressed,threeModifierObject, &ThreeDimension::setGreenToRedGradient);

    threeModifierObject->setAxisMinSliderX(axisMinSliderX);
    threeModifierObject->setAxisMaxSliderX(axisMaxSliderX);
    threeModifierObject->setAxisMinSliderZ(axisMinSliderZ);
    threeModifierObject->setAxisMaxSliderZ(axisMaxSliderZ);

    sqrtSinModelRB->setChecked(true);
    modeItemRB->setChecked(true);
    themeList->setCurrentIndex(2);

    threeModifierThread->start();
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：removeAllDock
 * 函数描述：移除并隐藏所有的dock
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::removeAllDock()
{
    for(int i=0;i<m_docks.length();++i)
    {
        removeDockWidget(m_docks[i]);
    }
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：showDock
 * 函数描述：显示指定序号的dock
 * 输入参数：index 指定序号，如果不指定，则会显示所有
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::showDock(const QList<int> &index)
{
    if (index.isEmpty())
    {
        for(int i=0;i<m_docks.length();++i)
        {
            m_docks[i]->show();
        }
    }
    else
    {
        foreach (int i, index) {
            m_docks[i]->show();
            m_docks[i]->raise();
        }
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：threadInit
 * 函数描述：子线程初始化
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::threadInit()
{
    dataComingObject = new DataComingThread();
    parseObject = new ParseThread();
    dataComingThread = new QThread();
    parseThread = new QThread();

    dataComingObject->moveToThread(dataComingThread);
    parseObject->moveToThread(parseThread);

    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<QMap<uint16_t,bool> >("QMap<uint16_t,bool>");

    connect(dataComingThread,&QThread::finished,dataComingThread,&QObject::deleteLater);
    connect(dataComingThread,&QThread::finished,dataComingObject,&QObject::deleteLater);
    connect(parseThread,&QThread::finished,parseThread,&QObject::deleteLater);
    connect(parseThread,&QThread::finished,parseObject,&QObject::deleteLater);

    connect(this,SIGNAL(comOnOff(bool,const QList<QVariant> &)),dataComingObject,SLOT(slot_comOnOffHandle(bool,const QList<QVariant> &)));
    connect(dataComingObject,SIGNAL(comClosed(bool)),this,SLOT(slot_updateComStat(bool)),Qt::QueuedConnection);
    connect(dataComingObject,SIGNAL(recOnePacket(const uint8_t*,uint32_t)),parseObject,SLOT(slot_parseData(const uint8_t*,uint32_t)),Qt::QueuedConnection);

    connect(parseObject,SIGNAL(ack_signal(QString)),this,SLOT(slot_updateAckText(QString)),Qt::QueuedConnection);
    connect(parseObject,SIGNAL(batteryVol_signal(int)),this,SLOT(slot_updateBatteryVol(int)),Qt::QueuedConnection);
    connect(parseObject,SIGNAL(chartData_signal(const SYSData_PerSensor_Packet*)),this,SLOT(slot_updateChartData(const SYSData_PerSensor_Packet*)),Qt::QueuedConnection);
    connect(parseObject,SIGNAL(connectedTable(QMap<uint16_t,bool>)),this,SLOT(slot_connectedTable(QMap<uint16_t,bool>)),Qt::QueuedConnection);
    connect(parseObject,SIGNAL(sendData(const uint8_t*,uint32_t)),dataComingObject,SLOT(slot_sendData(const uint8_t*,uint32_t)),Qt::QueuedConnection);

    connect(this,SIGNAL(sendBTRelayData(const uint8_t*,uint32_t)),parseObject,SLOT(slot_sendBTRelayPacket(const uint8_t*,uint32_t)),Qt::QueuedConnection);
    connect(this,SIGNAL(sendSensorRelayData(const uint8_t*,uint32_t)),parseObject,SLOT(slot_sendSensorRelayPacket(const uint8_t*,uint32_t)),Qt::QueuedConnection);

    dataComingThread->start();
    parseThread->start();
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：getChart1Display
 * 函数描述：获取选中的图例1通道以及数量
 * 输入参数：channel-选中的通道缓存(8个)
 * **返回值：选中通道数量
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
int MainWindow::getChart1Display(uint8_t *channel)
{
    int num = 0;
    if(ui->checkBox_com1_ch_1->isChecked())
    {
        num++;
        *channel++ = 0;
    }
    if(ui->checkBox_com1_ch_2->isChecked())
    {
        num++;
        *channel++ = 1;
    }
    if(ui->checkBox_com1_ch_3->isChecked())
    {
        num++;
        *channel++ = 2;
    }
    if(ui->checkBox_com1_ch_4->isChecked())
    {
        num++;
        *channel++ = 3;
    }
    if(ui->checkBox_com1_ch_5->isChecked())
    {
        num++;
        *channel++ = 4;
    }
    if(ui->checkBox_com1_ch_6->isChecked())
    {
        num++;
        *channel++ = 5;
    }
    if(ui->checkBox_com1_ch_7->isChecked())
    {
        num++;
        *channel++ = 6;
    }
    if(ui->checkBox_com1_ch_8->isChecked())
    {
        num++;
        *channel++ = 7;
    }

    return num;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：timerEvent
 * 函数描述：重写定时器事件函数
 * 输入参数：event-定时器事件指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == oneSecondTimerID)
    {

    }
    QMainWindow::timerEvent(event);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：eventFilter
 * 函数描述：重写事件过滤器函数
 * 输入参数：watched-过滤对象指针，event-事件指针
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == chartView1)   //图略1
    {
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            if(wheelEvent->delta() > 0) //网上滚动
            {
                chart1->zoomIn();       //放大
            }
            else
            {
                chart1->zoomOut();       //放大
            }
        }
    }

    return QWidget::eventFilter(watched,event);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：keyPressEvent
 * 函数描述：重写按键事件
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/01/06
 * ***************************************************************/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F1)
    {
        chart1_stop = !chart1_stop;
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_pushButton_chx_com_1_clicked
 * 函数描述：更新图例1通道颜色槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_pushButton_chx_com_1_clicked()
{
    QPalette palette = ui->line_chx_com_1->palette();
    int index = ui->comboBox_chx_com_1->currentIndex();
    QColor color = QColorDialog::getColor(palette.color(QPalette::WindowText),this,tr("选择图例1通道%1颜色").arg(index+1));
    if(color.isValid())
    {
        palette.setColor(QPalette::WindowText,color);
        chart1_splineSeriesList[index]->setColor(color);
        //chart1_scatterSeries[index]->setBrush(QBrush(color));
        ui->line_chx_com_1->setPalette(palette);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_comboBox_chx_com_1_currentIndexChanged
 * 函数描述：图例1选项框改变时槽函数
 * 输入参数：index-当前位置
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_comboBox_chx_com_1_currentIndexChanged(int index)
{
    QColor color = chart1_splineSeriesList[index]->color();     //获取线段的颜色
    QPalette palette = ui->line_chx_com_1->palette();
    palette.setColor(QPalette::WindowText,color);
    ui->line_chx_com_1->setPalette(palette);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_pushButton_sampleSet_1_clicked
 * 函数描述：设置图例1采样率槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_pushButton_sampleSet_1_clicked()
{
    int sample_val = ui->lineEdit_chx_com_1->text().toInt();
    chartSample_timer->setInterval(1000/sample_val);
    QMessageBox::information(this,QString("图例1"),QString("设置采样间隔%1ms成功！").arg(chartSample_timer->interval()));
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_1_stateChanged
 * 函数描述：图例1显示选项框1槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_1_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[0]->setVisible(true);
        //chart1_scatterSeries[0]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[0]->setVisible(false);
        //chart1_scatterSeries[0]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_2_stateChanged
 * 函数描述：图例1显示选项框2槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_2_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[1]->setVisible(true);
        //chart1_scatterSeries[1]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[1]->setVisible(false);
        //chart1_scatterSeries[1]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_3_stateChanged
 * 函数描述：图例1显示选项框3槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_3_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[2]->setVisible(true);
        //chart1_scatterSeries[2]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[2]->setVisible(false);
        //chart1_scatterSeries[2]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_4_stateChanged
 * 函数描述：图例1显示选项框4槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_4_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[3]->setVisible(true);
        //chart1_scatterSeries[3]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[3]->setVisible(false);
        //chart1_scatterSeries[3]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_5_stateChanged
 * 函数描述：图例1显示选项框5槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_5_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[4]->setVisible(true);
        //chart1_scatterSeries[4]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[4]->setVisible(false);
        //chart1_scatterSeries[4]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_6_stateChanged
 * 函数描述：图例1显示选项框6槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_6_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[5]->setVisible(true);
        //chart1_scatterSeries[5]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[5]->setVisible(false);
        //chart1_scatterSeries[5]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_7_stateChanged
 * 函数描述：图例1显示选项框7槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_7_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[6]->setVisible(true);
        //chart1_scatterSeries[6]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[6]->setVisible(false);
        //chart1_scatterSeries[6]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_com1_ch_8_stateChanged
 * 函数描述：图例1显示选项框8槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_com1_ch_8_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        chart1_splineSeriesList[7]->setVisible(true);
        //chart1_scatterSeries[7]->setVisible(true);
    }
    else
    {
        chart1_splineSeriesList[7]->setVisible(false);
        //chart1_scatterSeries[7]->setVisible(false);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox__chx_com_1_stateChanged
 * 函数描述：图例1显示选项框槽函数
 * 输入参数：arg1-选择状态
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox__chx_com_1_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
    {
        ui->comboBox_chx_com_1->setEnabled(true);
        ui->line_chx_com_1->setEnabled(true);
        ui->lineEdit_chx_com_1->setEnabled(true);
        ui->pushButton_chx_com_1->setEnabled(true);
        ui->pushButton_sampleSet_1->setEnabled(true);
        ui->checkBox_com1_ch_1->setEnabled(true);
        ui->checkBox_com1_ch_2->setEnabled(true);
        ui->checkBox_com1_ch_3->setEnabled(true);
        ui->checkBox_com1_ch_4->setEnabled(true);
        ui->checkBox_com1_ch_5->setEnabled(true);
        ui->checkBox_com1_ch_6->setEnabled(true);
        ui->checkBox_com1_ch_7->setEnabled(true);
        ui->checkBox_com1_ch_8->setEnabled(true);
        ui->pushButton_chart_1->setEnabled(true);
        ui->checkBox_chart1_sample->setEnabled(true);
        for(int i = 0;i < 8;i++)
        {
            chart1_splineSeriesList[i]->setVisible(true);
            //chart1_scatterSeries[i]->setVisible(true);
        }
        chartView1->setEnabled(true);
        chart1_stop = false;
    }
    else
    {
        ui->comboBox_chx_com_1->setEnabled(false);
        ui->line_chx_com_1->setEnabled(false);
        ui->lineEdit_chx_com_1->setEnabled(false);
        ui->pushButton_chx_com_1->setEnabled(false);
        ui->pushButton_sampleSet_1->setEnabled(false);
        ui->checkBox_com1_ch_1->setEnabled(false);
        ui->checkBox_com1_ch_2->setEnabled(false);
        ui->checkBox_com1_ch_3->setEnabled(false);
        ui->checkBox_com1_ch_4->setEnabled(false);
        ui->checkBox_com1_ch_5->setEnabled(false);
        ui->checkBox_com1_ch_6->setEnabled(false);
        ui->checkBox_com1_ch_7->setEnabled(false);
        ui->checkBox_com1_ch_8->setEnabled(false);
        ui->pushButton_chart_1->setEnabled(false);
        ui->checkBox_chart1_sample->setEnabled(false);
        for(int i = 0;i < 8;i++)
        {
            chart1_splineSeriesList[i]->setVisible(false);
            //chart1_scatterSeries[i]->setVisible(false);
        }
        chartView1->setEnabled(false);
        chart1_stop = true;
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_pushButton_chart_1_clicked
 * 函数描述：图例1设置开启OPENGL槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_pushButton_chart_1_clicked()
{
    if(chart1_openGL)
    {
        for(int i = 0;i < chart1_splineSeriesList.size();i++)
        {
            chart1_splineSeriesList[i]->setUseOpenGL(false);
            //chart1_scatterSeries[i]->setUseOpenGL(false);
        }
        ui->pushButton_chart_1->setStyleSheet(CSS_COLOR_NORMAL);
    }
    else
    {
        for(int i = 0;i < chart1_splineSeriesList.size();i++)
        {
            chart1_splineSeriesList[i]->setUseOpenGL(true);
            //chart1_scatterSeries[i]->setUseOpenGL(true);
        }
        ui->pushButton_chart_1->setStyleSheet(CSS_COLOR_GREEN);
    }
    chart1_openGL = !chart1_openGL;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_pushButton_Clear_1_clicked
 * 函数描述：图例1复位或清空曲线槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_pushButton_Clear_1_clicked()
{
    for(int i = 0;i < 8;i++)
    {
        chart1_splineSeriesList[i]->clear();
        //chart1_scatterSeries[i]->clear();
    }
    chart1_xAxis->setRange(0,100);
    chartView1->chart()->axisY()->setRange(-7450,-6950);
    chart1_xCount = 0;
}

MainWindow::~MainWindow()
{
    parseThread->terminate();   //结束进程
    parseThread->wait();        //等待结束
    dataComingThread->terminate();
    dataComingThread->wait();
    threeModifierThread->terminate();
    threeModifierThread->wait();

//    delete mapper_;
//    delete palette_;
    delete ui;
}

/******************************************************************
 * 作   者：PQP
 * 函数名称：updateComName
 * 函数描述：从系统文件中获取更新的串口列表
 * 输入参数：
 * **返回值：串口名称列表
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
QStringList MainWindow::updateComName()
{
    QStringList comName_list;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            comName_list.append(serial.portName());
            serial.close();
        }
    }

    return comName_list;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：generateSampleData
 * 函数描述：产生图例样例点
 * 输入参数：config-配置表
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::generateSampleData(int type, int rowCount, int colCount)
{
    // Remove previous data
    chart_data1.clear();
    // Append the new data depending on the type
    for (int i(0); i < rowCount; i++) {
        QVector<QPointF> points;
        points.reserve(colCount);
        for (int j(0); j < colCount; j++) {
            qreal x(0);
            qreal y(0);
            switch (type) {
            case 0:
                // data with sin + random component
                y = qAbs(qSin(M_PI / 10 * j))*65536;
                x = j;
                break;
            case 1:
                // linear data
                x = j;
                y = QRandomGenerator::global()->generateDouble()*65536;
                break;
            default:
                // unknown, do nothing
                break;
            }
            points.append(QPointF(x, y));
        }
        chart_data1.append(points);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：initSampleData
 * 函数描述：初始化采样序列
 * 输入参数：config-配置表
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::initSampleData(int data, int rowCount, int colCount)
{
    chart_data1.clear();

    for (int i(0); i < rowCount; i++) {
        QVector<QPointF> points;
        points.reserve(colCount);
        for (int j(0); j < colCount; j++) {
            qreal x;
            qreal y;
            x = j;
            y = data;
            points.append(QPointF(x, y));
        }
        chart_data1.append(points);
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_updateComconfigTable
 * 函数描述：更新串口的配置表
 * 输入参数：config-配置表
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_updateComconfigTable(QList<QVariant> &config)
{
    comConfigTable = config;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_chart1SampleTimerOut
 * 函数描述：图例1采样定时器溢出
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/01/02
 * ***************************************************************/
void MainWindow::slot_chart1SampleTimerOut()
{
    uint32_t i,j;

    for(i = 0;i < chart_data1.count();i++)
    {
        chart1_splineSeriesList[i]->replace(chart_data1.at(i));
        //chart1_scatterSeries[i]->replace(chart_data1.at(i));
        for(j = 0;j < chart1DataCnt;j++)
        {
            chart_data1[i][j] = QPointF(j,-7200);
        }
    }
    chart1DataCnt = 0;
}

/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_valShow
 * 函数描述：显示图例中的值槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/01/02
 * ***************************************************************/
void MainWindow::slot_valShow(QPointF point, bool stat)
{
    if(stat)
    {
        val_label->setText(QString::asprintf("x:%f\r\ny:%f",point.x(),point.y()));
        QPoint curPos = mapFromGlobal(QCursor::pos());
        val_label->move(curPos.x()-val_label->width()/2
                        ,curPos.y()-val_label->height()*1.5);
        val_label->show();
    }
    else
    {
        val_label->hide();
    }
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_action_COM_config_triggered
 * 函数描述：串口配置动作槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_action_COM_config_triggered()
{
    QStringList com_list;

    com_list = updateComName();
    ComConfigDialog *comConfigDialog = new ComConfigDialog(com_list,this);
    connect(comConfigDialog,SIGNAL(comConfigTable(QList<QVariant>&)),this,SLOT(slot_updateComconfigTable(QList<QVariant>&)));
    comConfigDialog->exec();
    disconnect(comConfigDialog,SIGNAL(comConfigTable(QList<QVariant>&)),this,SLOT(slot_updateComconfigTable(QList<QVariant>&)));
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_action_ConnectMe_triggered
 * 函数描述：联系客服动作槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_action_ConnectMe_triggered()
{
    QString qq_string("353708025");

    QString strAddress;
    strAddress = QString("tencent://message?v=3&uin=%1&site=qq&menu=yes").arg(qq_string);
    LPCWSTR wcharAddress = reinterpret_cast<const wchar_t*>(strAddress.utf16());
    ShellExecute(0, L"open", wcharAddress, L"", L"", SW_SHOW );
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_checkBox_chart1_sample_clicked
 * 函数描述：图例1采样率选项框槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_checkBox_chart1_sample_clicked()
{
    if(ui->checkBox_chart1_sample->isChecked())
    {
        chart1DataCnt = 0;
        //chartSample_timer[0]->start();
        chart1_stop = false;

    }
    else
    {
        //chartSample_timer[0]->stop();
        chart1_stop = true;
    }

}
/******************************************************************
 * 作   者：PQP
 * 函数名称：slot_connectedTable
 * 函数描述：传感器连接表更新槽函数
 * 输入参数：table-QMap<uint16_t,bool> 类型的ID表
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::slot_connectedTable(const QMap<uint16_t,bool> &table)
{
    int i;
    uint8_t sensor_number = 0;

    for(i = 0;i < ui->comboBox_ch_Sensor_1->count();i++)
    {
        if(table.value(i+0x0000))
        {
            ui->comboBox_ch_Sensor_1->setItemIcon(i,activePixmap);
            sensor_number++;
        }
        else
            ui->comboBox_ch_Sensor_1->setItemIcon(i,noactivePixmap);
    }
    slot_updateSensorNum(sensor_number);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_action_open_com_triggered
 * 函数描述：开关串口按钮槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_action_open_com_triggered()
{
    emit comOnOff(!dataComingObject->getComStat(),comConfigTable);
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_action_us_triggered
 * 函数描述：关于我们触发槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_action_us_triggered()
{
    QMessageBox *about_msgBox = new QMessageBox(this);

    about_msgBox->setIconPixmap(QPixmap(":/logo.bmp"));
    about_msgBox->setText(tr("压力曲线上位机,版本：V10"));
    about_msgBox->setWindowTitle(tr("关于我们"));
    about_msgBox->addButton(tr("确定"),QMessageBox::YesRole);
    about_msgBox->exec();
    delete about_msgBox;
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_action_QT_triggered
 * 函数描述：关于QT触发槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_action_QT_triggered()
{
    QMessageBox::aboutQt(this,QString("关于QT"));
}
/******************************************************************
 * 作   者：PQP
 * 函数名称：on_pushButton1_max_clicked
 * 函数描述：全屏或恢复槽函数
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2019/12/23
 * ***************************************************************/
void MainWindow::on_pushButton1_max_clicked()
{
    QValueAxis *yAxis = static_cast<QValueAxis *>(chart1->axisY());

    if(chart1_max)
    {
        ui->pushButton1_max->setText(tr("全屏"));
        yAxis->setTickCount(500/100+1);
    }
    else
    {
        ui->pushButton1_max->setText(tr("恢复"));
        yAxis->setTickCount(500/20+1);
    }
    chart1_max = !chart1_max;
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_action_Play_triggered
 * 函数描述：播放
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_action_Play_triggered()
{

}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_action_Save_triggered
 * 函数描述：录制保存
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_action_Save_triggered()
{

}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_New_triggered
 * 函数描述：新建窗口
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_New_triggered()
{
    //    //浮动窗口
    //    QDockWidget *dock = new QDockWidget("一个浮动窗口",this);
    //    addDockWidget(Qt::RightDockWidgetArea,dock);
    //    //给浮动窗口添加文本编辑区
    //    QTextEdit *MyDockQTE = new QTextEdit(this);
    //    dock->setWidget(MyDockQTE);

    //removeAllDock();
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_Open_triggered
 * 函数描述：打开
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_Open_triggered()
{

}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_Save_triggered
 * 函数描述：保存
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_Save_triggered()
{

}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_mune_2D_triggered
 * 函数描述：2D窗口视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_mune_2D_triggered()
{
    showDock(QList<int>()<< 5);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_3D_triggered
 * 函数描述：3D窗口视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_3D_triggered()
{
    showDock(QList<int>()<< 7<< 8);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_Graph_triggered
 * 函数描述：通道曲线窗口视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_Graph_triggered()
{
    showDock(QList<int>()<< 4<< 0);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_ColorBar_triggered
 * 函数描述：ColorBar窗口视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_ColorBar_triggered()
{
    showDock(QList<int>()<< 6);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_Com_Config_triggered
 * 函数描述：配置串口视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_Com_Config_triggered()
{
    showDock(QList<int>()<< 1);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_Sensor_Config_triggered
 * 函数描述：配置传感器视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_Sensor_Config_triggered()
{
    showDock(QList<int>()<< 2);
}
/******************************************************************
 * 作   者：LYC
 * 函数名称：on_menu_PressureCalibration_triggered
 * 函数描述：压力校验视图
 * 输入参数：
 * **返回值：
 * 编写日期：
 * 修改日期：2020/05/20
 * ***************************************************************/
void MainWindow::on_menu_PressureCalibration_triggered()
{
    showDock(QList<int>()<< 3);
}
