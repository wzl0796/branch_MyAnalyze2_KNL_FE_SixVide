#include "analyzetestdlg.h"
#include "core/globalflow/globalflow.h"

AnalyzeTestDlg::AnalyzeTestDlg(int analyze, QWidget *parent) : QWidget(parent) {
  createPage();

  connectSigAndSlot();
}

/**
 * @brief AnalyzeTestDlg::createPage
 */
void AnalyzeTestDlg::createPage() {
  //! 实时统计页面
  createRealWidget();

  //! 仿真页面
  createSimWidget();

  m_staticwidChgstck = new QStackedWidget();
  m_staticwidChgstck->addWidget(m_realWidget);
  m_staticwidChgstck->addWidget(m_camStatScroll);
  m_staticwidChgstck->setCurrentIndex(0);

  m_widget = new QWidget();
  m_widget->setStyleSheet("background-color:white; border-radius:10px;");

  QVBoxLayout *dataLay = new QVBoxLayout(m_widget);
  dataLay->addWidget(m_staticwidChgstck);

  QHBoxLayout *laymain = new QHBoxLayout(this);
  laymain->addWidget(m_widget);
}

/**
 * @brief AnalyzeTestDlg::connectSigAndSlot
 */
void AnalyzeTestDlg::connectSigAndSlot() {
  connect(m_viewIdBtn, SIGNAL(pressed()), this, SLOT(onRefreshStatsGpRes()));
  connect(m_mergerColorMaterialBtn, SIGNAL(clicked(bool)), this,
          SLOT(onMergerColorMatChgSlt(bool)));
}

/**
 * @brief AnalyzeTestDlg::搭建实时显示界面
 */
void AnalyzeTestDlg::createRealWidget() {

  m_realWidget = new QWidget();

  m_detectLbl = new QLabel(QString("%1:").arg(myLan.total));
  m_detectLbl->setFixedHeight(BTN_HEI - 10);
  m_detectLbl->setFixedWidth(1600);

  m_metaititleLb = new QLabel(QString("%1:").arg(myLan.metal));
  m_metaititleLb->setFixedSize(200, BTN_HEI - 10);

  m_viewIdBtn = new QPushButton();
  m_viewIdBtn->setFixedSize(BTN_WID + 70, BTN_HEI - 10);
  m_viewIdBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
  m_viewIdBtn->setIconSize(QSize(60, 60));

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  if (pProfile->vUnitaddrlist.size() > 0 &&
      !pProfile->vUnitaddrlist[0].isEmpty()) {
    int nCamId = pProfile->vUnitaddrlist[0].at(0).toInt();
    QString strname =
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nCamId];
    m_viewIdBtn->setText(strname);
  }

  m_mergerColorMaterialBtn = new QPushButton(myLan.color_and_material);
  m_mergerColorMaterialBtn->setFixedSize(BTN_WID + 70, BTN_HEI - 10);
  m_mergerColorMaterialBtn->setCheckable(true);
  m_mergerColorMaterialBtn->setStyleSheet("QPushButton {"
                                          "  background-color: white;"
                                          "  color: black;"
                                          "  border: 2px solid #dfdfdf;"
                                          "  border-radius: 10px;"
                                          "  padding: 10px;"
                                          "  outline: none;"
                                          "  font: 26px \"Microsoft YaHei\";"
                                          "}"
                                          "QPushButton:checked {"
                                          "  background-color: #007454;"
                                          "  color: white;"
                                          "  border: 2px solid #007454;"
                                          "}"
                                          "QPushButton:!checked {"
                                          "  background-color: white;"
                                          "  color: black;"
                                          "  border: 2px solid #dfdfdf;"
                                          "}"
                                          "QPushButton:pressed {"
                                          "  padding-left: 3px;"
                                          "  padding-top: 3px;"
                                          "}");

  bool bMergerColorMat =
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial;
  m_mergerColorMaterialBtn->setChecked(bMergerColorMat);

  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_mergerColorMaterialBtn->show();
  } else {
    m_mergerColorMaterialBtn->hide();
  }

  m_Hline = new QFrame();
  m_Hline->setFixedHeight(2);
  m_Hline->setFrameShape(QFrame::HLine);
  m_Hline->setFrameShadow(QFrame::Sunken);
  m_Hline->setStyleSheet(QString("background-color:%1;").arg(g_ColorLightGray));

  QString trestyle = QString(
      "QTreeWidget::item{height:50px; text-align: left;}"
      "QTreeWidget QHeaderView::section{text-align:left; padding-left:4px;"
      "font-weight: bold; font-size: 30px;}"
      "QTreeWidget {alternate-background-color: #E0E0E0;}"
      "QTreeView::indicator{width:40px;height:50px}"
      "QTreeView::branch{width:40px;height:50px}");

  m_labtreewid = new QTreeWidget();
  m_labtreewid->setAlternatingRowColors(true);
  m_labtreewid->setStyleSheet(trestyle);
  m_labtreewid->setIndentation(40);

  QHBoxLayout *upLay = new QHBoxLayout();
  upLay->addWidget(m_metaititleLb);
  upLay->addWidget(m_detectLbl);
  upLay->addStretch();
  upLay->addWidget(m_viewIdBtn);
  upLay->addWidget(m_mergerColorMaterialBtn);

  QVBoxLayout *realLay = new QVBoxLayout(m_realWidget);
  realLay->addLayout(upLay);
  realLay->addWidget(m_Hline);
  realLay->addWidget(m_labtreewid);
}

void AnalyzeTestDlg::createSimWidget() {

  m_camStatContainer = new QWidget();
  m_camStatLayout = new QVBoxLayout(m_camStatContainer);
  m_camStatLayout->setSpacing(12);
  m_camStatLayout->setContentsMargins(6, 6, 6, 6);

  m_camStatScroll = new QScrollArea();
  m_camStatScroll->setWidget(m_camStatContainer);
  m_camStatScroll->setWidgetResizable(true);

  m_planEdit = new QPlainTextEdit();
  QFont fontsz = m_planEdit->font();
  fontsz.setPixelSize(24);
  m_planEdit->setFont(fontsz);
  m_camStatLayout->addWidget(m_planEdit);
  m_planEdit->setReadOnly(true);
}

/*!
 * \brief 槽：切换统计组
 */
void AnalyzeTestDlg::onRefreshStatsGpRes() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  bool bEnmerger =
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial;

  m_StatusGpId += 1;
  if (m_StatusGpId >= pProfile->nStaticGroup ||
      (bEnmerger && m_StatusGpId >= pProfile->nStaticGroup - 1)) {
    m_StatusGpId = 0;
  }

  QString strname;
  if (m_StatusGpId < pProfile->vUnitaddrlist.size() &&
      !pProfile->vUnitaddrlist[m_StatusGpId].isEmpty()) {
    int nCamId =
        QString("%1").arg(pProfile->vUnitaddrlist[m_StatusGpId].at(0)).toInt();
    strname = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nCamId];
  }

  m_viewIdBtn->setText(strname);
  refreshStatusItem(true);
}

void AnalyzeTestDlg::onRefreshSimRes(QString str) {
  m_planEdit->appendPlainText(str);
}

/*!
 * \brief 槽：颜色材质合并开关
 * \param checked
 */
void AnalyzeTestDlg::onMergerColorMatChgSlt(bool checked) {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

  if (checked) {
    int nLastGroupIdx = pProfile->nStaticGroup - 1;
    bool bEmpty = false;

    if (pProfile->nStaticGroup <= 0 || nLastGroupIdx < 0 ||
        nLastGroupIdx >= pProfile->vUnitaddrlist.size() ||
        pProfile->vUnitaddrlist[nLastGroupIdx].isEmpty()) {
      bEmpty = true;
    } else {
      for (const QString &strcamid : pProfile->vUnitaddrlist[nLastGroupIdx]) {
        int nCamIdx = strcamid.toInt();
        if (pProfile->slabSecondary[nCamIdx].trimmed().isEmpty()) {
          bEmpty = true;
          break;
        }
      }
    }

    if (bEmpty) {
      if (infoWidget) {
        infoWidget->raise();
        infoWidget->setLabelText(myLan.set_majorProduct_first);
        infoWidget->showSecs();
      }
      m_mergerColorMaterialBtn->setChecked(false);
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial = false;
      gFlow.saveSpecifyAnalyze(struGsh.nAnalyzeIndex);
      return;
    }
  }

  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial = checked;
  gFlow.saveSpecifyAnalyze(struGsh.nAnalyzeIndex);

  if (checked && m_StatusGpId == pProfile->nStaticGroup - 1) {
    onRefreshStatsGpRes();
  } else {
    refreshStatusItem();
  }
}

/**
 * @brief AnalyzeTestDlg::刷新饼状图&柱状图显示
 */
