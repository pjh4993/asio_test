#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP
#include <tuple>
#include <ctutkernel/log.hpp>
#include <string>
#include <cstring>
#include "request.hpp"
namespace http{
	namespace server{
		struct request;
		class request_parser{
			public:
				request_parser();
				enum result_type{good,bad,indeterminate};
				std::tuple<result_type>parse(request& req,
							char* begin, int bytes_transferred){
                    memcpy(&req, begin, sizeof(char)*bytes_transferred);
                    if(req.body_length != bytes_transferred + sizeof(request::method_type)){
                        return std::make_tuple(indeterminate);
                    }else{
                        switch(req.method){
                            case request::sign_in:
                            case request::log_in:
                                if(req.id.empty() || req.pwd.empty())
                                    return std::make_tuple(bad);
                                break;
                            case request::log_out:
                            case request::get_user_gps:
                            case request::get_item_gps:
                                break;
                            case request::send_user_gps:
                                if(req.gps_long <= -90 || req.gps_long >= 90 || req.gps_lat <= -180 || req.gps_lat >= 180)
                                    return std::make_tuple(bad);
                                break;
                        }
                        return std::make_tuple(good);
                    }
                    return std::make_tuple(indeterminate);
				}
			private:
		};
	}
}
#endif
