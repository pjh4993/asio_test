#include "request_handler.hpp"
#include <string>
#include <ctime>
#include "reply.hpp"
#include "request.hpp"
#include "db_conn.hpp"
#include <ctutkernel/log.hpp>
namespace http{
	namespace server{
		request_handler::request_handler()
            :actionDb("127.0.0.1","3306","root","jackthe7582@"){
		}
        std::string request_handler::get_secret(std::string id, std::string pwd){
            std::string query = "select secret from Users where id='"+id+"' && pwd='"+pwd+"';";
            sql::ResultSet *rs = actionDb.setQuery("action",query);
            return rs->getString("scr");
        }
        std::string request_handler::get_id(std::string secret){
            std::string query = "select uid from Users where scr='"+secret+"';";
            sql::ResultSet *rs = actionDb.setQuery("action",query);
            return std::to_string(rs->getInt("uid"));
        }
        void request_handler::insert_new_user(std::string id, std::string pwd){
            std::string scr = std::to_string(std::time(nullptr));
            std::string query = "insert into (id,pwd,scr)values('"+id+"', '"+pwd+"', '"+scr+");";
            actionDb.setQuery("action",query);
        }
		void request_handler::handle_request(const request& req ,reply& rep){
            std::string uid;
		    switch(req.method){
                case request::log_in:
                    rep.secret = get_secret(req.id, req.pwd);
                    rep.status = (req.secret.empty())? reply::bad_request : reply::ok;
                    break;
                case request::log_out:
                    uid = get_id(req.secret);
                    rep.status = (uid.empty())? reply::bad_request : reply::ok;
                    break;
                case request::sign_in:
                    rep.secret = get_secret(req.id, req.pwd);
                    if(req.secret.empty()){
                       insert_new_user(req.id, req.pwd); 
                    }
                    rep.status = (req.secret.empty())? reply::created : reply::bad_request;
                    break;
                case request::get_user_gps:
                    rep.secret = get_secret(req.id, req.pwd);
                    if(!req.secret.empty()){
                        //make gps_manager to make gps_user_vector
                    }
                    rep.status = (req.secret.empty())? reply::bad_request: reply::ok;
                    break;
                case request::get_item_gps:
                    rep.secret = get_secret(req.id, req.pwd);
                    if(!req.secret.empty()){
                        //make gps_manager to make gps_item_vector
                        //manage gps by secret
                    }
                    rep.status = (req.secret.empty())? reply::bad_request: reply::ok;
                    break;
                case request::send_user_gps:
                    uid = get_id(req.secret);
                    if(!uid.empty()){
                        //manage gps by secret
                    }
                    rep.status = (uid.empty())? reply::bad_request: reply::ok;
                    break;
            }
		}
    }
}
