#include "shapenesspage.h"

shapenesspage::shapenesspage(QWidget *parent) : QWidget(parent) {
  cratePage();
  layoutPage();
  connectAndSlt();
}

void shapenesspage::cratePage() {
  m_staticParamsetGb = new QGroupBox(myLan.params_set);
  m_staticParamsetGb->setMaximumWidth(560);
  //    m_staticParamsetGb->setMaximumHeight(850);
  m_shapelst << QString("%1(mm)").arg(myLan.length)
             << QString("%1(mm)").arg(myLan.width)
             << QString("%1(mm)").arg(myLan.area)
             << QString("%1").arg(myLan.length)
             << QString("%1").arg(myLan.width) << QString("%1").arg(myLan.area);

  for (int i = 0; i < 3; i++) {
    m_shapeParamschk[i] = new MyCheckBox(m_shapelst.at(i));
    m_framelb[i] = new QLabel("——");
  }

  for (int i = 0; i < 6; i++) {
    m_shapeParamsle[i] = new MyLineEdit();
    if (i % 2 == 0) {
      m_shapeParamsle[i]->setText(QString("0"));
    } else {
      m_shapeParamsle[i]->setText(QString("100"));
    }
    m_shapeParamsle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");

    m_shapeParamsle[i]->setDisabled(true);
  }

  m_bEnshapelb = new QLabel(QString("%1").arg(myLan.shapeness_enable));
  m_bEnshapesw = new mySwitchButton();
  m_bEnshapesw->setFixedSize(150, 60);

  //! 形选标签使能
  m_shplabEnGb = new QGroupBox(myLan.label);
  m_shplabEnGb->setMinimumWidth(1130);

  for (int i = 0; i < MAX_LABELS; i++) {
    m_shplabchk[i] = new MyCheckBox("");
  }

  //! 分类
  m_classficationGb = new QGroupBox(myLan.setclassifythd);
  m_classficationGb->setMaximumHeight(300);
  for (int i = 0; i < MAX_CLASSIFY_NUM - 1; i++) {
    m_classifylb[i] =
        new QLabel(QString("%1%2").arg(myLan.threshold).arg(i + 1));
    m_classifyle[i] = new MyLineEdit();
    m_classifyle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
  }
}

void shapenesspage::layoutPage() {
  //! 形选参数
  QGridLayout *glayshape = new QGridLayout(m_staticParamsetGb);
  glayshape->setContentsMargins(0, 50, 0, 0);

  glayshape->addWidget(m_shapeParamschk[0], 0, 0, 1, 2);
  glayshape->addWidget(m_shapeParamsle[0], 0, 2, 1, 1);
  glayshape->addWidget(m_framelb[0], 0, 3, 1, 1);
  glayshape->addWidget(m_shapeParamsle[1], 0, 4, 1, 1);

  glayshape->addWidget(m_shapeParamschk[1], 1, 0, 1, 2);
  glayshape->addWidget(m_shapeParamsle[2], 1, 2, 1, 1);
  glayshape->addWidget(m_framelb[1], 1, 3, 1, 1);
  glayshape->addWidget(m_shapeParamsle[3], 1, 4, 1, 1);

  glayshape->addWidget(m_shapeParamschk[2], 2, 0, 1, 2);
  glayshape->addWidget(m_shapeParamsle[4], 2, 2, 1, 1);
  glayshape->addWidget(m_framelb[2], 2, 3, 1, 1);
  glayshape->addWidget(m_shapeParamsle[5], 2, 4, 1, 1);

  glayshape->addWidget(m_bEnshapelb, 3, 0, 1, 2);
  glayshape->addWidget(m_bEnshapesw, 3, 2, 1, 2);

  //! 形选标签
  QGridLayout *glaylab = new QGridLayout(m_shplabEnGb);
  glaylab->setContentsMargins(0, 50, 0, 0);

  for (int i = 0; i < MAX_LABELS; i++) {
    glaylab->addWidget(m_shplabchk[i], i / 5, i % 5, 1, 1);
  }

  QHBoxLayout *hlaytop = new QHBoxLayout();
  hlaytop->addWidget(m_staticParamsetGb);
  hlaytop->addWidget(m_shplabEnGb);
  hlaytop->addStretch();

  //! 分类
  QGridLayout *gclassifyLay = new QGridLayout(m_classficationGb);
  for (int i = 0; i < MAX_CLASSIFY_NUM - 1; i++) {
    gclassifyLay->addWidget(m_classifylb[i], i / 5, 2 * i, Qt::AlignRight);
    gclassifyLay->addWidget(m_classifyle[i], i / 5, 2 * i + 1, Qt::AlignLeft);
  }

  //! 总布局
  QVBoxLayout *vlayshape = new QVBoxLayout(this);
  vlayshape->addLayout(hlaytop);
  vlayshape->addSpacing(20);
  vlayshape->addWidget(m_classficationGb);
  vlayshape->addSpacing(15);
}

