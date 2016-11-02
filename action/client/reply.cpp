#include "reply.hpp"
#include <string>
#include <cstdlib>
#include <ctutkernel/log.hpp>
namespace http{
	namespace client{
        std::vector<boost::asio::const_buffer> reply::to_buffer(){
            /*
            std::array<char,1024> buffer_;
            dlog("reply::to_buffer","memcpy to buffer_");
            memcpy(buffer_.data(), this, size);
            */
            std::vector<boost::asio::const_buffer> buffer_;
            buffer_.push_back(boost::asio::buffer("hi"));
            return buffer_;
		}
        reply reply::stock_reply(reply::status_type status){
            dlog("reply::stock_reply","reply just status");
            reply rep;
            rep.status = status;
            return rep;
        }
	}
}
