#include <iostream>
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <ctime>
using boost::asio::ip::tcp;
using boost::asio::ip::udp;
std::string make_daytime_string(){
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
class tcp_connection: public boost::enable_shared_from_this<tcp_connection>{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;
		static pointer create(boost::asio::io_service& ios){
			return pointer(new tcp_connection(ios));
		}
		tcp::socket& socket(){
			return socket_;
		}
		void start(){
			message_ = make_daytime_string();
			boost::asio::async_write(socket_,boost::asio::buffer(message_),
					boost::bind(&tcp_connection::handle_write,shared_from_this()));
		}
	private:
		tcp_connection(boost::asio::io_service& ios)
			:socket_(ios){
		}
		void handle_write(){
		}
		tcp::socket socket_;
		std::string message_;
};
class tcp_server{
	public:
		tcp_server(boost::asio::io_service& ios)
			: acceptor_(ios,tcp::endpoint(tcp::v4(),13)){
			start_accept();	
		}
	private:
		void start_accept(){
			tcp_connection::pointer new_connection =
				tcp_connection::create(acceptor_.get_io_service());
			acceptor_.async_accept(new_connection->socket(),
				boost::bind(&tcp_server::handle_accept, this, new_connection,
					boost::asio::placeholders::error));
		}
		void handle_accept(tcp_connection::pointer new_connection,
			const boost::system::error_code& error){
			if(!error){
				new_connection->start();
			}
			start_accept();
		}
		tcp::acceptor acceptor_;
};
class udp_server{
	public:
		udp_server(boost::asio::io_service& ios)
			: socket_(ios,udp::endpoint(udp::v4(),13)){
			start_receive();
		}
	private:
		void start_receive(){
			socket_.async_receive_from(
				boost::asio::buffer(recv_buffer),remote_endpoint,
				boost::bind(&udp_server::handle_receive,this,
					boost::asio::placeholders::error));
		}
		void handle_receive(const boost::system::error_code& error){
			if(!error||error == boost::asio::error::message_size){
				boost::shared_ptr<std::string> message(
					new std::string(make_daytime_string()));
				socket_.async_send_to(boost::asio::buffer(*message),remote_endpoint,
					boost::bind(&udp_server::handle_send,this,message));
				start_receive();
			}
		}
		void handle_send(boost::shared_ptr<std::string> message){
		}
		udp::socket socket_;
		udp::endpoint remote_endpoint;
		boost::array<char,1> recv_buffer;
};
int main(void){
	try{
		boost::asio::io_service ios;
		tcp_server server1(ios);
		udp_server server2(ios);
		ios.run();
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
