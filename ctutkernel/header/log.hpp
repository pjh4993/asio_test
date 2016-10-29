#ifndef MY_LOG_HPP
#define MY_LOG_HPP
#define vlog(x,y) ctutkernel::util::Log::verbose(x,y)
#define dlog(x,y) ctutkernel::util::Log::debug(x,y)
#include <iostream>
#include <string>
namespace ctutkernel{
	namespace util{
		class Log{
			public:
				static void verbose(std::string tag_in, std::string text_in);
				static void debug(std::string tag_in, std::string text_in);
		};
	}
}
#endif
