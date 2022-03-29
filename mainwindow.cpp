#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Serial/WzSerialPort.h"
#include "ThreadWorkers/WzReadSerialThread.h"
#include "ThreadWorkers/WzReadCANThread.h"
#include "QMessageBox"
#include "QTextBlock"
#include "qprocess.h"
#include "myProcess/WzProcess.h"
#include <QFileDialog>
#include "touch/formtouch.h"

QString MainWindow::m_result;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    pFormTouch = new FormTouch();
    pFormTouch->hide();
    //serial init
    pSerialPort0 = new WzSerialPort();
    pSerialPort1 = new WzSerialPort();

    start_read_thread();
    start_read_thread2();

    m_autoSendTimer[0] = new QTimer();
    m_autoSendTimer[1] = new QTimer();
    m_readIOTimer = new QTimer();
    connect(m_autoSendTimer[0], SIGNAL(timeout()), this, SLOT(send_timer_slot()));
    connect(m_autoSendTimer[1], SIGNAL(timeout()), this, SLOT(send_timer_slot2()));
    connect(m_readIOTimer, SIGNAL(timeout()), this, SLOT(slot_update_IO_info()));
    //================ can ======================//
    pCanPort0 = new WzCan();
    pCanPort1 = new WzCan();

    start_can_read_thread0();
    start_can_read_thread2();

    m_canSendTimer[0] = new QTimer();
    m_canSendTimer[1] = new QTimer();
    connect(m_canSendTimer[0], SIGNAL(timeout()), this, SLOT(slot_can_send_timeout()));
    connect(m_canSendTimer[1], SIGNAL(timeout()), this, SLOT(slot_can_send_timeout2()));

    m_readIOTimer->start(1000);

    set_buttong_size_fixed();
}

MainWindow::~MainWindow()
{
    delete ui;

    m_pworker->readFlag = 0;
    m_pworker2->readFlag = 0;

    m_pthread->quit();
    m_pthread->destroyed();
    m_pthread->wait();

    m_pthread2->quit();
    m_pthread2->destroyed();
    m_pthread2->wait();

    //==========================//
    m_pCANWorker->readFlag = 0;
    m_pCANWorker2->readFlag = 0;

    m_canThread->quit();
    m_canThread->destroyed();
    m_canThread->wait();

    m_canThread2->quit();
    m_canThread2->destroyed();
    m_canThread2->wait();

    delete m_pworker;
    delete m_pthread;
    delete m_pworker2;
    delete m_pthread2;
    delete pFormTouch;
}

void MainWindow::start_read_thread()
{

    m_pworker = new WzReadSerialThread();
    m_pthread = new QThread();
    m_pworker->moveToThread(m_pthread);
    //qDebug()<< "start,thread id = " << QThread::currentThreadId();

    connect(m_pthread, &QThread::finished, m_pworker, &QObject::deleteLater);
    connect(this, SIGNAL(sig_dowork(WzSerialPort *)), m_pworker, SLOT(slot_dowork(WzSerialPort *)));
    connect(m_pworker,SIGNAL(sig_finish()),this,SLOT(slot_finish()));
    connect(m_pworker, SIGNAL(sig_recv_data(char*)), this, SLOT(slot_update_serial_recv(char*)));

     m_pthread->start();
}

void MainWindow::start_read_thread2()
{
    m_pworker2 = new WzReadSerialThread();
    m_pthread2 = new QThread();
    m_pworker2->moveToThread(m_pthread2);
    //qDebug()<< "start,thread id = " << QThread::currentThreadId();

    connect(m_pthread2, &QThread::finished, m_pworker2, &QObject::deleteLater);
    connect(this, SIGNAL(sig_dowork2(WzSerialPort*)),m_pworker2, SLOT(slot_dowork(WzSerialPort*)));
    connect(m_pworker2, SIGNAL(sig_finish()), this, SLOT(slot_finish2()));
    connect(m_pworker2, SIGNAL(sig_recv_data(char*)), this, SLOT(slot_update_serial_recv2(char*)));
    m_pthread2->start();
}

