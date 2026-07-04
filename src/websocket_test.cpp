#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

int main() {
    std::cout << "Setting up WebSocket connection..." << std::endl;

    std::string host="stream.binance.com";
    std::string port="9443";
    std:: string target ="/ws/btcusd@depth5@100ms";

    net::io_context ioc;

    //resolv hostname to ip
    tcp::resolver resolver(ioc);
    auto const results= resolver.resolve(host,port);
    std::cout<<"Hostname resolved successfully"<<std::endl;

    //create TCP socket and connect
    beast::tcp_stream stream(ioc);
    stream.connect(results);
    std::cout<<"TCP connection established"<<std::endl;


    //SSL context setup
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();


    //wrap TCP with SSL
    beast::ssl_stream<beast::tcp_stream>ssl_stream(std::move(stream),ctx);

    //SNI(SERVER NAME INDICATOR)-required by many servers
    if(!SSL_set_tlsext_host_name(ssl_stream.native_handle(),host.c_str())){
        std::cout<<"Failed to set SNI hostname"<<std::endl;
        return 1;
    }

    //perform SSL handshake
    ssl_stream.handshake(ssl::stream_base::client);
    
    std::cout<<"SSL handshake successfull"<<std::endl;

    // create WebSocket stream on top of SSL stream
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws(std::move(ssl_stream));
    
    // perform WebSocket handshake
    ws.handshake(host, target);
    
    std::cout << "WebSocket handshake successful" << std::endl;

    //read message in a loop
    beast::flat_buffer buffer;
    
    for(int i=0;i<5;i++){
        //read one message
        ws.read(buffer);

        //convert to string and print
        std::string message=beast::buffers_to_string(buffer.data());
        std::cout<<"Message"<<i+1<<": "<<message<<std::endl;

        //clear buffer for next message
        buffer.consume(buffer.size());
    }

    //close connection cleanly
    ws.close(websocket::close_code::normal);
    std::cout<<"connection closed"<<std::endl;

    
    return 0;
}