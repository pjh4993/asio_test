#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP
#include <string>
#include <vector>
#include <boost/asio.hpp>
namespace http{
	namespace client{
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
            //std::array<char,1024> to_buffer(int size);
            std::vector<boost::asio::const_buffer> to_buffer();
			static reply stock_reply(status_type status);
		};
	}
}
#endif
