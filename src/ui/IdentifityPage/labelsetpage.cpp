#include "labelsetpage.h"

labelsetPage::labelsetPage(QWidget *parent) : QWidget(parent), m_nIdx(0) {
  createPage();
  layOutPage();
  connectAndSlt();
}

void labelsetPage::createPage() {
  m_tabBar = new QTabBar();
  m_tabBar->addTab(myLan.params_set);
  m_tabBar->addTab(myLan.lab_Meger);
  m_tabBar->addTab(myLan.label_priority);
  m_tabBar->addTab(myLan.label_mix);
  m_tabBar->setExpanding(false);

  m_tabBar->setStyleSheet(
      "QTabBar::tab {min-width: 150px; padding: "
      "8px;min-height:45px;margin-right:5px }"
      "QTabBar::tab:selected { background-color: #007454; color: white; }"
      "QTabBar::tab:!selected { background-color: #f0f0f0; color: #333333; }");
  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_tabBar->setTabEnabled(2, false);
    m_tabBar->setTabEnabled(3, false);
  }

  m_typeswBtn = new QPushButton(
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nIdx]);
  m_typeswBtn->setFixedSize(BTN_WID + 60, BTN_HEI);
  m_typeswBtn->setIconSize(QSize(50, 50));
  m_typeswBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));

  m_labMerPage = new labelmerger;
  m_labPriPage = new labelprority();
  m_labMixPage = new labelmixpage();

  //! 标签参数
  m_labParamsPage = new QWidget();
  m_materialParamsSetGb = new QGroupBox();
  m_materialScroarea = new QScrollArea();
  m_materialScroarea->setWidgetResizable(true);
  m_materialScroarea->setWidget(m_materialParamsSetGb);

  QStringList titlelst;
  titlelst << myLan.lab_hide << myLan.coefficient << myLan.threshold
           << myLan.index << myLan.secondary << myLan.properity;
  for (int i = 0; i < LAB_ALL; i++) {
    m_titlelb[i] = new QLabel(titlelst.at(i));
  }

  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_titlelb[LAB_SECONDARY]->hide();
  }
  m_titlelb[LAB_PAOPERTY]->hide();

  for (int i = 0; i < MAX_MIXLAB_NUM; i++) {
    m_labchk[i] = new MyCheckBox("");
    m_labchk[i]->setCheckBtnSize(60, 60);
    m_labconvertcofle[i] = new MyLineEdit();
    m_labconvertcofle[i]->setFixedWidth(120);
    m_labthdsetle[i] = new MyLineEdit();
    m_labthdsetle[i]->setFixedWidth(120);
    m_propertysetBtn[i] = new QPushButton();
    m_propertysetBtn[i]->setFixedWidth(120);
    m_propertysetBtn[i]->hide();
    m_labIdxle[i] = new MyLineEdit();
    m_labIdxle[i]->setFixedWidth(120);
    m_secondarychk[i] = new MyCheckBox("");
    m_secondarychk[i]->setCheckBtnSize(60, 60);
  }

  m_Metallb = new QLabel(QString("%1-%2").arg(myLan.metal).arg(myLan.threshold));
  m_Metalle = new MyLineEdit();
  m_Metalle->setFixedWidth(120);
  if (struCnfg.nMachineType == nMachine_Crop) {
    m_Metallb->hide();
    m_Metalle->hide();
  }

  m_stackwid = new QStackedWidget();
  m_stackwid->addWidget(m_labParamsPage);
  m_stackwid->addWidget(m_labMerPage);
  m_stackwid->addWidget(m_labPriPage);
  m_stackwid->addWidget(m_labMixPage);
}

