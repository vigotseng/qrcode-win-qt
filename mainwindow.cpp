
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qrencode.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QStringList>
#include <QTextStream>
#include <QtConcurrentRun>

QString  baseUrl = "http://csgyn.com/t/?qr=";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    quitApp(false),
    workThreadFinish(false)
{
    ui->setupUi(this);

    ui->progressBar->hide();

    m_font = QSharedPointer<QFont>(new QFont("Microsoft YaHei", 50));
    m_fm = QSharedPointer<QFontMetrics>(new QFontMetrics(*m_font));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startMultiThreadTask(const QStringList &fileContent, QString outputPath)
{
    quitApp = false;
    workThreadFinish = false;

    ui->startBtn->setEnabled(false);
    ui->openInputFileBtn->setEnabled(false);
    ui->chooseLogoBtn->setEnabled(false);
    int thread_count = QThread::idealThreadCount();
    WorkTask* thread_pool[thread_count];
    for (int i = 0; i < thread_count; ++i) {
        thread_pool[i] = new WorkTask();
    }
    int thread_idx = 0;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(fileContent.length());
    ui->progressBar->show();
    int idx = 0;

    foreach (QString line, fileContent) {
        if (quitApp) {
            break;
        }
        WorkTask *workTask = thread_pool[thread_idx ++];
        workTask->setContent(line);
        workTask->setLogoImage(logoImage);
        workTask->setOutputPath(outputPath);
        workTask->setFont(m_font);
        workTask->setFm(m_fm);
        workTask->start();

        if (thread_idx == thread_count) {
            for (int i = 0; i < thread_count; ++i) {
                thread_pool[i]->wait();
            }

            thread_idx = 0;
            idx += thread_count;
        }

        ui->progressBar->setValue(idx);
    }

    for (int i = 0; i < thread_idx; ++i) {
        thread_pool[i]->wait();
    }

    ui->progressBar->hide();
    workThreadFinish = true;
    ui->startBtn->setEnabled(true);
    ui->openInputFileBtn->setEnabled(true);
    ui->chooseLogoBtn->setEnabled(true);
}

void WorkTask::run()
{
    QString data = baseUrl + m_content;
    QRcode *pQRcode = QRcode_encodeString(data.toLocal8Bit().data(), 1, QR_ECLEVEL_Q, QR_MODE_8, 1);

    int scale = 25;
    int spacing = 25;

    QImage image(2 * spacing + pQRcode->width * scale, 2 * spacing + pQRcode->width * scale + 75, QImage::Format_RGB888);
    image.fill(Qt::white);

    QPainter painter(&image);

    uchar *pSrc = pQRcode->data;
    for (int col = 0; col < pQRcode->width; ++col) {
        for (int row = 0; row < pQRcode->width; ++row) {
            if (*pSrc & 1) {
                painter.fillRect(spacing + col * scale, spacing + row *scale, scale, scale, Qt::black);
            }
            pSrc ++;
        }
    }
    QRcode_free(pQRcode);

    painter.drawPixmap((image.width() - m_logoPixmap.width()) / 2,
                       (image.height() - m_logoPixmap.height()) / 2,
                       m_logoPixmap);

    int text_width = m_fm->width(m_content);
    int text_height = m_fm->height();

    painter.setFont(*m_font);
    painter.drawText((image.width() - text_width)/ 2, (image.height() - text_height / 2 + 5), m_content);

    image.save(m_outputPath + QDir::separator() + m_content + ".png");
}

void WorkTask::setContent(const QString &content)
{
    m_content = content;
}

void WorkTask::setLogoImage(const QPixmap &logoImage)
{
    m_logoPixmap = logoImage;
}

void WorkTask::setOutputPath(const QString &outputPath)
{
    m_outputPath = outputPath;
}

void WorkTask::setFm(const QSharedPointer<QFontMetrics> &fm)
{
    m_fm = fm;
}

void WorkTask::setFont(const QSharedPointer<QFont> &font)
{
    m_font = font;
}

void MainWindow::on_openInputFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("请打开输入的文本文件"), ".", tr("*.txt"));
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有提供输入文件"), QMessageBox::Ok);
        return;
    }

    fileContent.clear();

    QFile inputFile(fileName);
    if (inputFile.open(QFile::ReadOnly)) {
        QTextStream inStream(&inputFile);
        while (!inStream.atEnd()) {
            QString line;
            inStream >> line;
            if (!line.trimmed().isEmpty()) {
                fileContent << line;
            }
        }
    }

    inputFile.close();
}

void MainWindow::on_chooseLogoBtn_clicked()
{
    QString logoFIleName = QFileDialog::getOpenFileName(this, tr("请选择logo文件"), ".", tr("*.png;; *.bmp;; *.jpg"));
    if (logoFIleName.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有提供Logo文件"), QMessageBox::Ok);
        return;
    }

    logoImage.load(logoFIleName);

    logoImage = logoImage.scaledToHeight(200);
    logoImage = logoImage.scaledToWidth(200);
}

void MainWindow::on_startBtn_clicked()
{
    QString outputPath = QFileDialog::getExistingDirectory(this, tr("请选择保存输出文件的目录"), ".");
    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有提供输出目录"), QMessageBox::Ok);
        return;
    }

    startMultiThreadTask(fileContent, outputPath);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    quitApp = true;
    while (!workThreadFinish);
}
