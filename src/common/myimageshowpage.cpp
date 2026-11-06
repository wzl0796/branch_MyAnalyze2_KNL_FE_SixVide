#include "myimageshowpage.h"
#include "mymessagebox.h"
#include "core/globalflow/globalflow.h"
#include "core/io/udp/myudp.h"
#include <QMouseEvent>

static uchar oriBuf[IMAGE_HEIGHT*SENSOR_PIXELS_1024*3];          // 用于存储原始图片的数据
static uchar disBuf[IMAGE_HEIGHT*SENSOR_PIXELS_1024*3];          // 用于存储原始图片的数据

myViewLable::myViewLable()
{

}

void myViewLable::mousePressEvent(QMouseEvent *event)
{
    int tmpX = event->pos().x();
    int tmpY = event->pos().y();

    emit showPos(tmpX, tmpY);
}

myImageShowPage::myImageShowPage(QWidget *parent) : QDialog(parent)
{
    initParams();

    createPage();

    connectSigAndSlt();
}

/**
 * @brief myImageShowPage::初始化参数
 */
void myImageShowPage::initParams()
{
    nScale = 1;
    nHSliderDis = 0;
    nVSliderDis = 0;
    m_nImgHeight = IMAGE_HEIGHT;
    m_nImgWidth = SENSOR_PIXELS_1024;
    lastScrollBias.setX(0);
    lastScrollBias.setY(0);
}

/**
 * @brief myImageShowPage::界面构造
 */
void myImageShowPage::createPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);
    m_widget->setGeometry((WIN_WID - 1100)/2, (WIN_HEI - 700)/2, 1100, 700);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
    m_widget->show();

    imgLbl = new myViewLable();
    imgLbl->setScaledContents(true);
//    imgLbl->setFixedSize(1024,512);

    QVBoxLayout *Lay = new QVBoxLayout(m_widget);

    m_simImgScrollArea = new QScrollArea();
    m_simImgScrollArea->verticalScrollBar()->setStyleSheet(
                "QScrollBar:vertical{"
                "width:40px;"
                "background:#c0c0c0;"
                "margin:0px,0px,0px,0px;"
                "padding-top:0px;"
                "padding-bottom:0px;"
                "}"

                );
    m_simImgScrollArea->horizontalScrollBar()->setStyleSheet(
                "QScrollBar:horizontal{"
                "height:40px;"
                "background:#ebebeb;"
                "margin:0px,0px,0px,0px;"
                "padding-left:0px;"
                "padding-right:0px;"
                "}"
                );
    m_simImgScrollArea->setBackgroundRole(QPalette::Dark);
    m_simImgScrollArea->setWidgetResizable(true);
    m_simImgScrollArea->setWidget(imgLbl);
    m_simImgScrollArea->setFixedSize(SENSOR_PIXELS_1024,IMAGE_HEIGHT);

    RGBNLbl = new QLabel();
    RGBNLbl->setText("R:220 G:220 B:220");
    RGBNLbl->setFixedSize(BTN_WID*4, BTN_HEI);

    backBtn = new QPushButton();
    backBtn->setText("返回");
    backBtn->setFixedSize(BTN_WID, BTN_HEI);

    enLargeBtn = new QPushButton();
    enLargeBtn->setFixedSize(BTN_HEI-10,BTN_HEI-10);
    enLargeBtn->setText("+");

    reduceBtn = new QPushButton();
    reduceBtn->setFixedSize(BTN_HEI-10,BTN_HEI-10);
    reduceBtn->setText("-");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch(0);
    btnLayout->addWidget(enLargeBtn);
    btnLayout->addWidget(reduceBtn);
    btnLayout->addStretch(0);

    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addStretch(0);
    downLayout->addLayout(btnLayout);
    downLayout->addWidget(RGBNLbl, 0, Qt::AlignLeft);
    downLayout->addWidget(backBtn, 0, Qt::AlignRight);

    Lay->addStretch(0);
    Lay->addWidget(m_simImgScrollArea, 0, Qt::AlignCenter);
//    Lay->addWidget(RGBNLbl, 0, Qt::AlignCenter);
    Lay->addStretch(0);
    Lay->addLayout(downLayout);
}

/**
 * @brief myImageShowPage::信号槽绑定
 */