void MainWindow::start_can_read_thread0()
{
    m_pCANWorker = new WzReadCANThread();
    m_canThread = new QThread();
    m_pCANWorker->moveToThread(m_canThread);
    //qDebug()<< "start,thread id = " << QThread::currentThreadId();

    connect(m_canThread, &QThread::finished, m_pCANWorker, &QObject::deleteLater);
    connect(this, SIGNAL(sig_can_dowork(WzCan*)), m_pCANWorker, SLOT(slot_dowork(WzCan*)));
    connect(m_pCANWorker, SIGNAL(sig_recv_data(char*, int)), this, SLOT(slot_update_can_recv(char*, int)));
    connect(m_pCANWorker,SIGNAL(sig_finish()),this,SLOT(slot_can_finish()));
    m_canThread->start();
}

void MainWindow::start_can_read_thread2()
{
    m_pCANWorker2 = new WzReadCANThread();
    m_canThread2 = new QThread();
    m_pCANWorker2->moveToThread(m_canThread2);
    //qDebug()<< "start,thread id = " << QThread::currentThreadId();

    connect(m_canThread2, &QThread::finished, m_pCANWorker2, &QObject::deleteLater);
    connect(this,SIGNAL(sig_can_dowork2(WzCan*)),m_pCANWorker2, SLOT(slot_dowork(WzCan*)));
    connect(m_pCANWorker2, SIGNAL(sig_recv_data(char*, int)), this, SLOT(slot_update_can_recv2(char*, int)));
    connect(m_pCANWorker2,SIGNAL(sig_finish()),this,SLOT(slot_can_finish()));
    m_canThread2->start();
}

void MainWindow::set_buttong_size_fixed()
{
    ui->pushButton_open_0->setFixedWidth(ui->pushButton_open_0->width());
    ui->pushButton_open_1->setFixedWidth(ui->pushButton_open_1->width());

    ui->pushButton_open_can->setFixedWidth(ui->pushButton_open_can->width());
    ui->pushButton_open_can_2->setFixedWidth(ui->pushButton_open_can_2->width());
}

unsigned char *MainWindow::string_to_hex(QString str)
{
    unsigned char tmpA;
    QString tmpStr = str.remove(QRegExp("\\s"));
    qDebug() << "new str: " << tmpStr;
    for(int i=0; i<tmpStr.size(); i+=4)
    {
        tmpA = tmpStr.mid(i+2,2).toUInt(Q_NULLPTR, 16);
        tmpA &= 0xff;
        qDebug("%02x", tmpA);
        m_CharBuf[i/4] = tmpA;
    }
    return m_CharBuf;
}


void MainWindow::on_pushButton_open_0_clicked()
{
    char portname[48];
    int baudrate;
    char parity;
    char databit;

    QString str;

    if(true == pSerialPort0->isOpen()){
        pSerialPort0->close();
        m_autoSendTimer[0]->stop();
        ui->pushButton_open_0->setText("com0 is closed");
        return;
    }


    parity = ui->comboBox_parity_0->currentText().constData()->toLatin1();
    if(parity == 'N'){
        parity = 0;
    }else if(parity == 'O'){
        parity = 1;
    }else if(parity == 'E'){
        parity = 2;
    }else{
        parity = 0;
    }

    ::memset(portname, 0, sizeof(portname));
    if( ui->comboBox_portname_0->currentText().contains("com0") ){
        strcpy(portname, "/dev/ttyAMA0");
    }else if(ui->comboBox_portname_0->currentText().contains("com1")){
        strcpy(portname, "/dev/ttyAMA1");
    }else if(ui->comboBox_portname_0->currentText().contains("com2")){
        strcpy(portname, "/dev/ttyAMA2");
    }else if(ui->comboBox_portname_0->currentText().contains("com3")){
        strcpy(portname, "/dev/ttyAMA3");
    }else if(ui->comboBox_portname_0->currentText().contains("com4")){
        strcpy(portname, "ttyAMA4");
    }else if(ui->comboBox_portname_0->currentText().contains("com5")){
        strcpy(portname, "ttyAMA5");
    }
    baudrate = ui->comboBox_baud_0->currentText().toInt();
    databit = ui->comboBox_databit_0->currentText().toInt();

    qDebug("pname:%s, baud:%d, parity:%2xH,databit:%02xH.\n", portname, baudrate, parity, databit);//add for test
    if(true == pSerialPort0->open(portname, baudrate, parity, databit, 1, 0))
    {
        ui->pushButton_open_0->setText("com0 is openning");
        m_pworker->readFlag = 1;//add for test
        //str.clear();
        //str.append("hello thread1.");
        emit sig_dowork(pSerialPort0);
    }else{
        QMessageBox::information(NULL, "Error", "Fuck com connot be open!", QMessageBox::Yes);
        return ;
    }

}

