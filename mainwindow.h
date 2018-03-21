#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void initSystemInfo();
    void initBasicInfo();
    void initCPUInfo();
    void updateSystemInfo();
    void updateTimeInfo();
    void updateCPUInfo();
    void updateMemInfo();

    void on_pushButton_shutdown_clicked();

private:
    Ui::MainWindow *ui;
    QStringList cpuInfoList;
};

#endif // MAINWINDOW_H