void myImageShowPage::connectSigAndSlt()
{
    connect(m_simImgScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),     this, SLOT(horizonSlideMoved(int)));
    connect(m_simImgScrollArea->verticalScrollBar(),   SIGNAL(valueChanged(int)),     this, SLOT(verticalSlideModed(int)));
    connect(m_simImgScrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(horizonSlideRangeChanged(int,int)));
    connect(m_simImgScrollArea->verticalScrollBar(),   SIGNAL(rangeChanged(int,int)), this, SLOT(verticalSlideRangeChanged(int,int)));
    connect(enLargeBtn,  SIGNAL(pressed()),        this, SLOT(onEnlargeBtnClicked()));
    connect(reduceBtn,   SIGNAL(pressed()),        this, SLOT(onReduceBtnClicked()));
    connect(backBtn,     SIGNAL(pressed()),        this, SLOT(onBackBtnPressedSlt()));
    connect(imgLbl,      SIGNAL(showPos(int,int)), this, SLOT(showMousePos(int,int)));
}

/**
 * @brief myImageShowPage::refreshPage
 * @param index
 */
void myImageShowPage::refreshPage(int index)
{
    m_simImgScrollArea->horizontalScrollBar()->setSliderPosition(0);
    m_simImgScrollArea->verticalScrollBar()->setSliderPosition(0);

    bool bIsImg = true;
    QString strImg;
    QImage  image;
    int oriIndex;

    strImg.sprintf("/tmp/%d.png",index);
//    strImg.sprintf("E:/tmp/%d.png",index);
    bIsImg = image.load(strImg);

    if (bIsImg) {
        for (int i = 0; i < IMAGE_HEIGHT; i++) {
            for (int j = 0; j < SENSOR_PIXELS_1024; j++) {
                QColor rgb = image.pixel(j, i);
                oriIndex = (i*SENSOR_PIXELS_1024+j)*3;
                oriBuf[oriIndex] = rgb.red();
                oriBuf[oriIndex+1] = rgb.green();
                oriBuf[oriIndex+2] = rgb.blue();
            }
        }

        imgLbl->setPixmap(QPixmap(strImg));
        imgLbl->setScaledContents(true);
    }
}

/**
 * @brief myImageShowPage::关闭
 */
void myImageShowPage::onBackBtnPressedSlt()
{
    this->close();
}

/**
 * @brief myImageShowPage::显示坐标位置
 * @param x
 * @param y
 */
void myImageShowPage::showMousePos(int x, int y)
{
    uchar r,g,b;
#if 0
    printf("x:%d, y:%d\n", x, y);
    fflush(0);
    printf("nHSliderDis:%d, nVSliderDis:%d\n", nHSliderDis, nVSliderDis);
    fflush(0);
#endif

    int tmpX = x/nScale;
    int tmpY = y/nScale;

    r = oriBuf[(tmpY*SENSOR_PIXELS_1024+tmpX)*3];
    g = oriBuf[(tmpY*SENSOR_PIXELS_1024+tmpX)*3+1];
    b = oriBuf[(tmpY*SENSOR_PIXELS_1024+tmpX)*3+2];

    QString str = QString("x=%1,y=%2  R:%3,G:%4,B:%5").arg(tmpX).arg(tmpY).arg(r).arg(g).arg(b);
    RGBNLbl->setText(str);
}

/**
 * @brief myImageShowPage::horizonSlideMoved
 */
void myImageShowPage::horizonSlideMoved(int nDisX)
{
    nHSliderDis = nDisX;
}

/**
 * @brief myImageShowPage::verticalSlideModed
 */
void myImageShowPage::verticalSlideModed(int nDisY)
{
    nVSliderDis = nDisY;
}

/**
 * @brief myImageShowPage::horizonSlideRangeChanged
 * @param min
 * @param max
 */
void myImageShowPage::horizonSlideRangeChanged(int min, int max)
{
    if(lastScrollBias.x() == 0){
        return;
    }
    bool isEnlarge = false;
    if(max > lastScrollBias.x()){
        isEnlarge = true;
    } else if(max < lastScrollBias.x()){
        isEnlarge = false;
    } else {
        return;
    }
    lastScrollBias.setX(max);
    int horizon = getTransferScrollValue(m_simImgScrollArea->horizontalScrollBar()->value(),
                                         m_simImgScrollArea->verticalScrollBar()->value(), isEnlarge).x();

    m_simImgScrollArea->horizontalScrollBar()->setValue(horizon);
}

/**
 * @brief myImageShowPage::verticalSlideRangeChanged
 * @param min
 * @param max
 */
void myImageShowPage::verticalSlideRangeChanged(int min, int max)
{
    if(lastScrollBias.y() == 0){
        return;
    }
    bool isEnlarge = false;
    if(max > lastScrollBias.y()){
        isEnlarge = true;
    } else if(max < lastScrollBias.y()){
        isEnlarge = false;
    } else {
        return;
    }

    lastScrollBias.setY(max);
    int vertical = getTransferScrollValue(m_simImgScrollArea->horizontalScrollBar()->value(),
                                          m_simImgScrollArea->verticalScrollBar()->value(), isEnlarge).y();

    m_simImgScrollArea->verticalScrollBar()->setValue(vertical);
}

