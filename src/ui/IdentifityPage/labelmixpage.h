#ifndef LABELMIXPAGE_H
#define LABELMIXPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include "core/globalflow/mylanguage.h"
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"

class labelmixpage : public QWidget
{
    Q_OBJECT
public:
    explicit labelmixpage(QWidget *parent = nullptr);
    void createPage();
    void layOutPage();
    void connectAndSlt();
    void refreshPage(int nCamId);

private slots:
    void onMapComboChanged(int index);

private:
    QGroupBox    *m_mixGroup;
    QScrollArea  *m_scrollArea;
    QTableWidget *m_tableWidget;
    int           m_nCamId;
    bool          m_bUpdating;
};

#endif // LABELMIXPAGE_H
