#include <cstdlib>
#include <deque>
#include <atomic>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include "chat_message.hpp"
using boost::asio::ip::tcp;
typedef std::deque<chat_message> chat_message_queue;
class chat_client{
	public:
		std::atomic<bool> socket_closed;
		chat_client(boost::asio::io_service& ios,
				tcp::resolver::iterator endpoint_iterator)
			:	io_service_(ios),
				socket_(ios),
				socket_closed(false){
				do_connect(endpoint_iterator);	
			}
		void write(const chat_message& msg){
			io_service_.post(
					[this,msg](){
						bool write_in_progress = !write_msgs_.empty();
						write_msgs_.push_back(msg);
						if(!write_in_progress){
							do_write();
						}
					});
		}
		void close(){
			io_service_.post([this](){socket_.close();});
		}
	private:
		void do_connect(tcp::resolver::iterator endpoint_iterator){
			boost::asio::async_connect(socket_,endpoint_iterator,
					[this](boost::system::error_code ec, tcp::resolver::iterator){
						if(!ec){
							std::cout << "Connected" << std::endl;
							do_read_header();
						}
					});
		}
		void do_read_header(){
			boost::asio::async_read(socket_,
					boost::asio::buffer(read_msg_.data(),chat_message::header_length),
					[this](boost::system::error_code ec,std::size_t len){
						std::cout << "Read MSG header" << std::endl;
						if(!ec && read_msg_.decode_header()){
							std::cout << "Read body" << std::endl;
							do_read_body();
						}else{
							std::cout << read_msg_.decode_header() << "Close socket" << std::endl;
							std::cout << ec.message() << std::endl;
							socket_.close();
							socket_closed = true;
						}
					});
		}
		void do_read_body(){
			boost::asio::async_read(socket_,
					boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
					[this](boost::system::error_code ec, std::size_t len){
						std::cout << "Read Msg" << std::endl;
						if(!ec){
							std::cout.write(read_msg_.body(),read_msg_.body_length());
							std::cout << std::endl;
							do_read_header();
						}else{
							socket_.close();
							socket_closed = true;
						}
					});
		}
		void do_write(){
			boost::asio::async_write(socket_,
					boost::asio::buffer(write_msgs_.front().data(),
						write_msgs_.front().length()),
					[this](boost::system::error_code ec, std::size_t len){
						if(!ec){
							std::cout << "Write MSG" << std::endl;
							write_msgs_.pop_front();
							if(!write_msgs_.empty()){
								do_write();
							}
							else{
								socket_.close();
								socket_closed = true;
							}
						}
					});
		}
		boost::asio::io_service& io_service_;
		tcp::socket socket_;
		chat_message read_msg_;
		chat_message_queue write_msgs_;
};
int main(int argc, char* argv[]){
	try{
		if(argc!=3){
			std::cerr << "Usage : chat_client <host> <port> " << std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		tcp::resolver resolver_(ios);
		auto endpoint_iterator = resolver_.resolve({argv[1],argv[2]});
		chat_client c (ios,endpoint_iterator);
		std::thread t([&ios](){ios.run();});
		char line[chat_message::max_body_length+1];
		while(std::cin.getline(line,chat_message::max_body_length+1)&&c.socket_closed == false){
			std::cout <<"ME: " << line << std::endl;
			chat_message msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(),line,msg.body_length());
			c.write(msg);
		}
		c.close();
		t.join();
	}catch(std::exception& e){
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}
