#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QFuture"
#include "QFutureWatcher"
#include "QtConcurrent/QtConcurrentRun"
#include "QObject"
#include <QTime>
#include <QtGlobal>

#include <QApplication>
#include <QtConcurrent/QtConcurrentRun>
#include <QDebug>


#include <QImage>
#include <QDebug>
#include <QGuiApplication>
#include <QtConcurrent/QtConcurrentMap>

#include <QtWidgets/QApplication>
#include "QDebug"
#include "QFuture"
#include "QFutureWatcher"
#include "QtConcurrent/QtConcurrentRun"
#include "QObject"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int longTimeFunc()
{
    for (int i = 0; i < 100; ++i)
    {

      QThread::msleep(50);
    }
    qDebug() << 22 << QThread::currentThreadId();
    return 100;
}

void MainWindow::on_wwwww_clicked()
{
    {
        QFuture<int> future = QtConcurrent::run(longTimeFunc);
        void *tmp = &future;
        QFutureWatcher<int> * watcher = new QFutureWatcher<int>();
        QFutureWatcher<int> * watcher1 = new QFutureWatcher<int>();
        qDebug() << "start" << QThread::currentThreadId();
        QObject::connect(watcher, &QFutureWatcher<int>::finished, [watcher1, &future]() {
//            qDebug() << "zz";
//            future.setSuspended(true);
//            qDebug() << "zz1";
//            void *tmp1 = &future;
//            qDebug() << "zz3";
          qDebug() << future.result();
            qDebug() << "watcher1" << QThread::currentThreadId();
          delete watcher1;
        });
        watcher->setFuture(future);
        qDebug() << "end";
    }
    qDebug() << "end1";
//    QEventLoop llll;
//    llll.exec();
}


//静态函数
void MainWindow::Func(QPushButton * & btn)
{
    QTime time = QTime::currentTime();
    srand(time.msec() + time.second()*1000);
    btn->setText(QString("按钮_%1").arg(rand() % 20));
    qDebug()<<"thread ID"<<QThread::currentThreadId();
}

void MainWindow::on_map_clicked()
{
    QList<QPushButton*> list = this->findChildren<QPushButton*>();
    qDebug()<<"MainWindow thread ID"<<QThread::currentThreadId();
//    QFuture<void> f = QtConcurrent::map(list, funx); //map函数 不能运行非静态成员函数
    QFuture<void> f = QtConcurrent::map(list, &MainWindow::Func); //map函数 不能运行非静态成员函数
    f.waitForFinished();
}

int func2(int a)
{

    qDebug()<<"thread ID func2"<<QThread::currentThreadId();
    QThread::msleep(500);
    return a + 1;
}


void MainWindow::on_mapped_clicked()
{
    QList<int> alist;
    alist<<1<<3<<5<<7<<9;

    QFuture<int> f = QtConcurrent::mapped(alist,func2); //QFuture的类型为int
    f.waitForFinished();
    qDebug()<<"alist"<<alist;
    QList<int> newlist = f.results();
    qDebug()<<"newlist"<<newlist;
}

QPushButton* MainWindow::Func2(QPushButton * btn)
{
    QThread::msleep(2000);
    QTime time = QTime::currentTime();
    srand(time.msec() + time.second()*1000);
    btn->setText(QString("按钮_%1").arg(rand() % 20));
    qDebug()<<"thread ID"<<QThread::currentThreadId();
    return btn;
}

void MainWindow::on_mapped_2_clicked()
{
    QList<QPushButton*> list = this->findChildren<QPushButton*>();
    QFuture<QPushButton*> f2 = QtConcurrent::mapped(list, &MainWindow::Func2);
//    f2.waitForFinished();
}

int func3(int a)
{
    qDebug()<<"thread ID func3"<<QThread::currentThreadId() << a;
    return a + 1;
}

void sum(int& result, const int& b)
{
    qDebug()<<"thread ID sum "<<QThread::currentThreadId() << "result:" << &result << "b:" << &b;
    result += b;
}

void MainWindow::on_mappedReduced_clicked()
{
    QList<int> alist;
    alist<<1<<3<<5<<7<<9;

    QFuture<int> result = QtConcurrent::mappedReduced(alist,func3,sum);
    result.waitForFinished();
    qDebug()<<result.result();
}




