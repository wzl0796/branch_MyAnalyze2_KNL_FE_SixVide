#include "modbustcpclass.h"
#include "core/globalflow/globalflow.h"

modbusTcpClass::modbusTcpClass(QString ip, int nPort, QObject *parent) : QObject(parent)
{
    server_ip = ip;
    server_num = nPort;
}

/**
 * @brief modbusTcpClass::初始化TCP
 */
void modbusTcpClass::initTcpSerial()
{
    m_myModbus = new MyModbusC(TYPE_TCP);
    m_myModbus->setId(1);
    m_myModbus->setServer(server_ip, server_num);

    if (connectToModbus()) {
        printf("g_modbusTcp connect OK! \n");
    } else {
        printf("g_modbusTcp connect ERR! \n");
    }
    fflush(0);
}

/**
 * @brief modbusTcpClass::连接主机
 * @return
 */
bool modbusTcpClass::connectToModbus()
{
    return m_myModbus->connectToModbus();
}

/**
 * @brief modbusTcpClass::getPlcData
 */
void modbusTcpClass::getPlcData()
{
//#ifdef Q_OS_UNIX
    if (get_state) return;
    get_state = true;
    bool isRefresh = false;
    int nCmd = 0, targetCmd = 0;

    for (int i = 0; i < struGsh.nTotalMachineModeNum; i++) {

        //! 读实时值
        nCmd = (i+1+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+1;

        m_myModbus->readHoldingRegisters(nCmd, struGsh.nTotalMachineNum);
        gFlow.msleep(60);
        QVector<quint16> verDatas = m_myModbus->values();
        if(verDatas.count() > 0){
            isRefresh = true;
            for(int j = 0; j < verDatas.count(); j++) {
                struGsh.nPLCMachineData[j][i] = verDatas.at(j);
            }
        }

        //! 读目标值
        targetCmd = (i+11+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+1;
        m_myModbus->readHoldingRegisters(targetCmd, struGsh.nTotalMachineNum);
        gFlow.msleep(60);
        QVector<quint16> verTargetDatas = m_myModbus->values();
        if(verTargetDatas.count() > 0){
            isRefresh = true;
            for(int j = 0; j < verTargetDatas.count(); j++) {
                struGsh.nPLCMachineTargetData[j][i] = verTargetDatas.at(j);
            }
        }
    }

    if (isRefresh){

        for (int i = 0; i < struCnfg.analyzeNum; i++) {
            int nMachineNum = gFlow.getSpecifyAnalyzeGlobal(i)->machineNum;
            for (int j = 0; j < nMachineNum; j++) {
                machineInfo  *m_pMachineInfo = gFlow.getSpecifySorter(i, j);
                machineShare *pMachine       = gFlow.getSpecifyMachineShare(i, j);

                int specsType = m_pMachineInfo->modelMachine.model_specs;
                int nRollerNum = specsType == 1?2:1;

                for (int m = 0; m < struGsh.nTotalMachineModeNum; m++) {
                    for (int n = 0; n < nRollerNum; n++) {
                        int machine_index = gFlow.getMachineIndexInServer(i, j, n)-1;
                        if (m_pMachineInfo->modelMachine.model_type == MACHINE_RIDGE_VALLEY) {
                            if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_CURRENT_FAST) {
                                pMachine->n_ControlIndex = m;
                                pMachine->sCurrentFast[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                                double nMin = m_pMachineInfo->modelMachine.sModelValueMin[m];
                                if (pMachine->sCurrentFast[n] < nMin) { //! 砻谷机快辊电流小于5A 不取样
                                    pMachine->isSample = false;
                                } else {
                                    pMachine->isSample = true;
                                }
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_CURRENT_SLOW) {
                                pMachine->sCurrentSlow[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_SPEED_FAST) {
                                pMachine->sLineSpeedFast[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_SPEED_SLOW) {
                                pMachine->sLineSpeedSlow[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                                pMachine->sLineSpeedDiffer[n] = pMachine->sLineSpeedFast[n] - pMachine->sLineSpeedSlow[n];
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_SPEED_DIFFER) {
                                pMachine->sLineSpeedDiffer[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == LONGGU_LABLE_SPEED_SUM) {
                                pMachine->sLineSpeedSum[n]    = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            }
                        } else if (m_pMachineInfo->modelMachine.model_type == MACHINE_POLISH) {
                            if (m_pMachineInfo->modelMachine.sModelLable[m] == POLISH_FLOW) {
                                pMachine->sFlow[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == POLISH_CURRENT) {
                                pMachine->sCurrentFast[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                                double nMin = m_pMachineInfo->modelMachine.sModelValueMin[m];
                                if (pMachine->sCurrentFast[n] <= nMin) {
                                    pMachine->isSample = false;
                                } else {
                                    pMachine->isSample = true;
                                }
                            } else if (m_pMachineInfo->modelMachine.sModelLable[m] == POLISH_PRESSURE) {
                                pMachine->sPressure[n] = struGsh.nPLCMachineData[machine_index][m]*m_pMachineInfo->modelMachine.sModelConvertPixel[m];
                            }
                        }
                    }
                }
            }
        }

        emit updatePLCData();

        gFlow.updateAnalyzeSampleInfo();
    }
    get_state = false;
//#endif
}

/**
 * @brief modbusTcpClass::发送线速差信息到中控
 * @param analyzeNum
 * @param type
 * @param index
 * @param value
 */
void modbusTcpClass::sendMillInfoToPlc()
{
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        analyzeGlobal *pAnalyze = gFlow.getSpecifyAnalyzeGlobal(i);
        int num = pAnalyze->machineNum;
        for (int j = 0; j < num; j++) {
            QVector<int> vData;

            vData.push_back(struGsh.nMillData[26*(num-1-j)+6]);      //! 线速差 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+7]);      //! 设置电流 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+19]);     //! 线速差 右
            vData.push_back(struGsh.nMillData[26*(num-1-j)+20]);     //! 设置电流 右
            vData.push_back(struGsh.nMillData[26*(num-1-j)]);        //! 频率 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+1]);      //! 实际电流 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+2]);      //! 紧辊压力 左

            vData.push_back(struGsh.nMillData[26*(num-1-j)+13]);     //! 频率 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+14]);     //! 实际电流 左
            vData.push_back(struGsh.nMillData[26*(num-1-j)+15]);     //! 紧辊压力 左

            m_myModbus->writeHoldingRegisters(vData, TCP_SPEED_LEFT+j*20);
            gFlow.msleep(10);
        }
    }


}

/**
 * @brief modbusTcpClass::读线速差和设置电流
 * @param analyzeNum
 * @param type
 * @param value
 * @return
 */
void modbusTcpClass::readMillInfoFromPlc(QVector<int> &vData, int nCmd, int index)
{
    m_myModbus->readHoldingRegisters(nCmd,index);
    gFlow.msleep(20);
    QVector<quint16> verDatas = m_myModbus->values();
    if(verDatas.count() > 0) {
        for(int i = 0; i < verDatas.count(); i++) {
            vData.push_back(verDatas.at(i));
            printf("Plc back Data: %d \n", verDatas.at(i));
        }
    }
    fflush(0);
}

/**
 * @brief modbusTcpClass::读砻谷段目标值
 * @param vData
 * @param nCmd
 */
void modbusTcpClass::readTargetData(int type)
{
    bool isRefresh = false;
    int target_type = 0, n_cmd = 0, n_num = 0;
    switch (type) {
    case PLC_LONGGU:
        target_type = A_TARGET_TOTAL;
        n_cmd = TCP_MILL_TARGET;
        n_num = 4;
        break;
    case PLC_PAOGUANG:
        target_type = P_TARGET_TOTAL;
        n_cmd = TCP_POLISH_TARGET;
        n_num = 4;
        break;
    default:
        break;
    }

    m_myModbus->readHoldingRegisters(n_cmd, n_num);
    gFlow.msleep(20);
    QVector<quint16> verDatas = m_myModbus->values();
    if(verDatas.count() > 0) {
        for (int i = 0; i < target_type; i++) {
            if (verDatas[i*2] != struCnfg.nControlTarget[i][TARGET_MIN]) {
                struCnfg.nControlTarget[i][TARGET_MIN] = verDatas[i*2];
                isRefresh = true;
            }
            if (verDatas[i*2+TARGET_MAX] != struCnfg.nControlTarget[i][TARGET_MAX]) {
                struCnfg.nControlTarget[i][TARGET_MAX] = verDatas[i*2+TARGET_MAX];
                isRefresh = true;
            }
        }
    }

    if (isRefresh) {
        emit sigRefreshTargetData();
    }

}

/**
 * @brief modbusTcpClass::读取抛光机数据
 */
void modbusTcpClass::readPolishDataFromPlc(int i, int j, int index, int type)
{
    int num = 4;
    int nCmd = 0;

    switch (type) {
    case POLISH_CURRENT:
        nCmd = TCP_POLISH_CURRENT + index*20;
        break;
    case POLISH_PRESSURE:
        nCmd = TCP_POLISH_PRESSURE + index*20;
        break;
    default:
        break;
    }

    m_myModbus->readHoldingRegisters(nCmd,1);
    gFlow.msleep(10);
    QVector<quint16> verDatas = m_myModbus->values();

    struGsh.nMillData[i*num+j+type*num] = verDatas[0];
}

/**
 * @brief modbusTcpClass::sendSampleInfo
 * @param analyzeIndex
 */
void modbusTcpClass::sendSampleInfo(int analyzeIndex,int nValue)
{
    int nCmd = 0;

    analyzeShare* pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(analyzeIndex);
    int curSorterIndex = pAnalyzeShare->curSorterIndex;
    int curSampleIndex = pAnalyzeShare->curSampleIndex;

    int baseAdr = MACHINE_SAMPLE_INFO+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM;
    nCmd = baseAdr*MACHINE_BASE_ADDR+gFlow.getMachineIndexInServer(analyzeIndex, curSorterIndex, curSampleIndex);

    QVector<int> vData;

    vData.push_back(nValue);

//    qDebug() << "send SampleInfo data nCmd: " << nCmd << " value: " << nValue << "\t\n";
    m_myModbus->writeHoldingRegisters(vData, nCmd);
}

/**
 * @brief modbusTcpClass::发送数据到组态PLC
 * @param nCmd
 * @param value
 */
void modbusTcpClass::sendControlData(int nCmd, int value)
{
    QVector<int> vData;
    vData.push_back(value);
    m_myModbus->writeHoldingRegisters(vData, nCmd);
}

modbusTcpClass* g_modbusTcpClass;
