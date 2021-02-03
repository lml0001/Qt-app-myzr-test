#include "mainwindow.h"
#include "ui_mainwindow.h"




QString get_ip(const char *ifname)
{
    int sock_get_ip;
    int s32Ret = 0;
    char ipaddr[48];
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if((sock_get_ip = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        return QString("Disconnected");
    }
    memset(&ifr, 0,sizeof(ifr));
    memcpy(ifr.ifr_name, ifname, strlen(ifname));

    if(ioctl(sock_get_ip, SIOCGIFADDR, &ifr) <0)
    {
        s32Ret = -2;
        close(sock_get_ip);
        return QString("Disconnected");
    }

    sin = (struct sockaddr_in*)(&ifr.ifr_addr);
    close(sock_get_ip);
    return QString(inet_ntoa(sin->sin_addr));

}
int gpio_value[8];
void init_gpio_switch()
{
    int fd;
    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_1 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_1));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_1, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[1 - 1] = open(SYSFS_GPIO_RST_VAL_1, O_RDWR);

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_2 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_2));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_2, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[2 - 1] = open(SYSFS_GPIO_RST_VAL_2, O_RDWR);

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_3 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_3));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_3, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[3 - 1] = open(SYSFS_GPIO_RST_VAL_3, O_RDWR);

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_4 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_4));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_4, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[4 - 1] = open(SYSFS_GPIO_RST_VAL_4, O_RDWR);

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_5 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_5));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_5, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_IN, sizeof(SYSFS_GPIO_RST_DIR_VAL_IN));
    close(fd);
    gpio_value[5 - 1] = open(SYSFS_GPIO_RST_VAL_5, O_RDONLY);
    fd = open(SYSFS_GPIO_RST_EDGE_5,O_WRONLY);
    write(fd, "rising", strlen("rising"));
    close(fd);
    std::thread interrupt_loop_dio1([]{
    struct pollfd fds[1];
    fds[0].fd = gpio_value[5 - 1];
    fds[0].events  = POLLPRI;
    while(1)
    {
        int ret;
        char buff[256];
        ret = poll(fds,1,-1);
        std::cout<<"poll(fds,1,-1)"<<std::endl;
        if( ret == -1 )
        {

        }
        if( fds[0].revents & POLLPRI)
        {
            ret = lseek(gpio_value[5 - 1],0,SEEK_SET);
            if( ret == -1 )
            {

            }
            std::cout<<"#########gpio5_value########"<<std::endl;
            do
            {
                ret = read(gpio_value[5 - 1],buff,256);
                std::cout<<"read ret: "<<ret<<std::endl;
            }while(ret != 0);
            if( ret == -1 )
            {

            }
            std::cout<<"read(gpio_value[5 - 1],buff,1)"<<std::endl;
        }
    }
    });
    interrupt_loop_dio1.detach();

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_6 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_6));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_6, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_IN, sizeof(SYSFS_GPIO_RST_DIR_VAL_IN));
    close(fd);
    gpio_value[6 - 1] = open(SYSFS_GPIO_RST_VAL_6, O_RDONLY);
    fd = open(SYSFS_GPIO_RST_EDGE_6,O_WRONLY);
    write(fd, "rising", strlen("rising"));
    close(fd);
    std::thread interrupt_loop_dio2([]{
    struct pollfd fds[1];
    fds[0].fd = gpio_value[6 - 1];
    fds[0].events  = POLLPRI;
    while(1)
    {
        int ret;
        char buff[256];
        ret = poll(fds,1,-1);
        std::cout<<"poll(fds,1,-1)"<<std::endl;
        if( ret == -1 )
        {

        }
        if( fds[0].revents & POLLPRI)
        {
            ret = lseek(gpio_value[6 - 1],0,SEEK_SET);
            if( ret == -1 )
            {

            }
            std::cout<<"#########gpio6_value########"<<std::endl;
            do
            {
                ret = read(gpio_value[6 - 1],buff,256);
                std::cout<<"read ret: "<<ret<<std::endl;
            }while(ret != 0);
            if( ret == -1 )
            {

            }
            std::cout<<"read(gpio_value[6 - 1],buff,1)"<<std::endl;
        }
    }
    });
    interrupt_loop_dio2.detach();

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_7 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_7));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_7, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[7 - 1] = open(SYSFS_GPIO_RST_VAL_7, O_RDWR);

    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_PIN_VAL_8 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_8));
    close(fd);
    fd = open(SYSFS_GPIO_RST_DIR_8, O_WRONLY);
    write(fd, SYSFS_GPIO_RST_DIR_VAL_OUT, sizeof(SYSFS_GPIO_RST_DIR_VAL_OUT));
    close(fd);
    gpio_value[8 - 1] = open(SYSFS_GPIO_RST_VAL_8, O_RDWR);
    write(gpio_value[7 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    write(gpio_value[7 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    write(gpio_value[7 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
    write(gpio_value[8 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    write(gpio_value[8 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    write(gpio_value[8 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));

}
int wdgflag = 0;

MainWindow::MainWindow(QWidget *parent,QApplication *a) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    app = a;
    watchdogthread * wdg = new watchdogthread(nullptr,app);
    m_nTimerID0 = this->startTimer(5000);
    ui->setupUi(this);
    qDebug() << QSqlDatabase::drivers();//打印qt支持的数据库类型
    init_gpio_switch();
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("rm-2vcyj9v8ozxj7b2m4to.mysql.cn-chengdu.rds.aliyuncs.com");
    db.setDatabaseName("ejcdb");
    db.setUserName("lumenle");
    db.setPassword("As02df234");
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    ui->dbconnlabel->setStyleSheet("background-color: rgb(250, 0, 0);font-size:35px;color:blue");
    if(!db.open())
    {
        ui->dbconnlabel->setText("数据库打开失败");
    }
    else
    {
        ui->dbconnlabel->setText("数据库打开成功");
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nTimerID0){
        wdgflag = 0;
        ui->label_ipaddr->setStyleSheet("background-color: rgb(250, 0, 0);font-size:50px;color:blue");
        QString ip = get_ip("eth0");
        ui->label_ipaddr->setText(ip);
        QString HubName[100];//用来存储从数据库中找出来的信息
        QString ChannelStatus[100];
        double Temperature[100];
        int i = 0;
        QSqlQuery query;
        query.exec("select * from hubtable");
        while(query.next())
        {
            HubName[i] = query.value("HubName").toString();
            ChannelStatus[i] = query.value("ChannelStatus").toString();
            Temperature[i] = query.value("Temperature").toDouble();
            i++;
        }
        unsigned short chnstatus[2];
        chnstatus[0] = ChannelStatus[0].toUShort();
        if(chnstatus[0] != chnstatus[1])
        {
            char chnmask = 0x01;
            chnstatus[1] = chnstatus[0];
            if(chnstatus[1]&chnmask<<(1 - 1))
            {
                ui->label_1->setStyleSheet("QLabel{background-color:rgb(0,255,0);}");
                write(gpio_value[1 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
            }
            else
            {
                ui->label_1->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
                write(gpio_value[1 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
            }
            if(chnstatus[1]&chnmask<<(2 - 1))
            {
                ui->label_2->setStyleSheet("QLabel{background-color:rgb(0,255,0);}");
                write(gpio_value[2 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
            }
            else
            {
                ui->label_2->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
                write(gpio_value[2 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
            }
            if(chnstatus[1]&chnmask<<(3 - 1))
            {
                ui->label_3->setStyleSheet("QLabel{background-color:rgb(0,255,0);}");
                write(gpio_value[3 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
            }
            else
            {
                ui->label_3->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
                write(gpio_value[3 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
            }
            if(chnstatus[1]&chnmask<<(4 - 1))
            {
                ui->label_4->setStyleSheet("QLabel{background-color:rgb(0,255,0);}");
                write(gpio_value[4 - 1], SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
            }
            else
            {
                ui->label_4->setStyleSheet("QLabel{background-color:rgb(255,0,0);}");
                write(gpio_value[4 - 1], SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
            }

        }

        ui->textEdit->clear();
        int j = 0;
        for(j = 0; j < i; j++)//将这些信息都显示在下方的文本编辑框中
        {
            QString str = QString("HubName: %1   ChannelStatus: %2  Temperature: %3").arg(HubName[j]).arg(ChannelStatus[j]).arg(Temperature[j]);
            ui->textEdit->append(str);
        }
        query.prepare("update hubtable SET `information` = ? WHERE `HubName` = 'IMX6ULL-001'");
        query.addBindValue(ip);
        query.exec();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString temperature_str;
    temperature_str = ui->textEdit_2->toPlainText();
    QSqlQuery query;
    query.prepare("update hubtable SET `Temperature` = ? WHERE `HubName` = 'IMX6ULL-001'");
    query.addBindValue(temperature_str);
    query.exec();
}
