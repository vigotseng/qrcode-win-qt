
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPixmap>
#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

class WorkTask : public QThread
{
public:
    void run();

    void setContent(const QString &content);

    void setLogoImage(const QPixmap &logoImage);

    void setOutputPath(const QString &outputPath);

    void setFm(const QSharedPointer<QFontMetrics> &fm);

    void setFont(const QSharedPointer<QFont> &font);

private:
    QString m_content;
    QPixmap m_logoPixmap;
    QString m_outputPath;
    QSharedPointer<QFont>        m_font;
    QSharedPointer<QFontMetrics> m_fm;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startMultiThreadTask(const QStringList &fileContent, QString outputPath);

private slots:
    void on_openInputFileBtn_clicked();

    void on_chooseLogoBtn_clicked();

    void on_startBtn_clicked();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QPixmap  logoImage;
    QStringList fileContent;

    volatile bool        quitApp;
    volatile bool        workThreadFinish;

    QSharedPointer<QFont>        m_font;
    QSharedPointer<QFontMetrics> m_fm;
};

#endif // MAINWINDOW_H
