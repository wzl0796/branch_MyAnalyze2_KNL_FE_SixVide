#include "core/globalflow/globalflow.h"

/**
 * @brief GlobalFlow::判断方案名称存在
 * @param str
 * @return
 */
bool GlobalFlow::findSchmFromList(QString str)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;

    bool bFind = false;
    for(int i = 0; i < schmLst.size(); ++i){
        if(schmLst[i].name.compare(str) == 0){
            bFind = true;
            break;
        }
    }
    return bFind;
}

/**
 * @brief GlobalFlow::删除方案
 * @param index
 */
void GlobalFlow::delSchmFromList(int index)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;
    if(index < schmLst.size())
        schmLst.removeAt(index);
}

/**
 * @brief GlobalFlow::delSchmFromList
 * @param name
 */
void GlobalFlow::delSchmFromList(QString name)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;

    for(int i = 0; i < schmLst.size(); ++i){
        if(schmLst[i].name.compare(name) == 0)
            schmLst.removeAt(i);
    }
}

/**
 * @brief GlobalFlow::重命名方案
 * @param index
 * @param name
 */
void GlobalFlow::renameSchmName(int index, QString name)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;

    if(index < schmLst.size())
        schmLst[index].name = name;
}

/**
 * @brief GlobalFlow::从配置文件中获取方案参数
 * @param fileName
 * @return
 */
