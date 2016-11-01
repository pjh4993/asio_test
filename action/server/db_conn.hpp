#ifndef DB_CONN
#define DB_CONN
//for mysql connection
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
//for template Singleton
#include "singleton.hpp"
template<typename T>
std::mutex TemplateSingleton<T>::mtx;
namespace http{
    namespace server{
        class db_conn:public TemplateSingleton<db_conn>{
            public:
                db_conn(std::string ip,std::string port,std::string User,
                        std::string pwd){
                    driver = get_driver_instance();
                    std::string addr = "tcp://"+ip+":"+port;
                    conn = driver->connect(addr,User,pwd);
                }
                sql::ResultSet* setQuery(std::string schema,std::string query){
                    std::lock_guard<std::mutex> guard(mtx);
                    conn->setSchema(schema);
                    stmt = conn->createStatement();
                    return stmt->executeQuery(query);
                }
            private:
                sql::Driver *driver;
                sql::Connection *conn;
                sql::Statement *stmt;
        };
    }
}
#endif
