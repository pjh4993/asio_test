#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP
#include <string>
namespace http{
	namespace server{
		struct reply;
		struct request;
		class request_handler{
			public:
				request_handler(const request_handler&) = delete;
				request_handler& operator=(const request_handler&) = delete;
				explicit request_handler(const std::string& doc_root);
				void handle_request(const request& req, reply& rep);
			private:
				std::string doc_root_;
				static bool url_decode(const std::string& in, std::string& out);
		};
	}
}
#endif
