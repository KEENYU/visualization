#include "comconfigdialog.h"
#include "ui_comconfigdialog.h"



ComConfigDialog::ComConfigDialog(QStringList &com_name_list,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("串口配置"));

    setFixedSize(200,250);
    ui->comboBox_com->addItems(com_name_list);
}

ComConfigDialog::~ComConfigDialog()
{
    delete ui;
}

void ComConfigDialog::on_pushButton_yes_clicked()
{
    configTable.append(ui->comboBox_com->currentText());
    configTable.append(ui->comboBoxbaud->currentText());
    configTable.append(ui->comboBox_databit->currentText());
    configTable.append(ui->comboBox_check->currentIndex());
    configTable.append(ui->comboBox_StopBit->currentIndex());
    configTable.append(ui->comboBox_hardwareFlow->currentIndex());
    emit comConfigTable(configTable);

    accept();
}

void ComConfigDialog::on_pushButton_no_clicked()
{
    reject();
}
