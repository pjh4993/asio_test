#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP
#include <string>
#include <vector>
#include <boost/asio.hpp>
namespace http{
	namespace server{
		struct reply{
			enum status_type{
				ok = 200,
				created = 201,
				bad_request = 400,
				not_found = 404,
				service_unavailable = 503
			}status;
            std::string secret;
            std::vector<float> gps_long;
            std::vector<float> gps_lat;
			char* to_buffer(int size);
			static reply stock_reply(status_type status);
		};
	}
}
#endif
