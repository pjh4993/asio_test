#include "client.hpp"
#include "request.hpp"
#include "reply.hpp"
#include <cstdlib>
#include <future>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
using boost::asio::ip::tcp;
namespace http{
    namespace client{
        client::client(boost::asio::io_service &io_service, const std::string& address, const std::string& port, request& req, reply& rep)
            :   ios(io_service),
                socket_(ios),
                request_(req),
                reply_(rep),
                reply_strand(ios){
                    boost::asio::ip::tcp::resolver res(ios);
                    auto endpoint_iterator = res.resolve({address, port});
                    do_connect(endpoint_iterator);
                    do_read();
                };
        void client::do_connect(tcp::resolver::iterator endpoint_iterator){
            auto self(shared_from_this());
            boost::asio::async_connect(socket_,endpoint_iterator, 
                [this,self](boost::system::error_code ec, tcp::resolver::iterator it){
                    if(!ec){
                        do_write();
                        }
                    });

        }
        void client::run(){
            ios.run();
        }
        void client::close(){
            ios.post([this](){socket_.close();});
        }
        void client::do_read(){
            typedef std::promise<int> promise_type;
            promise_type promise;
            auto self(shared_from_this());
            void (promise_type::*setter)(const int&) = &promise_type::set_value;
            boost::asio::async_read(socket_, boost::asio::buffer(buffer_),
                    [this,&promise,self](boost::system::error_code ec, std::size_t bytes_transferred){
                        if(!ec){
                            reply_strand.post(bind(&client::do_set_reply, this, buffer_.data(), bytes_transferred,
                                        boost::bind(&promise_type::set_value, &promise)));
                            boost::system::error_code ignored_ec;
                            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,ignored_ec);
                        }
                        if(ec != boost::asio::error::operation_aborted){
                            close();
                        }
                });
            promise.get_future().wait();
        }
        void client::do_set_reply(char* data,int bytes_transferred , boost::function<void()> handler){
            memcpy(&reply_, data, bytes_transferred);
            handler();
        }
        void client::do_write(){
            auto self(shared_from_this());
            int size = sizeof(request_);
            char* data = (char*) malloc(size);
            boost::asio::async_write(socket_, boost::asio::buffer(data,size),
                    [this,self](boost::system::error_code ec, std::size_t size){
                        if(!ec){
                        }
                        if(ec != boost::asio::error::operation_aborted){
                            close();
                        }
                    });
        }
    }
}
