#ifndef GETIMAGEPAGE_H
#define GETIMAGEPAGE_H

#include <QWidget>
#include <QtCore>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QToolButton>
#include <QListWidget>
#include <QComboBox>
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"
#include "common/mycheckbox.h"
#include "common/myimageshowpage.h"
#include "core/globalflow/globalflow.h"
#include "core/io/myserial/myserialcom.h"
#include "time.h"
#include "core/globalflow/mylanguage.h"


#define MAX_IMG_NUM  48

typedef struct {
    int nUnitAddr;         /*! 拍照的相机地址*/
    int dateLen;           /*! 拍照总数据长度*/
    int dataDepth;         /*! 拍照行数*/
    int nProtoType;        /*! 执行本次抓拍的相机与上位机数据传输协议类型 */
    int result;            /*! 拍照结果*/
}CaptureConfig;

/*! \enum 采集图像功能返回的错误信息 */
enum {
    ERR_SUCCESS,        //成功
    ERR_TIMEOUT,        //超时
    ERR_DATA,           //像素数据错误
    ERR_CONFIG,         //拍照相机设置参数设置错误
    ERR_MALLOC_FAILED,  //内存分配失败
    ERR_INPUT_ARG,      //函数入参错误
    ERR_AUTO_BACKGROUND_CHECKERROR,   //自动背景校验错误
    ERR_UNKOWN          //未知错误
};

enum {
    TYPE_RGB,
    TYPE_NGB,
    TYPE_NNN,
    TYPE_TOTAL
};

/*! \enum protocal No.*/
enum{
    PROTOCAL_1   =  1,  //单帧中1024(R+G+B),512帧
    PROTOCAL_2   =  2   //单帧中640{(R+G+B+M),(R+G+B+M+N+P)},512帧
};

class getImagePage : public QWidget
{
    Q_OBJECT
public:
    explicit getImagePage(QWidget *parent = 0);

    ~getImagePage();

    void refreshPageDisplay(void);

signals:
    void sigbackBtnpressed();

public slots:
    void onComBoxChangedSlt(int index);

    void onViewBoxPressedSlt(int index);

    void onCaptureBtnPressedSlt(void);

    void onImgDisPlayBtnPressedSlt(int index);

    void onImgDeleteBtnPressedSlt(void);

    void onImgDeleteBtnReleasedSlt();

    void onDeleteTimerOutSlt();

    void onImgImportBtnPressedSlt();        //导入图片

    void onImgExportBtnPressedSlt(void);    //导出图片

    void onThreadValueBtnPressedSlt(void);

    void onSampleBtnPressedSlt(void);

    void onFeederBtnPressedSlt(void);

    void onGetBakBtnPressedSlt(void);

    void onCoCapChkPressedSlt(void);

    void onImgTypeBtnPressedSlt(int index);

    void onNNNBoxPressedSlt();

    void onImageDataReceived(QByteArray ba, int dataLength,int nUnit);

private:
    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshAnalyzeComBox(void);

    void refreshViewBoxDis(void);

    void refreshImgDisPlay(void);

    void refreshBtnState(void);

    void getImage(void);

    void setCaptureList(QList <CaptureConfig> &qlCaptureConfig);

    void sendCapturePara(QList <CaptureConfig> &qlCaptureConfig);

    void processImgDataFromCamera(CaptureConfig &captureConfig);

    int  stripFrameData(char *data, int nUnitAddr);

    void displayImgInfo(int nUnitAddr, QString fileName, int nImgType);

    bool processCaptureResult(QList<CaptureConfig> &qlCaptureConfig);

    bool saveBMPFile(QString fileName, int width, int height, uchar *rgbData);

    bool saveWideBMPFile(int index, int width, int height, uchar *rgbData);

    void deteleCurImg(int index);

    void refreshFeederBtnState(void);

    void setBigPic(bool bBig);

    int  stripFrameData_2048(char *data, int nUnitAddr);

    void showImg(int imgIndex, bool bShow);

    QPair<QPair<QString,QString>, QString> findImageName();

private:
    QComboBox    *analyzeCombox;
    QGroupBox    *imgDisGBox;
    QPushButton  *imgDisBtn[MAX_IMG_NUM];
    QPushButton  *imgItemBtn[MAX_IMG_NUM];
    QScrollArea  *m_pScroll;

    QGroupBox    *getImgGBox;
    QPushButton  *startGetBtn;
    QPushButton  *deleteBtn;
    MyCheckBox   *viewBox[MAX_CAM_NUM];

    QPushButton  *exportBtn[2];

    QLabel       *threadValueLbl;
    QPushButton  *threadValueBtn;

    QPushButton  *imgTypeBtn[TYPE_TOTAL-1];
    MyCheckBox   *nnnBox;
    QComboBox    *imgTypeComb;

    QToolButton  *feederBtn;
    MyCheckBox   *getBak;        //! 去背景
    MyCheckBox   *coCapchk;         //! 同步拍照
    QLabel       *sampleLineLbl;
    QPushButton  *sampleLineBtn;


    QSignalMapper *viewMap;
    QSignalMapper *imgDistMap;
    QSignalMapper *imgTypeMap;

    QPushButton  *m_backBtn;

private:
    QString  export_img;
    QString  import_img;
    QString  start_cam;
    QString  delete_img;
    QString  delete_all_img;
    QString  thread_value;
    QString  RGB_type;
    QString  NGB_type;
    QString  NNN_type;
    QString  overmaximg;

    QString  delete_warn;
    QString  str_cap;

    bool isCaptureAllView;
    bool isDeleteImg;

    int n_CapIndex;

    int imgIndex;

    int n_ImgType;

    bool m_bNNNEnable;

    int n_analyzeNum;

    int n_sample;

    int n_ImageCount;

    char *gImageData;

    char *gImageData_2048;

    QString m_sNameOfImg[MAX_IMG_NUM];

    int  m_picHeight;

    clock_t m_nPressTimeStart;
    QTimer *delTimer;

    QString m_picName[MAX_IMG_NUM];
};

#endif // GETIMAGEPAGE_H