bool GlobalFlow::getAnalyzeProfileParams(struProfile *profile, const QString fileName)
{
    QSettings setting(fileName, QSettings::IniFormat);

    for (int i = 0; i < LAMP_TYPE; i++) {
        QString lampname;
        int nval = 200;
        switch(i){
        case 0:
            lampname = myLan.up_bglight;
            nval=10;
            break;
        case 1:
            nval=240;
            lampname = myLan.up_matlight;
            break;
        case 2:
            nval=600;
            lampname = myLan.ultra_light;
            break;
        case 3:
            nval=600;
            lampname = myLan.ultra_light;
            break;
        default:
            lampname = QString("%1%2").arg(myLan.lamp).arg(i+1);
            break;
        }
        profile->nLampValue[i] = setting.value(QString("LampValue_%1").arg(i), nval).toInt();
        profile->nLampName[i] = setting.value(QString("LampName_%1").arg(i), lampname).toString();
        profile->bEnlamp[i] = setting.value(QString("LampEn_%1").arg(i), true).toBool();
    }

    for (int i = 0; i < PLUSE_TYPE; i++) {
        profile->nPluseValue[i] = setting.value(QString("PluseValue_%1").arg(i), 0).toInt();
        profile->nPluseName[i] = setting.value(QString("PluseName_%1").arg(i), QString("%1%2").arg(myLan.lamp).arg(i+5)).toString();
        profile->bEnPluse[i] = setting.value(QString("PluseEn_%1").arg(i), true).toBool();
    }

    profile->bHologenLampOn = setting.value(QString("HologenLampOn"), true).toBool();

    int tmpwave[MAX_WAVE_LINE] = {57, 61, 69, 73, 141, 205, 249, 329, 345, 373, 377, 381};
    for(int i = 0; i < MAX_CAM_NUM; i++){
        int RF=0,GF=0,BF=0,NF=0,RS=30,GS=30,BS=30,NS=30;
        switch(i){
        case 0:
            RF=0;
            GF=0;
            BF=1;
            NF=0;
            RS=50;
            GS=50;
            BS=40;
            NS=30;
            break;
        case 1:
            RF=0;
            GF=5;
            BF=3;
            NF=0;
            RS=5;
            GS=37;
            BS=10;
            NS=30;
            break;
        case 2:
            RF=0;
            GF=0;
            BF=0;
            NF=0;
            RS=30;
            GS=30;
            BS=30;
            NS=30;
            break;
        case 3:
            RF=340;
            GF=0;
            BF=0;
            RS=3;
            GS=30;
            BS=30;
            break;
        default:
            break;
        }

        profile->nGainFast[i][COLOR_TYPE_R] = setting.value(QString("GainFast_%1_1").arg(i+1), RF).toInt();
        profile->nGainFast[i][COLOR_TYPE_G] = setting.value(QString("GainFast_%1_2").arg(i+1), GF).toInt();
        profile->nGainFast[i][COLOR_TYPE_B] = setting.value(QString("GainFast_%1_3").arg(i+1), BF).toInt();
        profile->nGainFast[i][COLOR_TYPE_N] = setting.value(QString("GainFast_%1_4").arg(i+1), NF).toInt();

        profile->nGainSlow[i][COLOR_TYPE_R] = setting.value(QString("GainSlow_%1_1").arg(i+1), RS).toInt();
        profile->nGainSlow[i][COLOR_TYPE_G] = setting.value(QString("GainSlow_%1_2").arg(i+1), GS).toInt();
        profile->nGainSlow[i][COLOR_TYPE_B] = setting.value(QString("GainSlow_%1_3").arg(i+1), BS).toInt();
        profile->nGainSlow[i][COLOR_TYPE_N] = setting.value(QString("GainSlow_%1_4").arg(i+1), NS).toInt();

        for(int j = 0; j < MAX_WAVE_LINE; j++){
            profile->n12WavesVal[i][j] = setting.value(QString("12Wavesval_%1_%2").arg(i).arg(j), tmpwave[j]).toInt();
        }
    }


    profile->nStaisticType = setting.value(QString("staistictype"), 0).toInt();
    profile->ncoffvalue = setting.value(QString("coff"),0.3).toFloat();
    profile->nareathd = setting.value(QString("areathd"),50).toInt();
    profile->noffsetvalue = setting.value(QString("noffset"),130).toInt();
    profile->nStaticGroup = setting.value(QString("statsgp"),4).toInt();
    profile->nMeatal = setting.value(QString("metal"),20).toInt();
    profile->bEnMultilab = setting.value(QString("bmultilab"),false).toBool();

    profile->bEnautoSample = setting.value(QString("autosample"),false).toBool();
    profile->bEnSorterClass = setting.value(QString("bEnsorterclass"),false).toBool();

    for(int i=0; i<MAX_CLASSIFY_NUM; i++){
        profile->nMatclassifythd[i] = setting.value(QString("classifythd_%1").arg(i),(i+1)*10).toInt();
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        profile->slabSecondary[i] = setting.value(QString("slabSecondary_%1").arg(i),QString("")).toString();
    }

    //! 色选联调参数
    profile->strcSortclass.nmaxNum = setting.value(QString("sortmaxnum"),1).toInt();
    profile->strcSortclass.nMaxSpantime = setting.value(QString("nmaxadjtime"),5).toInt();
    profile->strcSortclass.nChannel = setting.value(QString("nChannel"),0).toInt();
    profile->strcSortclass.nLooptimes = setting.value(QString("nLoopspantime"),5).toInt();
    profile->strcSortclass.bScheduled = setting.value(QString("bScheduled"),false).toBool();
    profile->strcSortclass.nScheduletimes = setting.value(QString("nScheduletimes"),5).toInt();

    profile->strcSortclass.vlabParm.clear();
    profile->strcSortclass.vlabParm.resize(MAX_SORTERCLASS_NUM);
    QString vlabParmJson = setting.value(QString("vlabParm")).toString();
    if (!vlabParmJson.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(vlabParmJson.toUtf8());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (int idx = 0; idx < arr.size() && idx < MAX_SORTERCLASS_NUM; ++idx) {
                QJsonValue v = arr.at(idx);
                if (!v.isObject()) continue;
                QJsonObject obj = v.toObject();
                QHash<QString, sorterAdjlabParm> hash;
                for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
                    const QString &label = it.key();
                    QJsonValue pv = it.value();
                    if (!pv.isObject()) continue;
                    QJsonObject pobj = pv.toObject();
                    sorterAdjlabParm parm;
                    parm.nlowthd       = static_cast<float>(pobj.value("nlowthd").toDouble(0.0));
                    parm.nhigthd       = static_cast<float>(pobj.value("nhigthd").toDouble(0.0));
                    parm.bEn           = pobj.value("bEn").toBool(false);
                    parm.nshapeLowthd  = static_cast<float>(pobj.value("nshapeLowthd").toDouble(0.0));
                    parm.nshapeHigthd  = static_cast<float>(pobj.value("nshapeHigthd").toDouble(0.0));
                    parm.bshapeEn      = pobj.value("bshapeEn").toBool(false);
                    hash.insert(label, parm);
                }
                profile->strcSortclass.vlabParm[idx] = hash;
            }
        }
    }

    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        profile->strcSortclass.nMinNumthd[i] = setting.value(QString("sortMinnum_%1").arg(i),1000).toInt();
        profile->strcSortclass.nsourceIdx[i] = setting.value(QString("sourceidx_%1").arg(i),0).toInt();
        profile->strcSortclass.bEnClass[i] = setting.value(QString("sortclassEn_%1").arg(i),false).toBool();
        profile->strcSortclass.bEnACC[i] = setting.value(QString("bEnACC%1").arg(i),false).toBool();
        profile->strcSortclass.nAdjtimes[i] = setting.value(QString("nadjtimes_%1").arg(i),1).toInt();
        profile->strcSortclass.nAdjwaittime[i] = setting.value(QString("nadjwait_%1").arg(i),5).toInt();
        profile->strcSortclass.nsamplespan[i] = setting.value(QString("nsamplespan_%1").arg(i),5).toInt();

        QByteArray array = setting.value(QString("sortname_%1").arg(i),QString("")).toString().toLocal8Bit();
        memcpy(profile->strcSortclass.sortclassname[i], array.data(), array.size());
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        profile->nTotalThd[i] = setting.value(QString("totalthd_%1").arg(i),20).toInt();
    }

    bool bBottle = struCnfg.nMachineType == nMachine_Bottle ? true :false;
    for(int i=0 ;i<MAX_CAM_NUM; i++){
        int ndefaltcap = 384;
        int ndefaltCash = 256;
        if(bBottle){
            switch(i){
            case 0:
                ndefaltcap = 640;
                ndefaltCash = 0;
                break;
            case 1:
                ndefaltcap = 512;
                ndefaltCash = 0;
                break;
            case 2:
                ndefaltcap = 640;
                ndefaltCash = 0;
                break;
            case 3:
                ndefaltcap = 512;
                ndefaltCash = 0;
                break;
            default:
                break;
            }
        }
        profile->nType[i] =setting.value(QString("Megtype_%1").arg(i),1).toInt();
        profile->nCoidx[i] =setting.value(QString("coidx%1").arg(i),i).toInt();
        profile->strctAI.nCapLine[i] =setting.value(QString("capline_%1").arg(i),ndefaltcap).toInt();
        profile->strctAI.nCashLine[i] =setting.value(QString("cashline_%1").arg(i),ndefaltCash).toInt();
    }


    profile->strctAI.nModelLine =setting.value(QString("modeline"),3).toInt();
    int nAIres = struCnfg.nMachineType == nMachine_Bottle ? 2 : 1;
    profile->strctAI.nRes = setting.value(QString("nres"),nAIres).toInt();

    for(int i=0;i<MAX_CAM_NUM;i++){
        profile->strctAI.nModelIdx[i] =setting.value(QString("modelidx_%1").arg(i),0).toInt();
        profile->strctAI.nshapelegthlow[i] =setting.value(QString("legthlow_%1").arg(i),0).toInt();
        profile->strctAI.nshapelegthhig[i] =setting.value(QString("legthhig_%1").arg(i),100).toInt();
        profile->strctAI.nshapewidthlow[i] =setting.value(QString("widlow_%1").arg(i),0).toInt();
        profile->strctAI.nshapewidthhig[i] =setting.value(QString("widhig_%1").arg(i),100).toInt();
        profile->strctAI.bshapeEn[i] =setting.value(QString("shapeEn_%1").arg(i),false).toBool();
        profile->strctAI.bshapewidEn[i] =setting.value(QString("shapewidEn_%1").arg(i),false).toBool();
        profile->strctAI.bshapeAreaEn[i] =setting.value(QString("shapeareaEn_%1").arg(i),false).toBool();
        profile->strctAI.nshapearealow[i] =setting.value(QString("arealow_%1").arg(i),0).toInt();
        profile->strctAI.nshapeareahig[i] =setting.value(QString("areahig_%1").arg(i),100).toInt();

        for(int j=0; j< MAX_LABELS; j++){
            const bool enabledByDefault = j >= TRADITION_ARITH_COUNT;
            profile->strctAI.bEnlabs[i][j] =
                setting.value(QString("bEnlab_%1_%2").arg(i).arg(j), enabledByDefault).toBool();
            profile->strctAI.bShapelabEn[i][j] = setting.value(QString("bshapeEnlab_%1_%2").arg(i).arg(j),false).toBool();
        }

        for(int k=0; k<6*MAX_LABELS; k++){
            profile->strctAI.sens[i][k]  = setting.value(QString("bsens_%1_%2").arg(i).arg(k),50).toInt();
        }
    }

    for (int i = 0; i < TRADITION_ARITH_COUNT; ++i) {
        TraditionAlgorithmParams &options = profile->traditionArith[i];
        options.bEnable = setting.value(QString("traditionArith_%1_benable").arg(i), 0).toBool();
        options.mode = setting.value(QString("traditionArith_%1_mode").arg(i), 0).toInt();
        options.nSens = setting.value(QString("traditionArith_%1_nSens").arg(i), 10).toInt();
        options.bright = setting.value(QString("traditionArith_%1_bright").arg(i), true).toBool();
        options.rowCount = setting.value(QString("traditionArith_%1_rowCount").arg(i), 32).toInt();
        options.badPointCount = setting.value(QString("traditionArith_%1_badPointCount").arg(i), 1024).toInt();
        options.pass = setting.value(QString("traditionArith_%1_pass").arg(i), 0).toInt();
        options.cutEdge = setting.value(QString("traditionArith_%1_cutEdge").arg(i), false).toBool();
        options.independentDelay = setting.value(QString("traditionArith_%1_independentDelay").arg(i), false).toBool();
        options.lowerLimit = setting.value(QString("traditionArith_%1_lowerLimit").arg(i), 0).toInt();
        for (int j = 0; j < 2; ++j) {
            TraditionRetainParams &retain = options.retain[j];
            retain.mode = setting.value(QString("traditionArith_%1_retain_%2_mode").arg(i).arg(j), 0).toInt();
            retain.bright = setting.value(QString("traditionArith_%1_retain_%2_bright").arg(i).arg(j), true).toBool();
            retain.sensitivity = setting.value(QString("traditionArith_%1_retain_%2_sensitivity").arg(i).arg(j), 0).toInt();
            retain.bEn =  setting.value(QString("traditionArith_%1_retain_%2_ben").arg(i).arg(j), false).toBool();
        }
    }

   //! 传统算法

    for (int i = 0; i < MAX_CAM_NUM; ++i) {
        profile->bTraditionLabelNamesInitialized[i] =
            setting.value(QString("traditionLabelNamesInitialized_%1").arg(i), false).toBool();
        profile->traditionLabelNames[i].clear();
        for (int j = 0; j < MAX_LABELS; ++j) {
            profile->traditionLabelNames[i].append(
                setting.value(QString("traditionLabelName_%1_%2").arg(i).arg(j)).toString());
        }
        for (int j = 0; j < MAX_LABELS; ++j) {
            profile->bTraditionLabelEn[i][j] =
                setting.value(QString("traditionLabel_%1_%2_enabled").arg(i).arg(j), false).toBool();
            profile->nTraditionLabelAlgorithm[i][j] =
                qBound(0, setting.value(QString("traditionLabel_%1_%2_algorithm").arg(i).arg(j), 0).toInt(),
                       TRADITION_ARITH_COUNT - 1);
        }
    }

    //! 标签优先级
    for(int n=0; n<MAX_CAM_NUM; n++){
        QMap<QString, int> tmpmap;
        QString mapdata =  setting.value(QString("labmap_%1").arg(n)).toString();
        QVariantMap map = QJsonDocument::fromJson(mapdata.toUtf8()).object().toVariantMap();
        auto i = map.constBegin();
        while (i != map.constEnd()) {
            QVariant val = i.value();
            tmpmap[i.key()] = val.toInt();
            ++i;
        }
        profile->strctAI.vlabelidxmap.push_back(tmpmap);
    }

    //! 默认统计组个数 4
    for(int i =0; i<profile->nStaticGroup; i++){
        QStringList defalist;
        defalist <<QString::number(i);
        QStringList strlist = setting.value(QString("unitaddr_%1").arg(i),defalist).toStringList();
        profile->vUnitaddrlist.push_back(strlist);
    }

    //! 标签阈值
    QMap<QString, float> tmpmap;
    QString mapdata =  setting.value(QString("labthdmap")).toString();
    QVariantMap map = QJsonDocument::fromJson(mapdata.toUtf8()).object().toVariantMap();
    auto i = map.constBegin();
    while (i != map.constEnd()) {
        QVariant val = i.value();
        tmpmap[i.key()] = val.toFloat();
        ++i;
    }
    profile->strctAI.vlabthdmap = tmpmap;

    //! 标签转换系数
    QMap<QString, float> tmpcofmap;
    QString cofmapdata =  setting.value(QString("Area2wcofmap")).toString();
    QVariantMap cofmap = QJsonDocument::fromJson(cofmapdata.toUtf8()).object().toVariantMap();
    auto j = cofmap.constBegin();
    while (j != cofmap.constEnd()) {
        QVariant val = j.value();
        tmpcofmap[j.key()] = val.toFloat();
        ++j;
    }
    profile->Area2weightmap = tmpcofmap;


    //! 标签属性
    QMap<QString, bool> tmpprotymap;
    QString protymapdata =  setting.value(QString("labproperty")).toString();
    QVariantMap potymap = QJsonDocument::fromJson(protymapdata.toUtf8()).object().toVariantMap();
    auto n = potymap.constBegin();
    while (n != potymap.constEnd()) {
        QVariant val = n.value();
        tmpprotymap[n.key()] = val.toBool();
        ++n;
    }
    profile->labprotymap = tmpprotymap;

    //! 标签是否显示
    QMap<QString, bool> tmpshowmap;
    QString showmapdata =  setting.value(QString("blabshowmap")).toString();
    QVariantMap showvarmap = QJsonDocument::fromJson(showmapdata.toUtf8()).object().toVariantMap();
    auto k = showvarmap.constBegin();
    while (k != showvarmap.constEnd()) {
        QVariant val = k.value();
        tmpshowmap[k.key()] = val.toBool();
        ++k;
    }
    profile->blabshowmap = tmpshowmap;

    //! 互斥标签
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray data = setting.value(QString("labMutex_%1").arg(k)).toByteArray();
        QDataStream stream(data);
        QMap<QString, int> resmap;
        stream >> resmap;
        profile->strctAI.vlabMutexmap.push_back(resmap);
    }


    //! 标签合并
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray data = setting.value(QString("labMegrmap_%1").arg(k)).toByteArray();
        QDataStream stream(data);
        QMultiMap<QString, QString> resmap;
        stream >> resmap;
        profile->vMegerlabmap.push_back(resmap);
    }

    //! 标签下表
    profile->vlabIdxmap.clear();
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray data = setting.value(QString("labidxmap_%1").arg(k)).toByteArray();
        QDataStream stream(data);
        QMap<QString, int> idxmap;
        stream >> idxmap;
        profile->vlabIdxmap.push_back(idxmap);
    }

    //! 标签互混映射
    profile->vlabMixmap.clear();
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray data = setting.value(QString("labMixmap_%1").arg(k)).toByteArray();
        QDataStream stream(data);
        QMap<QString, QString> mixmap;
        stream >> mixmap;
        profile->vlabMixmap.push_back(mixmap);
    }

    //! 统计类别是是否显示
    QByteArray staclassdata = setting.value(QString("staclassEn")).toByteArray();
    QDataStream streamstaclass(staclassdata);
    QMap<QString, bool> staclassmap;
    streamstaclass >> staclassmap;
    profile->staclassEnmap = staclassmap;


    if (setting.status() == QSettings::NoError) {
        return true;
    }
    return false;
}