void MainWindow::on_pushButton_open_1_clicked()
{
    char  portname[48];
    int baudrate;
    char parity;
    char databit;

    QString str;

    if(true == pSerialPort1->isOpen()){
        pSerialPort1->close();
        m_autoSendTimer[1]->stop();
        ui->pushButton_open_1->setText("com1 is closed");
        return;
    }

    parity = ui->comboBox_parity_1->currentText().constData()->toLatin1();
    if(parity == 'N'){
        parity = 0;
    }else if(parity == 'O'){
        parity = 1;
    }else if(parity == 'E'){
        parity = 2;
    }else{
        parity = 0;
    }

    ::memset(portname, 0, sizeof(portname));
    if( ui->comboBox_portname_2->currentText().contains("com0") ){
        strcpy(portname, "/dev/ttyAMA0");
    }else if(ui->comboBox_portname_2->currentText().contains("com1")){
        strcpy(portname, "/dev/ttyAMA1");
    }else if(ui->comboBox_portname_2->currentText().contains("com2")){
        strcpy(portname, "/dev/ttyAMA2");
    }else if(ui->comboBox_portname_2->currentText().contains("com3")){
        strcpy(portname, "/dev/ttyAMA3");
    }else if(ui->comboBox_portname_2->currentText().contains("com4")){
        strcpy(portname, "ttyAMA4");
    }else if(ui->comboBox_portname_2->currentText().contains("com5")){
        strcpy(portname, "ttyAMA5");
    }

    baudrate = ui->comboBox_baud_1->currentText().toInt();
    databit = ui->comboBox_databit_1->currentText().toInt();

    qDebug("pname:%s, baud:%d, parity:%2xH,databit:%02xH.\n", portname, baudrate, parity, databit);//add for test
    if(true == pSerialPort1->open(portname, baudrate, parity, databit, 1, 0))
    {
        ui->pushButton_open_1->setText("com1 is openning");
        m_pworker2->readFlag = 1;
        //str.clear();
        //str.append("hello thread2.");
        emit sig_dowork2(pSerialPort1);
    }else{
        QMessageBox::information(NULL, "Error", "Error com connot be open!", QMessageBox::Yes);
    }
}

void MainWindow::on_pushButton_send_0_clicked()
{
    QTextDocument *document=Q_NULLPTR;
    QTextBlock textBlock;
    document=ui->plainTextEdit_send_0->document();
    for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
        pSerialPort0->send( textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length() );
    }
}

void MainWindow::on_pushButton_send_1_clicked()
{
    QTextDocument *document=Q_NULLPTR;
    QTextBlock textBlock;
    document=ui->plainTextEdit_send_1->document();
    for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
        pSerialPort1->send( textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length() );
    }
}

void MainWindow::slot_finish()
{
    ;
}

void MainWindow::slot_finish2()
{
    ;
}

void MainWindow::send_timer_slot()
{
    qDebug() << "Timer0 out" ;//add for test
    if(ui->checkBox_0->isChecked()){
        if(true == pSerialPort0->isOpen()){
            QTextDocument *document=Q_NULLPTR;
            QTextBlock textBlock;
            document=ui->plainTextEdit_send_0->document();
            for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
                pSerialPort0->send( textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length() );
            }
        }
    }else{
        m_autoSendTimer[0]->stop();
    }
}


void MainWindow::send_timer_slot2()
{
    qDebug() << "Timer1 out" ;//add for test
    if(ui->checkBox_1->isChecked()){
        if(true == pSerialPort1->isOpen()){
            QTextDocument *document=Q_NULLPTR;
            QTextBlock textBlock;
            document=ui->plainTextEdit_send_1->document();
            for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
                pSerialPort1->send( textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length() );
            }
        }
    }else{
        m_autoSendTimer[1]->stop();
    }
}

