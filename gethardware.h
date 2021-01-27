#ifndef GETHARDWARE_H
#define GETHARDWARE_H

#include <QObject>

class GetHardWare : public QObject
{
    Q_OBJECT
public:
    explicit GetHardWare(QObject *parent = nullptr);

    static QString getMachineName();//获取电脑名字
    static QString getCpuInfo();//获取CPU信息
    static QString getCpuSerial();//获取CPU序列号
    static QStringList getMemoryInfo();//获取内存信息
    static QString getDiskSerial();//获取磁盘序列号
    static QList<QStringList> getDiskInfo();//获取磁盘信息
    static QString getOSInfo();//获取系统信息
    static QString getBiosSerial();//获取主板序列号
    static QString getDispCard();//获取显卡信息
    static QStringList getMacAddress();//获取所有网卡的MAC地址
    static bool getInternetState();//获取联网状态

private:
    static QString GetWare(QString cmd);

};

#endif // GETHARDWARE_H
