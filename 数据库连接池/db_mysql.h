#ifndef _DB_MYSQL_H_
#define _DB_MYSQL_H_

#include <string>
#include <mysql.h>
#include <map>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include <list>
#include <pthread.h>
using namespace std;

class CDB_Mysql
{
public:
    CDB_Mysql();
    ~CDB_Mysql();
    int login_db(const char* host, int port, const char* user, const char* passwd);
    void logout_db();
    int query_no_result(char *strcmd, char *retMsg);
    int query_with_result(char*strcmd, char *retMsg);
    int free_result();
    int get_rec_num();
    bool is_lostconnect();

public:
    MYSQL *mysql;
    MYSQL_RES *res;

};

typedef std::shared_ptr<CDB_Mysql> db_mysql_ptr;

class Mysql_Poll
{
public:
    Mysql_Poll(std::string host, int port, std::string user, std::string passwd);
    ~Mysql_Poll();

public:
    db_mysql_ptr GetConnection();
    void ReleaseConnection(db_mysql_ptr conn);

private:
    std::list<db_mysql_ptr> mysql_list_;
    pthread_mutex_t *m_mutexlock;

    std::string dbhost;
    std::string dbuser;
    std::string dbpass;
    int dbport;
};

typedef std::shared_ptr<Mysql_Poll> dbmysql_poll_ptr;
#endif

