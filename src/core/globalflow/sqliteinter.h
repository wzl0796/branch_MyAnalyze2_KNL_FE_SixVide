#ifndef SQLITEINTER_H
#define SQLITEINTER_H

#include <QtCore>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QSqlError>
#include <QDebug>
#include "params/define.h"
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"

class sqliteInter
{
public:
    sqliteInter();
    virtual ~sqliteInter();
    int  initializationDB();
    void createTable(const QString& tab);
    void dropTable(const QString& tab);
    void renametable(const QString& tab);
    void testDB();
    QSqlQuery executeCmd(QString cmd);
    QSqlDatabase getDBHandle();

private:
    QSqlDatabase m_pDB;
};

extern sqliteInter *g_sqliteInter;

#endif // SQLITEINTER_H
