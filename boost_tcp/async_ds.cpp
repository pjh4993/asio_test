#include <iostream>
#include <ctime>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
std::string make_daytime_string(){
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
class tcp_connection: public boost::enable_shared_from_this<tcp_connection>{
	//enable_shared_from_this allows a shared_ptr or weak_ptr to the current object
	//to be obtained from within a memeber function
	//shared_ptr instance check strong/weak reference
	//cover memory leak problem
	//Circular references can occur so try to use weak_ptr
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;
		static pointer create(boost::asio::io_service& ios){
			return pointer(new tcp_connection(ios));
		}
		tcp::socket& socket(){
			return socket_;
		}
		//difference between async function and sync function
		//sync function block all the other sequence untill function end
		//async functino continue untill function end ( not blocking others )
		//so if one socket(stream) is being used by one async_write function 
		//program must ensure that the stream performs no other write operations until operation completes
		void start(){
			message_ = make_daytime_string();
			boost::asio::async_write(socket_,boost::asio::buffer(message_),
					boost::bind(&tcp_connection::handle_write, shared_from_this()));
		}
		//handler will be called when operation completes
		//boost::bind() support arbitrary function objects, functions, function pointers and memeber function pointers
		//bind any argument to a specific value or route input arguments into arbitrary positions.
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
		//when start accept function end, acceptor_ call handle_accept by handler
		//in handler it calls start_accept
		//Circular recursion
		void start_accept(){
			tcp_connection::pointer new_connection = 
				tcp_connection::create(acceptor_.get_io_service());
			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&tcp_server::handle_accept,this,new_connection,
						boost::asio::placeholders::error));
		}
		void handle_accept(tcp_connection::pointer new_connection,
				const boost::system::error_code& error){
			if(!error){
				std::cout << "New connection" << std::endl;
				new_connection->start();
			}else{
				std::cout << error.message() << std::endl;
			}
			start_accept();
		}
		tcp::acceptor acceptor_;
};
int main(void){
	boost::asio::io_service ios;
	tcp_server server(ios);
	try{
		ios.run();
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
