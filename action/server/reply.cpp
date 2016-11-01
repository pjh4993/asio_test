#include "reply.hpp"
#include <string>
#include <cstdlib>
namespace http{
	namespace server{
	    char* reply::to_buffer(int size){
            char* buffer_ = (char*)malloc(size*sizeof(char));
            memcpy(buffer_, this, size*sizeof(char));
			return buffer_;
		}
        reply reply::stock_reply(reply::status_type status){
            reply rep;
            rep.status = status;
            return rep;
        }
	}
}
