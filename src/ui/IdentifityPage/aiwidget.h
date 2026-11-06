#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QSignalMapper>
#include <QGridLayout>
#include <QTableWidget>
#include "common/mypushbutton.h"
#include "common/mycheckbox.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "params/define.h"
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"
#include "core/globalflow/globalflow.h"
#include "common/selectfiledialog.h"
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QButtonGroup>
#include <QCheckBox>
#include <QRadioButton>
#include <QStackedLayout>
#include <QFormLayout>
#include <QProcess>
#include <QHash>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QDirIterator>
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "core/globalflow/mylanguage.h"
#include <QIcon>
#include <QSize>
#include <QPainter>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "aiimagesave.h"

// 添加新的头文件引用
#include <QRunnable>
#include <QThreadPool>
#include <QAtomicInt>
#include <QPainter>

// 定义图片处理任务类
class ImageProcessTask : public QRunnable 
{
public:
    ImageProcessTask(const QString& imgPath, 
                    const BBox& box,
                    const QString& labelFolder,
                    const QString& labelName,
                    QAtomicInt& totalSize,
                    const qint64 maxSize) 
        : m_imgPath(imgPath)
        , m_box(box)
        , m_labelFolder(labelFolder)
        , m_labelName(labelName)
        , m_totalSize(totalSize)
        , m_maxSize(maxSize)
    {
        setAutoDelete(true);
    }

    void run() override 
    {
        QImage srcImg(m_imgPath);
        if(srcImg.isNull()) return;

        QFileInfo fi(m_imgPath);
        QString fileName = fi.fileName();

        //! 裁剪图片
        QRect rect(m_box.cx - m_box.bw/2,
                  m_box.cy - m_box.bh/2, 
                  m_box.bw,
                  m_box.bh);

        QImage cropImg = srcImg.copy(rect.normalized()
                                   .intersected(srcImg.rect()));

//        //! 添加标注信息
//        QPainter painter(&cropImg);
//        painter.setRenderHint(QPainter::Antialiasing);
//        QFont font = painter.font();
//        font.setPointSize(16);
//        painter.setFont(font);
//        painter.setPen(Qt::red);
//        painter.drawText(8, 28,
//            QString("prob:%1 label:%2")
//            .arg(m_box.prob, 0, 'f', 2)
//            .arg(m_labelName));

        //! 检查大小限制
        qint64 estimatedSize = cropImg.byteCount();
        if(m_totalSize.fetchAndAddRelaxed(estimatedSize) >= m_maxSize) {
            return;
        }

        // 保存图片
        QString savePath = QString("%1/%2_%3.png")
                          .arg(m_labelFolder)
                          .arg(fileName.left(fileName.lastIndexOf('.')))
                          .arg(QDateTime::currentMSecsSinceEpoch());

        cropImg.save(savePath, "PNG", 90);
    }

private:
    QString m_imgPath;
    BBox m_box;
    QString m_labelFolder;
    QString m_labelName; 
    QAtomicInt& m_totalSize;
    qint64 m_maxSize;
};

class AIWidget:public QWidget
{
    Q_OBJECT
public:
    AIWidget(QWidget* parent = 0);
    ~AIWidget(); // 添加析构函数声明

    void createwid(void);
    void pagelayout(void);
    void pagesigandslot(void);
    void refreshPage(void);
    void refreshIMGPreviewpage(QStringList imglist);
    QHash<int, QStringList> getLineGroups() const { return m_lineGroups; }

    void refreshControlSta();

public slots:
    void onTabIndexChg(int nIdx);

    void onModelIdxChanged(int nIdx);

    void onimgsetChanged(int nIdx);
    void oncapBtnsetChanged(int nIdx);
    void oncapChktypeChanged(int nIdx);

    void onrequireModelinfo();
    void onlabelEnChanged(int nIdx);
    void oncapThdChanged();  //! 采样阈值
    void onCoCapcamidxChanged(int nIdx);
    void onModeldatalineChanged(int nIdx);
    void onModelResChanged(int nIdx);
    void onCoffChanged();
    void onoffsetChanged();
    void onBackbtnSlt();
    void onRefreshGraphicview(QString imgpath);
    void onCamViewBtnPressSlt(void);
    void oncamworkModelchg(int nIdx);
    void onSyncBtnPressedSlt();
    void onAreathdChgSlt();
    void onLabelListWidgetItemClicked(QListWidgetItem *item);
    void onImgListWidgetItemClicked(QListWidgetItem *item);
    void onShowEnlargedImage(QPixmap pixmap); //! 显示放大图片
    void onCloseEnlargedImage(); //! 关闭放大图片
    void onUploadFinished(QString str);
    void onUpdataUploadTips(QString str, int type);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void createAIparamsPage();
    void createAIphotoPage();
    void createPhotoPreview();
    void layAIparamsPage();
    void layAIphotoPage();
    void layPhotoPreview();

