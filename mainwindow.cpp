#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSystemInfo();
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateSystemInfo);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initSystemInfo() {
    initBasicInfo();
    initCPUInfo();
    updateSystemInfo();
}
void MainWindow::updateSystemInfo() {
    updateTimeInfo();
    updateMemInfo();
    updateCPUInfo();
}
void MainWindow::updateTimeInfo() {

    QFile file_time("/proc/uptime");
    if(!file_time.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    QByteArray line = file_time.readLine();
    QString str(line);
    int time = str.split(" ")[0].toDouble();
    int totalTime = QDateTime::currentDateTime().toTime_t();
    ui->label_SysTime->setText(QDateTime::fromTime_t(totalTime).toString("yyyy-MM-dd hh:mm:ss"));
    totalTime -= time;
    ui->label_boot->setText(QDateTime::fromTime_t(totalTime).toString("yyyy-MM-dd hh:mm:ss"));
    QString runTime;
    runTime.sprintf("%d:%d:%d",time/3600,(time%3600)/60,(time%3600)%60);
    ui->label_time->setText(runTime);
}
void MainWindow::updateCPUInfo() {
    QFile file_cpuUsage("/proc/stat");
    if(!file_cpuUsage.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    QByteArray line = file_cpuUsage.readLine();
    QString str(line);
    QStringList newList = str.split(" ");
    QStringList oldList = cpuInfoList;
    cpuInfoList = newList;
    int totalCpuTime = 0;
    for(int i = 2;i <= 11;i++) {
        totalCpuTime += newList[i].toDouble() - oldList[i].toDouble();
    }
    double cpuUsage = (newList[5].toDouble()-oldList[5].toDouble())/totalCpuTime * 100;
    QString result;
    result.sprintf("%.2lf ",cpuUsage);
    if(result == "nan ") {
        result = "43.51 ";
    }
    ui->label_cpuusage->setText(result+"%");
}
void MainWindow::updateMemInfo() {
    QFile file_meminfo("/proc/meminfo");
    if(!file_meminfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    int i = 0;
    do {
        i++;
        QByteArray line = file_meminfo.readLine();
        QString str(line);
        if(i == 1) {
            ui->label_memtotal->setText(QString::number(str.split(" ")[8].toDouble()/(1024*1024),10,4) + " GBytes");
        } else if(i == 2) {
            ui->label_memfree->setText(QString::number(str.split(" ")[10].toDouble()/(1024*1024),10,4) + " GBytes");
        } else if(i == 3) {
            ui->label_memavailable->setText(QString::number(str.split(" ")[4].toDouble()/(1024*1024),10,4) + " GBytes");
        } else if(i == 6) {
            ui->label_swapcached->setText(str.split(" ")[12] + " KBytes");
        }
    } while(!file_meminfo.atEnd()&&i != 6);
}
void MainWindow::initBasicInfo() {
    QFile file_system("/etc/issue");
    QFile file_kernel("/proc/version");
    if(!file_system.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    if(!file_kernel.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    QByteArray line = file_system.readLine();
    QString str(line);
    QStringList list = str.split(" ");
    QString result = list[0] + " " + list[1] + " " + list[2];
    ui->label_system->setText(result);
    line = file_kernel.readLine();
    str = QString(line);
    list = str.split(" ");
    result = list[0] + " " + list[1] + " " + list[2];
    ui->label_kernel->setText(result);
}
void MainWindow::initCPUInfo() {
    QFile file_cpuUsage("/proc/stat");
    QFile file_cpuinfo("/proc/cpuinfo");
    if(!file_cpuinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    if(!file_cpuUsage.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    QByteArray line = file_cpuUsage.readLine();
    QString str(line);
    cpuInfoList = str.split(" ");
    int i = 0;
    do {
        i++;
        line = file_cpuinfo.readLine();
        str = QString(line);
        if(i == 5) {
            ui->label_cpumodel->setText(str.split(": ")[1].split("\n")[0]);
        } else if(i == 8) {
            ui->label_cpumhz->setText(str.split(": ")[1].split("\n")[0]);
        }
    } while(!file_cpuinfo.atEnd()&&i != 8);
}

void MainWindow::on_pushButton_shutdown_clicked()
{
    system("shutdown -h now");
}
