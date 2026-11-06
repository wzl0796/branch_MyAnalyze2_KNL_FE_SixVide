#include "aiimagesave.h"

#include <future>
#include <QFile>
#include <QString>
#include <QtConcurrent>
#include <QElapsedTimer>
#include <QFileDevice>
#include <QCryptographicHash>

#define MAX_IMG_CONVERT_TIME (60 * 15) /*! 最大图像转换时间(15分钟） */

#define MAX_REQ_TIMEOUT 100      /* 单包请求超时时间 单位ms */
#define MAX_WAIT_PKG_TMIEOUT  3 /* 等到单包数据的超时时间 单位s */

float g_process = 0;

AiImageSaveTask::AiImageSaveTask():m_aiudpSocket(nullptr)
{
}

AiImageSaveTask::~AiImageSaveTask()
{
}

void AiImageSaveTask::setTask(QVector<QPair<int, QString> > nUids)
{
    m_nUids = nUids;
}

QVector<QPair<int, QString> > AiImageSaveTask::getTask()
{
    return m_nUids;
}

void AiImageSaveTask::doOneTask(int nId, QString fileName)
{
	QString tips;
    //! 查询AI板是否进入可以拷贝的状态
    bool res = imageConvertAndCompress(nId, tips);


    if (!res) {
		m_mapUploadState[nId] = tips;
		return;
	}

    //! 查询文件大小，以及创建文件
    int pkgCount = 0;
    qint64 fileSize = 0;
    int file_state = getFileSize(nId, pkgCount, fileSize);
//    qDebug()<<"file size"<<fileSize <<"file count"<<pkgCount;
    if (file_state != STATUS_OK) {
        m_mapUploadState[nId] = getOneStatusString(nId, file_state);
        return;
    }

    // qDebug()<<"check file size finished";

    if (fileName.isEmpty()) {
#ifdef Q_OS_WIN
        fileName = QString("/userdata/%1_%2.zip")
                       .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
                       .arg(nId);
#else
        fileName = QString("/userdata/tmpudisk/%1_%2.zip")
                       .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
                       .arg(nId);
#endif
    }
    // qDebug()<<" start create file finished" << fileName;

    //! 创建文件
    int create_state = createFile(nId, fileName, fileSize);

    // qDebug()<<"create file finished" << fileName;

    if (create_state != STATUS_OK) {
        m_mapUploadState[nId] = getOneStatusString(nId, create_state);
        return;
    }

    //! 启动线程开始写入文件
	int state = saveImageData(nId, fileName, pkgCount);

    // qDebug()<<"save image data finished";

    m_mapUploadState[nId] = getOneStatusString(nId, state);
}

bool AiImageSaveTask::imageConvertAndCompress(int nId, QString &tips)
{
	int nTick = 0;
	int nErrTick = 0;
	int nCompressTick = -1; //! 记录开始压缩的时间
    QString info;
    while (nTick <= MAX_IMG_CONVERT_TIME) {
		struAIOperate stAIStatus;
		memset(&stAIStatus, 0, sizeof(stAIStatus));
        bool res = getSocStatus(nId,stAIStatus);
        if (!res) {
            //! 请求超时的错误提示信息
            if(nTick > 3)
                info = myLan.timeout + QString("(%1)").arg(nTick);
            if (nErrTick >= MAX_TYR_TIMES) {   //! 超时
                tips = gFlow.getCamName(nId) + " : " + myLan.timeout;
				emit sigUpdateTips(info, 0);
				return false;
			}
			nErrTick++;
        } else {
            if (stAIStatus.nImgNum > 0) {
				m_mapPicNum[nId] = stAIStatus.nImgNum; //! 记录导出图像数量
			}

            if (stAIStatus.nStatus == ERROR_NO_FILE) { //! 无图像文件
                tips = gFlow.getCamName(nId) + " : " + myLan.no_find_file;
				emit sigUpdateTips(info, 0);
				return false;
			}

			nErrTick = 0;
            if (stAIStatus.nImgExportStatus == IMG_CONVERT) { //! 图像转换
                info =myLan.imgconvert + QString("(%1%)").arg(stAIStatus.nImgExportPercent);
            } else if (stAIStatus.nImgExportStatus == IMG_SAVING)   {
                info = myLan.save + QString("(%1%)").arg(stAIStatus.nImgExportPercent);
			} else if (stAIStatus.nImgExportStatus == IMG_COMPRESS) { //! 图像压缩
                if (nCompressTick == -1) {
					nCompressTick = nTick;
				}
				int nElapsed = nTick - nCompressTick;
				nElapsed = nElapsed > 0 ? nElapsed : 0;
                info = QString("%1(%2:%3)").arg(myLan.imgcompress).arg(nElapsed / 60).arg(nElapsed % 60);
            } else if (stAIStatus.nImgExportStatus == IMG_COPY) { //! 图像拷贝
                return true;
            } else {
                tips = gFlow.getCamName(nId) + " : " + myLan.unknown_error;
				return false;
			}
		}

        QString showTips = gFlow.getCamName(nId) + " : " + info;
        if(nTick > 3)
		emit sigUpdateTips(showTips, 0);

        sleep(1);
		nTick++;
	}

	return false;
}

