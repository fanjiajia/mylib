#include "db_mysql.h"

CDB_Mysql::CDB_Mysql()
{
    mysql = NULL;
    res = NULL;
}

CDB_Mysql::~CDB_Mysql()
{
    if(res) 
    {
        mysql_free_result(res);
    }
    if(mysql)
    {
        mysql_close(mysql);
    }
}

int CDB_Mysql::login_db(const char* host, int port, const char* user, const char* passwd)
{
    mysql = mysql_init(NULL);
    if (!mysql) return -1;
    mysql_options( mysql, MYSQL_SET_CHARSET_NAME , "utf8");
    mysql = mysql_real_connect(mysql, host, user, passwd, NULL, port, NULL, 0);
    if (NULL == mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
        return -1;
    }
    mysql->reconnect = 1;
    return 0;
}

void CDB_Mysql::logout_db()
{
    if(mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}

int CDB_Mysql::query_no_result(char *strQuery, char *retMsg)
{
    if(NULL == mysql) return 1;
    if(0 != mysql_real_query(mysql, strQuery,(unsigned int) strlen(strQuery)))
    {
        strcpy(retMsg, mysql_error(mysql));
        return -1;
    }
    return 0;
}

int CDB_Mysql::query_with_result(char*strcmd, char *retMsg)
{
    if(NULL == mysql) return 1;
    if(0 != mysql_real_query(mysql, strcmd,(unsigned int) strlen(strcmd)))
    {
        strcpy(retMsg, mysql_error(mysql));
        return -1;
    }
    res = mysql_store_result(mysql); 
    return 0;
}

int CDB_Mysql::free_result()
{
    if(res)
    {
        mysql_free_result(res); 
        res = NULL;
    }
    return 0;
}

int CDB_Mysql::get_rec_num()
{
    if(res == NULL) return 0;
    return mysql_num_rows(res);
}

bool CDB_Mysql::is_lostconnect()
{
    if (mysql) return false;
    return true;
}

Mysql_Poll::Mysql_Poll(std::string host, int port, std::string user, std::string passwd)
{
    dbhost = host;
    dbport = port;
    dbuser = user;
    dbpass = passwd;
    m_mutexlock = new pthread_mutex_t;
    pthread_mutex_init(m_mutexlock, NULL);
}

Mysql_Poll::~Mysql_Poll()
{
    pthread_mutex_destroy(m_mutexlock);
    delete m_mutexlock;
}

db_mysql_ptr Mysql_Poll::GetConnection()
{
    db_mysql_ptr res = NULL;
    pthread_mutex_lock(m_mutexlock);
    if (mysql_list_ .empty())
    {
        db_mysql_ptr conn(new CDB_Mysql());
        if (0 == conn->login_db(dbhost.c_str(), dbport, dbuser.c_str(), dbpass.c_str())) res = conn;
    }
    else
    {
        res = mysql_list_.front();
        mysql_list_.pop_front();
    }
    pthread_mutex_unlock(m_mutexlock);
    return res;
}

void Mysql_Poll::ReleaseConnection(db_mysql_ptr conn)
{
    pthread_mutex_lock(m_mutexlock);
    mysql_list_.push_back(conn);
    pthread_mutex_unlock(m_mutexlock);
}

#if 0
int main()
{
    dbmysql_poll_ptr mysqlpoll(new Mysql_Poll("127.0.0.1", 3306, "user", "pass"));
    mysql_library_init(0, NULL, NULL);
    db_mysql_ptr m_ptr = mysqlpoll->GetConnection();
    if (m_ptr != NULL)
    {
        std::string query = "select * from trans_mgr";
        char retMsg[1024] = { '\0' };
        m_ptr->query_with_result((char *)query.c_str(), retMsg);
        printf("get %d line data!\n", m_ptr->get_rec_num());
        m_ptr->free_result();
        mysqlpoll->ReleaseConnection(m_ptr);
    }
    mysql_library_end();
    return 0;
}
#endif

