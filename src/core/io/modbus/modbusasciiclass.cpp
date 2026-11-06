#include "modbusasciiclass.h"
#include "core/globalflow/globalflow.h"

#define PRINT_FOR_PLC 1

QList<modbusASCIIclass*>       g_modbusAscii;
QList<QThread*>                g_mod_ascii_thread;

modbusASCIIclass::modbusASCIIclass(int index , QObject *parent) : QObject(parent)
{
    port_num = index;
    portName = QString("ttyS%1").arg(port_num);

}

/**
 * @brief modbusASCIIclass::openDevice
 * @return
 */
void modbusASCIIclass::openDevice()
{
    mymodbus_ascii = new QSerialPort();
    mymodbus_ascii->setBaudRate(9600);
    mymodbus_ascii->setParity(QSerialPort::EvenParity);
    mymodbus_ascii->setStopBits(QSerialPort::OneStop);
    mymodbus_ascii->setDataBits(QSerialPort::Data7);
    mymodbus_ascii->setPortName(portName);

    int ret = 0;
    ret = mymodbus_ascii->open(QIODevice::ReadWrite);

    if (ret != 1) {
        printf("open mymodbus_ascii%d is err!", port_num);
        fflush(0);
    } else {
        printf("open mymodbus_ascii%d is ok!", port_num);
        fflush(0);
    }
}

/**
 * @brief modbusASCIIclass::getData
 * @param nCmd
 */
void modbusASCIIclass::getData(int nCmd)
{
    int writeLen = 17;
    int oriLen = 6;
    uchar originData[oriLen];
    uchar lrc;
    char packet[writeLen];

    originData[0] = 0x01;
    originData[1] = 0x03;
    originData[2] = nCmd/256;
    originData[3] = nCmd%256;
    originData[4] = 0x00;
    originData[5] = 0x01;
    lrc = cal_lrc2(originData, 6);


    packet[0] = 0x3A;  //:
    packet[1] = 0x30;
    packet[2] = 0x31;  //地址 0x01
    packet[3] = 0x30;
    packet[4] = 0x33;  //功能码 0x03
    packet[5] = asciiToHex(nCmd>>12 & 0x0f);
    packet[6] = asciiToHex(nCmd>>8 & 0x0f);
    packet[7] = asciiToHex(nCmd>>4 & 0x0f);
    packet[8] = asciiToHex(nCmd & 0x0f);
    packet[9] = 0x30;
    packet[10] = 0x30;
    packet[11] = 0x30;
    packet[12] = 0x31;
    packet[13] = asciiToHex(lrc>>4 & 0x0f);
    packet[14] = asciiToHex(lrc & 0x0f);;
    packet[15] = 0x0D;
    packet[16] = 0x0A;

    mymodbus_ascii->readAll();
    mymodbus_ascii->write(packet,writeLen);
}

/**
 * @brief modbusASCIIclass::writeData
 * @param nCmd
 * @param value
 */
void modbusASCIIclass::writeData(int nCmd, int value)
{
    int writeLen = 17;
    int oriLen = 6;
    uchar originData[oriLen];
    uchar lrc;
    char packet[writeLen];

    originData[0] = 0x01;
    originData[1] = 0x06;
    originData[2] = nCmd/256;
    originData[3] = nCmd%256;
//    originData[4] = 0x00;
//    originData[5] = 0x01;
    originData[4] = value/256;
    originData[5] = value%256;
    lrc = cal_lrc2(originData, oriLen);

    packet[0] = 0x3A;  //:
    packet[1] = 0x30;
    packet[2] = 0x31;  //地址 0x01
    packet[3] = 0x30;
    packet[4] = 0x36;  //功能码 0x06
    packet[5] = asciiToHex(nCmd>>12 & 0x0f);
    packet[6] = asciiToHex(nCmd>>8 & 0x0f);
    packet[7] = asciiToHex(nCmd>>4 & 0x0f);
    packet[8] = asciiToHex(nCmd & 0x0f);
    //packet[9] = 0x30;
    //packet[10] = 0x30;
    //packet[11] = 0x30;
    //packet[12] = 0x31;
    packet[9] = asciiToHex(value>>12 & 0x0f);
    packet[10] = asciiToHex(value>>8 & 0x0f);
    packet[11] = asciiToHex(value>>4 & 0x0f);
    packet[12] = asciiToHex(value & 0x0f);
    packet[13] = asciiToHex(lrc>>4 & 0x0f);
    packet[14] = asciiToHex(lrc & 0x0f);;
    packet[15] = 0x0D;
    packet[16] = 0x0A;

    mymodbus_ascii->readAll();
    mymodbus_ascii->write(packet,writeLen);
}

/**
 * @brief modbusASCIIclass::readSerialData
 * @param buf
 * @param len
 * @param timeout
 * @return
 */
int modbusASCIIclass::readSerialData(bool print)
{
    int ret = 0;
    int nDispLen = 0;
    int count = 1;
    int len = 15;
    char buf[len];

    memset(buf, 0, len);
    while (mymodbus_ascii != NULL) {
        ret += mymodbus_ascii->read(buf+ret, len-ret);
        if ((--count > 0) && (ret < len)) {
            gFlow.msleep(50);
        } else {
            break;
        }
    }

    nDispLen = (ret > len) ? len : ret;

    if(print) {
        printf("modbus ascii read  << %d\n", ret);
        for (int i = 0; i < nDispLen; i++) {
            printf("%02x ", buf[i]);
            if(i%5==4){
                printf(" // ");
            }
        }
        printf("\n");
        fflush(0);
    }


    ret = ((hexToAscii(uchar(buf[7])) & 0x0f) << 12)|
          ((hexToAscii(uchar(buf[8])) & 0x0f) << 8) |
          ((hexToAscii(uchar(buf[9])) & 0x0f) << 4) |
          (hexToAscii(uchar(buf[10])) & 0x0f);
    return ret;
}

/**
 * @brief modbusASCIIclass::asciiToHex
 * @param ascii
 * @return
 */
uchar modbusASCIIclass::asciiToHex(uchar ascii)
{
    if(ascii >= 0 && ascii <= 9){
        ascii += 0x30;
    }else if(ascii >= 10 && ascii <= 15){
        ascii += 0x37;
    }else{
        ascii = 0xff;
    }

    return ascii;
}

/**
 * @brief modbusASCIIclass::hexToAscii
 * @param hex
 * @return
 */
unsigned char modbusASCIIclass::hexToAscii(unsigned char hex)
{
    if(hex >= 0x30 && hex <= 0x39){
        hex -= 0x30;
    }else if(hex >= 0x41 && hex <= 0x46){
        hex -= 0x37;
    }else if(hex >= 0x61 && hex <= 0x66){
        hex -= 0x57;
    }else{
        hex = 0xff;
    }

    return hex;
}

/**
 * @brief modbusASCIIclass::计算lrc
 * @param data
 * @param len
 * @return
 */
unsigned char modbusASCIIclass::cal_lrc2(unsigned char *data, int len)
{
    unsigned char lrc = 0;
    for (int i = 0; i < len; i++) {
        lrc += *data++;
    }

    if(lrc > 0xff) {
        lrc = lrc%0x100;
    }
    lrc = 0xff-lrc+1;
    lrc = lrc&0xff;
    return lrc;
}

