#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "gethardware.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "MachineName: " << GetHardWare::getMachineName();
    qDebug() << "CpuInfo: " << GetHardWare::getCpuInfo();

    qDebug() << "MemoryInfo: " << GetHardWare::getMemoryInfo();
    qDebug() << "OSInfo: " << GetHardWare::getOSInfo();
    qDebug() << "DiskSerial: " << GetHardWare::getDiskSerial();
    qDebug() << "MacAddress: " << GetHardWare::getMacAddress();
    qDebug() << "DispCard: " << GetHardWare::getDispCard();
    qDebug() << "InternetState: " << GetHardWare::getInternetState();
    qDebug() << "DiskInfo: " << GetHardWare::getDiskInfo();

    qDebug() << "CpuSerial: " << GetHardWare::getCpuSerial();
    qDebug() << "BiosSerial: " << GetHardWare::getBiosSerial();
}

MainWindow::~MainWindow()
{
    delete ui;
}

