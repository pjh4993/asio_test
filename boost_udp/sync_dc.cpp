#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::udp;
int main(int argc, char* argv[]){
	try{
		if(argc!=3){
			std::cerr << "Usage: client <host> <Signal>" << std::endl;
			return 1;
		}
		boost::asio::io_service ios;
		udp::resolver resolver(ios);
		udp::resolver::query que(udp::v4(),argv[1],"daytime");
		udp::endpoint receiver_endpoint = *resolver.resolve(que);
		udp::socket sock(ios);
		sock.open(udp::v4());
		boost::array<char, 1> send_buf = {{*(argv[2])}};
		sock.send_to(boost::asio::buffer(send_buf),receiver_endpoint);
		boost::array<char,128> recv_buf;
		udp::endpoint sender_endpoint;
		//will block until data has been received successsfully or an error occurs
		size_t len = sock.receive_from(boost::asio::buffer(recv_buf),sender_endpoint);
		std::cout.write(recv_buf.data(),len);
		//in udp connection
		//provides connectionless service
		//don't care if every packet is received
		//used in unicast, multicast, broadcast
	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
