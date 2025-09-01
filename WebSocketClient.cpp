#include "C:\libraries\boost_1_88_0\boost_1_88_0\libs\beast\example\common\root_certificates.hpp"

#include<iostream>
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<boost/asio/ssl.hpp>
#include<boost/beast/websocket/ssl.hpp>
#include<thread>
#include<memory>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ssl = boost::asio::ssl;
namespace web = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;

class conn : public std::enable_shared_from_this<conn>
{
	beast::flat_buffer bf;
	asio::io_context& io;
	web::stream<ssl::stream<beast::tcp_stream>> ws;
	tcp::resolver rs;
	const char* host = ""; //URL
	std::string host1 = "";//URL
	std::string port = "443";//Secured port

	public:
		conn(asio::io_context& ioc,ssl::context& slc) :rs(asio::make_strand(ioc)), io(ioc), ws{asio::make_strand(io),slc} { std::cout << "construtor called" << std::endl; }

	//connects and performs handshake to upgrade the protocol to websocket		
		void con()
		{
			if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host)) { return; }
			//ssl setup before connect

		//	if (! SSL_set_tlsext_host_name(ws.next_layer().native_handle(),host))
		//	{
		//	}
			std::cout << "con() is called" << std::endl;
			ws.next_layer().set_verify_callback(ssl::host_name_verification(host));
			rs.async_resolve(host, port, [i{shared_from_this()}](beast::error_code ec, tcp::resolver::results_type res) {
				//if error return
				if (ec)
				{
					std::cout << ec.message() << std::endl;
					return;
				}
			/*	std::cout << "size:" << (res.size()) << std::endl;
				for (auto a : res)
				{
					std::cout << a.endpoint() << std::endl;
					beast::get_lowest_layer(i->ws).connect(a);
				}
			*/

				//blocks until connected 
		 		auto aad=beast::get_lowest_layer(i->ws).connect(res);
				std::cout << "connected"<< aad << std::endl; 
				//ssl handshake	
				i->ws.next_layer().handshake(ssl::stream_base::client);

				std::cout << "SSL handshake completed" << std::endl;
				//websocket handshake
				
				std::string s = i->host1+ ':' +std::to_string(aad.port());

				i->ws.async_handshake(s, "" /*Domain*/,[i](boost::beast::error_code ec)
					{
						if (ec)
						{
							std::cout << "handshake error:" << ec.message() << std::endl;
							return;
						}
						std::cout << "handshake completed\n"<<"reading:" << std::endl;
						//recursive read function call
						i->red();
					});
				});
		}
	//asyncronous reads from the websocket
		void red()
		{
			std::cout << "read called" << std::endl;
			ws.async_read(bf, [k{ shared_from_this() }](beast::error_code ec, std::size_t br) {
				if (ec) { std::cout << "error" << ec.message() << std::endl; return; }
				std::cout << "read start" << std::endl;
				std::string sss = beast::buffers_to_string(k->bf.cdata());
				std::cout << "bytes recived:" << br << std::endl;
				std::cout << "data:" << " :" << sss << std::endl;
				k->bf.consume(k->bf.size());
				k->red();
				});
		}
};
int main()
{
	asio::io_context ioc;
	ssl::context slc(ssl::context::tls_client);
	slc.set_verify_mode(ssl::verify_none);
	load_root_certificates(slc);
	std::make_shared<conn>(ioc,slc)->con();
	ioc.run();
	return 0;
}