void AnalyzeTestDlg::refreshBarView() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  const bool isMergerColorMat =
      (struCnfg.nMachineType == nMachine_Bottle) &&
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial &&
      (m_StatusGpId == 0) && (pProfile->nStaticGroup >= 2) &&
      (pProfile->vUnitaddrlist.size() >= pProfile->nStaticGroup);
  if (isMergerColorMat) {
    refreshBarViewMerger();
    return;
  }

  //! 刷新物料统计
  refreshMatStaRes();

  //! 确保 m_StatusGpId 在有效范围内
  if (m_StatusGpId < 0 || m_StatusGpId >= pProfile->nStaticGroup ||
      m_StatusGpId >= pProfile->vUnitaddrlist.size() ||
      pProfile->vUnitaddrlist[m_StatusGpId].isEmpty()) {
    return;
  }

  const int ncamid =
      QString(pProfile->vUnitaddrlist[m_StatusGpId].at(0)).toInt();
  const QStringList &AIlablst = struGsh.vMegreLablst[ncamid];
  const bool isBottleType = (struCnfg.nMachineType == nMachine_Bottle);
  const int statusColumnIndex = m_labtreewid->columnCount() - 1;
  bool bPercentType = pProfile->nStaisticType == 1 ? true : false;
  bool bEnSorterAdj = pProfile->bEnSorterClass;

  if (AIlablst.empty()) {
    return;
  }

  //! 缓存图标，避免重复创建
  const QIcon redIcon(":/res/png/action_circle_red.png");
  const QIcon normalIcon(":/res/png/action_circle.png");

  //! 构建 label 到 item 的映射，避免频繁的 findItems 调用
  QMap<QString, QTreeWidgetItem *> labelItemMap;

  //! 第一遍：建立映射关系
  for (int i = 0; i < m_labtreewid->topLevelItemCount(); ++i) {
    QTreeWidgetItem *topItem = m_labtreewid->topLevelItem(i);
    const QString &topLabel = topItem->data(0, Qt::DisplayRole).toString();
    labelItemMap[topLabel] = topItem;

    //! 添加子项映射
    for (int j = 0; j < topItem->childCount(); ++j) {
      QTreeWidgetItem *childItem = topItem->child(j);
      const QString &childLabel =
          childItem->data(0, Qt::DisplayRole).toString();
      labelItemMap[childLabel] = childItem;
    }
  }

  //! 瓶片增加预遍历
  float nDiff = 0;
  QString strmaxvalchild;
  if (isBottleType) {
    float maxRes = 0.0;
    float totalValue = 0;
    for (const QString &var : AIlablst) {
      QStringList childlst;
      if (pProfile->vMegerlabmap[ncamid].contains(var)) {
        childlst = pProfile->vMegerlabmap[ncamid].values(var);
      }

      if (childlst.isEmpty()) {
        childlst << var;
      }

      // if(!struGsh.vlabStamap[m_StatusGpId].contains(childlst.at(0)))
      // continue;

      //! 遍历统计类别
      const QStringList &catlst =
          struGsh.vlabStamap[m_StatusGpId][childlst.at(0)].staRes.keys();
      for (int k = 0; k < catlst.size(); k++) {
        for (const QString &childname : childlst) {
          // if(!struGsh.vlabStamap[m_StatusGpId].contains(childname)) continue;
          const QString &str =
              struGsh.vlabStamap[m_StatusGpId][childname].staInfo[catlst.at(k)];
          if (str.isEmpty())
            continue;

          const QStringList &strParts = str.split("/");
          if (strParts.size() < 2)
            continue;

          const QString &strtype = strParts.at(1);
          if (strtype.isEmpty())
            continue;

          const int statype = strtype.mid(1).toInt();
          if (statype != STATISTIC_A3)
            continue;

          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            float nres = struGsh.vlabStamap[m_StatusGpId][childname]
                             .staRes[catlst.at(k)][0] /
                         struGsh.vlabTotalmap[m_StatusGpId][str];

            float nVal = 0.0;
            if (bPercentType) {
              nVal = QString::number(nres, 'f', 4).toFloat();
            } else {
              nVal = (int)(nres * 1000000);
            }

            totalValue += nVal;
            if (nVal > maxRes) {
              maxRes = nVal;
              strmaxvalchild = childname;
            }
          }
        }
      }
    }

    if (totalValue > 0) {
      if (bPercentType) {
        nDiff = QString::number(1.0 - totalValue, 'f', 4).toFloat();
      } else {
        nDiff = (int)(1000000.0 - totalValue);
      }
    }
  }

  //! 第二遍：更新数据
  for (const QString &var : AIlablst) {
    //! 判断该当前是否有子项
    bool bHasChild = false;
    QStringList childlst;
    if (pProfile->vMegerlabmap[ncamid].contains(var)) {
      childlst = pProfile->vMegerlabmap[ncamid].values(var);
    }

    if (childlst.isEmpty()) {
      childlst << var;
    } else {
      bHasChild = true;
    }

    //! 遍历统计类别
    bool bMegrlab = false;
    float ntotaRes = 0.0;
    int nTotalVal = 0;
    // if(!struGsh.vlabStamap[m_StatusGpId].contains(childlst.at(0))) continue;
    const QStringList &catlst =
        struGsh.vlabStamap[m_StatusGpId][childlst.at(0)].staRes.keys();

    for (int k = 0; k < catlst.size(); k++) {
      ntotaRes = 0.0;
      bMegrlab = false;

      for (const QString &childname : childlst) {
        bool blabsta = false;
        float nres = 0.0;

        QTreeWidgetItem *childItem = labelItemMap.value(childname, nullptr);
        if (!childItem)
          continue;

        // if(!struGsh.vlabStamap[m_StatusGpId].contains(childname)) continue;
        const QString &str =
            struGsh.vlabStamap[m_StatusGpId][childname].staInfo[catlst.at(k)];
        if (str.isEmpty())
          continue;

        const QStringList &strParts = str.split("/");
        if (strParts.size() < 2)
          continue;

        const QString &strtype = strParts.at(1);
        if (strtype.isEmpty())
          continue;

        const int statype = strtype.mid(1).toInt();
        QString valstr, topvalstr;

        switch (statype) {
        case STATISTIC_A1: //! 直接用
          nres = struGsh.vlabStamap[m_StatusGpId][childname]
                     .staRes[catlst.at(k)][0];
          ntotaRes += nres;
          valstr = QString::number((int)nres);
          topvalstr = QString::number((int)ntotaRes);
          break;
        case STATISTIC_A3: //! 乘系数 算占比
          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            nres = struGsh.vlabStamap[m_StatusGpId][childname]
                       .staRes[catlst.at(k)][0] /
                   struGsh.vlabTotalmap[m_StatusGpId][str];
            if (bPercentType) {
              nres = QString::number(nres, 'f', 4).toFloat();
              if (childname == strmaxvalchild) {
                nres += nDiff;
              }
            }
            ntotaRes += nres;
          }

          if (isBottleType && !bPercentType) {
            int nVal = (int)(nres * 1000000);
            if (childname == strmaxvalchild) {
              nVal += nDiff;
            }
            nTotalVal += nVal;
            valstr = QString::number(nVal);
            topvalstr = QString::number(nTotalVal);

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childname)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nlowthd > nVal ||
                     nVal > pProfile->strcSortclass
                                .vlabParm[struGsh.nSorterAdjIdx][childname]
                                .nhigthd))
                  blabsta = true;
              }

              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(var)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][var]
                             .nlowthd > nTotalVal ||
                     nTotalVal > pProfile->strcSortclass
                                     .vlabParm[struGsh.nSorterAdjIdx][var]
                                     .nhigthd))
                  bMegrlab = true;
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childname) &&
                  nVal > pProfile->strctAI.vlabthdmap[childname]) {
                blabsta = true;
              }

              if (pProfile->strctAI.vlabthdmap.contains(var) &&
                  nTotalVal > pProfile->strctAI.vlabthdmap[var]) {
                bMegrlab = true;
              }
            }
          } else {
            valstr = QString("%1%").arg(QString::number(nres * 100, 'f', 4));
            topvalstr =
                QString("%1%").arg(QString::number(ntotaRes * 100, 'f', 4));

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childname)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nlowthd > 100 * nres ||
                     100 * nres >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nhigthd))
                  blabsta = true;
              }

              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(var)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][var]
                             .nlowthd > 100 * ntotaRes ||
                     100 * ntotaRes > pProfile->strcSortclass
                                          .vlabParm[struGsh.nSorterAdjIdx][var]
                                          .nhigthd))
                  bMegrlab = true;
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childname)) {
                blabsta = (100 * nres > pProfile->strctAI.vlabthdmap[childname])
                              ? true
                              : false;
              }

              if (pProfile->strctAI.vlabthdmap.contains(var)) {
                bMegrlab = (100 * ntotaRes > pProfile->strctAI.vlabthdmap[var])
                               ? true
                               : false;
              }
            }
          }
          break;
        case STATISTIC_A4: //! 直接算占比
          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            nres = struGsh.vlabStamap[m_StatusGpId][childname]
                       .staRes[catlst.at(k)][0] /
                   struGsh.vlabTotalmap[m_StatusGpId][str];
            ntotaRes += nres;
          }

          valstr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
          topvalstr =
              QString("%1%").arg(QString::number(ntotaRes * 100, 'f', 2));

          if (bEnSorterAdj) {
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(childname)) {
              if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childname]
                           .nlowthd > 100 * nres ||
                   100 * nres > pProfile->strcSortclass
                                    .vlabParm[struGsh.nSorterAdjIdx][childname]
                                    .nhigthd))
                blabsta = true;
            }

            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(var)) {
              if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                  (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][var]
                           .nlowthd > 100 * ntotaRes ||
                   100 * ntotaRes > pProfile->strcSortclass
                                        .vlabParm[struGsh.nSorterAdjIdx][var]
                                        .nhigthd))
                bMegrlab = true;
            }
          } else {
            if (pProfile->strctAI.vlabthdmap.contains(childname) &&
                nres * 100 > pProfile->strctAI.vlabthdmap[childname]) {
              blabsta = true;
            }
            if (pProfile->strctAI.vlabthdmap.contains(var) &&
                ntotaRes * 100 > pProfile->strctAI.vlabthdmap[var]) {
              bMegrlab = true;
            }
          }
          break;
        case STATISTIC_A2:
          nres = struGsh.vlabStamap[m_StatusGpId][childname]
                     .staRes[catlst.at(k)][0];
          valstr = QString("%1").arg(QString::number(nres, 'f', 2));
          ntotaRes += nres;
          topvalstr = QString("%1").arg(QString::number(ntotaRes, 'f', 2));
          break;
        case STATISTIC_A5:
          break;
        default:
          continue; //! 跳过不需要处理的情况
        }

        //! 设置子项数据
        childItem->setData(k + 1, Qt::DisplayRole, valstr);
        childItem->setData(statusColumnIndex, Qt::DecorationRole,
                           blabsta ? redIcon : normalIcon);

        //! 设置父项数据
        if (bHasChild) {
          QTreeWidgetItem *topItem = labelItemMap.value(var, nullptr);
          if (topItem) {
            topItem->setData(k + 1, Qt::DisplayRole, topvalstr);
            topItem->setData(statusColumnIndex, Qt::DecorationRole,
                             bMegrlab ? redIcon : normalIcon);
          }
        }
      }
    }
  }
}

