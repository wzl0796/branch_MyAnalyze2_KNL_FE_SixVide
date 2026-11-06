#include "myprocess.h"
#include "core/globalflow/sqliteinter.h"
#include "common/mymessagebox.h"

QList<MyProcess*>  g_Process;

MyProcess::MyProcess()
{
    m_bTimeout = false;
    bFinishLoop = false;
    bRunning = false;
    nLastOutPut = 0;
    isSendSVCmd = false;
    nWipeCnt = 0;
    nLastRecycleNum = 0;
}

MyProcess::~MyProcess()
{
    quit();
    wait();
}

/**
 * @brief MyProcess::设置分析仪序号
 * @param analyze
 */
void MyProcess::setAnalyzeIndex(int analyze)
{
    m_analyzeIndex = analyze;
    m_pAnalyzeShare  = gFlow.getSpecifyAnalyzeShare(m_analyzeIndex);
    m_pAnalyzeGlobal = gFlow.getSpecifyAnalyzeGlobal(m_analyzeIndex);
}

/**
 * @brief MyProcess::开始运行
 */
void MyProcess::run()
{

    bFinishLoop = false;
    bRunning = true;
    bool bMachineBottle = struCnfg.nMachineType == nMachine_Bottle? true : false;
    auto *pProfileParms = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

    bool bResult = false;
    QString keyname;
    const auto &matInfo = struGsh.vsMatinfo[0];
    for(const QString &title : matInfo.staInfo.keys()) {
        QString cattypestr = matInfo.staInfo[title];
        if(cattypestr.contains("M0/A1") &&
                matInfo.staRes.contains(title)){
                keyname = title;
                bResult = true;
                break;
        }
    }

    if(bMachineBottle && struGsh.vlabTotalmap.size()>0){
        auto& totalMap = struGsh.vlabTotalmap[0];
        for (auto it = totalMap.begin(); it != totalMap.end(); ++it) {
            const QString& var = it.key();
            QStringList lst = var.split("/");
            if (lst.size() < 2) continue;
            int nstatype = lst[1].mid(1).toInt();

            if (nstatype == STATISTIC_A3){
                keyname = var;
                bResult = true;
                break;
            }
        }
    }

    while(bRunning){
        if(pProfileParms->bEnClass[nCounter]){

            struGsh.nSorterAdjIdx = nCounter;

            //! 取样前清灰
            if(bclean){
                struGsh.bStatics = false;
                bclean = false;

                gFlow.snvchannelonoffctrl(0);//! 关电磁阀

                gFlow.sendFeederPara(struGsh.nAnalyzeIndex, true, false);//! 关振动器
                msleep(300);

                gFlow.sendWipePara(1, struGsh.nAnalyzeIndex); //! 清灰加风刀控制
                if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel != 0){
                    quint8 knifedata = 0;
                    knifedata = (1<< (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel -1));
                    gFlow.snvchannelonoffctrl(knifedata);
                }

                msleep(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeDelay);

                gFlow.sendFeederPara(struGsh.nAnalyzeIndex, true, true); //! 开振动器

                struGsh.bStatics = true;

                // qDebug()<<"====== clean and blow";
            }

            //! 电磁阀开关控制
            int nchanel = pProfileParms->nChannel;
            if(m_bFirst[nCounter]){
                struGsh.bStatics = false;

                quint8 data = 0;
                data = data | (1<<nCounter);
                if(nchanel!=0){
                   data = data |(1<<(nchanel-1));
                }
                gFlow.snvchannelonoffctrl(data);
                m_bFirst[nCounter] = false;
                QThread::msleep(pProfileParms->nAdjwaittime[nCounter]*1000);//! 取样延迟

                struGsh.bStatics = true;
                emit sigautosamplesta(nCounter,false);
                // qDebug()<<" =====  open value";
            }


            int nLowthd = 0;
            if(bMachineBottle){//! 瓶片
                if(bResult){
                    nLowthd = struGsh.vlabTotalmap[0][keyname];
                    if(pProfileParms->bEnACC[nCounter]){
                        nLowthd -=m_accLastCount;
                    }

                    // qDebug("nLowthd=%d,  ncounter=%d  nMinNumthd[nCounter]=%d",
                    //         nLowthd, nCounter, pProfileParms->nMinNumthd[nCounter]);
                }
            }else{
                if(bResult){
                    nLowthd =(int) struGsh.vsMatinfo[0].staRes[keyname][0];
                    if(pProfileParms->bEnACC[nCounter]){
                        nLowthd -=m_accLastCount;
                    }
                //    qDebug("**********  nLowthd=%d,   ncounter=%d  nMinNumthd[nCounter]=%d",
                //           nLowthd, nCounter, pProfileParms->nMinNumthd[nCounter]);
                }
            }

            if(nLowthd < pProfileParms->nMinNumthd[nCounter]
                && m_Maxthdtime.elapsed()<pProfileParms->nMaxSpantime*60000){//! 仅支持一个统计组
                    if(!bRunning){
                        break;
                    }
                    if(pProfileParms->bScheduled){  //! 关电磁阀
                        gFlow.snvchannelonoffctrl(0);
                        QThread::msleep(1000*pProfileParms->nScheduletimes);
                        quint8 data = 0;
                        data = data | (1<<nCounter);
                        if(nchanel!=0){
                            data = data |(1<<(nchanel-1));
                        }
                        gFlow.snvchannelonoffctrl(data);
                        QThread::msleep(1000*pProfileParms->nScheduletimes);
                    }else{
                        QThread::msleep(1000);
                    }
                continue;
            }else if(nLowthd >=pProfileParms->nMinNumthd[nCounter]){
                quint8 data1 = 0;
                // if(nchanel!=0){
                //    data1 = data1 |(1<<(nchanel-1));
                // }
                gFlow.snvchannelonoffctrl(data1);


                if(!bMachineBottle && bResult){
                    float nCurrentnum = 0.0;
                    do{
                        nCurrentnum = struGsh.vsMatinfo[0].staRes[keyname][0];
                        if(!bRunning){
                             break;
                        }
                        QThread::msleep(5000);
                    }while((struGsh.vsMatinfo[0].staRes[keyname][0] - nCurrentnum >10.0)&&bRunning);
                }else if(bMachineBottle && bResult){
                    float nCurrentnum = 0.0;
                    do{
                        nCurrentnum = struGsh.vlabTotalmap[0][keyname];
                        if(!bRunning){
                            break;
                        }
                        QThread::msleep(5000);
                    }while((struGsh.vlabTotalmap[0][keyname] - nCurrentnum >6000.0)&&bRunning);
                }

            //    qDebug()<<"========= wait feeder finish";


                if(pProfileParms->bEnACC[nCounter]){
//                       qDebug()<<"ACC model ncurrent Channel="<<nCounter
//                              <<"nAdjtimes="<<nAdjtimes
//                             <<"pProfileParms->nAdjtimes[nCounter]"<<pProfileParms->nAdjtimes[nCounter];
                    if(bMachineBottle){
                        m_accLastCount = struGsh.vlabTotalmap[0][keyname];
                    }else{
                        m_accLastCount = struGsh.vsMatinfo[0].staRes[keyname][0];
                    }

                    if((!m_bTimeout) && (nAdjtimes >= pProfileParms->nAdjtimes[nCounter]-1)){
                        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
                            gFlow.sendStaticsrestoSorter();
                        }

//                           qDebug()<<"****** acc to reacord db";
                        struGsh.batchname = pProfileParms->sortclassname[nCounter];
                        gFlow.recordStatusToDB();  //! 当次化验结果写到数据库

                        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                            emit sigUploadWeb();
                        }
                    }
                }else{
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
                        gFlow.sendStaticsrestoSorter();
                    }

                    struGsh.batchname = pProfileParms->sortclassname[nCounter];
                    gFlow.recordStatusToDB();

                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                        emit sigUploadWeb();
                    }
                }

                if(m_nNormalChanel == nCounter){
                    struGsh.bAutosampleAlarm = false;
                    m_nNormalChanel = -1;
                }

            }else if(m_Maxthdtime.elapsed() >= pProfileParms->nMaxSpantime*60000){
            //    qDebug()<<"******* sample time out nchannel="<<nCounter;

                m_bTimeout = true;

                quint8 data1 = 0;
                // if(nchanel!=0){
                //    data1 = data1 |(1<<(nchanel-1));
                // }
                gFlow.snvchannelonoffctrl(data1);//! 关闭电磁阀的命令

                if(pProfileParms->bEnACC[nCounter]){
                    if(bMachineBottle){
                        m_accLastCount = struGsh.vlabTotalmap[0][keyname];
                    }else{
                        m_accLastCount = struGsh.vsMatinfo[0].staRes[keyname][0];
                    }
                }

                if(m_nNormalChanel == -1){
                    struGsh.bAutosampleAlarm = true;
                    m_nNormalChanel = nCounter;
                }
                 if(!bRunning){
                     break;
                }
                QThread::msleep(1000);
            }

            nAdjtimes += 1;
            m_bFirst[nCounter] = true;
            if(nAdjtimes >= pProfileParms->nAdjtimes[nCounter]){
                if(!bRunning){
                    break;
                }
                QThread::msleep(pProfileParms->nsamplespan[nCounter]*1000);//! 次与次之间等待延迟

                nCounter +=1;
                if(nCounter >= pProfileParms->nmaxNum){
                    nCounter = 0;
                    bFinishLoop = true;
                }
                nAdjtimes = 0;
            }
        }else{
            nCounter +=1;
            if(nCounter >= pProfileParms->nmaxNum){
                bFinishLoop = true;
                nCounter = 0;
            }
        }

        bclean = true;

        struGsh.bStatics = false; //! 清空统计数值时，不进行计数和刷新
        if(pProfileParms->bEnACC[nCounter]){
            if(nAdjtimes == 0){
                gFlow.resetStats();
                m_accLastCount = 0;
                m_bTimeout = false;
//              qDebug()<<"ACC reset";
            }
        }else{
            gFlow.resetStats();
        //  qDebug()<<"reset";
        }
        emit sigautosamplesta(nCounter,true);

        if(!bRunning){
            break;
        }

        if(bFinishLoop){
            bFinishLoop = false;

            gFlow.snvchannelonoffctrl(0);//! 关电磁阀
            msleep(50);
            gFlow.sendFeederPara(struGsh.nAnalyzeIndex, true, false);//! 关振动器  
            msleep(50);
            g_serial->SwMotorOnOff(false);  //! 关伺服
            msleep(pProfileParms->nLooptimes*1000*60);
            g_serial->SwMotorOnOff(true);  //! 开伺服
        }else{
            msleep(1000);
        }

        m_Maxthdtime.restart();
    }
}

/**
 * @brief MyProcess::定时器开始
 */
void MyProcess::startTimerSlt()
{
    m_bTimeout = false;
    m_accLastCount = 0;
    bFinishLoop = false;
    bRunning = true;
    nCounter = 0;  //! 记录次
    struGsh.nSorterAdjIdx = 0;
    m_Maxthdtime.restart(); //! 记录时间
    bclean = true;
    struGsh.bStatics = false;
    m_nNormalChanel = -1;
    struGsh.bAutosampleAlarm = false;

    for(int i=0; i<6; i++){
       m_bFirst[i] = true;
    }
}

/**
 * @brief MyProcess::定时器停止
 */
void MyProcess::stopTimerSlt()
{
    bRunning = false;
}




