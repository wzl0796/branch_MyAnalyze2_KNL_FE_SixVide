#include "labelmergerpage.h"

labelmerger::labelmerger(QWidget *parent) : QWidget(parent) {
  createPage();
  layOutPage();
  connectAndSlt();
}

void labelmerger::createPage() {
  m_MerlabGb = new QGroupBox();

  m_orilablb = new QLabel();
  m_orilablb->setFixedSize(QSize(220, 100));

  QPixmap arrowpixmap(":/res/png/bi_directionarrow.png");
  m_orilablb->setPixmap(arrowpixmap.scaled(
      m_orilablb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

  m_orilablstwid = new QListWidget();
  m_orilablstwid->setSelectionMode(QAbstractItemView::MultiSelection);

  QString tooboxstyle = QString(
      "QTreeWidget {border:1px solid gray; border-radius: 5px;font-size:26px;} "
      "QTreeWidget::item{height:50px;text-align:left;padding-left:4px}"
      "QTreeView::indicator{width:40px;height:50px}"
      "QTreeView::branch{width:40px;height:50px}");
  m_MergrtreWid = new QTreeWidget();
  m_MergrtreWid->setColumnCount(1);
  m_MergrtreWid->setHeaderHidden(true);
  m_MergrtreWid->setStyleSheet(tooboxstyle);

  m_mergerBtn = new QPushButton(myLan.merger);
  m_mergerBtn->setFixedSize(BTN_WID + 50, BTN_HEI);
  m_mergerBtn->setIconSize(QSize(50, 50));
  m_mergerBtn->setIcon(QIcon(":/res/png/merger.png"));

  m_removeBtn = new QPushButton(myLan.leftMove);
  m_removeBtn->setFixedSize(BTN_WID + 50, BTN_HEI);
  m_removeBtn->setIconSize(QSize(50, 50));
  m_removeBtn->setIcon(QIcon(":/res/png/del.png"));
}

void labelmerger::layOutPage() {
  QVBoxLayout *laybtn = new QVBoxLayout();
  laybtn->addWidget(m_mergerBtn);
  laybtn->addWidget(m_removeBtn);

  QHBoxLayout *hlayup = new QHBoxLayout(m_MerlabGb);
  hlayup->addWidget(m_orilablstwid);
  hlayup->addSpacing(60);
  hlayup->addWidget(m_orilablb);
  hlayup->addSpacing(20);
  hlayup->addWidget(m_MergrtreWid);
  hlayup->addSpacing(100);
  hlayup->addLayout(laybtn);

  QHBoxLayout *hlaymain = new QHBoxLayout();
  hlaymain->addWidget(m_MerlabGb);
  setLayout(hlaymain);
}

void labelmerger::connectAndSlt() {
  connect(m_mergerBtn, &QPushButton::pressed, this,
          &labelmerger::onMegerlabBtnPressed);
  connect(m_removeBtn, &QPushButton::pressed, this,
          &labelmerger::onRemovelabBtnPressed);
}

void labelmerger::refreshPage(int nCamId) {
  refreshBtnStaus();

  m_ntype = nCamId;
  refreshlablstwiditem();
  refreMergrlabtreewid();
}

void labelmerger::refreshBtnStaus() {
  m_mergerBtn->setDisabled(struGsh.nFeederState);
  m_removeBtn->setDisabled(struGsh.nFeederState);
}

void labelmerger::onMegerlabBtnPressed() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

  QList<QListWidgetItem *> lstitem = m_orilablstwid->selectedItems();
  if (lstitem.size() > 0) {
    //! 检查选中的合并标签中是否包含次一级标签
    for (QListWidgetItem *item : lstitem) {
      QString labName = item->text().trimmed();
      bool isSecondary = false;

      // 1. 检查当前相机 m_ntype 是否将该标签设为了次一级标签
      if (m_ntype >= 0 && m_ntype < MAX_CAM_NUM &&
          pProfile->slabSecondary[m_ntype].trimmed() == labName) {
        isSecondary = true;
      }

      // 2. 检查当前相机所在统计组内的相机
      if (!isSecondary) {
        for (int i = 0; i < pProfile->nStaticGroup; i++) {
          if (i < pProfile->vUnitaddrlist.size() &&
              pProfile->vUnitaddrlist[i].contains(QString::number(m_ntype))) {
            for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
              int camIdx = strcamid.toInt();
              if (camIdx >= 0 && camIdx < MAX_CAM_NUM &&
                  pProfile->slabSecondary[camIdx].trimmed() == labName) {
                isSecondary = true;
                break;
              }
            }
          }
          if (isSecondary)
            break;
        }
      }

      // 3. 全局统计组检查（若单一统计组机型）
      if (!isSecondary && pProfile->nStaticGroup == 1) {
        for (int camIdx = 0; camIdx < MAX_CAM_NUM; ++camIdx) {
          if (pProfile->slabSecondary[camIdx].trimmed() == labName) {
            isSecondary = true;
            break;
          }
        }
      }

      if (isSecondary) {
        infoWidget->raise();
        infoWidget->setLabelText(
            QString("%1[%2]！").arg(myLan.please_cancle_secondary_lab)
            .arg(labName));
        infoWidget->showSecs();
        return;
      }
    }

    setExportTypeWidget dlg(g_mainWidget);
    QStringList lst;
    lst << myLan.newitem << myLan.historyitem;
    dlg.setcontentText(lst);
    int ret = dlg.exec();
    if (ret == QDialog::Rejected) {
      return;
    } else {
      int nIdx = dlg.getSelIndex();

      if (nIdx == 0) { //! 新建项
        QString str;
        myInputMethod input("", "", g_mainWidget);
        int ret = input.exec();
        if (ret == QDialog::Accepted) {
          str = input.getText().trimmed();
          if (str.compare("") == 0) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.input_forbid_empyty);
            infoWidget->showSecs();
            return;
          }

          //! 检查名称重复(不区分大小写)
          for (const QListWidgetItem *item : lstitem) {
            QString itemText = item->text();
            if (str.compare(itemText, Qt::CaseInsensitive) == 0) {
              infoWidget->raise();
              infoWidget->setLabelText("名称已存在!");
              infoWidget->showSecs();
              return;
            }
          }

          //! 新建
          QTreeWidgetItem *topitem = new QTreeWidgetItem();
          topitem->setText(0, str);
          m_MergrtreWid->addTopLevelItem(topitem);
          for (QListWidgetItem *item : lstitem) {
            QString labname = item->text();
            QTreeWidgetItem *childitem = new QTreeWidgetItem(topitem);
            childitem->setText(0, labname);
            if (pProfile->nStaticGroup == 1) {
              for (int nCam = 0; nCam < gFlow.getMachineCamNum() &&
                                 nCam < pProfile->vMegerlabmap.size();
                   nCam++) {
                pProfile->vMegerlabmap[nCam].insert(str, labname);
              }
            } else {
              pProfile->vMegerlabmap[m_ntype].insert(str, labname);
            }
          }
        } else {
          return;
        }
      } else { //! 历史项
        QStringList titlelst;
        for (int i = 0; i < m_MergrtreWid->topLevelItemCount(); i++) {
          titlelst << m_MergrtreWid->topLevelItem(i)->text(0);
        }
        if (titlelst.size() > 0) {
          SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
          dialog->setDir("", titlelst, false);
          if (dialog->exec() == QDialog::Accepted) {
            QString name = dialog->getFileName();
            int nItemidx = titlelst.indexOf(name);

            for (QListWidgetItem *item : lstitem) {
              QString labname = item->text();
              QTreeWidgetItem *childitem =
                  new QTreeWidgetItem(m_MergrtreWid->topLevelItem(nItemidx));
              childitem->setText(0, labname);
              if (pProfile->nStaticGroup == 1) {
                for (int nCam = 0; nCam < gFlow.getMachineCamNum() &&
                                   nCam < pProfile->vMegerlabmap.size();
                     nCam++) {
                  pProfile->vMegerlabmap[nCam].insert(name, labname);
                }
              } else {
                pProfile->vMegerlabmap[m_ntype].insert(name, labname);
              }
            }
          }
          delete dialog;
        }
      }

      refreshlablstwiditem();
      gFlow.ResetMegreLabLst(m_ntype);
      gFlow.ResetLabIdx(m_ntype);
    }
  }
}