void MainWindow::slot_update_serial_recv(char *p)
{
    QString tmpStr(p);
    ui->textEdit_receive_0->append(tmpStr);
    qDebug()<<"get serial1 data in thread";
}

void MainWindow::slot_update_serial_recv2(char *p)
{

    QString tmpStr(p);
    ui->textEdit_receive_1->append(tmpStr);
    qDebug()<<"get serial2 data in thread";
}


void MainWindow::slot_can_finish()
{

}

void MainWindow::slot_can_finish2()
{

}

void MainWindow::slot_can_send_timeout()
{
    qDebug() << "Timer1 out" ;//add for test
    if(ui->checkBox_can0->isChecked()){
        if(true == pCanPort0->isOpen()){
            on_pushButton_send_clicked();
        }
    }else{
        m_canSendTimer[0]->stop();
    }
}

void MainWindow::slot_can_send_timeout2()
{
    qDebug() << "Timer1 out" ;//add for test
    if(ui->checkBox_can1->isChecked()){
        if(true == pCanPort1->isOpen()){
            on_pushButton_send_2_clicked();
        }
    }else{
        m_canSendTimer[1]->stop();
    }
}

void MainWindow::slot_update_can_recv(char *p, int len)
{
    if(len<10){
        p[len] = 0;

        ui->textEdit_can0->append(p);
    }
    //qDebug()<<"get can111 data in thread";
}

void MainWindow::slot_update_can_recv2(char *p, int len)
{
    if(len<10){
        p[len] = 0;

        ui->textEdit_can1->append(p);
    }
    //qDebug()<<"get can222 data in thread";
}

