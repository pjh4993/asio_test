#include <iostream>
#include <cstdlib>
#include <boost/aligned_storage.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;
class Log{
	public:
#ifdef DEBUG 
		inline static void d(std::string tag,std::string log){
			std::cout << "[DEBUG - " << tag << " : " << log << "]" << std::endl;
		}
#endif
#ifdef INFO
		inline static void i(std::string tag,std::string log){
			std::cout << "[INFO - " <<tag << " : " << log << "]" << std::endl;
		}
#endif
		inline static void v(std::string tag,std::string log){
			std::cout << "[VERBOSE - " << tag << " : " << log << "]" << std::endl;
		}
};
//class to manage the memory to be used for handler-based custom allocation
//it contains a single block of memory which may be returned for allocation requst
//if the memory is in use when an allocation request is made, the allocator delegates allocation to global heap
class handler_allocator : private boost::noncopyable{
	public:
		handler_allocator()
			: in_use_(false){

			}
		void* allocate(std::size_t size){
			if(!in_use_ && size < storage_.size){
				in_use_ = true;
				Log::d("handler_allocator::allocate","use storage_.address()");
				return storage_.address();
			}else{
				Log::d("handler_allocator::allocate","use new operator");
				return ::operator new(size);
			}
		}
		void deallocate(void* ptr){
			if(ptr == storage_.address()){
				in_use_ = false;
			}else{
				::operator delete(ptr);
			}
		}
	private:
		//Whether the handler-based custom allcation storage has been used
		bool in_use_;
		//Storage space used for handler-based custom memory alloation
		boost::aligned_storage<128> storage_;
};
//Wrapper class template for handler objects to allow handler's memory
//allocation to be customized. Calls to operator() are forwarded to the encapsulated handler
template <typename Handler>
class custom_alloc_handler{
	public:
		custom_alloc_handler(handler_allocator& a,Handler h)
			: allocator_(a),
			handler_(h){
			Log::d("custom_alloc_handler::constructor", "construct alloc handler"); 
		}
		template <typename Arg1>
		void operator()(Arg1 arg1){
			handler_(arg1);
		}
		template <typename Arg1, typename Arg2>
		void operator()(Arg1 arg1, Arg2 arg2){
			handler_(arg1,arg2);
		}
		friend void* asio_handler_allocate(std::size_t size,
				custom_alloc_handler<Handler>* this_handler){
			Log::d("custom_alloc_handler::asio_handler_allocate", std::to_string(size)); 
			return this_handler->allocator_.allocate(size);
		}
		friend void asio_handler_deallocate(void* ptr, std::size_t size,
				custom_alloc_handler<Handler>* this_handler){
			Log::d("custom_alloc_handler::asio_handler_deallocate",std::to_string(size)); 
			this_handler->allocator_.deallocate(ptr);
		}
	private:
		handler_allocator& allocator_;
		Handler handler_;
};
template <typename Handler>
inline custom_alloc_handler<Handler> make_custom_alloc_handler(handler_allocator& a, Handler h){
	return custom_alloc_handler<Handler>(a,h);
}
class session : public boost::enable_shared_from_this<session>{
	public:
		session(boost::asio::io_service& ios): socket_(ios){
		}
		tcp::socket& socket(){
			return socket_;
		}
		void start(){
			Log::d("session::start","before async_read_some");
			socket_.async_read_some(boost::asio::buffer(data_),
					make_custom_alloc_handler(allocator_,
						boost::bind(&session::handle_read,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)));
			Log::d("session::start","after async_read_some");
		}
		void handle_read(const boost::system::error_code& error,
				size_t bytes_transferred){
			Log::d("session::start",std::string(data_.begin(),data_.end()));
			if(!error){
				Log::d("session::handle_read","before async_write");
				boost::asio::async_write(socket_,
						boost::asio::buffer(data_,bytes_transferred),
						make_custom_alloc_handler(allocator_,
							boost::bind(&session::handle_write,
								shared_from_this(),
								boost::asio::placeholders::error)));
				Log::d("session::handle_read","after async_write");
			}
		}
		void handle_write(const boost::system::error_code& error){
			if(!error){
				Log::d("session::handle_write","before async_read");
				socket_.async_read_some(boost::asio::buffer(data_),
						make_custom_alloc_handler(allocator_,
							boost::bind(&session::handle_read,
								shared_from_this(),
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred)));
				Log::d("session::handle_write","before async_write");
			}
		}
	private:
		tcp::socket socket_;
		boost::array<char,1024> data_;
		handler_allocator allocator_;
};
typedef boost::shared_ptr<session> session_ptr;
class server{
	public:
		server(boost::asio::io_service& ios, short port)
			:io_service_(ios),
			acceptor_(ios,tcp::endpoint(tcp::v4(),port)){
			session_ptr new_session(new session(io_service_));
			acceptor_.async_accept(new_session->socket(),
				boost::bind(&server::handle_accept,this,new_session,
					boost::asio::placeholders::error));
		}
		void handle_accept(session_ptr new_session,
			const boost::system::error_code& error){
			if(!error){
				new_session->start();
			}
			new_session.reset(new session(io_service_));
			acceptor_.async_accept(new_session->socket(),
				boost::bind(&server::handle_accept,this,new_session,
					boost::asio::placeholders::error));
		}
	private:
		boost::asio::io_service& io_service_;
		tcp::acceptor acceptor_;
};
int main(int argc, char* argv[]){
	try{
		if(argc !=2 ){
			std::cerr << "Usage : server <Port>" << std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		using namespace std;
		server s(ios,stoi(argv[1]));
		ios.run();
	}catch(std::exception& e){
		std::cerr<<"Exception " << e.what() << std::endl;
	}
	return 0;
}
