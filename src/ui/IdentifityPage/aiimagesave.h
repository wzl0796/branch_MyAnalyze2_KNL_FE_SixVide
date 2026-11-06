#ifndef AI_IMAGE_SAVE_H
#define AI_IMAGE_SAVE_H

#include <atomic>
#include <string>
#include <iostream>
#include <list>
#include <QThread>
#include <stdio.h>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <QThread>
#include <QFile>
#include <QFutureWatcher>
#include <QByteArray>
#include <QMap>
#include <QVector>
#include "QtGui/private/qzipwriter_p.h"
#include "core/io/udp/myudpai.h"
#include "core/globalflow/globalflow.h"

extern float g_process;

/*!
 * \enum 枚举操作结果
 */
enum exImgerror{
    STATUS_OK = 0,          /*! 成功 */
    STATUS_FAILED,          /*! 失败 */
    STATUS_DOING,           /*! 执行中 */
    ERROR_NO_UDISK = 10,    /*! 未检测到U盘 */
    ERROR_NO_FILE,          /*! 无相关文件（软件、模型或图片） */
    ERROR_OPEN_FILE,        /*! 打开文件失败 */
    ERROR_FILE_LARGE,   /*! 文件过大 */
    ERROR_NOT_ENOUGH_MEM,   /*! U盘空间不足 */
    ERROR_UPLOAD        /*! 上传失败 */
};

/*!
 * \enum 枚举读数据包协议
 */
enum dataindex{
    PKT_R_HEADER_1 = 0,         //! 包头1
    PKT_R_HEADER_2,             //! 包头2
    PKT_R_HEADER_3,             //! 包头3
    PKT_R_HEADER_4,             //! 包头4
    PKT_R_MODULE_ADDR,          //! 模块地址
    PKT_R_CMD_CODE_1 = 8,       //! 命令编码1
    PKT_R_CMD_CODE_2,           //! 命令编码2
    PKT_R_DATA_LEN_1,           //! 数据长度1
    PKT_R_DATA_LEN_2,           //! 数据长度2
    PKT_R_DATA_1,               //! 数据1
    PKT_R_DATA_2,               //! 数据2
    PKT_R_DATA_3,               //! 数据3
    PKT_R_DATA_4,               //! 数据4
    PKT_R_DATA_5,               //! 数据5
    PKT_R_DATA_6,               //! 数据6
    PKT_R_DATA_7,               //! 数据7
    PKT_R_DATA_8,               //! 数据8
    PKT_R_DATA_9,               //! 数据9
    PKT_R_DATA_10,              //! 数据10
    PKT_R_DATA_11,              //! 数据11
    PKT_R_DATA_12,              //! 数据12
    PKT_R_DATA_13,              //! 数据13
    PKT_R_DATA_14,              //! 数据14
    PKT_R_DATA_15,              //! 数据15
    PKT_R_DATA_16,              //! 数据16
    PKT_R_DATA_17,              //! 数据17
    PKT_R_DATA_18,              //! 数据18
    PKT_R_DATA_19,              //! 数据19
    PKT_R_DATA_20,              //! 数据20
    PKE_R_HEAD_TAIL = 16,       //! 包头包尾长度
    PKT_R_CMD_LEN = 20,         //! 数据长度：除命令数据
    PKT_R_LEN_DEFAULT = 42      //! 默认读回包长度
};

enum exImgSta{
    IMG_CONVERT = 0x01,     /*! 图片转换 */
    IMG_COPY = 0x02,        /*! 图片压缩及导出中 */
    IMG_COMPRESS = 0x03,    /*! 图片压缩中 */
    IMG_SAVING = 0x04
};

/*!
 * \enum 枚举AI板图像导出结果
 */
enum exImgAISta{
    AI_IMAGE_OK = 0,                //! 成功
    AI_IMAGE_NO_RESPONSE = -1,      //! 无反馈
    AI_IMAGE_DATA_ERROR = -2,       //! 数据错误
    AI_IMAGE_INDEX_ERROR = -3,      //! 索引错误
    AI_IMAGE_MD5_ERROR = -4        //! MD5错误
};


#define AI_IMAGE_PKT_SIZE           16104       //! 图像数据包尺寸(接口板多补了2个字节)
#define AI_IMAGE_PKT_ORI_SIZE       16102       //! AI板图像包尺寸
#define AI_IMAGE_MD5_INDEX          18          //! MD5校验码索引
#define AI_IMAGE_MD5_SIZE           16          //! MD5校验码大小
#define AI_IMAGE_EFFECT_DATA_INDEX  34          //! 图像有效数据索引

#define AI_IMAGE_PKT_SIZE_2         61488       //! 图像数据包尺寸(接口板多补了2个字节)
#define AI_IMAGE_PKT_ORI_SIZE_2     61486       //! AI板图像包尺寸

template <typename T>
class AIImageBufferQueue
{
public:
    void push(const T &msg)
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_queue.push(msg);
        m_cv.notify_one();
    }

    void wait(T &msg)
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        while (m_queue.empty())
            m_cv.wait(lck);

        msg = m_queue.front();
        m_queue.pop();
    }

    bool wait_for(T &msg, int timeout)
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        while(m_queue.empty()) {
            if (m_cv.wait_for(lck, std::chrono::seconds(timeout)) == std::cv_status::timeout)
                return false;
        }

        msg = m_queue.front();
        m_queue.pop();
        return true;
    }

    int queueSize()
    {
        return m_queue.size();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};


class AiImageSaveTask : public QThread
{
    Q_OBJECT
public:
    AiImageSaveTask();
    ~AiImageSaveTask();

    void setTask(QVector<QPair<int, QString>> nUids);
    QVector<QPair<int, QString>> getTask();

signals:
    void sigUpdateTips(QString tips, int type);
    void sigFinish(QString info);

private:
    void doOneTask(int nId, QString fileName);
    bool imageConvertAndCompress(int nId, QString &tips);

    int getFileSize(int nUid, int& pkgCount, qint64& fileSize);
    int createFile(int nUid, QString fileName, qint64 fileSize);
    int saveImageData(int nUid, QString fileName, int pkgCount);

    int getUdpData(int nUid, int totalpkg);
    int writeInFile(int nUid, QString fileName, int pkg_count);

    void progressTips(int nId);

    QString getOneStatusString(int nId, int stat);   //! 获取一次上传的状态文本

    bool checkPacketRead(QByteArray ba, int cmd);
    bool checkPacketRead(QByteArray ba);

    void run();

    void formatTip(int time);

    //! 命令发送相关
    bool getSocStatus(int naddr,struAIOperate &stOperate);
    uint getImageDataSize(int nId, qint64& filesize);
    QByteArray getImageDataPackage(int nBoardId, int nPktId, int &nResult); //! 获取图像单包数据

private:
    const int MAX_TYR_TIMES = 20;   //! 获取状态最多尝试次数
    const qint64 BLOCK_SIZE = 4*1024*1024;  //! 4MB分块写入

    QVector<QPair<int, QString> > m_nUids;
    QMap<int, QString> m_mapUploadState;        //! 记录导出成功失败的状态    
    QMap<int, int> m_mapPicNum;                 //! 记录导出图片总数

    int m_totalPkg;
    AIImageBufferQueue<QByteArray> m_imgData;
    QVector<bool> m_pkgState;
    QString m_lastProcess;

    myUdpAI *m_aiudpSocket;
};

#endif // AI_IMAGE_SAVE_H
