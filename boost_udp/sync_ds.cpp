#include <iostream>
#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
using boost::asio::ip::udp;
std::string make_daytime_string(){
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
int main(){
	try{
		boost::asio::io_service ios;
		udp::socket sock(ios,udp::endpoint(udp::v4(),13));	
		for(;;){
			boost::array<char,1> recv_buf;
			udp::endpoint remote_endpoint;
			boost::system::error_code error;
			std::cout << "Wait for receive_from" << std::endl;
			//blocking unti receive from endpoint
			sock.receive_from(boost::asio::buffer(recv_buf),
					remote_endpoint,0,error);
			std::cout << "Receive from remote_endpoint " << std::endl;
			std::string str = std::string(recv_buf.begin(),recv_buf.end());
			if(str=="A"){
				std::cout << "Termination Code "<< std::endl;
				//need Termination
			}
			if(error && error!=boost::asio::error::message_size)
				throw boost::system::system_error(error);
			std::string message = make_daytime_string();
			boost::system::error_code ignored_error;
			std::cout << "Wait for send_to" << std::endl;
			//block until sending is over
			sock.send_to(boost::asio::buffer(message),
					remote_endpoint,0,ignored_error);
			std::cout << "Send complete" << std::endl;
		}
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
