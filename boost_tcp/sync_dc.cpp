#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class Log{
	public:
#ifdef DEBUG
		inline static void d(std::string tag, std::string log){
			std::cout << "[DEBUG - " << tag << " : " << log << "]" << std::endl;
		}
#endif
#ifdef INFO
		inline static void i(std::string tag, std::string log){
			std::cout << "[INFO - " << tag << " : " << log << "]" << std::endl;
		}
#endif
		inline static void v(std::string tag, std::string log){
			std::cout << "[VERBOSE - " << tag << " : " << log << "]" << std::endl;
		}

};
int main(int argc, char* argv[]){
	try{
		if(argc!=3){
			std::cerr << "Usage: client <host> <port>" << std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		std::string str;
		//provides core I/O functionality
		//socket, acceptor, deadline_timer
		//Using shared object between Thread is safe with specific exceptions of reset(), notify_fork()
		//calling reset() while unfinished run() ... results undefined behaviour
		//notify_fork() should not be called while any function of io_service is called in another Thread
		tcp::resolver res(ios);
		tcp::resolver::query que(argv[1],argv[2]);
		tcp::resolver::iterator endpoint_iterator = res.resolve(que);
		tcp::socket sock(ios);
		boost::asio::connect(sock,endpoint_iterator);
		//resolver perform forward resolution of a query to a list of entries
		//connect will connect to all entries untill connection success
		for(;;){
			boost::array<char,128> buf;
			boost::system::error_code err;
			std::getline(std::cin,str);
			Log::d("client::in_for","before write_some");
			sock.write_some(boost::asio::buffer(str),err);
			Log::d("client::in_for","after write_some");
			size_t len = sock.read_some(boost::asio::buffer(buf),err);
			//"socket.read_some" will block untill one or more bytes of data has been read successfully
			//if you need to ensure that the requested amount of data is read before blocking operation completes,
			//try to ues "boostt::asio::read" function
			//std::size_t read(SyncReadStream & s, const MutableBufferSequence & buffers)
			//Stream should support the SyncReadStream concept
			//MutableBufferSequence == boost::asio::buffer or std::vector
			if(err == boost::asio::error::eof)
				break;
			else if(err)
				throw boost::system::system_error(err);
			std::cout.write(buf.data(),len);
		}
		//in tcp connection
		//provides reliable ordered delivery of packets
		//Use error detection, retransmissions and acknowledgements
		//used in unicast
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
}
