#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP
#include <string>
#include "db_conn.hpp"
namespace http{
	namespace server{
		struct reply;
		struct request;
		class request_handler{
			public:
				request_handler(const request_handler&) = delete;
				request_handler& operator=(const request_handler&) = delete;
				explicit request_handler();
				void handle_request(const request& req, reply& rep);
            private:
                db_conn actionDb;
                std::string get_secret(std::string id, std::string pwd);
                std::string get_id(std::string secret);
                void insert_new_user(std::string id, std::string pwd);
		};
	}
}
#endif