void labelsetPage::layOutPage() {
  glaymaterial = new QGridLayout();

  for (int i = 0; i < LAB_ALL; i++) {
    if (i == 0) {
      glaymaterial->addWidget(m_titlelb[i], 0, i, 1, 1, Qt::AlignLeft);
    } else {
      glaymaterial->addWidget(m_titlelb[i], 0, i, 1, 1, Qt::AlignCenter);
    }
  }

  for (int i = 0; i < MAX_MIXLAB_NUM; i++) {
    glaymaterial->addWidget(m_labchk[i], 1 + i, 0, Qt::AlignLeft);
    glaymaterial->addWidget(m_labconvertcofle[i], 1 + i, 1, Qt::AlignCenter);
    glaymaterial->addWidget(m_labthdsetle[i], 1 + i, 2, Qt::AlignCenter);
    glaymaterial->addWidget(m_labIdxle[i], 1 + i, 3, Qt::AlignCenter);
    glaymaterial->addWidget(m_secondarychk[i], 1 + i, 4, Qt::AlignCenter);
    glaymaterial->addWidget(m_propertysetBtn[i], 1 + i, 5, Qt::AlignCenter);
  }

  QVBoxLayout *laymaterialgb = new QVBoxLayout();
  laymaterialgb->addLayout(glaymaterial);
  laymaterialgb->addStretch();
  m_materialParamsSetGb->setLayout(laymaterialgb);

  QHBoxLayout *hlaydown = new QHBoxLayout();
  hlaydown->addWidget(m_Metallb);
  hlaydown->addWidget(m_Metalle);
  hlaydown->addStretch();

  QVBoxLayout *vlaymorewid = new QVBoxLayout(m_labParamsPage);
  vlaymorewid->addWidget(m_materialScroarea);
  vlaymorewid->addLayout(hlaydown);
  vlaymorewid->addSpacing(5);

  QHBoxLayout *hlaytop = new QHBoxLayout();
  hlaytop->addWidget(m_tabBar);
  hlaytop->addStretch();
  hlaytop->addWidget(m_typeswBtn);
  hlaytop->addSpacing(8);

  QVBoxLayout *hlaymain = new QVBoxLayout();
  hlaymain->addLayout(hlaytop);
  hlaymain->addWidget(m_stackwid);
  setLayout(hlaymain);
}

void labelsetPage::connectAndSlt() {
  connect(m_typeswBtn, &QPushButton::pressed, this,
          &labelsetPage::onTypeBtnPressedSlt); //! 相机视角
  connect(m_tabBar, &QTabBar::currentChanged, this,
          &labelsetPage::onTabchgedSlt);

  QSignalMapper *siglabshowmap = new QSignalMapper();     //! 标签chck
  QSignalMapper *siglabcoffmap = new QSignalMapper();     //! 标签转换系数
  QSignalMapper *siglabthdmap = new QSignalMapper();      //! 标签阈值
  QSignalMapper *siglabpropertymap = new QSignalMapper(); //! 标签属性
  QSignalMapper *siglabidxmap = new QSignalMapper();      //! 标签顺序
  QSignalMapper *siglabsecdarymap = new QSignalMapper();  //! 次一级

  for (int i = 0; i < MAX_MIXLAB_NUM; i++) {
    siglabshowmap->setMapping(m_labchk[i], i);
    connect(m_labchk[i], &MyCheckBox::pressed, siglabshowmap,
            QOverload<>::of(&QSignalMapper::map));

    siglabcoffmap->setMapping(m_labconvertcofle[i], i);
    connect(m_labconvertcofle[i], &MyLineEdit::pressed, siglabcoffmap,
            QOverload<>::of(&QSignalMapper::map));

    siglabthdmap->setMapping(m_labthdsetle[i], i);
    connect(m_labthdsetle[i], &MyLineEdit::pressed, siglabthdmap,
            QOverload<>::of(&QSignalMapper::map));

    siglabpropertymap->setMapping(m_propertysetBtn[i], i);
    connect(m_propertysetBtn[i], &QPushButton::pressed, siglabpropertymap,
            QOverload<>::of(&QSignalMapper::map));

    siglabidxmap->setMapping(m_labIdxle[i], i);
    connect(m_labIdxle[i], &MyLineEdit::pressed, siglabidxmap,
            QOverload<>::of(&QSignalMapper::map));

    siglabsecdarymap->setMapping(m_secondarychk[i], i);
    connect(m_secondarychk[i], &MyCheckBox::pressed, siglabsecdarymap,
            QOverload<>::of(&QSignalMapper::map));
  }

  connect(siglabshowmap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabhidPressedSlt);
  connect(siglabcoffmap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabcoeffChgSlt);
  connect(siglabthdmap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabthdChgSlt);
  connect(siglabpropertymap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabpropertyChgSlt);
  connect(siglabidxmap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabidxChgSlt);
  connect(siglabsecdarymap, QOverload<int>::of(&QSignalMapper::mapped), this,
          &labelsetPage::onLabsecondaryChgSlt);

  connect(m_Metalle, &MyLineEdit::pressed, this,
          &labelsetPage::onMetalnumChgSlt);
}

