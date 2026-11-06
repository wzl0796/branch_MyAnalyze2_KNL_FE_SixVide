#-------------------------------------------------
#
# Project created by QtCreator 2022-08-22T14:01:06
#
#-------------------------------------------------

QT += core gui widgets charts network serialport sql printsupport serialbus concurrent gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = app
TARGET = MyAnalyze2_KA1
TEMPLATE = app
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
RCC_DIR = build/rcc
CONFIG += qt debug_and_release warn_on thread c++17

#  开启release模式包含debug符号表， 且符号表和程序的剥离  qtmakeApp.sh中的strip可以注释掉
#    QMAKE_CXXFLAGS -= -O2
#    QMAKE_CXXFLAGS += -O0 -g
#   QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
#   QMAKE_CFLAGS_RELEASE   = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#   QMAKE_LFLAGS_RELEASE   = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

#   EXE = $$OUT_PWD/app/$$TARGET
#   DEBUGPATH=$$OUT_PWD/$$TARGET
#   DEBUGFILE=$${DEBUGPATH}.debug

#   QMAKE_STRIP =

#   QMAKE_POST_LINK += \
#       /opt/aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-objcopy --only-keep-debug "$${EXE}" "$${DEBUGFILE}" && \
#       /opt/aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-strip --strip-all "$${EXE}" && \
#       /opt/aarch64-rockchip-linux-gnu/bin/aarch64-rockchip-linux-gnu-objcopy --add-gnu-debuglink="$${DEBUGFILE}" "$${EXE}"


#QMAKE_CC += -g
#QMAKE_CXX += -g
#QMAKE_LINK += -g
#QMAKE_LINK_SHLIB_CMD +=-g
#CONFIG += force_debug_info
#QMAKE_CXXFLAGS += -g
#QMAKE_CFLAGS += -g
#QMAKE_CXXFLAGS += -O0

#开启运行时内存错误检测(编译前需在编译脚本所在位置执行软连接命令ln -s /opt/aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/lib64/libasan_preinit.o ./libasan_preinit.o)
#QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS += -fsanitize=address
#QMAKE_CXXFLAGS += -rdynamic -g
#QMAKE_LFLAGS += -rdynamic
#unix{
#    LIBS += -L/opt/aarch64-rockchip-linux-gnu/aarch64-rockchip-linux-gnu/lib64 -lasan
#}


INCLUDEPATH += . src

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += $$PWD/src/*.cpp\
    $$PWD/src/3rdparty/qjson/*.cpp \
    $$PWD/src/3rdparty/libqrencode/*.c \
    $$PWD/src/core/globalflow/*.cpp \
    $$PWD/src/core/mythread/*.cpp \
    $$PWD/src/core/io/udp/*.cpp \
    $$PWD/src/core/io/localsocket/*.cpp \
    $$PWD/src/core/io/myserial/*.cpp \
    $$PWD/src/core/io/iot/*.cpp \
    $$PWD/src/common/*.cpp \
    $$PWD/src/params/*.cpp \
    $$PWD/src/ui/*.cpp \
    $$PWD/src/ui/cameraSetPage/*.cpp \
    $$PWD/src/ui/systemPage/*.cpp \
    $$PWD/src/ui/statusBarPage/*.cpp \
    $$PWD/src/ui/homePage/*.cpp \
    $$PWD/src/ui/factorySetPage/*.cpp \
    $$PWD/src/ui/IdentifityPage/*.cpp \
    $$PWD/src/3rdparty/qextserialport/qextserialport.cpp \

HEADERS  += $$PWD/src/*.h \
    $$PWD/src/3rdparty/qjson/*.h \
    $$PWD/src/3rdparty/libqrencode/*.h \
    $$PWD/src/core/globalflow/*.h \
    $$PWD/src/core/mythread/*.h \
    $$PWD/src/core/io/udp/*.h \
    $$PWD/src/core/io/localsocket/*.h \
    $$PWD/src/core/io/myserial/*.h \
    $$PWD/src/core/io/iot/*.h \
    $$PWD/src/common/*.h \
    $$PWD/src/params/*.h \
    $$PWD/src/ui/*.h \
    $$PWD/src/ui/cameraSetPage/*.h \
    $$PWD/src/ui/systemPage/*.h \
    $$PWD/src/ui/statusBarPage/*.h \
    $$PWD/src/ui/homePage/*.h \
    $$PWD/src/ui/factorySetPage/*.h \
    $$PWD/src/ui/IdentifityPage/*.h \
    $$PWD/src/3rdparty/libdccrypt/include/dccrypt.h \
    $$PWD/src/3rdparty/qextserialport/qextserialport.h \
    $$PWD/src/3rdparty/qextserialport/qextserialport_global.h \
    $$PWD/src/3rdparty/qextserialport/qextserialport_p.h \

win32 {
    DEFINES += MAJORCAMNUM=4
    DEFINES += SVN_VERSION=10000
    SOURCES += $$PWD/src/3rdparty/qextserialport/qextserialport_win.cpp

    LIBS += -L$$PWD/src/3rdparty/libdccrypt/lib -ldccrypt_win32
}

unix {
    DEFINES += MAJORCAMNUM=$${CAMERANUM}
    DEFINES += SVN_VERSION=$$[MYSVN_VERSION]
    DEFINES += ARM_VERSION=$$[MYARM_VERSION]
    DEFINES += ARM_CODE=$$[MYARM_CODE]


    SOURCES += $$PWD/src/3rdparty/socketio/*.cpp \
               $$PWD/src/3rdparty/qextserialport/qextserialport_unix.cpp \


    HEADERS += $$PWD/src/3rdparty/printer/include/*.h \
               $$PWD/src/3rdparty/socketio/*.h

    INCLUDEPATH += $$PWD/src/3rdparty/printer/include/
    HEADERS += $$PWD/src/3rdparty/socketio/*.h

    LIBS += -L$$PWD/src/3rdparty/printer/lib -lCsnPrinterLibs \
            -L$$PWD/src/3rdparty/libgetpwd -lgetpwd \
            -L$$PWD/src/3rdparty/socketio -lsioclient -lboost_date_time -lboost_random -lboost_system -lpthread -ldl \
            -L$$PWD/src/3rdparty/libdccrypt/lib -ldccrypt
}

RESOURCES += \
    res.qrc

TRANSLATIONS += i18n/zh-cn.ts \
                i18n/en.ts \
                i18n/es.ts \
                i18n/ru.ts \
                i18n/vi.ts \
                i18n/th.ts \
                i18n/zh-tw.ts \
                i18n/tr.ts \
                i18n/fa.ts \
                i18n/fr.ts \
                i18n/ug.ts \
                i18n/ko.ts \
                i18n/ar.ts \
                i18n/bg.ts \
                i18n/sk.ts \
                i18n/pt.ts \
                i18n/my.ts \
                i18n/bn.ts \
                i18n/id.ts \
                i18n/pl.ts \
                i18n/it.ts \
                i18n/cb.ts \
                i18n/jv.ts \
                i18n/de.ts \
                i18n/hu.ts

DISTFILES +=
