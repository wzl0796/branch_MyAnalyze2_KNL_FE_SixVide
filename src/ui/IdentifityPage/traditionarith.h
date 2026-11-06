#ifndef TRADITIONARITH_H
#define TRADITIONARITH_H

#include <QWidget>
#include <functional>
#include <QTabBar>
#include <QStackedWidget>

#include "params/define.h"

class QComboBox;
class QButtonGroup;
class QGroupBox;
class QLabel;
class MyCheckBox;
class MyLineEdit;
class QPushButton;
class QScrollArea;

class traditionArith : public QWidget
{
    Q_OBJECT
    enum {
        PAGE_LAB_SET = 0,
        PAGE_PARAMS_SET,
        PAGE_ALL
    };
public:
    explicit traditionArith(QWidget *parent = nullptr);

    void cratePage();
    void layoutPage();
    void connectAndSlt();
    void refreshPage();

public slots:

private:
    void refreshTradition();
    void refreshLabelset();

private:
    QTabBar        *m_tabBar;
    QStackedWidget *m_stackwid;


    //! 标签设置
    QWidget     *m_labsetPage;
    QGroupBox   *m_labsetBox;
    QPushButton *m_labelCameraButton;
    QScrollArea *m_labelScrollArea;
    QLabel *m_labelName[MAX_LABELS];
    MyCheckBox *m_labelTraditionEnable[MAX_LABELS];
    QComboBox *m_labelTraditionAlgorithm[MAX_LABELS];
    int m_labelIds[MAX_LABELS];
    int m_labelCameraIndex;


    //! 传统参数设置
    QWidget     *m_traditionParamsPage;

    void refreshSelectedAlgorithm();
    void refreshRetainSettings();
    void editValue(MyLineEdit *edit, int minimum, int maximum,
                   const QString &title, const std::function<void(int)> &save);
    void editValue(MyLineEdit *edit, double minimum, double maximum,
                   const QString &title, const std::function<void(double)> &save,
                   int decimalPlaces = -1);
    void editValueImpl(MyLineEdit *edit, double minimum, double maximum, int decimalPlaces,
                       const QString &title, const std::function<void(double)> &save);
    void saveSensitivity(double value);

    static const int ALGORITHM_COUNT = TRADITION_ARITH_COUNT;
    int m_cameraIndex;
    int m_selectedAlgorithm;

    MyCheckBox *m_algorithmEnable[ALGORITHM_COUNT];
    QPushButton *m_algorithmSelect[ALGORITHM_COUNT];

    QLabel *m_nameLabel;
    QLabel *m_nameTitleLabel;
    QLabel *m_sensitivityTitleLabel;
    QLabel *m_modeTitleLabel;
    QLabel *m_rowCountTitleLabel;
    QLabel *m_badPointTitleLabel;
    QLabel *m_passTitleLabel;
    QLabel *m_lowerLimitTitleLabel;
    MyLineEdit *m_sensitivityEdit;
    QComboBox *m_modeCombo;
    QPushButton *m_brightButton;
    QPushButton *m_darkButton;
    MyLineEdit *m_rowCountEdit;
    MyLineEdit *m_badPointEdit;
    QPushButton *m_passOneButton;
    QPushButton *m_passTwoButton;
    MyCheckBox *m_cutEdgeCheck;
    MyCheckBox *m_independentDelayCheck;
    MyLineEdit *m_lowerLimitEdit;
    
    QGroupBox *m_parameterBox;
    QGroupBox *m_retainBox;
    QComboBox *m_retainModeCombo[2];
    QComboBox *m_retainLightCombo[2];
    MyLineEdit *m_retainSensitivityEdit[2];
    QButtonGroup *m_algorithmGroup;
    QButtonGroup *m_lightGroup;
    QButtonGroup *m_passGroup;
};

#endif // TRADITIONARITH_H
