#include <ctime>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
using boost::asio::ip::udp;
std::string make_daytime_string(){
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
class udp_server{
	public:
		udp_server(boost::asio::io_service& ios)
			:socket_(ios,udp::endpoint(udp::v4(),13)){
			start_receive();
		}
	private:
		//start_receive call handle_receive
		//handle_receive call handle_send
		void start_receive(){
			socket_.async_receive_from(
					boost::asio::buffer(recv_buffer),remote_endpoint,
					boost::bind(&udp_server::handle_receive,this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}
		void handle_receive(const boost::system::error_code& error,
				std::size_t bytes_transferr){
			if(!error || error==boost::asio::error::message_size){
				boost::shared_ptr<std::string> message(
						new std::string(make_daytime_string()));
				socket_.async_send_to(boost::asio::buffer(*message),remote_endpoint,
					boost::bind(&udp_server::handle_send,this,message,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				start_receive();
			}
		}
		void handle_send(boost::shared_ptr<std::string> message_ptr,
			const boost::system::error_code& err,
			std::size_t bytes){
		}
		udp::socket socket_;
		udp::endpoint remote_endpoint;
		boost::array<char,1> recv_buffer;
};
int main(void){
	try{
		boost::asio::io_service ios;
		udp_server server(ios);
		ios.run();
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