void shapenesspage::connectAndSlt() {
  //! 形选
  connect(m_bEnshapesw, SIGNAL(checkedChanged(bool)), this,
          SLOT(onshapeEnStaChgSlt(bool)));

  QSignalMapper *sigchkmap = new QSignalMapper(); //! 统计形选参数设置
  for (int i = 0; i < 3; i++) {
    sigchkmap->setMapping(m_shapeParamschk[i], i);
    connect(m_shapeParamschk[i], SIGNAL(pressed()), sigchkmap, SLOT(map()));
  }
  connect(sigchkmap, SIGNAL(mapped(int)), this, SLOT(onshapeEnablechgSlt(int)));

  QSignalMapper *sigshapelemap = new QSignalMapper();
  for (int i = 0; i < 6; i++) {
    sigshapelemap->setMapping(m_shapeParamsle[i], i);
    connect(m_shapeParamsle[i], SIGNAL(pressed()), sigshapelemap, SLOT(map()));
  }
  connect(sigshapelemap, SIGNAL(mapped(int)), this,
          SLOT(onShapeParamsChgSlt(int)));

  QSignalMapper *sigshplabEnmap = new QSignalMapper();
  for (int i = 0; i < MAX_LABELS; i++) {
    sigshplabEnmap->setMapping(m_shplabchk[i], i);
    connect(m_shplabchk[i], SIGNAL(pressed()), sigshplabEnmap, SLOT(map()));
  }
  connect(sigshplabEnmap, SIGNAL(mapped(int)), this,
          SLOT(onShapeLabelEnChgSlt(int)));

  //! 标签分类阈值
  QSignalMapper *sigclassfitylemap = new QSignalMapper();
  for (int i = 0; i < MAX_CLASSIFY_NUM - 1; i++) {
    sigclassfitylemap->setMapping(m_classifyle[i], i);
    connect(m_classifyle[i], SIGNAL(pressed()), sigclassfitylemap, SLOT(map()));
  }
  connect(sigclassfitylemap, SIGNAL(mapped(int)), this,
          SLOT(onClassfitythdChgSlt(int)));
}

void shapenesspage::refreshPage() {
  auto *pProfileAI =
      &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;

  //! 刷新标签
  if (!struGsh.vlabNamemap.isEmpty()) {
    QList<QString> namelist = struGsh.vlabNamemap[0].values();
    int ntotal = namelist.size();
    for (int i = 0; i < MAX_LABELS; i++) {
      if (i < ntotal) {
        m_shplabchk[i]->setText(namelist.at(i));
        m_shplabchk[i]->show();
      } else {
        m_shplabchk[i]->hide();
      }
      m_shplabchk[i]->setChecked(pProfileAI->bShapelabEn[0][i]);
    }
  }

  //! 刷新参数
  m_bEnshapesw->setChecked(
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape);

  m_shapeParamschk[0]->setChecked(pProfileAI->bshapeEn[0]);
  m_shapeParamsle[0]->setText(QString::number(pProfileAI->nshapelegthlow[0]));
  m_shapeParamsle[1]->setText(QString::number(pProfileAI->nshapelegthhig[0]));

  m_shapeParamschk[1]->setChecked(pProfileAI->bshapewidEn[0]);
  m_shapeParamsle[2]->setText(QString::number(pProfileAI->nshapewidthlow[0]));
  m_shapeParamsle[3]->setText(QString::number(pProfileAI->nshapewidthhig[0]));

  m_shapeParamschk[2]->setChecked(pProfileAI->bshapeAreaEn[0]);
  m_shapeParamsle[4]->setText(QString::number(pProfileAI->nshapearealow[0]));
  m_shapeParamsle[5]->setText(QString::number(pProfileAI->nshapeareahig[0]));
  if (m_shapeParamschk[0]->isChecked()) {
    m_shapeParamsle[0]->setDisabled(false);
    m_shapeParamsle[1]->setDisabled(false);
  } else {
    m_shapeParamsle[0]->setDisabled(true);
    m_shapeParamsle[1]->setDisabled(true);
  }

  if (m_shapeParamschk[1]->isChecked()) {
    m_shapeParamsle[2]->setDisabled(false);
    m_shapeParamsle[3]->setDisabled(false);
  } else {
    m_shapeParamsle[2]->setDisabled(true);
    m_shapeParamsle[3]->setDisabled(true);
  }
  if (m_shapeParamschk[2]->isChecked()) {
    m_shapeParamsle[4]->setDisabled(false);
    m_shapeParamsle[5]->setDisabled(false);
  } else {
    m_shapeParamsle[4]->setDisabled(true);
    m_shapeParamsle[5]->setDisabled(true);
  }

  //! 刷新分类阈值
  for (int i = 0; i < MAX_CLASSIFY_NUM - 1; i++) {
    m_classifyle[i]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->analyzeProfile.nMatclassifythd[i]));
  }

  if (struCnfg.nMachineType == nMachine_Bottle) {
    m_shplabEnGb->setHidden(true);
    m_classficationGb->setHidden(true);

    //        for(int i=0; i<3; i++){
    //            m_shapeParamschk[i]->setText(m_shapelst.at(3+i));
    //        }
  } else {
    m_shplabEnGb->setHidden(false);
    m_classficationGb->setHidden(false);
    //        for(int i=0; i<3; i++){
    //            m_shapeParamschk[i]->setText(m_shapelst.at(i));
    //        }
  }
}