void MainWindow::slot_update_IO_info()
{
    m_Process.start("/bin/sh -c \"echo 71 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();

    m_Process.start("/bin/sh -c \"echo 91 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();


    m_Process.start("/bin/sh -c \"echo 93 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();


    m_Process.start("/bin/sh -c \"echo in > /sys/class/gpio/gpio71/direction\"");
    m_Process.waitForFinished();


    m_Process.start("/bin/sh -c \"echo in > /sys/class/gpio/gpio91/direction\"");
    m_Process.waitForFinished();


    m_Process.start("/bin/sh -c \"echo in > /sys/class/gpio/gpio93/direction\"");
    m_Process.waitForFinished();

    m_Process.start("cat /sys/class/gpio/gpio71/value");
    m_Process.waitForFinished();
    m_Process.waitForReadyRead();

    ui->lineEdit_IO0->setText(m_Process.readAllStandardOutput().toStdString().data());

    m_Process.start("cat /sys/class/gpio/gpio91/value");
    m_Process.waitForFinished();
    m_Process.waitForReadyRead();
    ui->lineEdit_IO1->setText(m_Process.readAllStandardOutput().toStdString().data());

    m_Process.start("cat /sys/class/gpio/gpio93/value");
    m_Process.waitForFinished();
    m_Process.waitForReadyRead();
    ui->lineEdit_IO2->setText(m_Process.readAllStandardOutput().toStdString().data());



    qDebug()<<"IO read...";
}

void MainWindow::slot_update_usb(QByteArray res)
{
    ui->plainTextEdit_usb->appendPlainText(res.toStdString().data());
}

void MainWindow::slot_update_4g(QByteArray res)
{
    ui->plainTextEdit_4G->appendPlainText(res.toStdString().data());
}

void MainWindow::on_checkBox_0_clicked()
{
    int tmpLen;
    QString tmpStr;
    if(!pSerialPort0->isOpen()){
        return;
    }
    tmpLen = ui->comboBox_interv->currentText().length();
    tmpStr = ui->comboBox_interv->currentText().left(tmpLen-2);
    qDebug() << "get interval2: " << tmpStr.toInt();

    if(ui->checkBox_0->isChecked()){
        m_autoSendTimer[0]->start(tmpStr.toInt());
    }else{
        m_autoSendTimer[0]->stop();
    }
}


void MainWindow::on_checkBox_1_clicked()
{
    int tmpLen;
    QString tmpStr;
    if(!pSerialPort1->isOpen()){
        return;
    }
    tmpLen = ui->comboBox_interv2->currentText().length();
    tmpStr = ui->comboBox_interv2->currentText().left(tmpLen-2);
    qDebug() << "get interval2: " << tmpStr.toInt();
    if(ui->checkBox_1->isChecked()){
        m_autoSendTimer[1]->start(tmpStr.toInt());
    }else{
        m_autoSendTimer[1]->stop();
    }
}

void MainWindow::on_pushButton_open_can_clicked()
{
    char portname[48];
    int baudrate;
    int tmpLength;
    QString cmdStr;
    QString str;

    cmdStr.clear();
    memset(portname, 0, sizeof(portname));

    ::strcpy(portname, ui->comboBox_can0->currentText().toLocal8Bit().constData());

    tmpLength = ui->comboBox_can_baud0->currentText().toLocal8Bit().length()-1;
    baudrate = ui->comboBox_can_baud0->currentText().left(tmpLength).toInt()*1000;

    cmdStr.sprintf("ip link set %s type can bitrate %d", portname, baudrate);
    qDebug("portName: %s, baud:%d, cmd: %s\n", portname, baudrate, qPrintable(cmdStr));
    m_Process.start(cmdStr);
    m_Process.waitForFinished(2000);
    m_Process.waitForReadyRead();
    QByteArray resultArr  = m_Process.readAllStandardError();
    m_Process.kill();
    QString resultStr(resultArr);
    qDebug() << resultStr;
    if(resultStr.contains("Cannot find")){
        return ;
    }

    cmdStr.sprintf("ip link set %s up", portname);
    m_Process.start(cmdStr);
    m_Process.waitForFinished(2000);
    m_Process.waitForReadyRead();

    if(pCanPort0->isOpen() == false){
        pCanPort0->open(portname, baudrate, 0);
        m_pCANWorker->readFlag = 1;//add for test
        str.clear();
        str.append("hello can thread1.");
        emit sig_can_dowork(pCanPort0);
        ui->pushButton_open_can->setText("CAN open");
    }else{
        ui->pushButton_open_can->setText("CAN closed");
        m_canSendTimer[0]->stop();
    }

}

void MainWindow::on_pushButton_send_clicked()
{
    QTextBlock textBlock;
    QTextDocument *document=Q_NULLPTR;
    unsigned char num;
    if(pCanPort0->isOpen() == false){
        return;
    }
    document=ui->plainTextEdit_send_can0->document();
    for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
        qDebug("Can0 ID:%d, buf:%s, len:%d\n", ui->lineEdit_canID0->text().toInt(),  textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length());
        //字符串转换为hex
        string_to_hex(textBlock.text());
        //for(int i=0; i<8; i++){
        //    qDebug("0x%02x ", m_CharBuf[i]);
        //}
        pCanPort0->send(ui->lineEdit_canID0->text().toInt(),  m_CharBuf, 8);
    }
}

void MainWindow::on_pushButton_open_can_2_clicked()
{
    char portname[48];
    int baudrate;
    int tmpLength;
    QString cmdStr;
    QString str;

    cmdStr.clear();
    memset(portname, 0, sizeof(portname));

    ::strcpy(portname, ui->comboBox_can1->currentText().toLocal8Bit().constData());

    tmpLength = ui->comboBox_can_baud1->currentText().toLocal8Bit().length()-1;
    baudrate = ui->comboBox_can_baud1->currentText().left(tmpLength).toInt()*1000;

    cmdStr.sprintf("ip link set %s type can bitrate %d", portname, baudrate);
    qDebug("portName: %s, baud:%d, cmd: %s\n", portname, baudrate, qPrintable(cmdStr));
    m_Process.start(cmdStr);
    m_Process.waitForFinished(2000);
    m_Process.waitForReadyRead();
    QByteArray resultArr  = m_Process.readAllStandardError();
    m_Process.kill();
    QString resultStr(resultArr);
    qDebug() << resultStr;
    if(resultStr.contains("Cannot find")){
        return ;
    }

    cmdStr.sprintf("ip link set %s up", portname);
    m_Process.start(cmdStr);
    m_Process.waitForFinished(2000);
    m_Process.waitForReadyRead();

    if(pCanPort1->isOpen() == false){
        pCanPort1->open(portname, baudrate, 0);
        m_pCANWorker2->readFlag = 1;//add for test
        str.clear();
        str.append("hello can thread1.");
        emit sig_can_dowork(pCanPort1);
        ui->pushButton_open_can_2->setText("CAN open");
    }else{
        ui->pushButton_open_can_2->setText("CAN closed");
        m_canSendTimer[1]->stop();
    }

}

void MainWindow::on_pushButton_send_2_clicked()
{
    QTextBlock textBlock;
    QTextDocument *document=Q_NULLPTR;
    unsigned char num;
    if(pCanPort1->isOpen() == false){
        return;
    }
    document=ui->plainTextEdit_send_can1->document();
    for(textBlock=document->begin(); textBlock!=document->end(); textBlock=textBlock.next()){
        qDebug("Can0 ID:%d, buf:%s, len:%d\n", ui->lineEdit_canID1->text().toInt(),  textBlock.text().toLocal8Bit().constData(), textBlock.text().toLocal8Bit().length());
        //字符串转换为hex
        string_to_hex(textBlock.text());
        //for(int i=0; i<8; i++){
        //    qDebug("0x%02x ", m_CharBuf[i]);
        //}
        pCanPort1->send(ui->lineEdit_canID1->text().toInt(),  m_CharBuf, 8);
    }
}

void MainWindow::on_checkBox_can0_clicked()
{
    int tmpLen;
    QString tmpStr;
    if(false == pCanPort0->isOpen()){
        return;
    }
    tmpLen = ui->comboBox_inter_can0->currentText().length();
    tmpStr = ui->comboBox_inter_can0->currentText().left(tmpLen-2);
    qDebug() << "get can0 interval0: " << tmpStr.toInt();
    if(ui->checkBox_can0->isChecked()){
        m_canSendTimer[0]->start(tmpStr.toInt());
    }else{
        m_canSendTimer[0]->stop();
    }
}

void MainWindow::on_checkBox_can1_clicked()
{
    int tmpLen;
    QString tmpStr;
    if(!pCanPort1->isOpen()){
        return;
    }

    tmpLen = ui->comboBox_inter_can1->currentText().length();
    tmpStr = ui->comboBox_inter_can1->currentText().left(tmpLen-2);
    qDebug() << "get can1 interval1: " << tmpStr.toInt();
    if(ui->checkBox_can1->isChecked()){
        m_canSendTimer[1]->start(tmpStr.toInt());
    }else{
        m_canSendTimer[1]->stop();
    }
}

void MainWindow::on_pushButton_bell_clicked()
{
    QString cmdStr;

    cmdStr.clear();
    cmdStr.append("/bin/sh -c \"echo 123 > /sys/class/gpio/export\"");
    m_Process.start(cmdStr);
    m_Process.waitForFinished();

    cmdStr.clear();
    cmdStr.append("/bin/sh -c \"echo out > /sys/class/gpio/gpio123/direction\"");
    qDebug("cmd: %s\n", qPrintable(cmdStr));
    m_Process.start(cmdStr);
    m_Process.waitForFinished();

    cmdStr.clear();
    cmdStr.append("/bin/sh -c \"echo 1 > /sys/class/gpio/gpio123/value\"");
    qDebug("cmd: %s\n", qPrintable(cmdStr));
    m_Process.start(cmdStr);
    m_Process.waitForFinished();

    QThread::sleep(1);

    cmdStr.clear();
    cmdStr.append("/bin/sh -c \"echo 0 > /sys/class/gpio/gpio123/value\"");
    qDebug("cmd: %s\n", qPrintable(cmdStr));
    m_Process.start(cmdStr);
    m_Process.waitForFinished();

}

void MainWindow::on_pushButton_4G_clicked()
{
    QString cmdStr;
    cmdStr.clear();

    cmdStr.append("ls -lt /dev/ttyUSB0");
    qDebug("qDebug: %s", qPrintable(cmdStr));
    WzProcess *porcess = new WzProcess();
    porcess->m_process.start(cmdStr);
    connect(porcess, SIGNAL(sig_process(QByteArray)), this, SLOT(slot_update_4g(QByteArray)));
    delete porcess;
}

void MainWindow::on_pushButton_USB_clicked()
{
    QString cmdStr;
    cmdStr.clear();

    cmdStr.append("ls -lt /proc/scsi/");
    qDebug("qDebug: %s", qPrintable(cmdStr));
    WzProcess *porcess = new WzProcess();
    porcess->m_process.start(cmdStr);
    connect(porcess, SIGNAL(sig_process(QByteArray)), this, SLOT(slot_update_usb(QByteArray)));
    delete porcess;
}

void MainWindow::on_pushButton_ethOK_clicked()
{
    QString cmdStr;
    cmdStr.clear();

    cmdStr.sprintf("ifconfig eth0 %s up", ui->lineEdit_eth_IP->text().toLocal8Bit().constData());
    WzProcess *porcess = new WzProcess();
    porcess->m_process.start("ifconfig eth0 down");
    porcess->m_process.waitForFinished();
    porcess->m_process.start(cmdStr);
    qDebug() << "cmd ip string: " << cmdStr;
    porcess->m_process.waitForFinished();
    delete porcess;

}

void MainWindow::on_pushButton_ethOK_2_clicked()
{
    QString cmdStr;
    cmdStr.clear();

    cmdStr.clear();
    cmdStr.sprintf("ifconfig eth1 %s up", ui->lineEdit_eth_IP_2->text().toLocal8Bit().constData());
    WzProcess *porcess2 = new WzProcess();
    porcess2->m_process.start("ifconfig eth1 down");
    porcess2->m_process.waitForFinished();
    porcess2->m_process.start(cmdStr);
    qDebug() << "cmd IP 2 string: " << cmdStr;
    porcess2->m_process.waitForFinished();
    delete porcess2;
}

void MainWindow::on_pushButton_IoSet_0_clicked()
{
    m_Process.start("/bin/sh -c \"echo 96 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();

    m_Process.start("/bin/sh -c \"echo out > /sys/class/gpio/gpio96/direction\"");
    m_Process.waitForFinished();

    if(ui->comboBox_IO0->currentIndex() == 0){
        m_Process.start("/bin/sh -c \"echo 1 > /sys/class/gpio/gpio96/value\"");
        m_Process.waitForFinished();
    }else{
        m_Process.start("/bin/sh -c \"echo 0 > /sys/class/gpio/gpio96/value\"");
        m_Process.waitForFinished();
    }

}

void MainWindow::on_pushButton_IoSet_1_clicked()
{
    m_Process.start("/bin/sh -c \"echo 95 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();

    m_Process.start("/bin/sh -c \"echo out > /sys/class/gpio/gpio95/direction\"");
    m_Process.waitForFinished();

    if(ui->comboBox_IO1->currentIndex() == 0){
        m_Process.start("/bin/sh -c \"echo 1 > /sys/class/gpio/gpio95/value\"");
        m_Process.waitForFinished();
    }else{
        m_Process.start("/bin/sh -c \"echo 0 > /sys/class/gpio/gpio95/value\"");
        m_Process.waitForFinished();
    }
}

void MainWindow::on_pushButton_IoSet_2_clicked()
{
    m_Process.start("/bin/sh -c \"echo 94 > /sys/class/gpio/export\"");
    m_Process.waitForFinished();

    m_Process.start("/bin/sh -c \"echo out > /sys/class/gpio/gpio94/direction\"");
    m_Process.waitForFinished();

    if(ui->comboBox_IO2->currentIndex() == 0){
        m_Process.start("/bin/sh -c \"echo 1 > /sys/class/gpio/gpio94/value\"");
        m_Process.waitForFinished();
    }else{
        m_Process.start("/bin/sh -c \"echo 0 > /sys/class/gpio/gpio94/value\"");
        m_Process.waitForFinished();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString cmdStr;
    QString file_name = QFileDialog::getOpenFileName(NULL, "Open sound file",".","*.wav");
    qDebug() << file_name;
    cmdStr.clear();
    cmdStr.append("aplay ");
    cmdStr.append(file_name);
    m_Process.start(cmdStr);
    qDebug() << cmdStr;
    //m_Process.waitForFinished();
}

void MainWindow::on_pushButton_touch_start_clicked()
{
    pFormTouch->showMaximized();
}
