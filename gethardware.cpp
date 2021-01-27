#include "gethardware.h"
#include <QtNetwork/QHostInfo>
#include <QNetworkInterface>
#include <QProcess>
#include <QSysInfo>
#include <QSettings>
#include <Windows.h>
#include <QTcpSocket>
#include <QFileInfoList>
#include <QDir>

#define KB (1024)
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

GetHardWare::GetHardWare(QObject *parent) : QObject(parent)
{

}

//获取电脑名字
QString GetHardWare::getMachineName()
{
    QString machineName = QHostInfo::localHostName();
    return machineName;
}

//获取CPU信息
QString GetHardWare::getCpuInfo()
{
    return GetWare("wmic cpu get name");
}

//获取CPU序列号
QString GetHardWare::getCpuSerial()
{
    return GetWare("wmic cpu get processorid");
}

//获取内存信息
//返回信息依次为：已使用内存大小、未使用内存大小、总内存大小，单位为MB
//如：("5041", "3001", "8042")
QStringList GetHardWare::getMemoryInfo()
{
    QStringList m_memInfo;

    int memoryPercent;
    int memAll;
    int memUse;
    int memFree;

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);
    memoryPercent = statex.dwMemoryLoad;
    memAll = statex.ullTotalPhys / MB;
    memFree = statex.ullAvailPhys / MB;
    memUse = memAll - memFree;

    m_memInfo.append(QString::number(memUse));
    m_memInfo.append(QString::number(memFree));
    m_memInfo.append(QString::number(memAll));

    return m_memInfo;
}

//获取磁盘序列号
QString GetHardWare::getDiskSerial()
{
    return GetWare("wmic diskdrive where index=0 get serialnumber");//只能在Windows环境下使用
}

//获取磁盘信息
//返回信息依次为：磁盘名称、已使用空间大小、未使用空间大小、总空间大小、使用百分比
//如：   (("C:/", "92.9G", "122.5G", "215.5G", "43"),
//      ("D:/", "175.8G", "135.2G", "311.0G", "56"),
//      ("E:/", "90.9G", "220.1G", "311.0G", "29"),
//      ("F:/", "243.1G", "66.4G", "309.5G", "78"))
QList<QStringList> GetHardWare::getDiskInfo()
{
    QList<QStringList> diskList;

    QFileInfoList list = QDir::drives();
    foreach (QFileInfo dir, list)
    {
        QStringList curDisk;
        QString dirName = dir.absolutePath();
        LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
        ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
        if(GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
        {
            //已使用的空间大小
            QString use = QString::number((double) (liTotalBytes.QuadPart - liTotalFreeBytes.QuadPart) / GB, 'f', 1);
            use += "G";
            //未使用的空间大小
            QString free = QString::number((double) liTotalFreeBytes.QuadPart / GB, 'f', 1);
            free += "G";
            //总空间大小
            QString all = QString::number((double) liTotalBytes.QuadPart / GB, 'f', 1);
            all += "G";
            //使用率百分比
            int percent = 100 - ((double)liTotalFreeBytes.QuadPart / liTotalBytes.QuadPart) * 100;

            curDisk.append(dirName);
            curDisk.append(use);
            curDisk.append(free);
            curDisk.append(all);
            curDisk.append(QString::number(percent));
        }
        diskList.append(curDisk);
    }
    return diskList;
}

//获取系统信息
//如：Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
QString GetHardWare::getOSInfo()
{
    QString m_osInfo;
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
    }
    QString sysBit = "unknown";
    if(bIsWow64)
        sysBit = "64bit";
    else
        sysBit = "32bit";

    m_osInfo = QSysInfo::prettyProductName() + " " + sysBit;
    return m_osInfo;
}

//获取主板序列号
QString GetHardWare::getBiosSerial()
{
    return GetWare("wmic baseboard get serialnumber");
}

//获取显卡信息
QString GetHardWare::getDispCard()
{
    QString dcard;

    QSettings *DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\nvlddmkm\\Device0",QSettings::NativeFormat);
    QString type = DCard->value("Device Description").toString();
    delete DCard;

    QString dType = type;
    dType.trimmed();
    if(!dType.isEmpty())
        dcard = dType;

    DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\igfx\\Device0",QSettings::NativeFormat);
    type = DCard->value("Device Description").toString();
    delete DCard;

    dType = type;
    dType.trimmed();
    if(!dType.isEmpty())
        dcard = dcard + "\n" +dType;

    DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\amdkmdap\\Device0",QSettings::NativeFormat);
    type = DCard->value("Device Description").toString();
    delete DCard;

    dType = type;
    dType.trimmed();
    if(!dType.isEmpty())
        dcard = dcard + "\n" +dType;

    dcard.trimmed();
    return dcard;
}

//获取所有网卡的MAC地址
//如：("00:15:5D:A2:BD:C7", "00:50:56:C0:00:01", "00:50:56:C0:00:08", "D8:F2:CA:6D:6A:2D")
QStringList GetHardWare::getMacAddress()
{
    QStringList macList;
    QList<QNetworkInterface> netList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface item, netList)//遍历所有网卡
    {
        if((QNetworkInterface::IsUp & item.flags()) && (QNetworkInterface::IsRunning & item.flags())
                && (item.hardwareAddress().toStdString().length()>0))
        {
            macList.append(item.hardwareAddress());
        }
    }
    return macList;
}

//获取联网状态
bool GetHardWare::getInternetState()
{
    //能ping的通百度则说明可以联网
    QTcpSocket tcpClient;
    tcpClient.abort();
    tcpClient.connectToHost("www.baidu.com", 80);
    //100毫秒没有连接上则判断不在线
    return tcpClient.waitForConnected(100);
}

//调用Windows WMIC指令 获取硬件信息 此方法仅适用于Windows环境
QString GetHardWare::GetWare(QString cmd)
{
    //获取cpu名称：wmic cpu get Name
    //获取cpu核心数：wmic cpu get NumberOfCores
    //获取cpu线程数：wmic cpu get NumberOfLogicalProcessors
    //查询cpu序列号：wmic cpu get processorid
    //查询主板序列号：wmic baseboard get serialnumber
    //查询BIOS序列号：wmic bios get serialnumber
    //查看硬盘：wmic diskdrive get serialnumber
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    QStringList list = cmd.split(" ");
    result = result.remove(list.last(), Qt::CaseInsensitive);
    result = result.replace("\r", "");
    result = result.replace("\n", "");
    result = result.simplified();
    return result;
}