void AnalyzeTestDlg::refreshBarViewMerger() {
  refreshMatStaRes();

  if (m_labtreewid->topLevelItemCount() == 0) {
    refreshLabStaResMerger();
    return;
  }

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  const int nStaticGroup = pProfile->nStaticGroup;
  if (nStaticGroup < 2 || pProfile->vUnitaddrlist.size() < nStaticGroup ||
      pProfile->vUnitaddrlist[0].isEmpty()) {
    return;
  }

  const int lastGpId = nStaticGroup - 1;
  if (pProfile->vUnitaddrlist[lastGpId].isEmpty()) {
    return;
  }

  const int matCamId = QString(pProfile->vUnitaddrlist[lastGpId].at(0)).toInt();
  const QString secondaryLab = pProfile->slabSecondary[matCamId].trimmed();
  if (secondaryLab.isEmpty()) {
    return;
  }

  const bool bPercentType = (pProfile->nStaisticType == 1);
  const bool bEnSorterAdj = pProfile->bEnSorterClass;
  const int statusColumnIndex = m_labtreewid->columnCount() - 1;

  //! 计算红外材质组次一级标签占比
  QStringList secSubLabs;
  if (pProfile->vMegerlabmap[matCamId].contains(secondaryLab)) {
    secSubLabs = pProfile->vMegerlabmap[matCamId].values(secondaryLab);
  }
  if (secSubLabs.isEmpty()) {
    secSubLabs << secondaryLab;
  }

  const QStringList &catlst = struGsh.vlabStamap[0].first().staRes.keys();
  QMap<QString, float> secRatioMap;
  for (const QString &cat : catlst) {
    float secRatio = 0.0f;
    for (const QString &subName : secSubLabs) {
      if (!struGsh.vlabStamap[lastGpId].contains(subName))
        continue;
      const QString &infoStr =
          struGsh.vlabStamap[lastGpId][subName].staInfo.value(cat);
      if (infoStr.isEmpty())
        continue;
      if (struGsh.vlabTotalmap[lastGpId].value(infoStr) > 0) {
        secRatio += struGsh.vlabStamap[lastGpId][subName].staRes[cat][0] /
                    struGsh.vlabTotalmap[lastGpId][infoStr];
      }
    }
    secRatioMap[cat] = secRatio;
  }

  //! 计算可见组与剩余材质组的原始占比总和，推导剩余材质缩放系数 matScaleMap
  QMap<QString, float> visRawSumMap;
  QMap<QString, float> matOtherRawSumMap;
  for (int i = 0; i < m_labtreewid->topLevelItemCount(); ++i) {
    QTreeWidgetItem *topItem = m_labtreewid->topLevelItem(i);
    const int srcGp = topItem->data(0, Qt::UserRole + 1).toInt();
    const int childCount = topItem->childCount();

    QList<QTreeWidgetItem *> subItems;
    if (childCount > 0) {
      for (int c = 0; c < childCount; ++c) {
        subItems.append(topItem->child(c));
      }
    } else {
      subItems.append(topItem);
    }

    for (const QString &cat : catlst) {
      for (QTreeWidgetItem *item : subItems) {
        const QString labName = item->data(0, Qt::UserRole).toString();
        if (!struGsh.vlabStamap[srcGp].contains(labName))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][labName].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();
        if (statype != STATISTIC_A3 && statype != STATISTIC_A4)
          continue;

        if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
          float rawRatio = struGsh.vlabStamap[srcGp][labName].staRes[cat][0] /
                           struGsh.vlabTotalmap[srcGp][str];
          if (srcGp == 0) {
            visRawSumMap[cat] += rawRatio;
          } else {
            matOtherRawSumMap[cat] += rawRatio;
          }
        }
      }
    }
  }

  QMap<QString, float> matScaleMap;
  for (const QString &cat : catlst) {
    const float rRatio = secRatioMap.value(cat, 0.0f);
    const float sVis = visRawSumMap.value(cat, 0.0f);
    const float sMatOther = matOtherRawSumMap.value(cat, 0.0f);
    const float rRemain = qMax(0.0f, 1.0f - sVis * rRatio);
    matScaleMap[cat] = (sMatOther > 0.0f) ? (rRemain / sMatOther) : 0.0f;
  }

  //! 预遍历计算差额 nDiffMap 与最大标签项 (针对
  //! STATISTIC_A3，按统计类别分别独立计算)
  QMap<QString, float> nDiffMap;
  QMap<QString, QString> maxValChildMap;

  for (int k = 0; k < catlst.size(); ++k) {
    const QString &cat = catlst.at(k);
    const float rRatio = secRatioMap.value(cat, 0.0f);
    const float matScale = matScaleMap.value(cat, 0.0f);
    float totalValue = 0.0f;
    float maxRes = -1.0f;
    QString strmaxchild;

    for (int i = 0; i < m_labtreewid->topLevelItemCount(); ++i) {
      QTreeWidgetItem *topItem = m_labtreewid->topLevelItem(i);
      const int srcGp = topItem->data(0, Qt::UserRole + 1).toInt();
      const float scale = (srcGp == 0) ? rRatio : matScale;
      const int childCount = topItem->childCount();

      QList<QTreeWidgetItem *> subItems;
      if (childCount > 0) {
        for (int c = 0; c < childCount; ++c) {
          subItems.append(topItem->child(c));
        }
      } else {
        subItems.append(topItem);
      }

      for (QTreeWidgetItem *item : subItems) {
        const QString labName = item->data(0, Qt::UserRole).toString();
        if (!struGsh.vlabStamap[srcGp].contains(labName))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][labName].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();
        if (statype != STATISTIC_A3)
          continue;

        if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
          float rawRatio = struGsh.vlabStamap[srcGp][labName].staRes[cat][0] /
                           struGsh.vlabTotalmap[srcGp][str];
          float nres = rawRatio * scale;

          float nVal = 0.0f;
          if (bPercentType) {
            nVal = QString::number(nres, 'f', 4).toFloat();
          } else {
            nVal = (int)(nres * 1000000);
          }

          totalValue += nVal;
          if (nVal > maxRes) {
            maxRes = nVal;
            strmaxchild = labName;
          }
        }
      }
    }

    if (totalValue > 0) {
      if (bPercentType) {
        nDiffMap[cat] = QString::number(1.0 - totalValue, 'f', 4).toFloat();
      } else {
        nDiffMap[cat] = (int)(1000000.0 - totalValue);
      }
      maxValChildMap[cat] = strmaxchild;
    }
  }

  //! 更新数据与图标
  const QIcon redIcon(":/res/png/action_circle_red.png");
  const QIcon normalIcon(":/res/png/action_circle.png");

  for (int i = 0; i < m_labtreewid->topLevelItemCount(); ++i) {
    QTreeWidgetItem *topItem = m_labtreewid->topLevelItem(i);
    const int srcGp = topItem->data(0, Qt::UserRole + 1).toInt();
    const QString topOrigLab = topItem->data(0, Qt::UserRole).toString();
    const int childCount = topItem->childCount();
    const bool bHasChild = (childCount > 0);

    QList<QTreeWidgetItem *> subItems;
    if (bHasChild) {
      for (int c = 0; c < childCount; ++c) {
        subItems.append(topItem->child(c));
      }
    } else {
      subItems.append(topItem);
    }

    for (int k = 0; k < catlst.size(); ++k) {
      const QString &cat = catlst.at(k);
      const float rRatio = secRatioMap.value(cat, 0.0f);
      const float scale = (srcGp == 0) ? rRatio : matScaleMap.value(cat, 0.0f);

      float ntopRes = 0.0f;
      int ntopResInt = 0;
      bool bMerlabsta = false;
      QString topStr;

      for (QTreeWidgetItem *item : subItems) {
        const QString childOrigLab = item->data(0, Qt::UserRole).toString();
        bool bsta = false;
        QString restr;
        float nres = 0.0f;

        if (!struGsh.vlabStamap[srcGp].contains(childOrigLab))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][childOrigLab].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();

        switch (statype) {
        case STATISTIC_A1: {
          nres = struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] * scale;
          restr = QString::number((int)nres);
          ntopRes += nres;
          topStr = QString::number((int)ntopRes);
          break;
        }
        case STATISTIC_A3: {
          if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
            float rawRatio =
                struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] /
                struGsh.vlabTotalmap[srcGp][str];
            nres = rawRatio * scale;
          }

          if (!bPercentType) {
            int nresInt = (int)(nres * 1000000);
            if (childOrigLab == maxValChildMap.value(cat)) {
              nresInt += (int)nDiffMap.value(cat, 0.0f);
            }
            ntopResInt += nresInt;
            restr = QString::number(nresInt);
            topStr = QString::number(ntopResInt);

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nlowthd > nresInt ||
                     nresInt >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nhigthd)) {
                  bsta = true;
                }
              }
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(topOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nlowthd > ntopResInt ||
                     ntopResInt >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nhigthd)) {
                  bMerlabsta = true;
                }
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childOrigLab) &&
                  nresInt > pProfile->strctAI.vlabthdmap[childOrigLab]) {
                bsta = true;
              }
              if (pProfile->strctAI.vlabthdmap.contains(topOrigLab) &&
                  ntopResInt > pProfile->strctAI.vlabthdmap[topOrigLab]) {
                bMerlabsta = true;
              }
            }
          } else {
            float nresFloat = QString::number(nres, 'f', 4).toFloat();
            if (childOrigLab == maxValChildMap.value(cat)) {
              nresFloat += nDiffMap.value(cat, 0.0f);
            }
            ntopRes += nresFloat;
            restr =
                QString("%1%").arg(QString::number(nresFloat * 100, 'f', 4));
            topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 4));

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nlowthd > 100 * nresFloat ||
                     100 * nresFloat >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nhigthd)) {
                  bsta = true;
                }
              }
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(topOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nlowthd > 100 * ntopRes ||
                     100 * ntopRes >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nhigthd)) {
                  bMerlabsta = true;
                }
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childOrigLab)) {
                bsta = (100 * nresFloat >
                        pProfile->strctAI.vlabthdmap[childOrigLab]);
              }
              if (pProfile->strctAI.vlabthdmap.contains(topOrigLab)) {
                bMerlabsta =
                    (100 * ntopRes > pProfile->strctAI.vlabthdmap[topOrigLab]);
              }
            }
          }
          break;
        }
        case STATISTIC_A4: {
          if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
            float rawRatio =
                struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] /
                struGsh.vlabTotalmap[srcGp][str];
            nres = rawRatio * scale;
          }
          ntopRes += nres;
          restr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
          topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 2));
          if (bEnSorterAdj) {
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(childOrigLab)) {
              if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                           .nlowthd > 100 * nres ||
                   100 * nres >
                       pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                           .nhigthd)) {
                bsta = true;
              }
            }
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(topOrigLab)) {
              if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                           .nlowthd > 100 * ntopRes ||
                   100 * ntopRes >
                       pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                           .nhigthd)) {
                bMerlabsta = true;
              }
            }
          } else {
            if (pProfile->strctAI.vlabthdmap.contains(childOrigLab) &&
                nres * 100 > pProfile->strctAI.vlabthdmap[childOrigLab]) {
              bsta = true;
            }
            if (pProfile->strctAI.vlabthdmap.contains(topOrigLab) &&
                ntopRes * 100 > pProfile->strctAI.vlabthdmap[topOrigLab]) {
              bMerlabsta = true;
            }
          }
          break;
        }
        case STATISTIC_A2: {
          nres = struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] * scale;
          restr = QString("%1").arg(QString::number(nres, 'f', 2));
          ntopRes += nres;
          topStr = QString("%1").arg(QString::number(ntopRes, 'f', 2));
          break;
        }
        default:
          break;
        }

        if (bHasChild) {
          item->setData(k + 1, Qt::DisplayRole, restr);
          item->setData(statusColumnIndex, Qt::DecorationRole,
                        bsta ? redIcon : normalIcon);
        }
      }

      topItem->setData(k + 1, Qt::DisplayRole, topStr);
      topItem->setData(statusColumnIndex, Qt::DecorationRole,
                       bMerlabsta ? redIcon : normalIcon);
    }
  }
}

