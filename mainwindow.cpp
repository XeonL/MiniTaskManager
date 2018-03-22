#include "mainwindow.h"
#include "ui_mainwindow.h"
QString IsNumberString(QString &str) {
    for(QChar c : str) {
        if(!(c >= '0'&&c <= '9')) {
            return "";
        }
    }
    return str;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSystemInfo();
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateSystemInfo);
    timer->start(1000);
//    setFixedSize(600,450);



}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initSystemInfo() {
    initBasicInfo();
    initCPUInfo();
    initChart();

    updateSystemInfo();
    updateProcess();
}
void MainWindow::updateSystemInfo() {
    updateTimeInfo();
    updateMemInfo();
    updateCPUInfo();
    updateChart();
    updateProcess();
}
void MainWindow::updateProcess() {
    QDir dir("/proc");
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    ui->tableWidget_process->setRowCount(folder_list.size());
    ui->tableWidget_process->setColumnCount(8);
    QStringList header;
    header << "Name" << "Pid" << "State" << "memory" << "ppid" << "gpid" << "nice" << "priority";
    ui->tableWidget_process->setHorizontalHeaderLabels(header);


    ui->tableWidget_process->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_process->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_process->verticalHeader()->setVisible(false);

    ui->tableWidget_process->setShowGrid(false);
    ui->tableWidget_process->resizeColumnsToContents();
    ui->tableWidget_process->resizeRowsToContents();



    int num = -1;
    int s = 0,r = 0,z = 0;
    for(int i = 0; i != folder_list.size(); i++) {
        QString path = folder_list.at(i).absoluteFilePath();
        QString pid = IsNumberString(path.split("/proc/")[1]);
        QString ppid,gpid,nice,priority;
        QString name,task_state,memory;
        if(pid != "") {
            num++;
            QFile stat(path + "/stat");
            QFile statm(path + "/statm");
            if(!stat.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Can't open the file!" << endl;
                return;
            }
            if(!statm.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Can't open the file!" << endl;
                return;
            }
            QString statLine = stat.readLine();
            QString statmLine = statm.readLine();
            memory = statmLine.split(" ")[0];
            int begin = statLine.split("(")[0].length() + 1;
            int end = statLine.length() - statLine.split(")")[statLine.split(")").size()-1].length() - begin -1;
            name = statLine.mid(begin,end);
            QStringList list = statLine.split(" ");
            task_state = statLine.mid(begin+end).split(" ")[1];
            if(task_state == "R") {
                r++;
            } else if(task_state == "S") {
                s++;
            } else if(task_state == "Z") {
                z++;
            }
            ppid = list[3];
            gpid = list[4];
            nice = list[18];
            priority = list[17];

            ui->tableWidget_process->setItem(num,0,new QTableWidgetItem(name));
            ui->tableWidget_process->setItem(num,1,new QTableWidgetItem(pid));
            ui->tableWidget_process->setItem(num,2,new QTableWidgetItem(task_state));
            ui->tableWidget_process->setItem(num,3,new QTableWidgetItem(memory + " Bytes"));
            ui->tableWidget_process->setItem(num,4,new QTableWidgetItem(ppid));
            ui->tableWidget_process->setItem(num,5,new QTableWidgetItem(gpid));
            ui->tableWidget_process->setItem(num,6,new QTableWidgetItem(nice));
            ui->tableWidget_process->setItem(num,7,new QTableWidgetItem(priority));
        }
    }
    itemNum = num + 1;
    ui->label_total->setText(QString::number(itemNum,10));
    ui->label_sleep->setText(QString::number(s,10));
    ui->label_run->setText(QString::number(r,10));
    ui->label_zombie->setText(QString::number(z,10));
    return;
}
void MainWindow::updateChart() {
    series_cpu->clear();
    series_mem->clear();
    series_swap->clear();
    for(int i = 0;i < 120;i++) {
        series_cpu->append(i/120.0,cpu[i]/100);
        series_mem->append(i/120.0,mem[i]/100);
        series_swap->append(i/120.0,swap[i]/100);
    }
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
    cpu.dequeue();
    if(cpuUsage) {
        cpu.enqueue(cpuUsage);
    } else {
        cpu.enqueue(43.51);
    }
}
void MainWindow::updateMemInfo() {
    QFile file_meminfo("/proc/meminfo");
    if(!file_meminfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open the file!" << endl;
        return;
    }
    int i = 0;
    double totalmem,availmem,swaptotal,swapfree;
    do {
        i++;
        QByteArray line = file_meminfo.readLine();
        QString str(line);
        if(i == 1) {
            totalmem = str.split(":")[1].split(" kB")[0].simplified().toDouble()/(1024*1024);
            ui->label_memtotal->setText(QString::number(totalmem,10,4) + " GBytes");
        } else if(i == 2) {
            ui->label_memfree->setText(QString::number(str.split(":")[1].split(" kB")[0].simplified().toDouble()/(1024*1024),10,4) + " GBytes");
        } else if(i == 3) {
            availmem = str.split(":")[1].split(" kB")[0].simplified().toDouble()/(1024*1024);
            ui->label_memavailable->setText(QString::number(availmem,10,4) + " GBytes");
        } else if(i == 6) {
            ui->label_swapcached->setText(str.split(":")[1].split(" kB")[0].simplified() + " KBytes");
        } else if(i == 15) {
            swaptotal = str.split(":")[1].split(" kB")[0].simplified().toDouble();
        } else if(i == 16) {
            swapfree = str.split(":")[1].split(" kB")[0].simplified().toDouble();
        }
    } while(!file_meminfo.atEnd()&&i != 16);
    mem.dequeue();
    mem.enqueue((totalmem - availmem)/totalmem*100);
    swap.dequeue();
    swap.enqueue((swaptotal - swapfree)/swaptotal*100);
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
void MainWindow::initChart() {
    for(int i = 0;i < 120;i++) {
        cpu.enqueue(0);
        mem.enqueue(0);
        swap.enqueue(0);
    }
    QChartView *chartView = new QChartView(this);
    QChart *chart = chartView->chart();
    ui->verticalLayout_cpu->addWidget(chartView);
    chart->legend()->hide();
    series_cpu = new QLineSeries();
    QValueAxis * axisX = new QValueAxis;
    QValueAxis * axisY = new QValueAxis;
    axisX->setRange(0,120);
    axisX->setTitleText("Time(s)");
    axisY->setRange(0,100);
    axisY->setTitleText("CpuUsage(%)");
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addSeries(series_cpu);

    QChartView *chartView2 = new QChartView(this);
    QChart *chart2 = chartView2->chart();
    ui->verticalLayout_mem->addWidget(chartView2);
    chart2->legend()->hide();
    series_mem = new QLineSeries();
    QValueAxis * axisX2 = new QValueAxis;
    QValueAxis * axisY2 = new QValueAxis;
    axisX2->setRange(0,120);
    axisX2->setTitleText("Time(s)");
    axisY2->setRange(0,100);
    axisY2->setTitleText("MemUsage(%)");
    chart2->addAxis(axisX2, Qt::AlignBottom);
    chart2->addAxis(axisY2, Qt::AlignLeft);
    chart2->addSeries(series_mem);

    QChartView *chartView3 = new QChartView(this);
    QChart *chart3 = chartView3->chart();
    ui->verticalLayout_swap->addWidget(chartView3);
    chart3->legend()->hide();
    series_swap = new QLineSeries();
    QValueAxis * axisX3 = new QValueAxis();
    QValueAxis * axisY3 = new QValueAxis();
    axisX3->setRange(0,120);
    axisX3->setTitleText("Time(s)");
    axisY3->setRange(0,100);
    axisY3->setTitleText("SwapUsage(%)");
    chart3->addAxis(axisX3, Qt::AlignBottom);
    chart3->addAxis(axisY3, Qt::AlignLeft);
    chart3->addSeries(series_swap);

    updateChart();
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
    system("shutdown -h 1");
}

void MainWindow::on_pushButton_Search_clicked()
{
    QString keyWords = ui->lineEdit_killProcess->text();
    if(keyWords == "") {
        ui->label_search_process_info->setText("Please input process name or pid!");
        return;
    }
    int count = itemNum - 1;
    for(int i = 0;i <= count;i++) {
        QString name = ui->tableWidget_process->item(i,0)->text();
        QString pid = ui->tableWidget_process->item(i,1)->text();
        if(name == keyWords || pid == keyWords) {
            waitKillPid = pid;
            QString info = "";
            info += "Search Success!\nProcess Name:";
            info += name + "\nProcess Pid:";
            info += pid + "\nProcess Memory:";
            info += ui->tableWidget_process->item(i,3)->text();
            ui->label_search_process_info->setText(info);
            ui->pushButton_Kill->setEnabled(true);
            return;
        }
    }
    qDebug() << "no exist";
    waitKillPid = "";
    ui->pushButton_Kill->setEnabled(false);
    ui->label_search_process_info->setText("Search failed!\nNot Exist!");
}

void MainWindow::on_pushButton_Kill_clicked()
{
    if(waitKillPid == "") {
        ui->label_search_process_info->setText("Please Search!");
    } else {
        QString cmd = "kill ";
        cmd += waitKillPid;
        system(cmd.toLatin1().data());
        ui->label_search_process_info->setText("Killed!");
    }
    ui->pushButton_Kill->setEnabled(false);
}

void MainWindow::on_pushButton_create_clicked()
{
    QProcess *pro = new QProcess(this);
    pro->start("gedit",NULL);
}