int AiImageSaveTask::getFileSize(int nUid, int &pkgCount, qint64& fileSize)
{
    fileSize = 0;
    pkgCount = 0;

    QString showTip = gFlow.getCamName(nUid) + " : " + myLan.msg_getfilesize;
    emit sigUpdateTips(showTip, 0);

    //! 判断文件大小，磁盘空间是否满足条件
    pkgCount = getImageDataSize(nUid, fileSize);
    if (pkgCount == 0xFFFF) {
        return ERROR_FILE_LARGE;
    } else if (pkgCount == 0 || fileSize == 0) {
        return ERROR_NO_FILE;
    }

    return STATUS_OK;
}

int AiImageSaveTask::createFile(int nUid, QString fileName, qint64 fileSize)
{
    QString showTip = gFlow.getCamName(nUid) + " : " + myLan.create_file;
    emit sigUpdateTips(showTip, 0);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QIODevice::Truncate)) {
        return ERROR_OPEN_FILE;
    }
    file.close();

    if (!file.open(QFile::ReadWrite | QIODevice::Append)) {
        file.remove();
        return ERROR_OPEN_FILE;
    }

    qint64 totalWritten = 0;
    std::vector<char> buffer(BLOCK_SIZE, 0);
    const qint64 maxBlocks = (fileSize + BLOCK_SIZE - 1)/BLOCK_SIZE;
    for (qint64 i = 0; i < maxBlocks; ++i) {
        //! 计算当前块大小
        const qint64 currSize = qMin(BLOCK_SIZE, fileSize - totalWritten);

        //! 写入并刷新缓冲区
        qint64 written = file.write(buffer.data(), currSize);
        file.flush();

        if (written != currSize) {
            file.close();
            file.remove();
            return ERROR_NOT_ENOUGH_MEM;
        }
        totalWritten += written;

        //! 更新进度提示（每5%更新一次）
        if (maxBlocks/20 != 0 && i %(maxBlocks/20) == 0) {
            int process = static_cast<int>(totalWritten*100/fileSize);
            emit sigUpdateTips(showTip+QString(" (%1%)").arg(process), 0);
        }
    }
    file.close();

    return STATUS_OK;
}

void AiImageSaveTask::progressTips(int nId)
{
    QString process = QString::number(g_process, 'f', 1);
	if (process == "100.0") {
		process = "99.9";
	}

	if (m_lastProcess != process) {
        QString info = QString("%1:").arg(gFlow.getCamName(nId))
                + QString("%1(%2%)").arg(myLan.file_copy).arg(process);
		emit sigUpdateTips(info, 0);
	}
	m_lastProcess = process;
}

