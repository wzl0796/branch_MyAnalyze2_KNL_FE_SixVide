#ifndef ENGINEERSETPAGE_H
#define ENGINEERSETPAGE_H

#include <QWidget>
#include "params/define.h"
#include <QLayout>
#include "QPushButton"
#include "QLabel"
#include "QGroupBox"
#include "QSignalMapper"
#include "QStackedWidget"
#include "core/globalflow/mylanguage.h"
#include "common/delaycode.h"
#include "common/mycheckbox.h"
#include "common/mylineedit.h"

#define MAX_STATISTIC_CLASS 6

class selectLangWidget;

class engineerSetPage : public QWidget
{
    Q_OBJECT
public:
    explicit engineerSetPage(QWidget *parent = 0);
    void refreshDisplay();

signals:
    void updateLangList();

public slots:
    void onCodeBtnClicked();

    void onEncryptBtnClicked();         //! 加解密设置

    void onDeleteEncryptFilesBtnClicked();  //! 删除加密文件

    void onSetTimezoneBtnClicked();         //! 修改系统时区

    void onCalibrateSysTimeBtnClicked();    //! 校正系统时间

    void onFutureFinished();

    void onLangSetSlt();         //! 语言设置

    void onUpdateLangListSlot();

    void onEditfactorynamebtnSlt();

    void onEditnameleSlt();

    void onEditDeviceNameSlt();

    void onStaclasseditSlt();

    void onStaclasschkChgSlt(int nIdx);

    void onBackbtnPressedSlt();

protected:
    void showEvent(QShowEvent *ev);
    void hideEvent(QHideEvent *ev);
    void timerEvent(QTimerEvent *ev);

private:
    void initParams(void);
    void createPage(void);
    void connectSigAndSlot(void);
    void createQRcodeWidget();
    void createSelectPage();
    QString getMd5Password(QString);         //! 权限卡密码
    void refreshStaclasswid();
    bool execNtp(void);                     //! 执行NTP

private:
    enum {
      HOME_WIDGET,
      SELECT_WIDGET,
      LANGUAGE_WIDGET,
      FACTORYEDIT_WIDGET,
      STATISTICCLASS_WIDGET,
      SYSTEM_TIME_WIDGET,
      WIDGET_NUM
    };

    /*!
 * \enum 枚举时间类型
 */
    enum {
        TIME_YEAR = 0,
        TIME_MONTH,
        TIME_DAY,
        TIME_HOUR,
        TIME_MINUTE,
        TIME_SECOND,
        TIME_TYPE,
        DELIMITER_NUM = 4      //! 分隔符数量
    };

    //! 二维码扫描界面
    QLabel      *m_QRCodeLbl, *noticeLbl, *m_codeLbl;
    QPushButton *m_codeBtn;
    QWidget     *m_QRcodeWidget;

    QPushButton *m_EncryptBtn, *m_setLangBtn,
                *m_editFactoryNameBtn,
                *m_staClassBtn,
                *m_deleteEncryptFilesBtn,  //! 删除加密文件按钮
                *m_setTimezoneBtn;         //! 修改时区按钮

    QWidget     *m_selectWidget;          //! 语言设置
    selectLangWidget *m_langSetWidget;

    QWidget *m_factoryNmaeWid; //! 厂家名称
    QLabel  *m_factoryNamelb;
    MyLineEdit *m_facNamele;
    QLabel  *m_deviceNamelb;
    MyLineEdit *m_deviceNamele;
    QPushButton *m_back2Btn;


    QWidget    *m_staClassWid; //! 统计类别
    QGroupBox  *m_staclassgb;
    MyCheckBox *m_staClassEnchk[MAX_STATISTIC_CLASS];
    QPushButton *m_backBtn;


    QWidget    *m_timeSetWid;      //! 时间设置
    QPushButton *m_timezoneBtn;
    QPushButton *m_calSystimeBtn;
    QFrame     *m_frame;
    MyLineEdit *m_Timele[TIME_TYPE];
    QLabel     *m_splitlb[DELIMITER_NUM];
    QPushButton *m_back3Btn;

    QStackedWidget *m_stackedWidget;       //! 界面堆栈

    QString m_codeStr;

    QString lan_warn;
    QString lan_passwd;
    QString lan_pwderr;
    QString lan_deviceCodeErr;

    QString lang_ena;
    QString encryption;

    QFutureWatcher<bool> m_watcher;         //! 监视器
    int m_nTimerId = -1;

};

#endif // ENGINEERSETPAGE_H
