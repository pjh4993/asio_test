#include <iostream>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include <atomic>
#include <thread>
#include <cstdlib>
#include <deque>
using boost::asio::ip::tcp;
class client{
	public:
		client(boost::asio::io_service& io_service_,
				tcp::resolver::iterator endpoint_iterator)
			:	ios(io_service_),
				socket_(io_service_){
			do_connect(endpoint_iterator);
		}
		void write(){

		}
		void close(){
			ios.post([this](){socket_.close();});
		}
	private:
		void do_connect(tcp::resolver::iterator endpoint_iterator){
			boost::asio::async_connect(socket_,endpoint_iterator,
					[this](boost::system::error_code ec, tcp::resolver::iterator it){
						if(!ec){
							if(!do_login()){
							}else{
							}
						}
					});
		}
		int do_login(){
			try{
				boost::system::error_code ec;
				std::string id,pwd;
				for(int i=0;i<3;i++){
					std::cout << "ID : " ;
					std::getline(std::cin,id);
					std::cout << "PWD: " ;
					std::getline(std::cin,pwd);
					std::string tmp_body = id+" "+pwd;
					packet_buf.write("a"+std::to_string(tmp_body.size()),tmp_body);
					packet_buf.print();
					boost::asio::write(socket_,boost::asio::buffer(packet_buf.data(),packet_buf.length()),ec);
					if(!ec&&packet_buf.decode_header()==Operation::Hash){
						boost::asio::read(socket_,boost::asio::buffer(packet_buf.body(),packet_buf.body_length()),ec);
						std::cout << std::string(packet_buf.body(),packet_buf.body_length());
						std::cout << "Do_read_room_list" << std::endl;
						return 0;
					}else if(packet_buf.decode_header()==Operation::Login_fail){
						std::cerr << "Login Fail! reLog" << std::endl;
						continue;
					}else if(ec){
						std::cerr << ec.message() << std::endl;	
						break;
					}
					//boost::asio::read(socket_,boost::asio::buffer(packet_buf.data(),packet_buf.body_length()),ec);
				}
				return 1;
			}catch(std::exception& e){
				std::cerr << e.what() << std::endl;
			}
		}
		boost::asio::io_service& ios;
		tcp::socket socket_;
};
void print(const boost::system::error_code& ec){
	std::cout<< "15Sec"<<std::endl;
}
int main(int argc, char* argv[]){
	try{
		if(argc!=3){
			std::cout << "Usage : client <host> <port>" << std::endl;
			return 1;
		}
		//get io_service and chat_client
		boost::asio::io_service ios;
		tcp::resolver resolver(ios);
		auto endpoint_iterator = resolver.resolve({argv[1], argv[2]});
		chat_client c(ios, endpoint_iterator);
		std::thread t([&ios](){ios.run();});
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				/*
		while(std::cin.getline(line,chat_message::max_body_length+1) && c.is_connected){
			chat_message msg;
			msg.body_length(std:strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}
		*/
		c.close();
		t.join();
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