    void onHandleImgExport(void);
    void onHandleLogExport(void);
    void handleIMGpreview(void);
    void refreshModelComb(int nIdx);
    void refreshLabelEnable(int nCamId);
    void refreshCamWorkmodeComb();
    void refreshCapcaminfo(void);
    bool getFilesandUnzip(void);
    bool findImgs(void);
    bool cropImg(void);
    void onHandleImg(void);
    qint64 calculateDirectorySize(const QString& dirPath);
    void onPreVireCropImg(void);
    void Refreshimglstwid(void);
    bool unzipFileAsync(const QString &zipFile, const QString &unzipPath);

private:

    QWidget    *m_wid;
    QTabBar    *m_tabBar;
    QStackedLayout *m_layGb;
    QPushButton *m_viewSwBtn;
    QPushButton *m_syncSetBtn;
    QStackedLayout *m_laywidstck;
    bool        m_bSync = false;
    int         m_nCamId =0;

    //! AI参数设置
    QGroupBox  *m_aiLabelgb;    //! 标签使能
    MyCheckBox *m_modellbchk[MAX_LABELS];

    QWidget     *m_aiParamssetWid;
    QGroupBox   *m_aiSetsetgb; //! 参数设置
    QLabel      *m_modelswlb;
    QComboBox   *m_modelcomb;
    QLabel      *m_caplb[2];
    MyLineEdit  *m_caple[2];
    QLabel      *m_dataLinelb;
    QComboBox   *m_dataLinecomb;
    QLabel      *m_modelReslb;
    QComboBox   *m_modelRescomb;
    QLabel      *m_cofflb;
    MyLineEdit  *m_coffle;
    QLabel      *m_offsetlb;
    MyLineEdit  *m_offsetle;
    QLabel      *m_areathdlb;
    MyLineEdit  *m_areathdle;

    QGroupBox   *m_aiCapgb; //! 拍照设置
    QLabel      *m_captdlb;
    MyLineEdit  *m_captdle;

    QLabel      *m_captypelb;
    QRadioButton*m_capTypebtn[3];
    QButtonGroup*m_capTypegp;
    bool        m_bcaplabEn;  //! 拍标记图标志位  （标记图的使能和标签使能分开）

    QLabel      *m_camselectlb;
    QRadioButton*m_capCamidxbtn[MAX_CAM_NUM];
    QButtonGroup*m_capcambtngp;

    QLabel       *m_preViewtypelb;
    QRadioButton *m_preViewtypebtn[2];
    QButtonGroup *m_preViewtypegp;

    QPushButton  *m_capbtn[4];

    QGroupBox   *m_camworksetGb; //! SOC工作模式
    QLabel      *m_camIdlb[MAX_CAM_NUM];
    QComboBox   *m_camworkmodecomb[MAX_CAM_NUM];
    QListWidget *m_camworklstwid[MAX_CAM_NUM];

    //! 图片预览页面
    QWidget       *m_imgwid;  //! 预览原始标记图
    QGroupBox     *m_imgpreGb;
    QGraphicsView *m_imgview;
    QGraphicsScene *m_imgscene;
    QGraphicsPixmapItem *m_pixitem;
    QPushButton     *m_bckBtn;
    QListWidget     *m_imglistwid;

    QGroupBox *m_preVireCropImgGb = nullptr; //! 浏览裁剪图片相关成员变量
    QListWidget *m_labelListWidget = nullptr;
    QListWidget *m_imgListWidget = nullptr;
    QVector<QString> m_currentImgFiles;

    QHash<int, QStringList> m_lineGroups; //! key:行号  val：相机集合
    QStringList m_fileList;
    QStackedLayout *m_staclay = nullptr;
    
    bool m_bImageEnlarged = false; //! 图片放大状态标志
    QWidget *m_enlargedImgWidget = nullptr; //! 放大图片显示窗口
    QGraphicsView *m_enlargedImgView = nullptr; //! 放大图片视图
    QGraphicsScene *m_enlargedImgScene = nullptr; //! 放大图片场景
    QGraphicsPixmapItem *m_enlargedPixItem = nullptr; //! 放大图片项
    
    QFutureWatcher<bool> *m_unzipWatcher = nullptr; //! 异步解压监视器

    //! 图片导出
    AiImageSaveTask     *m_uploadTask;

};

#endif // AIWIDGET_H
