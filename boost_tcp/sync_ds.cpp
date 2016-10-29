#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
std::string make_daytime_string();
int main(void){
	try{
		boost::asio::io_service ios;
		tcp::acceptor acpt(ios,tcp::endpoint(tcp::v4(),13));
		for(;;){
			tcp::socket sock(ios);
			acpt.accept(sock);
			//accept a new connection from a peer into the given socket
			//function call will block until a new connection ohas been accepted succesfully or an error occurs
			std::string message = make_daytime_string();
			boost::system::error_code ignored_error;
			boost::asio::write(sock,boost::asio::buffer(message),ignored_error);
			//write a certain number of bytes of data to a stream
			//block until all of the data in buffers has beenen written or error occurred
		}
	}catch(std::exception& e){
			std::cerr << e.what() << std::endl;
	}
	return 0;
}
std::string make_daytime_string(){
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