void labelsetPage::refreshPage() {
  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_tabBar->setTabEnabled(2, false);
    m_tabBar->setTabEnabled(3, false);
  } else {
    m_tabBar->setTabEnabled(2, true);
    m_tabBar->setTabEnabled(3, true);
  }

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  if (pProfile->nStaticGroup <= 1) {
    m_nIdx = 0;
    m_typeswBtn->setText(
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nIdx]);
  }

  int nIdx = m_tabBar->currentIndex();
  onTabchgedSlt(nIdx);
}

void labelsetPage::onTabchgedSlt(int nIdx) {

  m_stackwid->setCurrentIndex(nIdx);

  switch (nIdx) {
  case 0: //! 标签参数
    reRreshlabWid();
    break;
  case 1: //! 标签合并
    m_labMerPage->refreshPage(m_nIdx);
    break;
  case 2: //! 标签优先级
    m_labPriPage->refreshPage(0);
    break;
  case 3: //! 标签互混
    m_labMixPage->refreshPage(m_nIdx);
    break;
  default:
    break;
  }
}

/*!
 * \brief 槽：相机视角
 */
void labelsetPage::onTypeBtnPressedSlt() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

  if (pProfile->nStaticGroup <= 1) {
    return;
  }

  m_nIdx += 1;
  if (m_nIdx >= gFlow.getMachineCamNum()) {
    m_nIdx = 0;
  }

  m_typeswBtn->setText(
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nIdx]);

  if (m_tabBar->currentIndex() == 0) {
    reRreshlabWid();
  } else if (m_tabBar->currentIndex() == 1) {
    m_labMerPage->refreshPage(m_nIdx);
  } else if (m_tabBar->currentIndex() == 3) {
    m_labMixPage->refreshPage(m_nIdx);
  }
}

void labelsetPage::onLabhidPressedSlt(int nIdx) {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  pProfile->blabshowmap[m_labchk[nIdx]->text()] =
      m_labchk[nIdx]->isChecked() ? true : false;
}

void labelsetPage::onLabcoeffChgSlt(int nIdx) {
  myInputPanel *inputPanel = new myInputPanel(
      floatType, 0, 10000.0, m_labconvertcofle[nIdx]->text().toInt(), "",
      g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    float n_sample = inputPanel->getValue();
    m_labconvertcofle[nIdx]->setText(QString("%1").arg(n_sample));

    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->analyzeProfile.Area2weightmap[m_labchk[nIdx]->text()] = n_sample;
  }
}

void labelsetPage::onLabthdChgSlt(int nIdx) {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  float nMax = 100.0;

  if (struCnfg.nMachineType == nMachine_Bottle &&
      pProfile->nStaisticType == 0) {
    nMax = 10000000.0;
  }

  myInputPanel *inputPanel =
      new myInputPanel(floatType, 0, nMax, m_labthdsetle[nIdx]->text().toInt(),
                       "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    float n_sample = inputPanel->getValue();
    m_labthdsetle[nIdx]->setText(QString("%1").arg(n_sample));

    pProfile->strctAI.vlabthdmap[m_labchk[nIdx]->text()] = n_sample;
  }
}

void labelsetPage::onLabpropertyChgSlt(int nIdx) {
  bool bRes = false;
  if (m_propertysetBtn[nIdx]->text() == "好") {
    bRes = false;
  } else {
    bRes = true;
  }

  if (bRes) {
    m_propertysetBtn[nIdx]->setText(QString("好"));
  } else {
    m_propertysetBtn[nIdx]->setText(QString("坏"));
  }

  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
      ->analyzeProfile.labprotymap[m_labchk[nIdx]->text()] = bRes;
}

void labelsetPage::onLabidxChgSlt(int nIdx) {
  int nMax = struGsh.vlabNamemap[m_nIdx].size();
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, nMax - 1, m_labIdxle[nIdx]->text().toInt(), "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    auto *pProfile =
        &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    int n_sample = inputPanel->getValue();
    m_labIdxle[nIdx]->setText(QString("%1").arg(n_sample));

    bool btop = false;
    QStringList lablst;
    const QStringList &Merlablst = pProfile->vMegerlabmap[m_nIdx].uniqueKeys();
    QString labname = m_labchk[nIdx]->text();
    QString topstr = gFlow.getMegerlabKeyByValue(labname, m_nIdx);

    if (Merlablst.contains(labname) || !topstr.isEmpty()) {
      btop = true;
    }

    if (btop) {
      QString topname;
      if (topstr.isEmpty()) {
        topname = labname;
      } else {
        topname = topstr;
      }

      lablst << topname;
      for (QString &sub : pProfile->vMegerlabmap[m_nIdx].values(topname)) {
        lablst << sub;
      }
    } else {
      lablst << labname;
    }

    for (int i = 0; i < lablst.size(); i++) {
      for (int j = 0; j < MAX_MIXLAB_NUM; j++) {
        if (m_labchk[j]->text() == lablst.at(i)) {
          m_labIdxle[j]->setText(QString("%1").arg(n_sample));
          if ((i != 0) || (!btop)) {
            pProfile->vlabIdxmap[m_nIdx][lablst.at(i)] = n_sample;
          }
          break;
        }
      }
    }
  }
}