/*!
 * \brief 刷新统计控件
 * \param bReset
 */
void AnalyzeTestDlg::refreshStatusItem(bool bReset) {
  if (struGsh.bEnSimulate) {
    m_viewIdBtn->hide();
    m_mergerColorMaterialBtn->hide();
    refreshSimulateRes(bReset);
    m_staticwidChgstck->setCurrentIndex(1);
  } else {
    auto *pProfile =
        &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QString strname;
    if (m_StatusGpId < pProfile->vUnitaddrlist.size() &&
        !pProfile->vUnitaddrlist[m_StatusGpId].isEmpty()) {
      int nCamId = QString("%1")
                       .arg(pProfile->vUnitaddrlist[m_StatusGpId].at(0))
                       .toInt();
      strname = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nCamId];
    }

    if (m_viewIdBtn->text() != strname && !strname.isEmpty()) {
      m_viewIdBtn->setText(strname);
    }
    m_viewIdBtn->show();

    if (struCnfg.nMachineType == nMachine_Bottle) {
      m_mergerColorMaterialBtn->setChecked(
          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial);
      m_mergerColorMaterialBtn->show();
    } else {
      m_mergerColorMaterialBtn->hide();
    }

    refreshMainpageItem(bReset);
    m_staticwidChgstck->setCurrentIndex(0);
  }
}

/*!
 * \brief 刷新详细页面的统计控件
 * \param bStatus
 */
void AnalyzeTestDlg::refreshMainpageItem(bool bStatus) {
  Q_UNUSED(bStatus);
  if (m_StatusGpId >= struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                          ->analyzeProfile.nStaticGroup) {
    onRefreshStatsGpRes();
  } else {
    refreshMatStaRes();
    refreshLabStaRes();
  }
}
/*!
 * \brief 刷新详细页面的统计控件  检测时刷新
 * \param bStatus
 */
void AnalyzeTestDlg::refreshInferResult() {
  if (struGsh.bEnSimulate) {
    refreshSimulateRes();
  } else {
    refreshBarView();
  }
}

/*!
 * \brief 刷新物料统计结果
 */
void AnalyzeTestDlg::refreshMatStaRes() {
  //! 预分配字符串空间，避免频繁的内存重分配
  QStringList matParts;
  matParts.reserve(struGsh.vsMatinfo[m_StatusGpId].staRes.size());

  //! 缓存常用数据，避免重复访问
  const auto &matInfo = struGsh.vsMatinfo[m_StatusGpId];
  const auto &matTotalMap = struGsh.vmatTotalmap[m_StatusGpId];

  for (const QString &title : matInfo.staRes.keys()) {
    const QString &str = matInfo.staInfo[title];
    if (str.isEmpty())
      continue;

    //! 只 split 一次，避免重复操作
    const QStringList &strParts = str.split("/");
    if (strParts.size() < 2) {
      qDebug() << "sta type error!";
      continue; // 使用 continue 而不是 break，继续处理其他项
    }

    const QString &strType = strParts.at(1);
    if (strType.isEmpty())
      continue;

    // 正确解析 statype，支持两位数类型（如 A10）
    const int statype = strType.mid(1).toInt();
    const float totalValue = matTotalMap.value(str, 0.0f);
    switch (statype) {
    case STATISTIC_A1: {
      const int value = (int)matInfo.staRes[title][0];
      matParts.append(QString("%1: %2").arg(title).arg(value));
      break;
    }

    case STATISTIC_A3:
    case STATISTIC_A4:
    case STATISTIC_A5:
    case STATISTIC_A6: {
      if (totalValue > 0.0f) {
        const float nVal = matInfo.staRes[title][0] / totalValue;
        matParts.append(
            QString("%1: %2").arg(title).arg(QString::number(nVal, 'f', 2)));
      } else {
        matParts.append(
            QString("%1: %2").arg(title).arg(QString::number(0, 'f', 2)));
      }
      break;
    }
    case STATISTIC_A7:
      matParts.append(QString("%1: %2    %3    %4    %5")
                          .arg(title)
                          .arg((int)matInfo.staRes[title][0])
                          .arg((int)matInfo.staRes[title][1])
                          .arg((int)matInfo.staRes[title][2])
                          .arg((int)matInfo.staRes[title][3]));
      break;
    case STATISTIC_A8:
    case STATISTIC_A10: {
      if (totalValue > 0.0f) {
        const float nVal = matInfo.staRes[title][0] / totalValue;
        const float nVal1 = matInfo.staRes[title][1] / totalValue;
        const float nVal2 = matInfo.staRes[title][2] / totalValue;

        matParts.append(QString("%1: %2%    %3%    %4%")
                            .arg(title)
                            .arg(QString::number(nVal * 100, 'f', 2))
                            .arg(QString::number(nVal1 * 100, 'f', 2))
                            .arg(QString::number(nVal2 * 100, 'f', 2)));
      } else {
        matParts.append(QString("%1: %2    %3    %4")
                            .arg(title)
                            .arg(QString::number(0, 'f', 2))
                            .arg(QString::number(0, 'f', 2))
                            .arg(QString::number(0, 'f', 2)));
      }
      break;
    }

    default:
      break;
    }
  }

  //! 使用 join 一次性构建最终字符串，比循环拼接更高效
  const QString matstr = matParts.join("     ");
  m_detectLbl->setText(matstr);

  //! 优化金属标签显示逻辑
  const bool isMetalType = (struCnfg.nMachineType == nMachine_Bottle);
  if (isMetalType &&
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect) {
    m_metaititleLb->show();
    m_metaititleLb->setText(
        QString("%1: %2").arg(myLan.metal).arg(struGsh.nMetaNum));
    if (struGsh.nMetaNum >
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nMeatal) {
      m_metaititleLb->setStyleSheet("color:red");
    } else {
      m_metaititleLb->setStyleSheet("color:black");
    }
  } else {
    m_metaititleLb->hide();
  }
}
/*!
 * \brief 刷新标签统计结果
 */
