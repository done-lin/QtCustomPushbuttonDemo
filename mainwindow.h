#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Serial/WzSerialPort.h"
#include "can/WzCan.h"
#include "ThreadWorkers/WzReadSerialThread.h"
#include "ThreadWorkers/WzReadCANThread.h"
#include "QThread"
#include <QTimer>
#include <QProcess>
#include "touch/formtouch.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QProcess m_Process;
    WzSerialPort *pSerialPort0;
    WzSerialPort *pSerialPort1;

    FormTouch *pFormTouch;

    WzCan *pCanPort0;
    WzCan *pCanPort1;

    void start_read_thread(void);
    void start_read_thread2(void);

    //++++++++++ serial ++++++++++++++++//
    WzReadSerialThread *m_pworker;
    QThread *m_pthread;

    WzReadSerialThread *m_pworker2;
    QThread *m_pthread2;

    QTimer *m_autoSendTimer[2];
    QTimer *m_readIOTimer;

    //+++++++++++++CAN++++++++++++++//
    WzReadCANThread *m_pCANWorker;
    QThread *m_canThread;

    WzReadCANThread *m_pCANWorker2;
    QThread *m_canThread2;

    void start_can_read_thread0(void);
    void start_can_read_thread2(void);

    QTimer *m_canSendTimer[2];
    static QString m_result;
    unsigned char m_CharBuf[32];
    //+++++++++++++++++++++++++++++++++++//

    void set_buttong_size_fixed(void);
    unsigned char *string_to_hex(QString str);
signals:
    void sig_dowork(WzSerialPort *pObj);
    void sig_dowork2(WzSerialPort *pObj);

    void sig_can_dowork(WzCan *pObj);
    void sig_can_dowork2(WzCan *pObj);

    void sig_upate_usb(QString str);
    //void sig_upate_4g(QString str);
private slots:
    //++++++++++ serial ++++++++++++++++//
    void slot_finish();
    void slot_finish2();
    void send_timer_slot();
    void send_timer_slot2();

    void slot_update_serial_recv(char *p);
    void slot_update_serial_recv2(char *p);
    //++++++++++ serial ++++++++++++++++//


    //++++++++++ serial ++++++++++++++++//
    void slot_can_finish();
    void slot_can_finish2();
    void slot_can_send_timeout();
    void slot_can_send_timeout2();

    void slot_update_can_recv(char *p, int len);
    void slot_update_can_recv2(char *p, int len);
    //++++++++++ serial ++++++++++++++++//

    void slot_update_IO_info(void);
    void slot_update_usb(QByteArray res);
    void slot_update_4g(QByteArray res);

    void on_pushButton_open_0_clicked();

    void on_pushButton_open_1_clicked();

    void on_pushButton_send_0_clicked();

    void on_pushButton_send_1_clicked();


    void on_checkBox_0_clicked();

    void on_checkBox_1_clicked();

    void on_pushButton_open_can_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_open_can_2_clicked();

    void on_pushButton_send_2_clicked();

    void on_checkBox_can0_clicked();

    void on_checkBox_can1_clicked();

    void on_pushButton_bell_clicked();

    void on_pushButton_4G_clicked();

    void on_pushButton_USB_clicked();

    void on_pushButton_ethOK_clicked();

    void on_pushButton_ethOK_2_clicked();

    void on_pushButton_IoSet_0_clicked();

    void on_pushButton_IoSet_1_clicked();

    void on_pushButton_IoSet_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_touch_start_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
