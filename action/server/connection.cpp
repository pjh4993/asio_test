#include "connection.hpp"
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <ctutkernel/log.hpp>
#include "connection_manager.hpp"
#include "request_handler.hpp"
namespace http{
	namespace server{
		connection::connection(boost::asio::ip::tcp::socket socket_in,
				connection_manager& manager_in,request_handler& handler_in)
			:socket_(std::move(socket_in)),
			connection_manager_(manager_in),
			request_handler_(handler_in){
		}
		void connection::start(){
			do_read();
		}
		void connection::stop(){
			socket_.close();
		}
		void connection::do_read(){
			auto self(shared_from_this());
            dlog("connection::do_read","read more");
			socket_.async_read_some(boost::asio::buffer(buffer_),
					[this,self](boost::system::error_code ec, std::size_t bytes_transferred){
						if(!ec){
							dlog("connection::do_read","read_request");
							request_parser::result_type result;
							std::tie(result) = request_parser_.parse(
									request_, buffer_.data(), bytes_transferred);
							if(result == request_parser::good){
								dlog("connection::do_read","request_parser::good");
								request_handler_.handle_request(request_,reply_);
								do_write();
							}else if(result == request_parser::bad){
								dlog("connection::do_read","request_parser::bad");
								reply::stock_reply(reply::bad_request);
								do_write();
							}else{
								dlog("connection::do_read","request_parser::indeterminate");
								do_read();
							}
						}else if(ec!=boost::asio::error::operation_aborted){
							connection_manager_.stop(shared_from_this());
						}
					});
		}
		void connection::do_write(){
			auto self(shared_from_this());
			dlog("connection::do_write","sizeof reply_"+std::to_string(sizeof(reply_)));
            /*
			boost::asio::async_write(socket_,reply_.to_buffer(),
					[this,self](boost::system::error_code ec, std::size_t size){
						if(!ec){
							dlog("connection::do_write","normal reaction");
							boost::system::error_code ignored_ec;
							socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,ignored_ec);
						}
						if(ec != boost::asio::error::operation_aborted){
							dlog("connection::do_write","error caused ");
							connection_manager_.stop(shared_from_this());
						}
					});
                    */
		}
	}
}