void AnalyzeTestDlg::refreshLabStaRes() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  const bool isMergerColorMat =
      (struCnfg.nMachineType == nMachine_Bottle) &&
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial &&
      (m_StatusGpId == 0) && (pProfile->nStaticGroup >= 2) &&
      (pProfile->vUnitaddrlist.size() >= pProfile->nStaticGroup);
  if (isMergerColorMat) {
    refreshLabStaResMerger();
    return;
  }

  //! 刷新标签统计
  m_labtreewid->clear();

  if (struGsh.vlabStamap[m_StatusGpId].isEmpty()) {
    return;
  }

  //! 确保 m_StatusGpId 在有效范围内（nStaticGroup 或 vUnitaddrlist
  //! 改变时可能越界）
  const int nStaticGroup = pProfile->nStaticGroup;
  if (m_StatusGpId < 0 || m_StatusGpId >= nStaticGroup ||
      m_StatusGpId >= pProfile->vUnitaddrlist.size() ||
      pProfile->vUnitaddrlist[m_StatusGpId].isEmpty()) {
    m_StatusGpId = 0;
    if (nStaticGroup <= 0 || pProfile->vUnitaddrlist.isEmpty() ||
        pProfile->vUnitaddrlist[0].isEmpty()) {
      return;
    }
  }

  //! 1. 动态设置列数并设置表头
  QStringList headerlst;
  headerlst << QString("    %1").arg(myLan.category);
  const QList<QString> &labstalst =
      struGsh.vlabStamap[m_StatusGpId].first().staInfo.keys();
  if (!labstalst.isEmpty()) {
    headerlst.append(labstalst);
    headerlst.append(myLan.status);
  } else {
    return;
  }

  const int headerSize = headerlst.size();
  m_labtreewid->setColumnCount(headerSize);
  m_labtreewid->setHeaderLabels(headerlst);

  int visibleCols = 0;
  for (int i = 0; i < headerSize; ++i) {
    const QString strname = headerlst.at(i);
    bool hide = false;
    if (pProfile->staclassEnmap.contains(strname) &&
        (!pProfile->staclassEnmap[strname])) {
      hide = true;
    }
    m_labtreewid->setColumnHidden(i, hide);
    if (!hide)
      ++visibleCols;
  }

  const int effectiveCols = qMax(1, visibleCols);
  const int baseArea = WIN_WID - 300;
  const int columnWidth = baseArea / effectiveCols;

  for (int i = 0; i < headerSize; ++i) {
    if (m_labtreewid->isColumnHidden(i)) {
      m_labtreewid->setColumnWidth(i, 0);
    } else {
      m_labtreewid->setColumnWidth(i, columnWidth);
    }
  }

  if (m_labtreewid->header()) {
    m_labtreewid->header()->setStretchLastSection(false);
    for (int i = headerSize - 1; i >= 0; --i) {
      if (!m_labtreewid->isColumnHidden(i)) {
        m_labtreewid->header()->setSectionResizeMode(i, QHeaderView::Stretch);
        break;
      }
    }
  }

  //! 2. 预处理配置数据
  const int ncamid =
      QString(pProfile->vUnitaddrlist[m_StatusGpId].at(0)).toInt();
  const QStringList &AIlablst = struGsh.vMegreLablst[ncamid];
  const QStringList &labidxlst = pProfile->vlabIdxmap[ncamid].keys();
  const bool isBottleType = (struCnfg.nMachineType == nMachine_Bottle);
  const int statusColumnIndex = headerSize - 1;
  bool bPercentType = pProfile->nStaisticType == 1 ? true : false;
  bool bEnSorterAdj = pProfile->bEnSorterClass;

  //! 3. 构建 label 到 item 的映射
  QMap<QString, QTreeWidgetItem *> labelItemMap;
  QList<QTreeWidgetItem *> itemsToInsert;
  QList<int> insertPositions;

  //! 4. 第一遍循环：创建所有 items 并建立映射关系
  for (const QString &var : AIlablst) {
    bool bshow = true;
    if (pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]) {
      bshow = false;
    } else if (!gFlow.bMegerlabshow(var, ncamid)) {
      bshow = false;
    }

    if (!bshow)
      continue;

    //! 创建顶层 item
    QTreeWidgetItem *topitem = new QTreeWidgetItem();
    topitem->setData(0, Qt::DisplayRole, var);
    labelItemMap[var] = topitem;

    //! 处理子项
    int nlabIdx = -1;
    bool bHasChild = false;
    QStringList childlst;

    if (pProfile->vMegerlabmap[ncamid].contains(var)) {
      childlst = pProfile->vMegerlabmap[ncamid].values(var);
      if (!childlst.isEmpty()) {
        bHasChild = true;
        for (const QString &childname : childlst) {
          QTreeWidgetItem *childitem = new QTreeWidgetItem();
          childitem->setData(0, Qt::DisplayRole, childname);
          topitem->addChild(childitem);
          labelItemMap[childname] = childitem;

          if (labidxlst.contains(childname)) {
            nlabIdx = pProfile->vlabIdxmap[ncamid][childname];
          }
        }
      }
    }

    if (childlst.isEmpty()) {
      childlst << var;
      if (labidxlst.contains(var)) {
        nlabIdx = pProfile->vlabIdxmap[ncamid][var];
      }
    }

    if (nlabIdx == -1) {          //! 确定插入位置
      insertPositions.append(-1); // 表示追加到末尾
    } else {
      insertPositions.append(nlabIdx);
    }
    itemsToInsert.append(topitem);
  }

  //! 针对瓶片加一次预遍历
  float nDiff = 0.0;
  QString strmaxvalchild;
  if (isBottleType) {
    float maxRes = 0.0;
    float totalValue = 0;
    for (int itemIndex = 0; itemIndex < itemsToInsert.size(); ++itemIndex) {
      QTreeWidgetItem *topitem = itemsToInsert[itemIndex];
      const QString &var = topitem->data(0, Qt::DisplayRole).toString();

      QStringList childlst;
      bool bHasChild = false;
      if (pProfile->vMegerlabmap[ncamid].contains(var)) {
        childlst = pProfile->vMegerlabmap[ncamid].values(var);
        if (!childlst.isEmpty()) {
          bHasChild = true;
        }
      }
      if (childlst.isEmpty()) {
        childlst << var;
      }

      // if(!struGsh.vlabStamap[m_StatusGpId].contains(childlst.at(0)))
      // continue;
      //! 遍历统计类别
      const QStringList &catlst =
          struGsh.vlabStamap[m_StatusGpId][childlst.at(0)].staRes.keys();
      for (int k = 0; k < catlst.size(); k++) {
        for (const QString &childname : childlst) {
          // if(!struGsh.vlabStamap[m_StatusGpId].contains(childname)) continue;
          const QString &str =
              struGsh.vlabStamap[m_StatusGpId][childname].staInfo[catlst.at(k)];
          if (str.isEmpty())
            continue;

          const QStringList &strParts = str.split("/");
          if (strParts.size() < 2)
            continue;

          const QString &offstr = strParts.at(1);
          if (offstr.isEmpty())
            continue;

          const int statype = offstr.mid(1).toInt();
          if (statype != STATISTIC_A3)
            continue;

          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            float nres = struGsh.vlabStamap[m_StatusGpId][childname]
                             .staRes[catlst.at(k)][0] /
                         struGsh.vlabTotalmap[m_StatusGpId][str];

            float nVal;
            if (bPercentType) {
              nVal = QString::number(nres, 'f', 4).toFloat();
            } else {
              nVal = (int)(nres * 1000000);
            }
            totalValue += nVal;
            if (nVal > maxRes) {
              maxRes = nVal;
              strmaxvalchild = childname;
            }
          }
        }
      }
    }

    if (totalValue > 0) {
      if (bPercentType) {
        nDiff = QString::number(1.0 - totalValue, 'f', 4).toFloat();
      } else {
        nDiff = (int)(1000000.0 - totalValue);
      }
    }
  }

  //! 5. 第二遍循环：填充统计数据
  for (int itemIndex = 0; itemIndex < itemsToInsert.size(); ++itemIndex) {
    QTreeWidgetItem *topitem = itemsToInsert[itemIndex];
    const QString &var = topitem->data(0, Qt::DisplayRole).toString();

    //! 获取子项列表
    QStringList childlst;
    bool bHasChild = false;
    if (pProfile->vMegerlabmap[ncamid].contains(var)) {
      childlst = pProfile->vMegerlabmap[ncamid].values(var);
      if (!childlst.isEmpty()) {
        bHasChild = true;
      }
    }
    if (childlst.isEmpty()) {
      childlst << var;
    }

    //! 遍历统计类别
    // if(!struGsh.vlabStamap[m_StatusGpId].contains(childlst.at(0))) continue;
    const QStringList &catlst =
        struGsh.vlabStamap[m_StatusGpId][childlst.at(0)].staRes.keys();
    for (int k = 0; k < catlst.size(); k++) {
      float ntopRes = 0.0;
      int ntopResInt = 0;
      bool bMerlabsta = false;
      QString topStr;

      for (const QString &childname : childlst) {
        bool bsta = false;
        QString restr;
        float nres = 0.0;
        // if(!struGsh.vlabStamap[m_StatusGpId].contains(childname)) continue;
        const QString &str =
            struGsh.vlabStamap[m_StatusGpId][childname].staInfo[catlst.at(k)];
        if (str.isEmpty())
          continue;

        const QStringList &strParts = str.split("/");
        if (strParts.size() < 2)
          continue;

        const QString &offstr = strParts.at(1);
        if (offstr.isEmpty())
          continue;

        const int statype = offstr.mid(1).toInt();

        switch (statype) {
        case STATISTIC_A1: //! 直接用
          nres = struGsh.vlabStamap[m_StatusGpId][childname]
                     .staRes[catlst.at(k)][0];
          restr = QString::number((int)nres);
          ntopRes += nres;
          topStr = QString::number((int)ntopRes);
          break;
        case STATISTIC_A3: //! 乘系数 算占比
          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            nres = struGsh.vlabStamap[m_StatusGpId][childname]
                       .staRes[catlst.at(k)][0] /
                   struGsh.vlabTotalmap[m_StatusGpId][str];
          }

          if (isBottleType && !bPercentType) {
            int nresInt = (int)(nres * 1000000);
            if (childname == strmaxvalchild) {
              nresInt += nDiff;
            }
            ntopResInt += nresInt;
            restr = QString::number(nresInt);
            topStr = QString::number(ntopResInt);

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childname)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nlowthd > nresInt ||
                     nresInt > pProfile->strcSortclass
                                   .vlabParm[struGsh.nSorterAdjIdx][childname]
                                   .nhigthd))
                  bsta = true;
              }

              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(var)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][var]
                             .nlowthd > ntopResInt ||
                     ntopResInt > pProfile->strcSortclass
                                      .vlabParm[struGsh.nSorterAdjIdx][var]
                                      .nhigthd))
                  bMerlabsta = true;
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childname) &&
                  nresInt > pProfile->strctAI.vlabthdmap[childname]) {
                bsta = true;
              }

              if (pProfile->strctAI.vlabthdmap.contains(var) &&
                  ntopResInt > pProfile->strctAI.vlabthdmap[var]) {
                bMerlabsta = true;
              }
            }
          } else {
            float nresFloat = QString::number(nres, 'f', 4).toFloat();
            if (childname == strmaxvalchild) {
              nresFloat += nDiff;
            }
            ntopRes += nresFloat;
            restr =
                QString("%1%").arg(QString::number(nresFloat * 100, 'f', 4));
            topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 4));

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childname)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nlowthd > 100 * nresFloat ||
                     100 * nresFloat >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childname]
                             .nhigthd))
                  bsta = true;
              }

              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(var)) {
                if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][var]
                             .nlowthd > 100 * ntopRes ||
                     100 * ntopRes > pProfile->strcSortclass
                                         .vlabParm[struGsh.nSorterAdjIdx][var]
                                         .nhigthd))
                  bMerlabsta = true;
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childname)) {
                bsta =
                    (100 * nresFloat > pProfile->strctAI.vlabthdmap[childname])
                        ? true
                        : false;
              }

              if (pProfile->strctAI.vlabthdmap.contains(var)) {
                bMerlabsta = (100 * ntopRes > pProfile->strctAI.vlabthdmap[var])
                                 ? true
                                 : false;
              }
            }
          }
          break;
        case STATISTIC_A4: //! 直接算占比
          if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0) {
            nres = struGsh.vlabStamap[m_StatusGpId][childname]
                       .staRes[catlst.at(k)][0] /
                   struGsh.vlabTotalmap[m_StatusGpId][str];
          }
          ntopRes += nres;

          restr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
          topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 2));

          if (bEnSorterAdj) {
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(childname)) {
              if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childname]
                           .nlowthd > 100 * nres ||
                   100 * nres > pProfile->strcSortclass
                                    .vlabParm[struGsh.nSorterAdjIdx][childname]
                                    .nhigthd))
                bsta = true;
            }

            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(var)) {
              if (struGsh.vlabTotalmap[m_StatusGpId][str] > 0 &&
                  (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][var]
                           .nlowthd > 100 * ntopRes ||
                   100 * ntopRes > pProfile->strcSortclass
                                       .vlabParm[struGsh.nSorterAdjIdx][var]
                                       .nhigthd))
                bMerlabsta = true;
            }
          } else {
            if (pProfile->strctAI.vlabthdmap.contains(childname) &&
                nres * 100 > pProfile->strctAI.vlabthdmap[childname]) {
              bsta = true;
            }
            if (pProfile->strctAI.vlabthdmap.contains(var) &&
                ntopRes * 100 > pProfile->strctAI.vlabthdmap[var]) {
              bMerlabsta = true;
            }
          }
          break;
        case STATISTIC_A2:
          if (pProfile->Area2weightmap.contains(childname)) {
            nres = struGsh.vlabStamap[m_StatusGpId][childname]
                       .staRes[catlst.at(k)][0];
          }
          restr = QString("%1").arg(QString::number(nres, 'f', 2));
          ntopRes += nres;
          topStr = QString("%1").arg(QString::number(ntopRes, 'f', 2));
          break;
        case STATISTIC_A5:
          break;
        default:
          break;
        }

        //! 设置子项数据
        if (bHasChild) {
          QTreeWidgetItem *childitem =
              topitem->child(childlst.indexOf(childname));
          if (childitem) {
            childitem->setData(k + 1, Qt::DisplayRole, restr);
            childitem->setData(statusColumnIndex, Qt::DecorationRole,
                               bsta ? QIcon(":/res/png/action_circle_red.png")
                                    : QIcon(":/res/png/action_circle.png"));
          }
        }
      }

      //! 设置顶层 item 数据
      topitem->setData(k + 1, Qt::DisplayRole, topStr);
      topitem->setData(statusColumnIndex, Qt::DecorationRole,
                       bMerlabsta ? QIcon(":/res/png/action_circle_red.png")
                                  : QIcon(":/res/png/action_circle.png"));
    }
  }

  //! 6. 第三遍循环：按顺序插入 items(创建索引对，用于排序)

  QList<QPair<int, int>> positionIndexPairs;
  for (int i = 0; i < insertPositions.size(); ++i) {
    positionIndexPairs.append(qMakePair(insertPositions[i], i));
  }

  //! 按照插入位置排序，-1（追加到末尾）放在最后
  std::sort(positionIndexPairs.begin(), positionIndexPairs.end(),
            [](const QPair<int, int> &a, const QPair<int, int> &b) {
              //! 如果都是 -1，保持原有顺序
              if (a.first == -1 && b.first == -1) {
                return a.second < b.second;
              }
              //! -1 放在最后
              if (a.first == -1)
                return false;
              if (b.first == -1)
                return true;
              //! 按位置从小到大排序
              return a.first < b.first;
            });

  //! 重构插入顺序：将不连续的 nlabIdx 重新映射为从0开始的连续下标
  QMap<int, int> indexMapping; // 原始nlabIdx -> 新的连续下标
  int newIndex = 0;

  //! 为有效的 nlabIdx 创建连续的映射
  for (const auto &pair : positionIndexPairs) {
    const int originalPosition = pair.first;
    if (originalPosition != -1) {
      if (!indexMapping.contains(originalPosition)) {
        indexMapping[originalPosition] = newIndex++;
      }
    }
  }

  //! 按重构后的顺序插入 items
  for (const auto &pair : positionIndexPairs) {
    const int originalIndex = pair.second;
    const int originalPosition = pair.first;

    if (originalPosition == -1) {
      //! 没有指定位置的项目追加到末尾
      m_labtreewid->addTopLevelItem(itemsToInsert[originalIndex]);
    } else {
      //! 使用重构后的连续下标插入
      int newPosition = indexMapping[originalPosition];
      m_labtreewid->insertTopLevelItem(newPosition,
                                       itemsToInsert[originalIndex]);
    }
  }
}

