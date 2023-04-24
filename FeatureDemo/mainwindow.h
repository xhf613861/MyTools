#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void Func(QPushButton * & btn);
    static QPushButton * Func2(QPushButton * btn);

private slots:

    void on_wwwww_clicked();

    void on_map_clicked();

    void on_mapped_clicked();

    void on_mappedReduced_clicked();

    void on_mapped_2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
