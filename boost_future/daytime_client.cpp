#include <array>
#include <future>
#include <iostream>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/use_future.hpp>
using boost::asio::ip::udp;
void get_daytime(boost::asio::io_service& ios, const char* hostname){
	try{
		udp::resolver res(ios);
		std::future<udp::resolver::iterator> iter = 
			res.async_resolve(
					{udp::v4(),hostname,"daytime"},
					boost::asio::use_future);
		udp::socket sock(ios,udp::v4());
		std::array<char,1> send_buf = {{0}};
		std::future<std::size_t> send_length = 
			sock.async_send_to(boost::asio::buffer(send_buf),
					*iter.get(),boost::asio::use_future);
		send_length.get();
		std::array<char,128> recv_buf;
		udp::endpoint sender_endpoint;
		std::future<std::size_t> recv_length = 
			sock.async_receive_from(
					boost::asio::buffer(recv_buf),
					sender_endpoint,
					boost::asio::use_future);
		std::cout.write(recv_buf.data(),recv_length.get());
	}catch(std::system_error &e){
		std::cerr << e.what() << std::endl;
	}
}
int main(int argc, char* argv[]){
	try{
		if(argc!=2){
			std::cerr << "Usage : daytime_client <host>" << std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		boost::asio::io_service::work wrk(ios);
		std::thread thread([&ios](){ios.run();});
		get_daytime(ios,argv[1]);
		ios.stop();
		thread.join();
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
