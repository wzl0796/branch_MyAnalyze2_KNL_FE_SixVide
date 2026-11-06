#include "sqliteinter.h"

sqliteInter *g_sqliteInter = new sqliteInter;

sqliteInter::sqliteInter()
{

}

sqliteInter::~sqliteInter()
{
    m_pDB.close();
}

QSqlDatabase sqliteInter::getDBHandle()
{
    return m_pDB;
}

/*!
 * \brief sqliteInter::initializationDB
 * \return
 */
int sqliteInter::initializationDB()
{
    if(m_pDB.contains("mydatabase")){
        m_pDB = QSqlDatabase::database("mydatabase");
    }else{
        m_pDB = QSqlDatabase::addDatabase("QSQLITE","mydatabase");
        m_pDB.setDatabaseName(CNF_DATABASE);
    }


    if(!m_pDB.open()) {
        printf("Can't open DB when initialization!\n");
        m_pDB.close();
        return -1;
    }

    //! 创建表格
    QString schemcode = struCnfg.schemeList[struCnfg.nCntScheme].code;
    createTable(schemcode);

    return 0;
}

/*!
 * \brief 创建表格
 * \param tab
 */
void sqliteInter::createTable(const QString &tab)
{
    QString strQuery = QString("CREATE TABLE IF NOT EXISTS '%1' "
                               "(LOGTIME TEXT,batch TEXT,duty TEXT,yield REAL,sta INT, scheme TEXT,data TEXT)")
                           .arg(tab);
    executeCmd(strQuery);
}

/*!
 * \brief 删除表格
 * \param tab
 */
void sqliteInter::dropTable(const QString &tab)
{
    QString strQuery = QString("DROP TABLE IF EXISTS '%1'").arg(tab);
    executeCmd(strQuery);
}

void sqliteInter::renametable(const QString &tab)
{
    QString strname = QString("%1").arg(tab).remove("@back");
    QString strQuery = QString("ALTER TABLE '%1' RENAME TO '%2'").arg(tab).arg(strname);
    executeCmd(strQuery);
}

void sqliteInter::testDB()
{
    QString sql;
    QDateTime date = QDateTime::currentDateTime();
    QString time = date.toString("yyyy-MM-dd hh:mm:ss");
    int nBatch = 1;

    sql = QString("SELECT * FROM BATCHDATA WHERE LOGTIME LIKE '%1%' AND scheme = 'GT'").arg(date.toString("yyyy-MM-dd"));
    QSqlQuery query = executeCmd(sql);
    query.first();
    do {
        if(!query.isValid())continue;
        ++nBatch;
    }while (query.next());

    for (int i = 0; i < 5; i++) {
        sql = QString("INSERT INTO STATISTICDATA "
                "(LOGTIME, no, value, batch)"
                "VALUES('%1',%2,%3,'%4')").arg(time).arg(i).arg(double(qrand()%100))
                .arg(date.toString("yyyy-MM-dd") + QString("-%1").arg(nBatch));
        executeCmd(sql);
    }

    sql = QString("INSERT INTO BATCHDATA "
            "(LOGTIME, batch, people, original, scheme)"
            "VALUES('%1','%2','%3','%4','%5')").arg(time).arg(date.toString("yyyy-MM-dd")+QString("-%1").arg(nBatch))
            .arg("张三").arg("A线").arg("GT");
    executeCmd(sql);
}

QSqlQuery sqliteInter::executeCmd(QString cmd)
{
    QSqlQuery query(m_pDB);
    query.prepare(cmd);
    bool rc = query.exec();
    if(!rc) {
        QSqlError lastError = query.lastError();
        printf("executeCmd %s\n", lastError.driverText().toStdString().c_str());
        fflush(0);
        return QSqlQuery();
    }

    return query;
}