int AiImageSaveTask::getUdpData(int nUid, int pkgCount)
{
     if(!m_aiudpSocket) {
        qDebug() << "UDP socket is null";
        return (int)STATUS_FAILED;
    }
    //! 请求上传所有包
    char data[3];
    data[0] = 3;    //! 1： 代表数据包   2：jls的压缩包 3:连续图像包
    data[1] = 0;
    data[2] = 0;
    QByteArray ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUid, CMD_SOC_EXPORT_IMAGES_NET, data, 3);
    m_aiudpSocket->write(ba);


    //! 尝试读取 pkgCount 数量包
    int index = 0;
    int error_count = 0;
    while (index < pkgCount) {
        index++;

        QByteArray baRet = m_aiudpSocket->read(MAX_REQ_TIMEOUT);
        if (checkPacketRead(baRet,CMD_SOC_EXPORT_IMAGES_NET)) {
			error_count = 0;
            m_imgData.push(baRet);
        } else {
            error_count++;
            // qDebug() << "Received UDP packet of index:" << error_count <<"baret size="<<baRet.size();

        }

        //! 连续 50*200ms 读取不到数据，认为是异常，直接停止
        if (error_count > 200) {
            break;
        }
    }

    // 结束标志包
    QByteArray baEnd;
    baEnd.append(0xFF);
    baEnd.append(0xFF);
    m_imgData.push(baEnd);

    return (int)STATUS_OK;
}

int AiImageSaveTask::writeInFile(int nUid, QString fileName, int pkgCount)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadWrite)) {
		return ERROR_OPEN_FILE;
	}
	// 包状态
	m_pkgState.resize(pkgCount);
	m_pkgState.fill(false, pkgCount);

	// 文件操作
	qint64 fileSize = file.size();
    auto mapInFile = [&file, fileSize, pkgCount](qint64 readCurId, const QByteArray ba)->bool
	{
		qint64 pos = 0;
        int nLen = ba.size();
        if (readCurId == (pkgCount - 1)) {
            pos = fileSize - nLen;
        } else {
			pos = readCurId * nLen;
		}

		uchar *file_data = file.map(pos, nLen);
		if (file_data) {
			memcpy(file_data, ba.data(), nLen);
            return file.unmap(file_data);
		}

        return false;
	};

    int success_index = 0;
    int error_count = 0;
    m_lastProcess = "0.0";
    while (true) {
        QByteArray baRet;
        bool status = m_imgData.wait_for(baRet, MAX_WAIT_PKG_TMIEOUT);
        //! 等包超时3次
        if (!status) {
            error_count++;
            if (error_count > 10)
                break;
            else
                continue;
        }

        //! 收到结束包
        if (baRet.size() == 2 && baRet[0] == (char)0xFF && baRet[1] == (char)0xFF){
            break;
        }

        if (checkPacketRead(baRet)) {
            if (baRet.size() == AI_IMAGE_PKT_SIZE
                    || baRet.size() == AI_IMAGE_PKT_ORI_SIZE
                    || baRet.size() == AI_IMAGE_PKT_SIZE_2
                    || baRet.size() == AI_IMAGE_PKT_ORI_SIZE_2) {
                int curId1 = (uchar)baRet[PKT_R_DATA_3];
                int curId2 = (uchar)baRet[PKT_R_DATA_4];
                qint64 readCurId = curId1*256 + curId2;
				QByteArray md5Remote = baRet.mid(AI_IMAGE_MD5_INDEX, AI_IMAGE_MD5_SIZE);
				QCryptographicHash md(QCryptographicHash::Md5);

                int len1 = (uchar)baRet[PKT_R_DATA_5];
                int len2 = (uchar)baRet[PKT_R_DATA_6];
                int nLen = len1 * 256 + len2;
				md.addData(baRet.mid(AI_IMAGE_EFFECT_DATA_INDEX, nLen));
				QByteArray md5Local = md.result();
                if (md5Local == md5Remote) { //! 判断图像包Md5校验
					QByteArray ba = baRet.mid(AI_IMAGE_EFFECT_DATA_INDEX, nLen);
                    bool success = mapInFile(readCurId, ba);
                    if (success) {
                        m_pkgState[readCurId] = true;
                        success_index++;
                        g_process = ((float)success_index * 100) / pkgCount;
                        progressTips(nUid);
                    }
				}
			}
		}
	}

    for (int i = 0; i < pkgCount; i++) {
        if (!m_pkgState[i]) {
            // qDebug() << "Missing package index:" << i;
			int nResult = STATUS_OK;
            QByteArray ba = getImageDataPackage(nUid, i, nResult);
            if (ba.isEmpty()) {
				printf("loss pkg : %d\n", i);
				fflush(0);
				file.close();
				file.remove();
                return STATUS_FAILED;
            } else {
				mapInFile(i, ba);
			}
			success_index++;

			g_process = ((float)success_index * 100) / pkgCount;
			progressTips(nUid);
		}
	}
	file.close();
    gFlow.SystemCmd("sync");

    //! 将参数信息打包仅图片zip包
