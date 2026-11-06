#!/bin/bash

MYPRODUCT=mIM-KAB_356X
MYVERSION=V1.02
CURTIME=$(date +%Y%m%d)
ARM_VERSION=1.02
ARM_CODE=80030214

# global function
print_usage()
{
    echo "----------------------------------------"
    echo ""
    echo "Please use parameters as follow"
    echo "Debug      Mode: ./qtbuild.sh debug"
    echo "Release    Mode: ./qtbuild.sh release"
    echo ""
    echo "----------------------------------------"
}

# debug && release && clean
case "$1" in
    "debug")
    MYMODEOPT="debug";
    break;;
    "release")
    MYMODEOPT="release";
    make distclean;
    break;;
    *)
    print_usage
    exit 1;;
esac

TOOLCHAIN=/opt/aarch64-rockchip-linux-gnu/bin
PATH=$TOOLCHAIN:$PATH
STRIP=$TOOLCHAIN/aarch64-rockchip-linux-gnu-strip
REL_DIR=release
QMAKE=/opt/Qt5.9.9-IPC3568/bin/qmake
export PATH

rm -f app/i18n/*
cp -f i18n/*.qm app/i18n/

#生成版本信息文件
./svninfo.sh

if [ $# -gt 1 ] ; then
	ARM_VERSION=$2
	ARM_CODE=$3
fi
cat >> svninfo.ini << EOF
ARM=$ARM_VERSION
CODE=$ARM_CODE
EOF

SVNINFO=./svninfo.ini
if [ -s $SVNINFO ] ; then
    source $SVNINFO
    $QMAKE -set MYSVN_VERSION $VERSION
    $QMAKE -set MYARM_VERSION $ARM
    $QMAKE -set MYARM_CODE    $CODE
fi

SVNINFO=./svnpredict.ini
if [ -s $SVNINFO ] ; then
    source $SVNINFO
    $QMAKE -set MYPREDICT_VERSION $VERSION
fi

# qmake & make
$QMAKE
make $MYMODEOPT -j4

# debug mode: exit
if [ $MYMODEOPT = "debug" ]
then
    exit 0
fi

# release mode: make ext3 disk
cp -rf app app_t
find app_t -name .svn -print | xargs rm -rf
rsync -a src/3rdparty/printer/lib/* app_t/lib/
genext2fs -b 65536 -N 256 -d app_t app.bin
rm -rf app_t
mv -f app.bin ${CURTIME}.${MYPRODUCT}_${MYVERSION}.bin
mv -f ${CURTIME}.${MYPRODUCT}_${MYVERSION}.bin $REL_DIR/${MYPRODUCT}_${MYVERSION}.bin