void labelsetPage::onMetalnumChgSlt() {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 10000, m_Metalle->text().toInt(), "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int n_sample = inputPanel->getValue();
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nMeatal =
        n_sample;
  }
}

/*!
 * \brief 槽：次一级标签
 * \param nIdx
 */
void labelsetPage::onLabsecondaryChgSlt(int nIdx) {
  if (nIdx < 0 || nIdx >= MAX_MIXLAB_NUM)
    return;

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  bool bBottle = (struCnfg.nMachineType == nMachine_Bottle);
  int nLastGroupIdx = pProfile->nStaticGroup - 1;

  if (m_secondarychk[nIdx]->isChecked()) {
    for (int i = 0; i < pProfile->nStaticGroup; i++) {
      if (i >= pProfile->vUnitaddrlist.size()) {
        continue;
      }
      QStringList camIdlst = pProfile->vUnitaddrlist[i];
      if (camIdlst.contains(QString::number(m_nIdx)) == false) {
        continue;
      }

      // 当机型为nMachine_Bottle时，只有最后一个统计组设置次一级标签，其他统计组不设置
      if (bBottle && i != nLastGroupIdx) {
        m_secondarychk[nIdx]->setChecked(false);
        for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
          int nCamIdx = strcamid.toInt();
          pProfile->slabSecondary[nCamIdx] = QString();
        }
        return;
      }

      for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
        int nCamIdx = strcamid.toInt();
        pProfile->slabSecondary[nCamIdx] = m_labchk[nIdx]->text();
      }
    }

    // 瓶机模式下，确保其他统计组不设置次一级标签
    if (bBottle) {
      for (int i = 0; i < nLastGroupIdx; i++) {
        if (i < pProfile->vUnitaddrlist.size()) {
          for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
            int nCamIdx = strcamid.toInt();
            pProfile->slabSecondary[nCamIdx] = QString();
          }
        }
      }
    }

    // 全局单选互斥：父子互斥、子子互斥、不同标签之间互斥（全表同一时刻只能有一个被勾选）
    for (int i = 0; i < MAX_MIXLAB_NUM; ++i) {
      if (i != nIdx) {
        m_secondarychk[i]->setChecked(false);
      }
    }
  } else {
    for (int i = 0; i < pProfile->nStaticGroup; i++) {
      if (i >= pProfile->vUnitaddrlist.size()) {
        continue;
      }
      QStringList camIdlst = pProfile->vUnitaddrlist[i];
      if (camIdlst.contains(QString::number(m_nIdx)) == false) {
        continue;
      }
      for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
        int nCamIdx = strcamid.toInt();
        pProfile->slabSecondary[nCamIdx] = QString();
      }
    }

    for (int i = 0; i < MAX_MIXLAB_NUM; ++i) {
      m_secondarychk[i]->setChecked(false);
    }
  }
}

/*!
 * \brief labelsetPage::reRreshlabWid
 */