/**
 * @brief myImageShowPage::放大按钮
 */
void myImageShowPage::onEnlargeBtnClicked()
{
    nScale = nScale*2;
    if(lastScrollBias.x() == 0){
        lastScrollBias.setX(1);
    }
    if(lastScrollBias.y() == 0){
        lastScrollBias.setY(1);
    }

    if(nScale > 8) {
        nScale = 8;
    }

    refreshImgDisplay();
}

/**
 * @brief myImageShowPage::缩小按钮
 */
void myImageShowPage::onReduceBtnClicked()
{
    nScale = nScale/2;
    if(nScale < 1) {
        nScale = 1;
    }
    refreshImgDisplay();
}

/**
 * @brief myImageShowPage::refreshImgDisplay
 */
void myImageShowPage::refreshImgDisplay()
{
    int disbufIndex;
    int chgBufIndex;
    for(int i=0; i<m_nImgHeight; i++){
        for(int j=0; j<m_nImgWidth; j++){
            disbufIndex = (i*m_nImgWidth+j)*3;
            chgBufIndex = (i*m_nImgWidth+j)*3;
            disBuf[disbufIndex] = oriBuf[chgBufIndex];
            disBuf[disbufIndex+1] = oriBuf[chgBufIndex+1];
            disBuf[disbufIndex+2] = oriBuf[chgBufIndex+2];
        }
    }

    QImage img(disBuf, m_nImgWidth, m_nImgHeight, QImage::Format_RGB888);
    QPixmap pixmap;
    pixmap = QPixmap::fromImage(img);
    imgLbl->setPixmap(pixmap.scaled(m_nImgWidth*nScale,m_nImgHeight*nScale, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation));
}

/**
 * @brief myImageShowPage::坐标转换
 * @param x
 * @param y
 * @param isEnlarge
 */
QPoint myImageShowPage::getTransferScrollValue(int x, int y, bool isEnlarge)
{
    int showWidth  = m_simImgScrollArea->width() - m_simImgScrollArea->verticalScrollBar()->width();
    int showHeight = m_simImgScrollArea->height() - m_simImgScrollArea->horizontalScrollBar()->height();
    int centerX    = showWidth/2 + x;
    int centerY    = showHeight/2+ y;

    //按比例放大/缩小
    if(isEnlarge){
        centerX *=2;
        centerY *=2;
    } else {
        centerX /=2;
        centerY /=2;
    }

    int horizonScrollValue = centerX  - showWidth/2;
    int verticalScrollValue = centerY - showHeight/2;

    return QPoint(horizonScrollValue, verticalScrollValue);
}


/**************************************************************************************
 **************************************图片显示类****************************************
***************************************************************************************/
GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent, qreal factor)
    : QGraphicsView(scene, parent), totalScaleFactor(factor)
{
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    setDragMode(ScrollHandDrag);
    setTransform(QTransform().scale(totalScaleFactor, totalScaleFactor));
}

bool GraphicsView::viewportEvent(QEvent *event)
{
    int x = 0, y = 0;
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        if (touchPoints.count() == 2) {
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length() / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor *= currentScaleFactor;
                currentScaleFactor = 1;
                totalScaleFactor = totalScaleFactor < 1 ? 1 : totalScaleFactor;
                totalScaleFactor = totalScaleFactor > 13 ? 13 : totalScaleFactor;
            }
            setTransform(QTransform().scale(totalScaleFactor * currentScaleFactor, totalScaleFactor * currentScaleFactor));
            return true;
        }
        QPointF scenepos = mapToScene(touchPoint0.pos().x(),touchPoint0.pos().y());
        x = (int)scenepos.x();
        y = (int)scenepos.y();
        if (x >= 0 && y >= 0){
            emit sigShowRGB(x, y);
        }
	break;
    }

    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPointF scenePos = mapToScene(mouseEvent->pos());
        x = (int)scenePos.x();
        y = (int)scenePos.y();
        if (x >= 0 && y >= 0){
            emit sigShowRGB(x, y);
        }
        break;
    }

    default:
        break;
    }
   
    return QGraphicsView::viewportEvent(event);
}

