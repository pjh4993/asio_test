#include "log.hpp"
#include <ctime>
#include <cstring>
#include <string>
#include <iostream>
namespace ctutkernel{
	namespace util{
		void Log::debug(std::string tag_in, std::string text_in){
			time_t now = time(0);
			char* dt = ctime(&now);
			std::cout<< std::string(dt,strlen(dt)-1) << " | " << tag_in << " : " << text_in << std::endl;
		}
		void Log::verbose(std::string tag_in, std::string text_in){
			std::cerr << tag_in << " : " << text_in << std::endl;
		}
	}
}
