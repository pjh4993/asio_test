#include <boost/asio.hpp>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>
using boost::asio::ip::tcp;
class shared_const_buffer{
	public:
		explicit shared_const_buffer(const std::string& data)
		: data_(new std::vector<char>(data.begin(),data.end())),
		buffer_(boost::asio::buffer(*data_)){
		}
		typedef const boost::asio::const_buffer* const_iterator;
		typedef boost::asio::const_buffer value_type;
		const boost::asio::const_buffer* begin() const{ return &buffer_; }
		const boost::asio::const_buffer* end() const {return &buffer_+1; }
	private:
		std::shared_ptr<std::vector<char>> data_;
		boost::asio::const_buffer buffer_;
};
class session
:public std::enable_shared_from_this<session>{
	public:
		session(tcp::socket socket)
		:socket_(std::move(socket)){

		}
		void start(){
			do_write();
		}
	private:
		void do_write(){
			std::time_t now = std::time(0);
			shared_const_buffer buffer (std::ctime(&now));
			auto self(shared_from_this());
			boost::asio::async_write(socket_,buffer,
					[this,self](boost::system::error_code ec, std::size_t len){
					});
		}
		tcp::socket socket_;
};
class server{
	public:
		server(boost::asio::io_service& ios,short port)
			:acceptor_(ios,tcp::endpoint(tcp::v4(),port)),
			socket_(ios){
				do_accept();
			}
	private:
		void do_accept(){
			acceptor_.async_accept(socket_,
					[this](boost::system::error_code ec){
						if(!ec){
							std::make_shared<session>(std::move(socket_))->start();
						}
						do_accept();
					});
		}
		tcp::acceptor acceptor_;
		tcp::socket socket_;
};
int main(int argc, char* argv[]){
	if(argc !=2 ){
		std::cerr << "Usage: reference_counted <port> " << std::endl;
		return 1;
	}
	try{
		boost::asio::io_service ios;
		server s(ios,std::atoi(argv[1]));
		ios.run();
	}catch(std::exception& e){
		std::cerr << "Exception : " << e.what() << std::endl;
	}
	return 0;
}