#if 0
    QString infoName = CNF_PATH + QString("AI_cap.json");
    QFile Infofile(infoName);
    QString fileNameTmp = fileName + "tmp";

    QZipWriter zipWriter(fileNameTmp);
    if (Infofile.exists()) {
        zipWriter.addFile("AI_cap.txt", &Infofile);
        zipWriter.addFile("png.zip", &file);
    }
    zipWriter.close();
    SystemCmd("sync");

    SystemCmd(QString("mv %1 %2").arg(fileNameTmp).arg(fileName));
    SystemCmd("sync");
#endif

	return STATUS_OK;
}

int AiImageSaveTask::saveImageData(int nUid, QString fileName, int pkgCount)
{
    // 参数有效性检查
    if (nUid < 0 || fileName.isEmpty() || pkgCount <= 0) {
        qDebug() << "Invalid parameters in saveImageData";
        return STATUS_FAILED;
    }

    //! 提示准备拷贝
    QString showTip = gFlow.getCamName(nUid) + " : " + myLan.file_copy;
    emit sigUpdateTips(showTip, 0);

    // 使用共享指针避免内存泄漏
    std::shared_ptr<int> readResult = std::make_shared<int>(STATUS_OK);
    std::shared_ptr<int> writeResult = std::make_shared<int>(STATUS_OK);

    try {
        std::future<int> readUdp = std::async(std::launch::async, [this, nUid, pkgCount, readResult]() 
        {
            try {
                *readResult = this->getUdpData(nUid, pkgCount);
                return *readResult;
            } catch (...) {
                *readResult = STATUS_FAILED;
                return 1;
            }
        });

        std::future<int> writeFile = std::async(std::launch::async, [this, nUid, &fileName, pkgCount, writeResult]() 
        {
            try {
                *writeResult = this->writeInFile(nUid, fileName, pkgCount);
                return *writeResult;
            } catch (...) {
                *writeResult = STATUS_FAILED; 
                return 1;
            }
        });

        // 等待两个任务完成并检查结果
        int readStatus = readUdp.get();
        int writeStatus = writeFile.get();

        if (readStatus != STATUS_OK || writeStatus != STATUS_OK) {
            return STATUS_FAILED;
        }

        return *writeResult;
    }
    catch (const std::exception& e) {
        qDebug() << "Exception in saveImageData:" << e.what();
        return STATUS_FAILED;
    }
    catch (...) {
        qDebug() << "Unknown exception in saveImageData";
        return STATUS_FAILED;
    }
}

bool AiImageSaveTask::checkPacketRead(QByteArray ba, int cmd)
{
    if (ba.size() > PKT_R_CMD_LEN) {
        int nLen = (ba[PKT_R_DATA_LEN_1]<<8) | ba[PKT_R_DATA_LEN_2];
        int nCmd = (ba[PKT_R_CMD_CODE_1]<<8) | ba[PKT_R_CMD_CODE_2];
        if (ba.size() >= (nLen+PKE_R_HEAD_TAIL) && nCmd == cmd
            && ((uchar)ba[PKT_R_HEADER_1] == 0xAA || (uchar)ba[PKT_R_HEADER_1] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_2] == 0xAA || (uchar)ba[PKT_R_HEADER_1] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_3] == 0xAA || (uchar)ba[PKT_R_HEADER_1] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_4] == 0xAA || (uchar)ba[PKT_R_HEADER_1] == 0xAd)) {
            return true;
        }
    }
    return false;
}

bool AiImageSaveTask::checkPacketRead(QByteArray ba)
{
    if (ba.size() > PKT_R_CMD_LEN) {
        int nLen = (ba[PKT_R_DATA_LEN_1] << 8) | ba[PKT_R_DATA_LEN_2];
        if (ba.size() >= (nLen + PKE_R_HEAD_TAIL)
            && ((uchar)ba[PKT_R_HEADER_1] == 0xAA || (uchar)ba[PKT_R_HEADER_1] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_2] == 0xAA || (uchar)ba[PKT_R_HEADER_2] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_3] == 0xAA || (uchar)ba[PKT_R_HEADER_3] == 0xAd)
            && ((uchar)ba[PKT_R_HEADER_4] == 0xAA || (uchar)ba[PKT_R_HEADER_4] == 0xAd)) {
            return true;
        }
    }
    return false;
}