/**
 * @brief GlobalFlow::保存方案参数至配置文件中
 * @param fileName
 * @return
 */
bool GlobalFlow::saveAnalyzeProfileParams(struProfile *profile, const QString fileName)
{
    QSettings setting(fileName, QSettings::IniFormat);

//    for (int i = 0; i < MAX_CAM_NUM; i++) {
//        setting.setValue(QString("ThreadValue_%1").arg(i), profile->nThreadValue[i]);
//    }

    for (int i = 0; i < LAMP_TYPE; i++) {
        setting.setValue(QString("LampValue_%1").arg(i), profile->nLampValue[i]);
        setting.setValue(QString("LampName_%1").arg(i), profile->nLampName[i]);
        setting.setValue(QString("LampEn_%1").arg(i), profile->bEnlamp[i]);
    }

    for (int i = 0; i < PLUSE_TYPE; i++) {
        setting.setValue(QString("PluseValue_%1").arg(i), profile->nPluseValue[i]);
        setting.setValue(QString("PluseName_%1").arg(i), profile->nPluseName[i]);
        setting.setValue(QString("PluseEn_%1").arg(i), profile->bEnPluse[i]);
    }
    setting.setValue(QString("HologenLampOn"), profile->bHologenLampOn);

    for(int i = 0; i < MAX_CAM_NUM; i++){
        setting.setValue(QString("GainFast_%1_1").arg(i+1), profile->nGainFast[i][COLOR_TYPE_R]);
        setting.setValue(QString("GainFast_%1_2").arg(i+1), profile->nGainFast[i][COLOR_TYPE_G]);
        setting.setValue(QString("GainFast_%1_3").arg(i+1), profile->nGainFast[i][COLOR_TYPE_B]);
        setting.setValue(QString("GainFast_%1_4").arg(i+1), profile->nGainFast[i][COLOR_TYPE_N]);

        setting.setValue(QString("GainSlow_%1_1").arg(i+1), profile->nGainSlow[i][COLOR_TYPE_R]);
        setting.setValue(QString("GainSlow_%1_2").arg(i+1), profile->nGainSlow[i][COLOR_TYPE_G]);
        setting.setValue(QString("GainSlow_%1_3").arg(i+1), profile->nGainSlow[i][COLOR_TYPE_B]);
        setting.setValue(QString("GainSlow_%1_4").arg(i+1), profile->nGainSlow[i][COLOR_TYPE_N]);


        for(int j = 0; j < MAX_WAVE_LINE; j++){
            setting.setValue(QString("12Wavesval_%1_%2").arg(i).arg(j), profile->n12WavesVal[i][j]);
        }
    }

    setting.setValue(QString("staistictype"), profile->nStaisticType);
    setting.setValue(QString("coff"), profile->ncoffvalue);
    setting.setValue(QString("areathd"), profile->nareathd);
    setting.setValue(QString("noffset"), profile->noffsetvalue);
    setting.setValue(QString("statsgp"), profile->nStaticGroup);
    setting.setValue(QString("metal"), profile->nMeatal);
    setting.setValue(QString("bmultilab"), profile->bEnMultilab);

    setting.setValue(QString("autosample"), profile->bEnautoSample);
    setting.setValue(QString("bEnsorterclass"), profile->bEnSorterClass);

    for(int i=0; i<MAX_CLASSIFY_NUM; i++){
        setting.setValue(QString("classifythd_%1").arg(i), profile->nMatclassifythd[i]);
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        setting.setValue(QString("slabSecondary_%1").arg(i), profile->slabSecondary[i]);
    }

    //! 色选联调参数
    setting.setValue(QString("sortmaxnum"), profile->strcSortclass.nmaxNum);
    setting.setValue(QString("nmaxadjtime"), profile->strcSortclass.nMaxSpantime);
    setting.setValue(QString("nChannel"), profile->strcSortclass.nChannel);
    setting.setValue(QString("nLoopspantime"), profile->strcSortclass.nLooptimes);
    setting.setValue(QString("bScheduled"), profile->strcSortclass.bScheduled);
    setting.setValue(QString("nScheduletimes"), profile->strcSortclass.nScheduletimes);

    QJsonArray arr;
    int vecSize = profile->strcSortclass.vlabParm.size();
    for (int idx = 0; idx < MAX_SORTERCLASS_NUM; ++idx) {
        QJsonObject obj;
        if (idx < vecSize) {
            const QHash<QString, sorterAdjlabParm> &hash = profile->strcSortclass.vlabParm.at(idx);
            for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
                QJsonObject pobj;
                const sorterAdjlabParm &parm = it.value();
                pobj.insert("nlowthd", parm.nlowthd);
                pobj.insert("nhigthd", parm.nhigthd);
                pobj.insert("bEn", parm.bEn);
                pobj.insert("nshapeLowthd", parm.nshapeLowthd);
                pobj.insert("nshapeHigthd", parm.nshapeHigthd);
                pobj.insert("bshapeEn", parm.bshapeEn);
                obj.insert(it.key(), pobj);
            }
        }
        arr.append(obj);
    }
    QJsonDocument doc(arr);
    setting.setValue(QString("vlabParm"), QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        setting.setValue(QString("sortMinnum_%1").arg(i), profile->strcSortclass.nMinNumthd[i]);
        setting.setValue(QString("sourceidx_%1").arg(i),    profile->strcSortclass.nsourceIdx[i]);
        setting.setValue(QString("sortclassEn_%1").arg(i),  profile->strcSortclass.bEnClass[i]);
        setting.setValue(QString("bEnACC%1").arg(i),  profile->strcSortclass.bEnACC[i]);
        setting.setValue(QString("nadjtimes_%1").arg(i),    profile->strcSortclass.nAdjtimes[i]);
        setting.setValue(QString("nadjwait_%1").arg(i),     profile->strcSortclass.nAdjwaittime[i]);
        setting.setValue(QString("nsamplespan_%1").arg(i),  profile->strcSortclass.nsamplespan[i]);

        QString name = QString::fromLocal8Bit(profile->strcSortclass.sortclassname[i]);
        setting.setValue(QString("sortname_%1").arg(i), name);
    }

    for(int i=0; i<MAX_CAM_NUM;i++){
        setting.setValue(QString("totalthd_%1").arg(i), profile->nTotalThd[i]);
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        setting.setValue(QString("Megtype_%1").arg(i), profile->nType[i]);
        setting.setValue(QString("coidx%1").arg(i), profile->nCoidx[i]);
        setting.setValue(QString("capline_%1").arg(i), profile->strctAI.nCapLine[i]);
        setting.setValue(QString("cashline_%1").arg(i), profile->strctAI.nCashLine[i]);
    }


    setting.setValue(QString("modeline"), profile->strctAI.nModelLine);
    setting.setValue(QString("nres"), profile->strctAI.nRes);

    for(int i=0;i<MAX_CAM_NUM;i++){
        setting.setValue(QString("modelidx_%1").arg(i), profile->strctAI.nModelIdx[i]);
        setting.setValue(QString("legthlow_%1").arg(i), profile->strctAI.nshapelegthlow[i]);
        setting.setValue(QString("legthhig_%1").arg(i), profile->strctAI.nshapelegthhig[i]);
        setting.setValue(QString("widlow_%1").arg(i),   profile->strctAI.nshapewidthlow[i]);
        setting.setValue(QString("widhig_%1").arg(i),   profile->strctAI.nshapewidthhig[i]);
        setting.setValue(QString("shapeEn_%1").arg(i),  profile->strctAI.bshapeEn[i]);
        setting.setValue(QString("shapewidEn_%1").arg(i), profile->strctAI.bshapewidEn[i]);
        setting.setValue(QString("shapeareaEn_%1").arg(i), profile->strctAI.bshapeAreaEn[i]);
        setting.setValue(QString("arealow_%1").arg(i), profile->strctAI.nshapearealow[i]);
        setting.setValue(QString("areahig_%1").arg(i), profile->strctAI.nshapeareahig[i]);

        for(int j=0; j<MAX_LABELS; j++){
            setting.setValue(QString("bEnlab_%1_%2").arg(i).arg(j), profile->strctAI.bEnlabs[i][j]);
            setting.setValue(QString("bshapeEnlab_%1_%2").arg(i).arg(j), profile->strctAI.bShapelabEn[i][j]);
        }

        for(int k=0; k<6*MAX_LABELS; k++){
            setting.setValue(QString("bsens_%1_%2").arg(i).arg(k), profile->strctAI.sens[i][k]);
        }
    }

    for (int i = 0; i < TRADITION_ARITH_COUNT; ++i) {
        const TraditionAlgorithmParams &options = profile->traditionArith[i];
        setting.setValue(QString("traditionArith_%1_benable").arg(i), options.bEnable);
        setting.setValue(QString("traditionArith_%1_mode").arg(i), options.mode);
        setting.setValue(QString("traditionArith_%1_nSens").arg(i), options.nSens);
        setting.setValue(QString("traditionArith_%1_bright").arg(i), options.bright);
        setting.setValue(QString("traditionArith_%1_rowCount").arg(i), options.rowCount);
        setting.setValue(QString("traditionArith_%1_badPointCount").arg(i), options.badPointCount);
        setting.setValue(QString("traditionArith_%1_pass").arg(i), options.pass);
        setting.setValue(QString("traditionArith_%1_cutEdge").arg(i), options.cutEdge);
        setting.setValue(QString("traditionArith_%1_independentDelay").arg(i), options.independentDelay);
        setting.setValue(QString("traditionArith_%1_lowerLimit").arg(i), options.lowerLimit);
        for (int j = 0; j < 2; ++j) {
            const TraditionRetainParams &retain = options.retain[j];
            setting.setValue(QString("traditionArith_%1_retain_%2_mode").arg(i).arg(j), retain.mode);
            setting.setValue(QString("traditionArith_%1_retain_%2_bright").arg(i).arg(j), retain.bright);
            setting.setValue(QString("traditionArith_%1_retain_%2_sensitivity").arg(i).arg(j), retain.sensitivity);
            setting.setValue(QString("traditionArith_%1_retain_%2_ben").arg(i).arg(j), retain.bEn);
        }
    }
    //！ 传统算法
    for (int i = 0; i < MAX_CAM_NUM; ++i) {
        setting.setValue(QString("traditionLabelNamesInitialized_%1").arg(i),
                         profile->bTraditionLabelNamesInitialized[i]);
        for (int j = 0; j < MAX_LABELS; ++j) {
            setting.setValue(QString("traditionLabelName_%1_%2").arg(i).arg(j),
                             j < profile->traditionLabelNames[i].size()
                                 ? profile->traditionLabelNames[i].at(j) : QString());
        }
        for (int j = 0; j < MAX_LABELS; ++j) {
            setting.setValue(QString("traditionLabel_%1_%2_enabled").arg(i).arg(j),
                             profile->bTraditionLabelEn[i][j]);
            setting.setValue(QString("traditionLabel_%1_%2_algorithm").arg(i).arg(j),
                             profile->nTraditionLabelAlgorithm[i][j]);
        }
    }

    //! 标签优先级
    for(int i=0; i<MAX_CAM_NUM; i++){
        QMap<QString,QVariant> tmpmap;
        QList<QString> keylist = profile->strctAI.vlabelidxmap[i].keys();
        for(int j=0 ; j<keylist.size(); j++){
            tmpmap[keylist.at(j)] = profile->strctAI.vlabelidxmap[i][keylist.at(j)];
        }
        QByteArray mapdata = QJsonDocument::fromVariant(QVariant::fromValue(tmpmap)).toJson(QJsonDocument::Compact);
        setting.setValue(QString("labmap_%1").arg(i), QString(mapdata));
    }

    //! 标签阈值
    QMap<QString,QVariant> tmpmap;
    QList<QString> keylist = profile->strctAI.vlabthdmap.keys();
    for(int j=0 ; j<keylist.size(); j++){
        tmpmap[keylist.at(j)] = profile->strctAI.vlabthdmap[keylist.at(j)];
    }
    QByteArray mapdata = QJsonDocument::fromVariant(QVariant::fromValue(tmpmap)).toJson(QJsonDocument::Compact);
    setting.setValue(QString("labthdmap"), QString(mapdata));

    //! 标签转换系数
    QMap<QString,QVariant> coftmpmap;
    QList<QString> cofkeylist = profile->Area2weightmap.keys();
    for(int j=0 ; j<cofkeylist.size(); j++){
        coftmpmap[cofkeylist.at(j)] = profile->Area2weightmap[cofkeylist.at(j)];
    }
    QByteArray cofmapdata = QJsonDocument::fromVariant(QVariant::fromValue(coftmpmap)).toJson(QJsonDocument::Compact);
    setting.setValue(QString("Area2wcofmap"), QString(cofmapdata));

    //! 标签属性
    QMap<QString,QVariant> tmpprotymap;
    QList<QString> protykeylist = profile->labprotymap.keys();
    for(int j=0 ; j<protykeylist.size(); j++){
        tmpprotymap[protykeylist.at(j)] = profile->labprotymap[protykeylist.at(j)];
    }
    QByteArray protymapdata = QJsonDocument::fromVariant(QVariant::fromValue(tmpprotymap)).toJson(QJsonDocument::Compact);
    setting.setValue(QString("labproperty"), QString(protymapdata));

    //! 标签是否显示
    QMap<QString,QVariant> tmpshowmap;
    QList<QString> showkeylist = profile->blabshowmap.keys();
    for(int j=0 ; j<showkeylist.size(); j++){
        tmpshowmap[showkeylist.at(j)] = profile->blabshowmap[showkeylist.at(j)];
    }
    QByteArray showmapdata = QJsonDocument::fromVariant(QVariant::fromValue(tmpshowmap)).toJson(QJsonDocument::Compact);
    setting.setValue(QString("blabshowmap"), QString(showmapdata));


    for(int i= 0; i<profile->vUnitaddrlist.size();i++){
        setting.setValue(QString("unitaddr_%1").arg(i), profile->vUnitaddrlist[i]);
    }

    //! 合并标签保存
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray arraydata;
        QDataStream steam(&arraydata,QIODevice::WriteOnly);
        steam << profile->vMegerlabmap[k];
        setting.setValue(QString("labMegrmap_%1").arg(k),arraydata);
    }

    //! 互斥标签
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray arraydata;
        QDataStream steam(&arraydata,QIODevice::WriteOnly);
        steam << profile->strctAI.vlabMutexmap[k];
        setting.setValue(QString("labMutex_%1").arg(k),arraydata);
    }

    //! 标签序号
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray arraydata;
        QDataStream steam(&arraydata,QIODevice::WriteOnly);
        steam << profile->vlabIdxmap[k];
        setting.setValue(QString("labidxmap_%1").arg(k),arraydata);
    }

    //! 标签互混映射
    for(int k=0; k<MAX_CAM_NUM; k++){
        QByteArray arraydata;
        QDataStream steam(&arraydata,QIODevice::WriteOnly);
        if (k < profile->vlabMixmap.size()) {
            steam << profile->vlabMixmap[k];
        }
        setting.setValue(QString("labMixmap_%1").arg(k),arraydata);
    }

    //! 统计类别使能
    QByteArray staclassdata;
    QDataStream staclasssteam(&staclassdata,QIODevice::WriteOnly);
    staclasssteam << profile->staclassEnmap;
    setting.setValue(QString("staclassEn"),staclassdata);

    setting.sync();

    if (setting.status() == QSettings::NoError) {
        return true;
    }
    return false;
}


/**
 * @brief GlobalFlow::发送所有方案参数
 */
void GlobalFlow::sendProfileParams()
{
    //! 1、发送灯光参数
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        sendLampPara(i);
        sendPlusePara(i);
    }
}
