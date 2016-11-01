#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include <string>
#include <vector>
namespace http{
	namespace server{
		struct request{
            enum method_type{
                log_in,
                log_out,
                sign_in,
                get_user_gps,
                get_item_gps,
                send_user_gps
            }method;
            int body_length;
            std::string id, pwd, secret;
            float gps_long, gps_lat;
		};
	}
}
#endif
