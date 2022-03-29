#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QByteArray>

class WzProcess : public QObject
{
    Q_OBJECT
public:
    WzProcess()
    {
        connect(&m_process, 
                SIGNAL(readyReadStandardOutput()), 
                this, 
                SLOT(onReadData()));
        m_process.setReadChannel(QProcess::StandardOutput);
    }
public:
    QProcess m_process;
signals:
    void sig_process(QByteArray param);

private slots:
    void onReadData()
    {
        QByteArray tmpBa;
        tmpBa = m_process.readAllStandardOutput();
        qDebug() << tmpBa;
        emit sig_process(tmpBa);
    }
};

