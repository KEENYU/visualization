#ifndef COMCONFIGDIALOG_H
#define COMCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ComConfigDialog;
}

class ComConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComConfigDialog(QStringList &com_name_list,QWidget *parent = 0);
    ~ComConfigDialog();

private slots:
    void on_pushButton_yes_clicked();

    void on_pushButton_no_clicked();

private:
    Ui::ComConfigDialog *ui;

    QList<QVariant> configTable;

    void comInit();
signals:
    void comConfigTable(QList<QVariant> &);
};

#endif // COMCONFIGDIALOG_H
