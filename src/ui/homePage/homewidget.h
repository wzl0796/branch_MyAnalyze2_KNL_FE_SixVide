#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <algorithm>
#include <QTimer>
#include <QLayout>
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
#include "ui/homePage/analyzetestdlg.h"

#define  MAX_EJECTOR_PACKET_ROW 1024
#define  MAX_CACHE_PACKET_ROW  1024
#define  EJ_SIZE_ROW           8


class homeWidget : public QWidget
{
    Q_OBJECT
public:
    /** \enum 枚举喷阀数据异常情况 */
    enum {
        EJ_NOT_FOUND = -1,  /*! 未找到喷嘴 */
        EJ_BEGIN_OUT = -2,  /*! 起始越界 */
        EJ_END_OUT = -3     /*! 终止越界 */
    };

public:
    explicit homeWidget(QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshDisPlay(bool bflag = false);

    void startorstoptimer(bool bflag);
    
    //! 获取以行号为索引的矩形框记录
    // QMap<uint, QMap<int, QList<BBox>>> getLineBoxMap() const { return m_lineBoxMap; }
    
    // //! 清理指定行号之前的矩形框记录
    // void clearLineBoxMapBeforeRow(uint nRow);
    
    // //! 获取指定行号指定相机的矩形框列表
    // QList<BBox> getBoxesByRowAndCamera(uint nRow, int nCamId) const;

signals:
    void sigRefreshSocinferResult(int nStatsId);
    void sigRefreshSimRes(QString str);

public slots:
    void onrefreshHomepaheBySocInfer(std::vector<BBox> vBox, int nCamId, uint nRow);
    void onrefreshpagebySocBottleInfer(QByteArray parray, int nCamId, uint nRow);
    void timeOutSlt();
    void onResetStaticsResult(int nCount, bool bRest);
    void onChangeScheme(void);
    void onSimulateEnchg(bool bEn);

private:
    bool bRsvallcamRes(void);
    std::vector<myfvKA1Box> preProBox(QList<int> camIdxlst, bool bSingle);

    //! 统计和框函数
    void _statisticRes(std::vector<myfvKA1Box> vBox, int nCamId);
    void _staMaterialRes(myfvKA1Box& box, int nCamId);
    void _staLabRes(myfvKA1Box& box, int nCamId);
    void _calLabDen(myfvKA1Box& box, int nCamId);

    //! 非和框函数
    void _calDen(QByteArray ary, int nCamId);
    void _staLab(QByteArray parray, int nCamId, uint nRow);
    void _staMat(QByteArray parray,int nCamId);

    //! 剔除
    QVector<QRect>  _predisBBox(std::vector<myfvKA1Box> vBox);      /*! 框前处理 */
    void _disposeEjData(std::vector<myfvKA1Box> vBox, uint nRow);   /*! 处理喷阀数据 */
    int  _getEjIndex(int nCamId, int nPx);                          /*! 根据像元获取喷阀序号 */
    void _sendEjData(int nCamId, char *pData, uint nRow);           /*! 发送喷阀数据 */
    void _adjustRect(QRect &rect);

    //! 统计每个相机的识别情况
    std::vector<BBox> _preProcessBox(const std::vector<BBox> &vBox, int nCamId);
    void _staticCamLabels(std::vector<myfvKA1Box>& mFvBox);
private:
    AnalyzeTestDlg *m_analyzeStaPage;     //! 分析仪统计页面

    QVector<QMap<QString, uint>> vPurityGp;    //! 统计识别结果统计

    QTimer          *m_timer;
    
    //! 以行号为索引的矩形框记录，格式：QMap<行号, QMap<相机ID, QList<BBox>>>
    // QMap<uint, QMap<int, QList<BBox>>> m_lineBoxMap;

    char            m_pEjData[MAX_EJECTOR_PACKET_ROW*EJ_SIZE_ROW];
    char            m_cacheEjData[MAX_CACHE_PACKET_ROW*EJ_SIZE_ROW];
};

#endif // HOMEWIDGET_H