void AnalyzeTestDlg::refreshLabStaResMerger() {
  m_labtreewid->clear();

  if (struGsh.vlabStamap[0].isEmpty()) {
    return;
  }

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  const int nStaticGroup = pProfile->nStaticGroup;
  if (nStaticGroup < 2 || pProfile->vUnitaddrlist.size() < nStaticGroup ||
      pProfile->vUnitaddrlist[0].isEmpty()) {
    return;
  }

  const int lastGpId = nStaticGroup - 1;
  if (pProfile->vUnitaddrlist[lastGpId].isEmpty()) {
    return;
  }

  const int colorCamId = QString(pProfile->vUnitaddrlist[0].at(0)).toInt();
  const int matCamId = QString(pProfile->vUnitaddrlist[lastGpId].at(0)).toInt();
  const QString secondaryLab = pProfile->slabSecondary[matCamId].trimmed();
  if (secondaryLab.isEmpty()) {
    return;
  }

  //! 1. 动态设置列数并设置表头（以组0的统计类别为基准）
  QStringList headerlst;
  headerlst << QString("    %1").arg(myLan.category);
  const QList<QString> &labstalst =
      struGsh.vlabStamap[0].first().staInfo.keys();
  if (!labstalst.isEmpty()) {
    headerlst.append(labstalst);
    headerlst.append(myLan.status);
  } else {
    return;
  }

  const int headerSize = headerlst.size();
  m_labtreewid->setColumnCount(headerSize);
  m_labtreewid->setHeaderLabels(headerlst);

  int visibleCols = 0;
  for (int i = 0; i < headerSize; ++i) {
    const QString strname = headerlst.at(i);
    bool hide = false;
    if (pProfile->staclassEnmap.contains(strname) &&
        (!pProfile->staclassEnmap[strname])) {
      hide = true;
    }
    m_labtreewid->setColumnHidden(i, hide);
    if (!hide)
      ++visibleCols;
  }

  const int effectiveCols = qMax(1, visibleCols);
  const int baseArea = WIN_WID - 300;
  const int columnWidth = baseArea / effectiveCols;

  for (int i = 0; i < headerSize; ++i) {
    if (m_labtreewid->isColumnHidden(i)) {
      m_labtreewid->setColumnWidth(i, 0);
    } else {
      m_labtreewid->setColumnWidth(i, columnWidth);
    }
  }

  if (m_labtreewid->header()) {
    m_labtreewid->header()->setStretchLastSection(false);
    for (int i = headerSize - 1; i >= 0; --i) {
      if (!m_labtreewid->isColumnHidden(i)) {
        m_labtreewid->header()->setSectionResizeMode(i, QHeaderView::Stretch);
        break;
      }
    }
  }

  const bool bPercentType = (pProfile->nStaisticType == 1);
  const bool bEnSorterAdj = pProfile->bEnSorterClass;
  const int statusColumnIndex = headerSize - 1;

  //! 2. 计算统计组 lastGpId 中次一级标签的统计占比 (针对各统计类别)
  QStringList secSubLabs;
  if (pProfile->vMegerlabmap[matCamId].contains(secondaryLab)) {
    secSubLabs = pProfile->vMegerlabmap[matCamId].values(secondaryLab);
  }
  if (secSubLabs.isEmpty()) {
    secSubLabs << secondaryLab;
  }

  const QStringList &catlst = struGsh.vlabStamap[0].first().staRes.keys();
  QMap<QString, float> secRatioMap;
  for (const QString &cat : catlst) {
    float secRatio = 0.0f;
    for (const QString &subName : secSubLabs) {
      if (!struGsh.vlabStamap[lastGpId].contains(subName))
        continue;
      const QString &infoStr =
          struGsh.vlabStamap[lastGpId][subName].staInfo.value(cat);
      if (infoStr.isEmpty())
        continue;
      if (struGsh.vlabTotalmap[lastGpId].value(infoStr) > 0) {
        secRatio += struGsh.vlabStamap[lastGpId][subName].staRes[cat][0] /
                    struGsh.vlabTotalmap[lastGpId][infoStr];
      }
    }
    secRatioMap[cat] = secRatio;
  }

  //! 3. 收集颜色项与材质项的 QTreeWidgetItem
  QList<QTreeWidgetItem *> itemsToInsert;
  QList<int> insertPositions;

  // A. 可见光颜色标签（组 0）
  const QStringList &colorLabList = struGsh.vMegreLablst[colorCamId];
  const QMap<QString, int> &colorIdxMap = pProfile->vlabIdxmap[colorCamId];
  for (const QString &var : colorLabList) {
    bool bshow = true;
    if (pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]) {
      bshow = false;
    } else if (!gFlow.bMegerlabshow(var, colorCamId)) {
      bshow = false;
    }
    if (!bshow)
      continue;

    QTreeWidgetItem *topitem = new QTreeWidgetItem();
    topitem->setData(0, Qt::DisplayRole,
                     QString("%1-%2").arg(secondaryLab, var));
    topitem->setData(0, Qt::UserRole, var);   // 原始标签名
    topitem->setData(0, Qt::UserRole + 1, 0); // 来源组0 (颜色)

    int nlabIdx = colorIdxMap.value(var, -1);

    if (pProfile->vMegerlabmap[colorCamId].contains(var)) {
      const QStringList childlst =
          pProfile->vMegerlabmap[colorCamId].values(var);
      for (const QString &childname : childlst) {
        QTreeWidgetItem *childitem = new QTreeWidgetItem();
        childitem->setData(0, Qt::DisplayRole,
                           QString("%1-%2").arg(secondaryLab, childname));
        childitem->setData(0, Qt::UserRole, childname);
        childitem->setData(0, Qt::UserRole + 1, 0);
        topitem->addChild(childitem);
        if (colorIdxMap.contains(childname) && nlabIdx == -1) {
          nlabIdx = colorIdxMap[childname];
        }
      }
    }

    itemsToInsert.append(topitem);
    insertPositions.append(nlabIdx);
  }

  // B. 红外材质标签（组 lastGpId），排除次一级标签
  const QStringList &matLabList = struGsh.vMegreLablst[matCamId];
  const QMap<QString, int> &matIdxMap = pProfile->vlabIdxmap[matCamId];
  for (const QString &var : matLabList) {
    if (var == secondaryLab)
      continue;
    if (pProfile->vMegerlabmap[matCamId].contains(var) &&
        pProfile->vMegerlabmap[matCamId].values(var).contains(secondaryLab)) {
      continue;
    }

    bool bshow = true;
    if (pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]) {
      bshow = false;
    } else if (!gFlow.bMegerlabshow(var, matCamId)) {
      bshow = false;
    }
    if (!bshow)
      continue;

    QTreeWidgetItem *topitem = new QTreeWidgetItem();
    topitem->setData(0, Qt::DisplayRole, var);       // 材质名称直接显示
    topitem->setData(0, Qt::UserRole, var);          // 原始标签名
    topitem->setData(0, Qt::UserRole + 1, lastGpId); // 来源材质组

    int nlabIdx = matIdxMap.value(var, -1);
    if (nlabIdx != -1) {
      nlabIdx += 1000; // 偏移确保材质项排在颜色项后面
    }

    if (pProfile->vMegerlabmap[matCamId].contains(var)) {
      const QStringList childlst = pProfile->vMegerlabmap[matCamId].values(var);
      for (const QString &childname : childlst) {
        if (childname == secondaryLab)
          continue;
        QTreeWidgetItem *childitem = new QTreeWidgetItem();
        childitem->setData(0, Qt::DisplayRole, childname);
        childitem->setData(0, Qt::UserRole, childname);
        childitem->setData(0, Qt::UserRole + 1, lastGpId);
        topitem->addChild(childitem);
      }
    }

    itemsToInsert.append(topitem);
    insertPositions.append(nlabIdx);
  }

  //! 计算可见组与剩余材质组的原始占比总和，推导剩余材质缩放系数 matScaleMap
  QMap<QString, float> visRawSumMap;
  QMap<QString, float> matOtherRawSumMap;
  for (QTreeWidgetItem *topitem : itemsToInsert) {
    const int srcGp = topitem->data(0, Qt::UserRole + 1).toInt();
    const int childCount = topitem->childCount();

    QList<QTreeWidgetItem *> subItems;
    if (childCount > 0) {
      for (int c = 0; c < childCount; ++c) {
        subItems.append(topitem->child(c));
      }
    } else {
      subItems.append(topitem);
    }

    for (const QString &cat : catlst) {
      for (QTreeWidgetItem *item : subItems) {
        const QString labName = item->data(0, Qt::UserRole).toString();
        if (!struGsh.vlabStamap[srcGp].contains(labName))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][labName].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();
        if (statype != STATISTIC_A3 && statype != STATISTIC_A4)
          continue;

        if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
          float rawRatio = struGsh.vlabStamap[srcGp][labName].staRes[cat][0] /
                           struGsh.vlabTotalmap[srcGp][str];
          if (srcGp == 0) {
            visRawSumMap[cat] += rawRatio;
          } else {
            matOtherRawSumMap[cat] += rawRatio;
          }
        }
      }
    }
  }

  QMap<QString, float> matScaleMap;
  for (const QString &cat : catlst) {
    const float rRatio = secRatioMap.value(cat, 0.0f);
    const float sVis = visRawSumMap.value(cat, 0.0f);
    const float sMatOther = matOtherRawSumMap.value(cat, 0.0f);
    const float rRemain = qMax(0.0f, 1.0f - sVis * rRatio);
    matScaleMap[cat] = (sMatOther > 0.0f) ? (rRemain / sMatOther) : 0.0f;
  }

  //! 4. 针对瓶片进行预遍历，计算归一化差额 nDiffMap 与最大标签项 (针对
  //! STATISTIC_A3，按统计类别独立计算)
  QMap<QString, float> nDiffMap;
  QMap<QString, QString> maxValChildMap;

  for (int k = 0; k < catlst.size(); ++k) {
    const QString &cat = catlst.at(k);
    const float rRatio = secRatioMap.value(cat, 0.0f);
    const float matScale = matScaleMap.value(cat, 0.0f);
    float totalValue = 0.0f;
    float maxRes = -1.0f;
    QString strmaxchild;

    for (QTreeWidgetItem *topitem : itemsToInsert) {
      const int srcGp = topitem->data(0, Qt::UserRole + 1).toInt();
      const float scale = (srcGp == 0) ? rRatio : matScale;
      const int childCount = topitem->childCount();

      QList<QTreeWidgetItem *> subItems;
      if (childCount > 0) {
        for (int c = 0; c < childCount; ++c) {
          subItems.append(topitem->child(c));
        }
      } else {
        subItems.append(topitem);
      }

      for (QTreeWidgetItem *item : subItems) {
        const QString labName = item->data(0, Qt::UserRole).toString();
        if (!struGsh.vlabStamap[srcGp].contains(labName))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][labName].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();
        if (statype != STATISTIC_A3)
          continue;

        if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
          float rawRatio = struGsh.vlabStamap[srcGp][labName].staRes[cat][0] /
                           struGsh.vlabTotalmap[srcGp][str];
          float nres = rawRatio * scale;

          float nVal = 0.0f;
          if (bPercentType) {
            nVal = QString::number(nres, 'f', 4).toFloat();
          } else {
            nVal = (int)(nres * 1000000);
          }

          totalValue += nVal;
          if (nVal > maxRes) {
            maxRes = nVal;
            strmaxchild = labName;
          }
        }
      }
    }

    if (totalValue > 0) {
      if (bPercentType) {
        nDiffMap[cat] = QString::number(1.0 - totalValue, 'f', 4).toFloat();
      } else {
        nDiffMap[cat] = (int)(1000000.0 - totalValue);
      }
      maxValChildMap[cat] = strmaxchild;
    }
  }

  //! 5. 填充统计数据与状态
  const QIcon redIcon(":/res/png/action_circle_red.png");
  const QIcon normalIcon(":/res/png/action_circle.png");

  for (QTreeWidgetItem *topitem : itemsToInsert) {
    const int srcGp = topitem->data(0, Qt::UserRole + 1).toInt();
    const QString topOrigLab = topitem->data(0, Qt::UserRole).toString();
    const int childCount = topitem->childCount();
    const bool bHasChild = (childCount > 0);

    QList<QTreeWidgetItem *> subItems;
    if (bHasChild) {
      for (int c = 0; c < childCount; ++c) {
        subItems.append(topitem->child(c));
      }
    } else {
      subItems.append(topitem);
    }

    for (int k = 0; k < catlst.size(); ++k) {
      const QString &cat = catlst.at(k);
      const float rRatio = secRatioMap.value(cat, 0.0f);
      const float scale = (srcGp == 0) ? rRatio : matScaleMap.value(cat, 0.0f);

      float ntopRes = 0.0f;
      int ntopResInt = 0;
      bool bMerlabsta = false;
      QString topStr;

      for (QTreeWidgetItem *item : subItems) {
        const QString childOrigLab = item->data(0, Qt::UserRole).toString();
        bool bsta = false;
        QString restr;
        float nres = 0.0f;

        if (!struGsh.vlabStamap[srcGp].contains(childOrigLab))
          continue;
        const QString &str =
            struGsh.vlabStamap[srcGp][childOrigLab].staInfo.value(cat);
        if (str.isEmpty())
          continue;

        const QStringList strParts = str.split("/");
        if (strParts.size() < 2)
          continue;
        const int statype = strParts.at(1).mid(1).toInt();

        switch (statype) {
        case STATISTIC_A1: {
          nres = struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] * scale;
          restr = QString::number((int)nres);
          ntopRes += nres;
          topStr = QString::number((int)ntopRes);
          break;
        }
        case STATISTIC_A3: {
          if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
            float rawRatio =
                struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] /
                struGsh.vlabTotalmap[srcGp][str];
            nres = rawRatio * scale;
          }

          if (!bPercentType) {
            int nresInt = (int)(nres * 1000000);
            if (childOrigLab == maxValChildMap.value(cat)) {
              nresInt += (int)nDiffMap.value(cat, 0.0f);
            }
            ntopResInt += nresInt;
            restr = QString::number(nresInt);
            topStr = QString::number(ntopResInt);

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nlowthd > nresInt ||
                     nresInt >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nhigthd)) {
                  bsta = true;
                }
              }
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(topOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nlowthd > ntopResInt ||
                     ntopResInt >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nhigthd)) {
                  bMerlabsta = true;
                }
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childOrigLab) &&
                  nresInt > pProfile->strctAI.vlabthdmap[childOrigLab]) {
                bsta = true;
              }
              if (pProfile->strctAI.vlabthdmap.contains(topOrigLab) &&
                  ntopResInt > pProfile->strctAI.vlabthdmap[topOrigLab]) {
                bMerlabsta = true;
              }
            }
          } else {
            float nresFloat = QString::number(nres, 'f', 4).toFloat();
            if (childOrigLab == maxValChildMap.value(cat)) {
              nresFloat += nDiffMap.value(cat, 0.0f);
            }
            ntopRes += nresFloat;
            restr =
                QString("%1%").arg(QString::number(nresFloat * 100, 'f', 4));
            topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 4));

            if (bEnSorterAdj) {
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(childOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nlowthd > 100 * nresFloat ||
                     100 * nresFloat >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                             .nhigthd)) {
                  bsta = true;
                }
              }
              if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                      .contains(topOrigLab)) {
                if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                    (pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nlowthd > 100 * ntopRes ||
                     100 * ntopRes >
                         pProfile->strcSortclass
                             .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                             .nhigthd)) {
                  bMerlabsta = true;
                }
              }
            } else {
              if (pProfile->strctAI.vlabthdmap.contains(childOrigLab)) {
                bsta = (100 * nresFloat >
                        pProfile->strctAI.vlabthdmap[childOrigLab]);
              }
              if (pProfile->strctAI.vlabthdmap.contains(topOrigLab)) {
                bMerlabsta =
                    (100 * ntopRes > pProfile->strctAI.vlabthdmap[topOrigLab]);
              }
            }
          }
          break;
        }
        case STATISTIC_A4: {
          if (struGsh.vlabTotalmap[srcGp].value(str) > 0) {
            float rawRatio =
                struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] /
                struGsh.vlabTotalmap[srcGp][str];
            nres = rawRatio * scale;
          }
          ntopRes += nres;
          restr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
          topStr = QString("%1%").arg(QString::number(ntopRes * 100, 'f', 2));
          if (bEnSorterAdj) {
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(childOrigLab)) {
              if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                           .nlowthd > 100 * nres ||
                   100 * nres >
                       pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][childOrigLab]
                           .nhigthd)) {
                bsta = true;
              }
            }
            if (pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx]
                    .contains(topOrigLab)) {
              if (struGsh.vlabTotalmap[srcGp].value(str) > 0 &&
                  (pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                           .nlowthd > 100 * ntopRes ||
                   100 * ntopRes >
                       pProfile->strcSortclass
                           .vlabParm[struGsh.nSorterAdjIdx][topOrigLab]
                           .nhigthd)) {
                bMerlabsta = true;
              }
            }
          } else {
            if (pProfile->strctAI.vlabthdmap.contains(childOrigLab) &&
                nres * 100 > pProfile->strctAI.vlabthdmap[childOrigLab]) {
              bsta = true;
            }
            if (pProfile->strctAI.vlabthdmap.contains(topOrigLab) &&
                ntopRes * 100 > pProfile->strctAI.vlabthdmap[topOrigLab]) {
              bMerlabsta = true;
            }
          }
          break;
        }
        case STATISTIC_A2: {
          nres = struGsh.vlabStamap[srcGp][childOrigLab].staRes[cat][0] * scale;
          restr = QString("%1").arg(QString::number(nres, 'f', 2));
          ntopRes += nres;
          topStr = QString("%1").arg(QString::number(ntopRes, 'f', 2));
          break;
        }
        default:
          break;
        }

        if (bHasChild) {
          item->setData(k + 1, Qt::DisplayRole, restr);
          item->setData(statusColumnIndex, Qt::DecorationRole,
                        bsta ? redIcon : normalIcon);
        }
      }

      topitem->setData(k + 1, Qt::DisplayRole, topStr);
      topitem->setData(statusColumnIndex, Qt::DecorationRole,
                       bMerlabsta ? redIcon : normalIcon);
    }
  }

  //! 6. 排序并插入 items
  QList<QPair<int, int>> positionIndexPairs;
  for (int i = 0; i < insertPositions.size(); ++i) {
    positionIndexPairs.append(qMakePair(insertPositions[i], i));
  }

  std::sort(positionIndexPairs.begin(), positionIndexPairs.end(),
            [](const QPair<int, int> &a, const QPair<int, int> &b) {
              if (a.first == -1 && b.first == -1) {
                return a.second < b.second;
              }
              if (a.first == -1)
                return false;
              if (b.first == -1)
                return true;
              return a.first < b.first;
            });

  QMap<int, int> indexMapping;
  int newIndex = 0;
  for (const auto &pair : positionIndexPairs) {
    const int originalPosition = pair.first;
    if (originalPosition != -1) {
      if (!indexMapping.contains(originalPosition)) {
        indexMapping[originalPosition] = newIndex++;
      }
    }
  }

  for (const auto &pair : positionIndexPairs) {
    const int originalIndex = pair.second;
    const int originalPosition = pair.first;
    if (originalPosition == -1) {
      m_labtreewid->addTopLevelItem(itemsToInsert[originalIndex]);
    } else {
      int newPosition = indexMapping[originalPosition];
      m_labtreewid->insertTopLevelItem(newPosition,
                                       itemsToInsert[originalIndex]);
    }
  }
}

void AnalyzeTestDlg::refreshSimulateRes(bool bReset) {
  if (bReset) {
    m_planEdit->clear();
  }

  if (struGsh.nFeederState && !bReset) {
    m_planEdit->clear();
  }
}
