#include "mainwindow.h"
#include <QApplication>
#include <winsock2.h>
#include <dbghelp.h>
#include <windows.h>
#include <winnt.h>
#include <QMessageBox>
#include <QString>
#include <QTime>

/*
工程: debug_test
日期: 2021-03-19
作者: DS小龙哥
环境: win10 QT5.12.6 MinGW32
功能: 程序异常捕获接口
*/
LONG ExceptionCapture(EXCEPTION_POINTERS *pException)
{
    //当前时间串
    const int TIMESTRLEN = 32;
    WCHAR timeStr[TIMESTRLEN];
    SYSTEMTIME time;
    GetLocalTime(&time);
    swprintf_s(timeStr, TIMESTRLEN, L"%4d%02d%02d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    WCHAR strname[MAX_PATH];
    swprintf_s(strname, MAX_PATH, L"application_%s.dump", timeStr);

    //创建 Dump 文件
    HANDLE hDumpFile = CreateFile(strname, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if( hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, nullptr, nullptr);
    }


    //完成一些数据保存工作
    //.....

    //弹出错误对话框并退出程序
    QMessageBox::critical(nullptr,"错误提示",QString("当前程序遇到异常.\n  异常文件:%1").arg(QString::fromWCharArray(strname)),QMessageBox::Ok,QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //注冊异常捕获函数
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionCapture);
    MainWindow w;
    w.show();
    return a.exec();
}