void labelsetPage::reRreshlabWid() {
  bool bBootle = struCnfg.nMachineType == nMachine_Bottle ? true : false;
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

  m_Metallb->hide();
  m_Metalle->hide();
  if (bBootle && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect) {
    m_Metallb->show();
    m_Metalle->show();
  }

  if (bBootle && pProfile->nStaisticType == 0) {
    m_titlelb[LAB_THD]->setText(myLan.threshold);
  } else {
    m_titlelb[LAB_THD]->setText(
        QString("%1%2").arg(myLan.threshold).arg("(%)"));
  }

  if (bBootle) {
    m_titlelb[LAB_SECONDARY]->setText(myLan.majorProduct);
  } else {
    m_titlelb[LAB_SECONDARY]->setText(myLan.secondary);
  }

  //! 判断当前相机视角是否允许设置次一级标签（瓶机模式下仅限最后一个统计组）
  int nLastGroupIdx = pProfile->nStaticGroup - 1;
  bool bCanSetSecondary = true;
  if (bBootle) {
    bCanSetSecondary = false;
    if (nLastGroupIdx >= 0 && nLastGroupIdx < pProfile->vUnitaddrlist.size()) {
      if (pProfile->vUnitaddrlist[nLastGroupIdx].contains(
              QString::number(m_nIdx))) {
        bCanSetSecondary = true;
      }
    }
  }

  if (bCanSetSecondary) {
    m_titlelb[LAB_SECONDARY]->show();
  } else {
    m_titlelb[LAB_SECONDARY]->hide();
  }

  for (int i = 0; i < 60; i++) {
    m_labchk[i]->hide();
    m_labchk[i]->setDisabled(false);
    m_labconvertcofle[i]->hide();
    m_labconvertcofle[i]->setDisabled(false);
    m_labthdsetle[i]->hide();
    m_propertysetBtn[i]->hide();
    m_labIdxle[i]->hide();
    m_secondarychk[i]->hide();
    m_secondarychk[i]->setDisabled(false);
    m_secondarychk[i]->setChecked(false);
  }

  int nIdx = 0;
  for (QString toplab : struGsh.vMegreLablst[m_nIdx]) {
    bool bchild = false;
    QStringList childlst;
    childlst << toplab;
    if (pProfile->vMegerlabmap[m_nIdx].contains(toplab)) {
      childlst.append(pProfile->vMegerlabmap[m_nIdx].values(toplab));
      bchild = true;
    }

    for (int n = 0; n < childlst.size(); n++) {
      m_labchk[nIdx]->show();
      m_labchk[nIdx]->setText(childlst.at(n));
      m_labchk[nIdx]->setChecked(pProfile->blabshowmap[childlst.at(n)]);

      if (!bCanSetSecondary) {
        m_secondarychk[nIdx]->hide();
        m_secondarychk[nIdx]->setDisabled(true);
        m_secondarychk[nIdx]->setChecked(false);
      } else {
        m_secondarychk[nIdx]->show();
        m_secondarychk[nIdx]->setDisabled(
            false); // 父标签与子标签均允许勾选为次一级标签
        if (!pProfile->slabSecondary[m_nIdx].isEmpty() &&
            pProfile->slabSecondary[m_nIdx] == childlst.at(n)) {
          m_secondarychk[nIdx]->setChecked(true);
        } else {
          m_secondarychk[nIdx]->setChecked(false);
        }
      }

      if (n == 0) {
        glaymaterial->setAlignment(m_labchk[nIdx], Qt::AlignLeft);
        if (bchild) {
          m_labchk[nIdx]->setDisabled(true);
        }
      } else {
        glaymaterial->setAlignment(m_labchk[nIdx], Qt::AlignCenter);
      }

      m_labconvertcofle[nIdx]->show();
      if (n == 0 && bchild) {
        m_labconvertcofle[nIdx]->setText(QString(""));
        m_labconvertcofle[nIdx]->setDisabled(true);
      } else {
        m_labconvertcofle[nIdx]->setText(
            QString("%1").arg(pProfile->Area2weightmap[childlst.at(n)]));
      }

      m_labthdsetle[nIdx]->show();
      m_labthdsetle[nIdx]->setText(
          QString("%1").arg(pProfile->strctAI.vlabthdmap[childlst.at(n)]));

      //          m_propertysetBtn[nIdx]->show();
      //          QString str =
      //          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.labprotymap[childlst.at(n)]?QString(myLan.good):QString(myLan.bad);
      //          m_propertysetBtn[nIdx]->setText(str);

      //! 刷新标签序号
      QString stridx;
      if (bchild && n == 0) {
        for (int k = 1; k < childlst.size(); k++) {
          if (pProfile->vlabIdxmap[m_nIdx].keys().contains(childlst.at(k))) {
            stridx =
                QString("%1").arg(pProfile->vlabIdxmap[m_nIdx][childlst.at(k)]);
            break;
          }
        }
      } else {
        if (pProfile->vlabIdxmap[m_nIdx].keys().contains(childlst.at(n))) {
          stridx =
              QString("%1").arg(pProfile->vlabIdxmap[m_nIdx][childlst.at(n)]);
        }
      }
      m_labIdxle[nIdx]->show();
      m_labIdxle[nIdx]->setText(stridx);

      nIdx += 1;
    }
  }

  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_Metalle->setText(QString("%1").arg(
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nMeatal));
  }
}