QString AiImageSaveTask::getOneStatusString(int nId, int stat)
{
	QMap<int, QString> mapStr;
    mapStr[STATUS_DOING] = myLan.upgrade + "...";
    mapStr[STATUS_OK] = myLan.sucess;
    if (m_mapPicNum.contains(nId)) {
        if (m_mapPicNum[nId] > 0) {
			mapStr[STATUS_OK] += " " + QString("%1").arg(m_mapPicNum[nId]);
		}
	}
    mapStr[STATUS_FAILED] = myLan.failed;
    mapStr[ERROR_NO_UDISK] = myLan.no_find_udisk;
    mapStr[ERROR_NO_FILE] = myLan.no_find_file;
    mapStr[ERROR_OPEN_FILE] = myLan.msg_file_open_failed;
    mapStr[ERROR_FILE_LARGE] = myLan.msg_file_too_large;
    mapStr[ERROR_NOT_ENOUGH_MEM] = myLan.msg_udisk_not_enough;
    mapStr[ERROR_UPLOAD] = myLan.msg_data_upload_error;
    mapStr[AI_IMAGE_NO_RESPONSE] = myLan.msg_no_data;
    mapStr[AI_IMAGE_DATA_ERROR] = myLan.msg_data_error;
    mapStr[AI_IMAGE_INDEX_ERROR] = myLan.msg_data_index_error;
    mapStr[AI_IMAGE_MD5_ERROR] = myLan.msg_data_check_error;

    if (mapStr[stat].isEmpty()) {
        return myLan.unknown_error;
	}
    return (gFlow.getCamName(nId) + " : " + mapStr[stat]);
}

void AiImageSaveTask::run()
{
    if(m_aiudpSocket == nullptr){
        m_aiudpSocket = new myUdpAI(6402);
    }

	QElapsedTimer total_timer;
	total_timer.start();

    for (QPair<int, QString> info : m_nUids) {
        doOneTask(info.first, info.second);
    }

    sleep(3);
    gFlow.SystemCmd("sync");

	int time = total_timer.elapsed() / 1000;
	formatTip(time);

    if(m_aiudpSocket){
        qDebug()<<"delete udp socket";
        delete m_aiudpSocket;
        m_aiudpSocket = nullptr;
    }
}

void AiImageSaveTask::formatTip(int time)
{
	QString str_time;
    int minute = time / 60;
    int second = time - minute * 60;

    auto handleTimeTip = [&str_time](int time, QString symbol)
    {
        str_time += (QString::number(time) + symbol);
	};

    handleTimeTip(minute, ":");
    handleTimeTip(second, "");

    QString info{""};
    if (!str_time.isEmpty()) {
        info = myLan.elapsed_time + " : " + str_time + "\n";
	}

	// 格式化提示
    QStringList uploadState;
    for (QPair<int, QString> info : m_nUids) {
        uploadState.append(m_mapUploadState.value(info.first));
    }

	// 4个以下换行显示
    // 4个以上包含4个，2个合并成一行、换行显示
    if (uploadState.size() < 4) {
		QString showInfo = uploadState.join("\n");
        info += showInfo;
    } else {
		QStringList endState;
        QString oneState;
        for (int i = 0; i < uploadState.size(); i++) {
            if (i % 2 == 0) {
				if (i == (uploadState.size() - 1))
					endState.append(uploadState[i]);
				else
                    oneState += (uploadState[i] + ", ");
            } else {
				oneState += uploadState[i];
				endState.append(oneState);
				oneState.clear();
			}
		}

		QString showInfo = endState.join("\n");
		info += showInfo;
	}

    emit sigFinish(info);
}


