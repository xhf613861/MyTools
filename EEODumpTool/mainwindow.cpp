#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

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


void MainWindow::on_pushButton_3_clicked()
{
    m_fullFilePath = QFileDialog::getOpenFileName(this,QStringLiteral("文件对话框！"),"C:",QStringLiteral("(*txt)"));

    ui->sourceFile->setText(m_fullFilePath);
    m_readData.clear();
    qDebug() << " data lines :" << m_readData.size();
    QFileInfo sourceFile(m_fullFilePath);
    m_pathDir = sourceFile.absoluteDir().absolutePath();
}


void MainWindow::on_execute_clicked()
{
    if (ui->comboBox->currentText() == "default") {
        qDebug() << "please select version !!!";
        return;
    }

    readData();
    writeData();
}

void MainWindow::readData()
{
    QFile file(m_fullFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         while (!file.atEnd())
         {
             QByteArray line = file.readLine();
             if (QString(line).contains(ui->comboBox->currentText())) {
                 m_readData.push_back(line);
             }
         }
         file.close();
    }

    qDebug() << " read file count : " << m_readData.size();
}

void MainWindow::writeData()
{
    QString dstFile = m_pathDir + QString("/%1.txt").arg(ui->comboBox->currentText());
    qDebug() << dstFile;

    QFile file(dstFile);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        foreach (auto item, m_readData) {
            file.write(item);
        }
        file.flush();
        file.close();
    }
}

void MainWindow::getAllFiles(const QString &dirPath, QStringList &fileList)
{
    QDir dir(dirPath);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    foreach(QFileInfo fileInfo, list)
    {
        if(fileInfo.isDir())
        {
            getAllFiles(fileInfo.absoluteFilePath(), fileList);
        }
        else
        {
            if (!fileInfo.absoluteFilePath().contains(ui->comboBox->currentText()))
                continue;
            if (!fileInfo.absoluteFilePath().contains(ui->lineEdit->text()))
                continue;

            QString logFile = fileInfo.absoluteFilePath();
            unsigned tmp = logFile.indexOf(".dmp");
            logFile = logFile.left(tmp);

            fileList.append(logFile + ".xlog.log");
        }
    }
}

//(源文件目录路劲，目的文件目录，文件存在是否覆盖)
bool copyDirectory(const QString& srcPath, const QString& dstPath, bool coverFileIfExist)
{
    QDir srcDir(srcPath);
    QDir dstDir(dstPath);
    if (!dstDir.exists()) { //目的文件目录不存在则创建文件目录
        if (!dstDir.mkdir(dstDir.absolutePath()))
            return false;
    }
    QFileInfoList fileInfoList = srcDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if (fileInfo.isDir()) {    // 当为目录时，递归的进行copy
            if (!copyDirectory(fileInfo.filePath(),dstDir.filePath(fileInfo.fileName()),coverFileIfExist))
                return false;
        }
        else {            //当允许覆盖操作时，将旧文件进行删除操作
            if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
                dstDir.remove(fileInfo.fileName());
            }
            /// 进行文件copy
            if (!QFile::copy(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()))) {
                return false;
            }
        }
    }
    return true;
}

//(源文件文件路径，目的文件路径，文件存在是否覆盖)
bool copyFile(QString srcPath, QString dstPath, bool coverFileIfExist)
{
    srcPath.replace("\\", "/");
    dstPath.replace("\\", "/");
    if (srcPath == dstPath) {
        return true;
    }

    if (!QFile::exists(srcPath)) {  //源文件不存在
        return false;
    }

    if (QFile::exists(dstPath)) {
        if (coverFileIfExist) {
            QFile::remove(dstPath);
        }
    }

    if (!QFile::copy(srcPath, dstPath)){
        return false;
    }
    return true;
}


void MainWindow::on_pushButton_4_clicked()
{
    //文件夹路径
    QString srcDirPath = QFileDialog::getExistingDirectory(
               this, "choose src Directory",
                "/");

    if (srcDirPath.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << "srcDirPath=" << srcDirPath;
        srcDirPath += "/";
    }

    if (ui->lineEdit->text().isEmpty())
        return;

    if (ui->comboBox->currentText() == "default")
        return;

    QString outPutDir = srcDirPath + "outdir/";
    qDebug() << "outPutDir=" << outPutDir;
    QDir outDir(outPutDir);
    if (!outDir.exists()) {
        outDir.mkdir(outPutDir);
    }

    QStringList fileList;
    getAllFiles(srcDirPath, fileList);

    int index = 1;
    foreach (auto item, fileList) {
        qDebug() << item;
        copyFile(item, outPutDir + QString::number(index++) + ".txt", true);
    }
}

