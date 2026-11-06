#include "ui/mainwindow.h"
#include "mymanager.h"
#include <QTextCodec>
#include <QFile>
#include <QApplication>

#define FONT_PATH   "/app/fonts/"


QWidget *g_splashwidget = nullptr;

/**
 * @brief 设置开机界面
 */
QWidget* setLogo(void)
{
    QWidget *splash = new QWidget;
    splash->setGeometry(0, 0, WIN_WID, WIN_HEI);
    splash->setWindowFlags(Qt::FramelessWindowHint);
    splash->setStyleSheet("background-color:white");

    QLabel *pixLbl = new QLabel;
    pixLbl->setFixedSize(1200, 900);
    pixLbl->setPixmap(QPixmap(":/res/png/Vendor_Meyer.png").scaled(QSize(800, 600)));
    pixLbl->setAlignment(Qt::AlignCenter);

    QLabel *textLbl = new QLabel;
    textLbl->setObjectName("textLbl");

    QMovie *movie;
    movie = new QMovie(":/res/png/2.gif");
    movie->setObjectName("movie");
    movie->setScaledSize(QSize(50,50));

    QLabel *circleLbl = new QLabel;
    circleLbl->setFixedSize(50,50);
    circleLbl->setMovie(movie);
    movie->start();

    QVBoxLayout *lay = new QVBoxLayout(splash);
    lay->addWidget(pixLbl, 0, Qt::AlignCenter);
    lay->addWidget(circleLbl, 0, Qt::AlignCenter);
    lay->addStretch();

    splash->show();

    return splash;
}

/*!
 * \brief 获取字体
 * \param str：字体名称
 * \return
 */
QFont getTypeface(QString str)
{
    // 获取应用程序目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString fontPath = appDir + "/fonts/";

    QFont font;
    int fontId = QFontDatabase::addApplicationFont(QString("%1%2.ttf").arg(fontPath).arg(str));
    if (fontId != -1) {
        QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
        font.setFamily(fontName);
    }

    return font;
}


/*!
 * \brief 根据当前语言获取字体
 * \param nLangId：语言索引
 * \return
 */
QFont getFont(int nLangId)
{
    if (nLangId == LANG_THAILAND) {
        return getTypeface("leelawadee");
    } else if (nLangId == LANG_AFARSI
               || nLangId == LANG_UYGHUR
               || nLangId == LANG_AARABIC ) {//! 波斯 维吾尔  阿拉伯文
        return getTypeface("tahoma");
    } else if (nLangId == LANG_ABENGALI) {//! 孟加拉语
        return getTypeface("vrinda");
    } else if (nLangId == LANG_MYANMAR) {//! 缅甸
        return getTypeface("padauk-bold");
    } else if (nLangId == LANG_AKOREAN) {
        return QFont();     //! 韩语使用系统默认字体
    }

    return getTypeface("wqy-microhei");
}

/**
 * @brief 设置样式表
 * @return
 */
QByteArray getQssStyel(void)
{
    QByteArray array;

    QFile qss;

    qss.setFileName(":/res/qss/ui_zh-cn.qss");

    qss.open(QFile::ReadOnly);

    array = qss.readAll();

    qss.close();

    return array;
}

/*!
 * \brief qMain
 * \param argc
 * \param argv
 * \return
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QDir::setCurrent(QCoreApplication::applicationDirPath()+"/");

#ifdef Q_OS_UNIX
    //! 设置开机图片
    // g_splashwidget = setLogo();
#endif
    a.setStyleSheet(getQssStyel());

    qRegisterMetaType<BBox>();
    qRegisterMetaType<myfvKA1Box>();
    qRegisterMetaType<std::vector<BBox>>();

    gFlow.initAll();

    //! 设置字体
    a.setFont(getFont(myLan.nLang));

    myManager *MainManager = new myManager;

    MainWindow w;
    g_mainWidget = &w;
    w.show();

    return a.exec();
}