bool AiImageSaveTask::getSocStatus(int naddr, struAIOperate &stOperate)
{
    if(m_aiudpSocket){
        char data[1];
        data[0]= 1;
        QByteArray ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, naddr, CMD_SOC_UP_STATE, data, 1);

        QByteArray baRet = m_aiudpSocket->writeRead(ba);

        if (checkPacketRead(baRet,CMD_SOC_UP_STATE)) {
            stOperate.nStatus = baRet[PKT_R_DATA_1];
            stOperate.nImgExportStatus = baRet[PKT_R_DATA_2];
            stOperate.nImgExportPercent = baRet[PKT_R_DATA_3];
            stOperate.bStorageValid = baRet[PKT_R_DATA_4] == 0 ? false: true;
            stOperate.nStorageFree = (uint)baRet[PKT_R_DATA_5]*256 + (uint)baRet[PKT_R_DATA_6];
            stOperate.nImgNum = (uint)baRet[PKT_R_DATA_7]*256 + (uint)baRet[PKT_R_DATA_8];
            return true;
        }
    }

    return false;
}

uint AiImageSaveTask::getImageDataSize(int nId, qint64 &filesize)
{
    if(m_aiudpSocket){
        char data[3];
        const int IMG_DATA_SIZE = 61470;  //! 工程化16086  非工程化61470
        data[0]=0;                //! 代表返回数据包个数和大小
        data[1]= IMG_DATA_SIZE/256;
        data[2]=IMG_DATA_SIZE%256;

       

        for(int j=0; j<20; j++){
            QByteArray ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nId, CMD_SOC_EXPORT_IMAGES_NET, data, 3);
            QByteArray baRet = m_aiudpSocket->writeRead(ba,1000);
            // qDebug()<< baRet.size();
            if (checkPacketRead(baRet,CMD_SOC_EXPORT_IMAGES_NET)) {
                filesize = (baRet[PKT_R_DATA_3]&0xff)*256*256*256
                           +(baRet[PKT_R_DATA_4]&0xff)*256*256
                           +(baRet[PKT_R_DATA_5]&0xff)*256
                           +(baRet[PKT_R_DATA_6]&0xff);
                int packCount = (baRet[PKT_R_DATA_1]&0xff)*256
                                + (baRet[PKT_R_DATA_2]&0xff);

                return packCount;
            }
            sleep(2);
        }
    }

    return 0;

}

QByteArray AiImageSaveTask::getImageDataPackage(int nBoardId, int nPktId, int &nResult)
{
    char data[3];
    data[0] = 1;
    data[1] = nPktId/256;
    data[2] = nPktId%256;
    int nCurrentId = 0;

    for (int j = 0; j < 3; ++j) {
        nResult = AI_IMAGE_DATA_ERROR;
        QByteArray ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nBoardId, CMD_SOC_EXPORT_IMAGES_NET, data, 3);
        QByteArray baRet = m_aiudpSocket->writeRead(ba);
        if (!checkPacketRead(baRet)) {
            nResult = AI_IMAGE_NO_RESPONSE;
        } else {
            if (baRet.size() == AI_IMAGE_PKT_SIZE
                || baRet.size() == AI_IMAGE_PKT_ORI_SIZE
                || baRet.size() == AI_IMAGE_PKT_SIZE_2
                || baRet.size() == AI_IMAGE_PKT_ORI_SIZE_2){

                nCurrentId = (baRet[PKT_R_DATA_3]<<8) + (baRet[PKT_R_DATA_4]);
                if (nPktId == nCurrentId) {  //! 判断图像包ID
                    QByteArray md5Remote = baRet.mid(AI_IMAGE_MD5_INDEX, AI_IMAGE_MD5_SIZE);
                    QCryptographicHash md(QCryptographicHash::Md5);
                    int nLen = (uint)baRet[PKT_R_DATA_5]*256 + (uint)baRet[PKT_R_DATA_6];
                    md.addData(baRet.mid(AI_IMAGE_EFFECT_DATA_INDEX, nLen));
                    QByteArray md5Local = md.result();
                    if (md5Local == md5Remote) {    //! 判断图像包Md5校验
                        nResult = AI_IMAGE_OK;
                        return baRet.mid(AI_IMAGE_EFFECT_DATA_INDEX, nLen);
                    } else {
                        nResult = AI_IMAGE_MD5_ERROR;
                    }
                } else {
                    nResult = AI_IMAGE_INDEX_ERROR;
                }
            }
        }

        msleep(100);
    }
    return QByteArray();
}