myView::myView(QWidget *parent, bool bSimulate/* = true*/)
    : QDialog(parent)
    , m_bSimulate(bSimulate)
    , m_totalScaleFactor(1.0)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0, 0, 0, 100);");

    int width = 640+160;
    int height = 1024+400;

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);
//    m_widget->setGeometry((WIN_WID-width)/2, (WIN_HEI-height)/2, width, height);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
    m_widget->show();

    pDataDisplay = new uchar[96*96*COLOR_TYPE_N];
    memset(pDataDisplay, 0, 96*96*COLOR_TYPE_N);

    pBigDataDisplay= new uchar[SENSOR_PIXELS_1024*BIG_IMAGE_HEIGHT*COLOR_TYPE_N];
    memset(pBigDataDisplay, 0, SENSOR_PIXELS_1024*BIG_IMAGE_HEIGHT*COLOR_TYPE_N);

    scene = new QGraphicsScene();
    view = new GraphicsView(scene);
    if(m_bSimulate){
        view->setMaximumWidth(SENSOR_PIXELS_1024*1.1);
    }else{
        view->setFixedSize(WIN_WID*3/4, WIN_HEI*3/4);
    }

    pixmapItem = new QGraphicsPixmapItem;
    pixmapItem->setPixmap(QPixmap::fromImage(QImage(pDataDisplay, m_bSimulate ? 96 : SENSOR_PIXELS_1024,
                                                    m_bSimulate ? 96 : IMAGE_HEIGHT, QImage::Format_RGB888)));
    scene->addItem(pixmapItem);

    m_RGBLbl = new QLabel("R: \tG: \tB:");
    m_RGBLbl->setMaximumSize(SENSOR_PIXELS_1024*1.1, BTN_HEI);
//    m_RGBLbl->setStyleSheet("background-color: rgb(225, 225, 225, 255);");
    m_RGBLbl->setAlignment(Qt::AlignCenter);

    m_lenAreaLbl = new QLabel(QString("Len: \tArea: \t"));
//    m_lenAreaLbl->setStyleSheet("background-color: rgb(225, 225, 225, 255);");
    m_lenAreaLbl->setMaximumWidth(SENSOR_PIXELS_1024*1.1);

    if(!m_bSimulate){
        m_lenAreaLbl->hide();
    }

    m_reduceBtn = new QPushButton; //缩小
    m_reduceBtn->setIcon(QIcon(":/res/png/Edit_ZoomOut.png"));
    m_reduceBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_reduceBtn->setFixedSize(BTN_HEI, BTN_HEI);

    m_largeBtn = new QPushButton;  //放大
    m_largeBtn->setIcon(QIcon(":/res/png/Edit_ZoomIn.png"));
    m_largeBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_largeBtn->setFixedSize(BTN_HEI, BTN_HEI);

    m_slider = new QSlider(Qt::Vertical);
    m_slider->setFocusPolicy(Qt::StrongFocus);
    m_slider->setTickPosition(QSlider::TicksRight);
    //    m_slider->setFixedSize(70, /*260*//*IMG_ROW*4.1*/height-300);
    m_slider->setFixedWidth(70);
    m_slider->setPageStep(1);
    m_slider->setRange(0, 100);
    m_slider->setValue(0);

    QWidget *rightWidget = new QWidget;
    rightWidget->setFixedWidth(120);

    QVBoxLayout *vLay = new QVBoxLayout(rightWidget);
    vLay->setSpacing(20);
    vLay->addWidget(m_largeBtn, 0, Qt::AlignHCenter);
    vLay->addWidget(m_slider, 0, Qt::AlignHCenter);
    vLay->addWidget(m_reduceBtn, 0, Qt::AlignHCenter);

    QVBoxLayout *picLayout = new QVBoxLayout;
    picLayout->setMargin(30);
    picLayout->addStretch();
    picLayout->addWidget(view, 0, Qt::AlignCenter);
    picLayout->addWidget(m_RGBLbl, 0, Qt::AlignCenter);
    picLayout->addWidget(m_lenAreaLbl, 0, Qt::AlignCenter);
    picLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout(m_widget);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(picLayout);
    mainLayout->addWidget(rightWidget);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(m_widget, 0, Qt::AlignCenter);

    view->show();

    connect(view, SIGNAL(sigShowRGB(int, int)), this, SLOT(sltShowRGB(int, int)));
    connect(m_reduceBtn, SIGNAL(clicked(bool)), this, SLOT(onReduceBtnSlt()));
    connect(m_largeBtn,  SIGNAL(clicked(bool)), this, SLOT(onLargeBtnSlt()));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChangedSlt(int)));
}

myView::~myView()
{
    if (pDataDisplay != NULL) {
        delete[] pDataDisplay;
        pDataDisplay = NULL;
    }

    if (pBigDataDisplay != NULL) {
        delete[] pBigDataDisplay;
        pBigDataDisplay = NULL;
    }
}
/*!
 * \brief 加载图片
 * \param index
 */
