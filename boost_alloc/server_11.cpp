#include <iostream>
#include <array>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class handler_allocator{
	public:
		handler_allocator()
			: in_use_(false){
		}
		handler_allocator(const handler_allocator&) = delete;
		handler_allocator& operator=(const handler_allocator&) = delete;
		void* allocate(std::size_t size){
			if(!in_use_ && size < sizeof(storage_)){
				in_use_ = true;
				return &storage_;
			}else{
				return ::operator new(size);
			}
		}
		void deallocate(void* pointer){
			if(pointer == &storage_){
				in_use_ = false;
			}else{
				::operator delete(pointer);
			}
		}
	private:
		typename std::aligned_storage<1024>::type storage_;
		bool in_use_;
};
template <typename Handler>
class custom_alloc_handler{
	public:
		custom_alloc_handler(handler_allocator& a, Handler h)
			: allocator_(a),
			  handler_(h){
		}
		template <typename ...Args>
		void operator()(Args&&... args){
			handler_(std::forward<Args>(args)...);
		}
		friend void* asio_handler_allocate(std::size_t size,
				custom_alloc_handler<Handler>* this_handler){
			return this_handler->allocator_.allocate(size);
		}
	private:
		handler_allocator& allocator_;
		Handler handler_;
};
template <typename Handler>
inline custom_alloc_handler<Handler> make_custom_alloc_handler(
		handler_allocator& a, Handler h){
	return custom_alloc_handler<Handler>(a,h);
}
class session
	: public std::enable_shared_from_this<session>
{
	public:
		session(tcp::socket socket)
			:socket_(std::move(socket)){
		}
		void start(){
			do_read();
		}
	private:
		void do_read(){
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(data_),
					make_custom_alloc_handler(allocator_,
						[this,self](boost::system::error_code ec, std::size_t length){
							if(!ec){
								do_write(length);
							}
						}));
		}
		void do_write(std::size_t length){
			auto self(shared_from_this());
			boost::asio::async_write(socket_,boost::asio::buffer(data_,length),
					make_custom_alloc_handler(allocator_,
						[this,self](boost::system::error_code ec, std::size_t size){
							if(!ec){
								do_read();
							}
						}));
		}
		tcp::socket socket_;
		std::array<char,1024> data_;
		handler_allocator allocator_;
};
class server{
	public:
		server(boost::asio::io_service& ios, short port)
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
	if(argc!=2){
		std::cerr << "Usage : server <port> " << std::endl;
		return 1;
	}
	try{
		boost::asio::io_service ios;
		server s(ios,std::atoi(argv[1]));
		ios.run();
	}catch(std::exception& e){
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}
