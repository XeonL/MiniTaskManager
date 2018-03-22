#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QtCharts/QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QQueue>
#include <QDir>
#include <QFileInfoList>
using namespace QtCharts;
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
    void initChart();
    void updateSystemInfo();
    void updateTimeInfo();
    void updateCPUInfo();
    void updateMemInfo();
    void updateChart();
    void updateProcess();

    void on_pushButton_shutdown_clicked();

    void on_pushButton_Search_clicked();

    void on_pushButton_Kill_clicked();

private:
    Ui::MainWindow *ui;
    QStringList cpuInfoList;
    QLineSeries *series_cpu;
    QLineSeries *series_mem;
    QLineSeries *series_swap;
    QQueue<double> cpu,mem,swap;
    QString waitKillPid = "";
    int itemNum = 0;
};

#endif // MAINWINDOW_H