void myView::setViewData(int index)
{
//    QString strImg;
//    if (m_bSimulate) {
//        strImg.sprintf("/tmp/simulate_%d.png",index);

//        m_lenAreaLbl->setText(QString("Len: %1   Area: %2")
//                              .arg(struGsh.simLenMap[index])
//                              .arg(struGsh.simAreaMap[index]));
//    } else {
//        strImg.sprintf("/tmp/%d.png",index);
//    }

//    setViewData(strImg);
}

/*!
 * \brief 加载图片
 * \param picName
 */
void myView::setViewData(QString picName)
{
    QImage  image;
    bool bIsImg = image.load(picName);
    if (bIsImg) {
        m_width = image.width();
        m_height = image.height();
        qDebug("imgwid=%d  imgHei=%d",m_width,m_height);
        bool bBig = (BIG_IMAGE_HEIGHT == m_height);
        if(!bBig){
            if (pDataDisplay != NULL) {
                delete[] pDataDisplay;
                pDataDisplay = NULL;
            }

            pDataDisplay = new uchar[m_width*m_height*COLOR_TYPE_N];
            memset(pDataDisplay, 0, m_width*m_height*COLOR_TYPE_N);
        }
        uchar *tmp = bBig ? pBigDataDisplay : pDataDisplay;

        int oriIndex;
        for (int i = 0; i < m_height; i++) {
            for (int j = 0; j < m_width; j++) {
                QColor rgb = image.pixel(j, i);
                oriIndex = (i*m_width+j)*3;

                tmp[oriIndex] = rgb.red();
                tmp[oriIndex+1] = rgb.green();
                tmp[oriIndex+2] = rgb.blue();
            }
        }

        if (m_bSimulate) {
            if(96 == m_width){
                view->setFixedSize(256*1.3,256*1.3);
                view->scale((256*1.3-4)/m_width, (256*1.3-4)/m_height);
                m_RGBLbl->setFixedSize(256*1.3, BTN_HEI);
            } else {
                view->setFixedSize(256*1.3,256*1.3);
                m_RGBLbl->setFixedSize(256*1.3, BTN_HEI);
            }
            pixmapItem->setPixmap(QPixmap::fromImage(image));
        } else {
            pixmapItem->setPixmap(QPixmap::fromImage(image));
        }
    }else{
        infoWidget->raise();
        infoWidget->setLabelText("图片加载失败");
        infoWidget->show();
        this->hide();
    }

    m_slider->setValue(0);
}

/*!
 * \brief 槽：根据坐标显示rgb
 * \param x
 * \param y
 */
void myView::sltShowRGB(int x, int y)
{

    if(x<0 || x>=m_width || y<0 || y >=m_height){
        return;
    }

    bool bBig = (BIG_IMAGE_HEIGHT == m_height);
    uchar *tmp = bBig ? pBigDataDisplay : pDataDisplay;

    int red   = tmp[(y*m_width + x)*COLOR_TYPE_N];
    int green = tmp[(y*m_width + x)*COLOR_TYPE_N + COLOR_TYPE_G];
    int blue  = tmp[(y*m_width + x)*COLOR_TYPE_N + COLOR_TYPE_B];

    m_RGBLbl->setText(QString("X: %1   Y: %2,    R: %3   G: %4   B: %5")
                      .arg(x).arg(y)
                      .arg(red).arg(green).arg(blue));
}

/*!
 * \brief myView::onSliderValueChangedSlt
 * \param value
 */
void myView::onSliderValueChangedSlt(int value)
{
    m_totalScaleFactor = 1 + (13-1)*value/100.0;
    m_totalScaleFactor = m_totalScaleFactor < 1 ? 1 : m_totalScaleFactor;
    m_totalScaleFactor = m_totalScaleFactor > 13 ? 13 : m_totalScaleFactor;

    view->setTransform(QTransform().scale(m_totalScaleFactor, m_totalScaleFactor));
}

//! 缩小
void myView::onReduceBtnSlt()
{
    int tmp = m_slider->value()-5;
    m_slider->setValue(tmp);
}

//! 放大
void myView::onLargeBtnSlt()
{
    int tmp = m_slider->value()+5;
    m_slider->setValue(tmp);
}

void myView::mousePressEvent(QMouseEvent *event)
{
    QPoint tmp = event->globalPos();
    int startX = m_widget->x();
    int startY = m_widget->y();

    if(!(tmp.x()> startX && tmp.x() < startX+m_widget->width()
            && tmp.y()> startY && tmp.y() < startY+m_widget->height()))
        this->hide();
}
