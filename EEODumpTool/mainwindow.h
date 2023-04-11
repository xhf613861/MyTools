#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_3_clicked();

    void on_execute_clicked();

    void on_pushButton_4_clicked();

private:
    void readData();
    void writeData();
    void getAllFiles(const QString &dirPath, QStringList &fileList);

private:
    Ui::MainWindow *ui;

    QString m_pathDir;
    QString m_fullFilePath;

    QVector<QByteArray> m_readData;
};
#endif // MAINWINDOW_H
