#include "mainwindow.h"

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




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;


    w.show();
    return a.exec();
}




//QImage scale(const QImage &image)
//{
//    qDebug() << "Scaling image in thread" << QThread::currentThread();
//    return image.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//}

//int main(int argc, char *argv[])
//{
//    QGuiApplication app(argc, argv);

//    const int imageCount = 20;

//    // 创建一个列表包含 imageCount 个图像
//    QList<QImage> images;
//    for (int i = 0; i < imageCount; ++i)
//        images.append(QImage(1600, 1200, QImage::Format_ARGB32_Premultiplied));

//    // 使用 QtConcurrent::mapped 对每个图像应用 scale 函数（缩放）
//    QFuture<QImage> thumbnails = QtConcurrent::mapped(images, scale);

//    // 暂停计算
//    thumbnails.pause();
//    qDebug() << "isPaused" << thumbnails.isPaused();

//    // 恢复计算
//    if(thumbnails.isPaused()) {
//        thumbnails.resume();
//        qDebug() << "isStarted" << thumbnails.isPaused();
//    }

//    // 阻塞调用线程并等待计算完成，确保所有结果可用
//    thumbnails.waitForFinished();

//    // 返回 future 的所有结果
//    QList<QImage> list = thumbnails.results();
//    foreach (QImage image, list) {
//        qDebug() << image;
//    }

//    qDebug() << "********************";

//    // 返回 future 的结果数
//    int nCount = thumbnails.resultCount();
//    for (int i = 0; i < nCount; ++i) {
//        QImage image = thumbnails.resultAt(i);
//        qDebug() << image;
//    }

//    return 0;
//}



//void hello(const QString &name)
//{
//    qDebug() << "Hello" << name << "from" << QThread::currentThread();
//}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);


//    qDebug() << "Main Thread" << QThread::currentThread();

//    // 在一个单独的线程中调用 hello()
//    QFuture<void> f1 = QtConcurrent::run(hello, QString("Qter"));
//    QFuture<void> f2 = QtConcurrent::run(hello, QString("Pythoner"));

//    // 阻塞调用线程并等待计算完成，确保所有结果可用
//    f1.waitForFinished();
//    f2.waitForFinished();

//    MainWindow w;
//    w.show();
//    return a.exec();
//}