void labelmerger::onRemovelabBtnPressed() {
  if (m_MergrtreWid->currentItem() == nullptr) {
    return;
  } else {
    auto *pProfile =
        &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QTreeWidgetItem *item = m_MergrtreWid->currentItem();
    QString name = item->text(0);
    QString topitemname;
    int nChildCount = item->childCount();
    bool isParent = (item->parent() == nullptr);

    if (isParent) { //! 移除父标签（顶层项）
      for (int i = 0; i < nChildCount; ++i) {
        delete item->takeChild(i);
        --i;
        nChildCount = item->childCount();
      }

      if (pProfile->vMegerlabmap[m_ntype].contains(name)) {
        if (pProfile->nStaticGroup == 1) {
          for (int nCam = 0; nCam < gFlow.getMachineCamNum() &&
                             nCam < pProfile->vMegerlabmap.size();
               nCam++) {
            pProfile->vMegerlabmap[nCam].remove(name);
          }
        } else {
          pProfile->vMegerlabmap[m_ntype].remove(name);
        }
      }

      //! 如果移除的是父标签且父标签是次一级标签，则在全局参数中把该次一级标签取消掉
      // 1. 检查并取消当前相机及所在统计组相机的次一级标签
      for (int i = 0; i < pProfile->nStaticGroup; i++) {
        if (i < pProfile->vUnitaddrlist.size() &&
            pProfile->vUnitaddrlist[i].contains(QString::number(m_ntype))) {
          for (const QString &strcamid : pProfile->vUnitaddrlist[i]) {
            int camIdx = strcamid.toInt();
            if (camIdx >= 0 && camIdx < MAX_CAM_NUM &&
                pProfile->slabSecondary[camIdx].trimmed() == name) {
              pProfile->slabSecondary[camIdx] = QString();
            }
          }
        }
      }

      // 2. 检查并取消当前相机 m_ntype 的次一级标签
      if (m_ntype >= 0 && m_ntype < MAX_CAM_NUM &&
          pProfile->slabSecondary[m_ntype].trimmed() == name) {
        pProfile->slabSecondary[m_ntype] = QString();
      }

      // 3. 若为单一统计组机型，清理所有相机的该次一级标签
      if (pProfile->nStaticGroup == 1) {
        for (int camIdx = 0; camIdx < MAX_CAM_NUM; ++camIdx) {
          if (pProfile->slabSecondary[camIdx].trimmed() == name) {
            pProfile->slabSecondary[camIdx] = QString();
          }
        }
      }
    } else {
      topitemname = item->parent()->text(0);
      if (pProfile->vMegerlabmap[m_ntype].contains(topitemname)) {
        if (pProfile->nStaticGroup == 1) {
          for (int nCam = 0; nCam < gFlow.getMachineCamNum() &&
                             nCam < pProfile->vMegerlabmap.size();
               nCam++) {
            pProfile->vMegerlabmap[nCam].remove(topitemname, name);
          }
        } else {
          pProfile->vMegerlabmap[m_ntype].remove(topitemname, name);
        }
      }

      if (pProfile->strctAI.vlabthdmap.keys().contains(topitemname)) {
        pProfile->strctAI.vlabthdmap.remove(topitemname);
      }
    }
    delete item;
    refreshlablstwiditem();
    gFlow.ResetMegreLabLst(m_ntype);
    gFlow.ResetLabIdx(m_ntype);
  }
}

void labelmerger::refreshlablstwiditem() {
  m_orilablstwid->clear();

  if (m_ntype < struGsh.vlabNamemap.size()) {
    auto *pProfile =
        &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QList<QString> keylablst = pProfile->vMegerlabmap[m_ntype].keys();
    foreach (QString value, struGsh.vlabNamemap[m_ntype]) {
      bool bshow = true;
      for (QString var : keylablst) {
        QList<QString> vallablst = pProfile->vMegerlabmap[m_ntype].values(var);
        if (vallablst.contains(value)) {
          bshow = false;
        }
      }

      if (bshow) {
        m_orilablstwid->addItem(value);
      }
    }
  }
}

void labelmerger::refreMergrlabtreewid() {
  m_MergrtreWid->clear();

  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
  for (QString key : pProfile->vMegerlabmap[m_ntype].uniqueKeys()) {
    QTreeWidgetItem *topitem = new QTreeWidgetItem();
    topitem->setText(0, key);
    for (QString value : pProfile->vMegerlabmap[m_ntype].values(key)) {
      QTreeWidgetItem *childitem = new QTreeWidgetItem(topitem);
      childitem->setText(0, value);
    }
    m_MergrtreWid->addTopLevelItem(topitem);
  }
}
