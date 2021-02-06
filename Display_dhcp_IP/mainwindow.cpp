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
int spi_fd[2];
char spi_dev[2][32] = {"/dev/spidev0.0","/dev/spidev1.0"};
uint8_t mode = 0;
uint8_t bits = 8;
uint32_t speed = 500000;
uint16_t delay = 0;
int init_sx1278_spi(int &spi_fd, char * spi_dev)
{
    int ret = 0;
    spi_fd = open(spi_dev, O_RDWR);
    if (spi_fd < 0)
    {
        return -1;
    }
    /*
     * spi mode
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        return -2;
    }

    ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
    {
        return -2;
    }
    /*
     * bits per word
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
         return -2;
    }

    ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        return -2;
    }
    /*
     * max speed hz
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        return -2;
    }

    ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        return -2;
    }
    std::cout<<"spi mode: "<<mode<<std::endl;
    std::cout<<"bits per word: "<<bits<<std::endl;
    std::cout<<"max speed: "<<speed<<"Hz"<<std::endl;

}
int spi_transfer(int spi_fd,uint8_t tx[],uint8_t rx[],uint16_t len)
{
    int ret;
    struct spi_ioc_transfer tr;  /* 这种写法一定要赋初值 */
    memset(&tr,0,sizeof(struct spi_ioc_transfer));
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = len;
    tr.delay_usecs = 0;
    tr.speed_hz = 500000;
    tr.bits_per_word = 8;
    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_set_SleepMode(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x01|0x80, 0x08 /*sx1278 set sleep mode*/
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_EntryLora(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x01|0x80, 0x88 /*sx1278 entry lora*/
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_set_frequency(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x06|0x80, 0x85 /*434MHz @ 26m*/
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x07|0x80;
    tx[1] = 0x3b;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x08|0x80;
    tx[1] = 0x13;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_set_outpowerparameter(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x09|0x80,0xFF                   //20dbm
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_set_SpreadFactor(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x1d|0x80,0x68    //Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x1e|0x80;  //SFactor &  LNA gain set by the internal AGC loop
    tx[1] = 0x97;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x26|0x80;  //LowDataRateOptimize e
    tx[1] = 0x08;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x1f|0x80;  //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x20|0x80;  //RegPreambleMsb
    tx[1] = 0x00;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x21|0x80;  //RegPreambleLsb 8+4=12byte Preamble  16+4=20
    tx[1] = 0x10;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x41|0x80;  //RegDioMapping2 DIO5=00, DIO4=01
    tx[1] = 0x01;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_set_Standby(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x01|0x80,0x09     //Entry standby mode
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x4d|0x80;  //Tx for 20dBm
    tx[1] = 0x87;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x24|0x80;  //RegHopPeriod NO FHSS
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_init_lora(int spi_fd)
{
    int ret = sx1278_set_SleepMode(spi_fd);
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ret = sx1278_EntryLora(spi_fd);
    if (ret < 1)
    {
        ret = -2;
        return ret;
    }
    ret = sx1278_set_frequency(spi_fd);
    if (ret < 1)
    {
        ret = -3;
        return ret;
    }
    ret = sx1278_set_outpowerparameter(spi_fd);
    if (ret < 1)
    {
        ret = -4;
        return ret;
    }
    ret = sx1278_set_SpreadFactor(spi_fd);
    if (ret < 1)
    {
        ret = -5;
        return ret;
    }
    ret = sx1278_set_Standby(spi_fd);
    if (ret < 1)
    {
        ret = -6;
        return ret;
    }

}
int sx1278_clear_irq(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x12|0x80,0xff //LoRaClearIrq
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_LoRaEntryRx(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x40|0x80,0x01, //DIO0=00, DIO1=00, DIO2=00, DIO3=01  DIO0=00--RXDONE
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x11|0x80; //Open RxDone interrupt & Timeout
    tx[1] = 0x3f;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x12|0x80; //LoRaClearIrq
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x0f&0x7f; //Read RxBaseAddr
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    uint8_t rxaddr = rx[1];
    tx[0] = 0x0d|0x80; //RxBaseAddr -> FiFoAddrPtr
    tx[1] = rxaddr;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x01|0x80; //Continuous Rx Mode
    tx[1] = 0x0d;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_LoRaRxWaitStable(int spi_fd,uint8_t &state)
{
    int ret;
    uint8_t tx[] = {
        0x18&0x7f,0xff //Determine whether the state of stable Rx
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    state = rx[1];
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}

int sx1278_lora_rx(int spi_fd,std::string &msg)
{
    int ret;
    uint8_t tx[] = {
        0x10&0x7f,0xff //last packet addr
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    uint8_t last_addr = rx[1];
    tx[0] = 0x0d|0x80; //RxBaseAddr -> FiFoAddrPtr
    tx[1] = last_addr;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x13&0x7f; //Number for received bytes
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    uint8_t packet_size = rx[1];
    uint8_t t_buf[512];
    t_buf[0] = 0x00&0x7f;  //Read LR_RegFifo
    memset(&t_buf[1],0xff,256);
    uint8_t r_buf[512] = {0, };
    memset(r_buf,0x00,512);
    ret = spi_transfer(spi_fd,t_buf,r_buf,packet_size);
    msg = (char *)&r_buf[1];
    tx[0] = 0x12|0x80; //LoRaClearIrq
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_LoRaEntryTx(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x40|0x80,0x41     //DIO0=01, DIO1=00, DIO2=00, DIO3=01
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x12|0x80; //LoRaClearIrq
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x11|0x80;  //Open TxDone interrupt
    tx[1] = 0xf7;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    tx[0] = 0x0e&0x7f;  //RegFiFoTxBaseAddr Read
    tx[1] = 0xff;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    uint8_t tx_addr = rx[1];
    tx[0] = 0x0d|0x80;  //RegFifoAddrPtr Set
    tx[1] = tx_addr;
    ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    if (ret < 1)
    {
        ret = -1;
        return ret;
    }
    return ret;
}
int sx1278_lora_tx(std::string &msg,int spi_fd)
{
    int ret;
    uint8_t t_buf[512];
    t_buf[0] = 0x00|0x80;  //Write LR_RegFifo
    if(msg.length()>255)
    {
        ret = -2;
        return ret;
    }
    else {
        uint8_t msglen = msg.length();
        uint8_t tx[] = {
            0x22|0x80, msglen //RegPayloadLength  msglen
        };
        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
        memcpy(&t_buf[1],msg.c_str(),msg.length());
        uint8_t r_buf[512] = {0, };
        memset(r_buf,0x00,512);
        ret = spi_transfer(spi_fd,t_buf,r_buf,msg.length());
        tx[0] = 0x01|0x80; //Tx mode
        tx[1] = 0x0b;
        ret = spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
        if (ret < 1)
        {
            ret = -1;
            return ret;
        }
    }

}
std::string get_sx1278_chipid(int spi_fd)
{
    int ret;
    uint8_t tx[] = {
        0x42&0x7f, 0xFF
    };
    uint8_t rx[ARRAY_SIZE(tx)] = {0, };
    struct spi_ioc_transfer tr;  /* 这种写法一定要赋初值 */
    memset(&tr,0,sizeof(struct spi_ioc_transfer));
    spi_transfer(spi_fd,tx,rx,ARRAY_SIZE(tx));
    char chipid_str[16];
    snprintf(chipid_str,4,"%.2X",rx[1]);
    std::string chipid(chipid_str);
    return chipid;
}
int wdgflag = 0;
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
            }while(ret != 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            sx1278_clear_irq(spi_fd[0]);
            if( ret == -1 )
            {

            }
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
            }while(ret != 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::string recv_msg =  "0";
            ret = sx1278_lora_rx(spi_fd[1], recv_msg);
            if( ret == -1 )
            {
                std::cout<<"#########sx1278 lora interrupt rx failed#########"<<std::endl;
            }
            else {
                std::cout<<"#########sx1278 lora recv_msg: "<<recv_msg<<"########"<<std::endl;
            }
            sx1278_clear_irq(spi_fd[1]);
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
    init_sx1278_spi(spi_fd[0], spi_dev[0]);
    init_sx1278_spi(spi_fd[1], spi_dev[1]);
    std::string chipid = get_sx1278_chipid(spi_fd[0]);
    std::cout<<"spi_dev0 chipid: "<<chipid<<std::endl;
    chipid = get_sx1278_chipid(spi_fd[1]);
    std::cout<<"spi_dev1 chipid: "<<chipid<<std::endl;
    int spi_init_ret = sx1278_init_lora(spi_fd[0]);
    if(spi_init_ret < 1)
    {
        std::cout<<"spi_dev0 init sx1278 lora failed ret: "<<spi_init_ret<<std::endl;
    }
    spi_init_ret = sx1278_init_lora(spi_fd[1]);
    if(spi_init_ret < 1)
    {
        std::cout<<"spi_dev1 init sx1278 lora failed ret: "<<spi_init_ret<<std::endl;
    }
    sx1278_LoRaEntryTx(spi_fd[0]);
    sx1278_LoRaEntryRx(spi_fd[1]);
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
    std::string send_msg = temperature_str.toStdString();
    int ret = sx1278_lora_tx(send_msg,spi_fd[0]);
    if(ret < 0)
    {
        std::cout<<"#######sx1278 send failed###########"<<std::endl;
    }
    QSqlQuery query;
    query.prepare("update hubtable SET `Temperature` = ? WHERE `HubName` = 'IMX6ULL-001'");
    query.addBindValue(temperature_str);
    query.exec();
}
