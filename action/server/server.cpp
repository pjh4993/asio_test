#include "server.hpp"
#include <ctutkernel/log.hpp>
#include <signal.h>
#include <utility>
namespace http{
	namespace server{
		server::server(const std::string& address, const std::string& port)
			:	ios(),
				signals_(ios),
				acceptor_(ios),
				connection_manager_(),
				socket_(ios),
				request_handler_(){
					signals_.add(SIGINT);
					signals_.add(SIGTERM);
					#if defined(SIGQUIT)
					signals_.add(SIGQUIT);
					#endif
					do_await_stop();
					boost::asio::ip::tcp::resolver res(ios);
					boost::asio::ip::tcp::endpoint enp = *res.resolve({address,port});
					acceptor_.open(enp.protocol());
					acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
					acceptor_.bind(enp);
					acceptor_.listen();
					do_accept();
				}
		void server::run(){
			ios.run();
		}
		void server::do_accept(){
			acceptor_.async_accept(socket_,
					[this](boost::system::error_code ec){
						if(!acceptor_.is_open()){
							return;
						}
						if(!ec){
							dlog("server::do_accept","acceptor_ is opened and accept new connection");
							connection_manager_.start(std::make_shared<connection>(
										std::move(socket_),connection_manager_,request_handler_));
						}
						do_accept();
					});
		}
		void server::do_await_stop(){
			signals_.async_wait(
					[this](boost::system::error_code ec, int signum){
                        switch(signum){
                            case SIGINT:
                                dlog("server::do_await_stop","SIGINT come");
                            break;
                            case SIGTERM:
                                dlog("server::do_await_stop","SIGTERM come");
                            break;
                            case SIGQUIT:
                                dlog("server::do_await_stop","SIGQUIT come");
                            break;
                        }
						acceptor_.close();
						connection_manager_.stop_all();
					});
		}
	}
}