/*!
 * \brief shapenesspage::onshapeEnStaChgSlt
 * \param bsta
 */
void shapenesspage::onshapeEnStaChgSlt(bool bsta) {
  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape = bsta;
}

/*!
 * \brief 槽：形选参数使能
 * \param nIdx: 0:长短  1：宽窄  2：面积
 */
void shapenesspage::onshapeEnablechgSlt(int nIdx) {
  bool bchk = m_shapeParamschk[nIdx]->isChecked();
  if (bchk) {
    if (nIdx == 0) {
      m_shapeParamsle[0]->setDisabled(false);
      m_shapeParamsle[1]->setDisabled(false);
    } else if (nIdx == 1) {
      m_shapeParamsle[2]->setDisabled(false);
      m_shapeParamsle[3]->setDisabled(false);
    } else {
      m_shapeParamsle[4]->setDisabled(false);
      m_shapeParamsle[5]->setDisabled(false);
    }
  } else {
    if (nIdx == 0) {
      m_shapeParamsle[0]->setDisabled(true);
      m_shapeParamsle[1]->setDisabled(true);
    } else if (nIdx == 1) {
      m_shapeParamsle[2]->setDisabled(true);
      m_shapeParamsle[3]->setDisabled(true);
    } else {
      m_shapeParamsle[4]->setDisabled(true);
      m_shapeParamsle[5]->setDisabled(true);
    }
  }

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    if (nIdx == 0) {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->analyzeProfile.strctAI.bshapeEn[i] = bchk;
    } else if (nIdx == 1) {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->analyzeProfile.strctAI.bshapewidEn[i] = bchk;

    } else {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->analyzeProfile.strctAI.bshapeAreaEn[i] = bchk;
    }
  }
}

/*!
 * \brief shapenesspage::onShapeParamsChgSlt
 * \param nIdx
 */
void shapenesspage::onShapeParamsChgSlt(int nIdx) {
  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, 100000,
                       m_shapeParamsle[nIdx]->text().toInt(), "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int n_sample = inputPanel->getValue();
    m_shapeParamsle[nIdx]->setText(QString("%1").arg(n_sample));
    switch (nIdx) {
    case 0:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapelegthlow[i] = n_sample;
      }
      break;
    case 1:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapelegthhig[i] = n_sample;
      }
      break;
    case 2:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapewidthlow[i] = n_sample;
      }
      break;
    case 3:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapewidthhig[i] = n_sample;
      }
      break;
    case 4:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapearealow[i] = n_sample;
      }
      break;
    case 5:
      for (int i = 0; i < MAX_CAM_NUM; i++) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.strctAI.nshapeareahig[i] = n_sample;
      }
      break;
    default:
      break;
    }
  }
}

/*!
 * \brief shapenesspage::onShapeLabelEnChgSlt
 * \param nIdx
 */
void shapenesspage::onShapeLabelEnChgSlt(int nIdx) {
  bool bchk = m_shplabchk[nIdx]->isChecked();

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->analyzeProfile.strctAI.bShapelabEn[i][nIdx] = bchk;
  }
}

/*!
 * \brief shapenesspage::onClassfitythdChgSlt
 * \param nIdx
 */
void shapenesspage::onClassfitythdChgSlt(int nIdx) {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 100000, m_classifyle[nIdx]->text().toInt(), "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int n_sample = inputPanel->getValue();
    m_classifyle[nIdx]->setText(QString("%1").arg(n_sample));
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->analyzeProfile.nMatclassifythd[nIdx] = n_sample;
  }
}
