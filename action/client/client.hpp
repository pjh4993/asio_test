#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <string>
#include <array>
#include "request.hpp"
#include "reply.hpp"

//#include "client_connection.hpp"
//#include "client_connection_manager.hpp"
//#include "client_request_handler.hpp"
using boost::asio::ip::tcp;
namespace http{
    namespace client{
        class client;
        class client : public std::enable_shared_from_this<client>{
            public:
                client(const client&) = delete;
                client& operator=(const client&) = delete;
                explicit client(boost::asio::io_service& io_service,const std::string &address, const std::string& port, request& req, reply& rep);
                void run();
                void close();
            private:
                void do_connect(tcp::resolver::iterator endpoint_iterator);
                void do_write();
                void do_read();
                void do_set_reply(char* data, int bytes_transferred,boost::function<void()> handler);
                boost::asio::io_service& ios;
                boost::asio::ip::tcp::socket socket_;
                request request_;
                reply reply_;
                std::array<char,1024> buffer_;
                boost::asio::strand reply_strand;
                //client_connection_manager connection_manager;
                //client_request_handler request_handler;
        };
    }
}
#endif
