#include<iostream>
#include<thread>
#include<boost/asio.hpp>
#include<boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<memory>
#include<vector>
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace webs = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;
const int port_l=9080;
const int port_r=8080;

//apppi
class resv : public std::enable_shared_from_this<resv>
{
	asio::io_context& iocon;
	public:
		resv(asio::io_context ioc) :iocon(ioc) { connect(); }
	void connect()
	{
		
	}

};
//client handeler
// ------------------------------------------------------------------------------------------------------------------
//websocket class
class gserv : public std::enable_shared_from_this<gserv>
{
	beast::flat_buffer fb;
	webs::stream<beast::tcp_stream> sweb;
	int clno;
	public:
		gserv(int cln,tcp::socket&& ss) :clno(cln), sweb(std::move(ss)) {}
	
	//accepts the socket and upgrades its protocol from http to websocket server side ;)
		void waccept()
		{
			std::cout << "waiting for handshake req" << std::endl;
			sweb.async_accept([i{shared_from_this()}](boost::system::error_code ec) {
				if (ec) { std::cout << ec.message() << std::endl; return; }
				std::cout << "hand shake successfull !!"<< std::endl;
				i->red();
				});
		}
		void red()
		{
			std::cout << "read() loop for client :" << clno << std::endl;
			sweb.async_read(fb, [k{ shared_from_this() }](beast::error_code ec, std::size_t br) {
				if (ec) { std::cout << ec.message() << std::endl; return; }
				auto sss=beast::buffers_to_string(k->fb.cdata());
				std::cout << "bytes recived:"<< br << std::endl;
				std::cout<<"client id:" <<" :"<<k->clno << sss << std::endl;
				k->sweb.write(k->fb.data());
				k->fb.consume(k->fb.size());
				k->red();
				});
		}
};
//listens to the port and creates a websocket with the clients recursively
class lolo : public std::enable_shared_from_this<lolo>
{
	asio::io_context& iocon;
	tcp::acceptor acc;
	int soc;
	public:
		lolo(asio::io_context& aio, unsigned short int port, std::string addr) :iocon(aio), acc(iocon, { asio::ip::make_address(addr), port_l }) { soc = 0; }
	
	//accepts the socket connection request
		void accept() 
		{
			std::cout << "waiting to connect" << std::endl;
			acc.async_accept(iocon, [i{shared_from_this()}](boost::system::error_code ec,tcp::socket s) {
				//counts the nubmer of accpted conn
				//passes the data socket to establish a websocket connection
				std::shared_ptr<gserv> temp = std::make_shared<gserv>(i->soc,std::move(s));
				std::cout << "test::inside connection handeler and made a sharedptr object of gserv" << std::endl;
				std::cout << "called the read function on the data socket of client no:"<<(i->soc++) << std::endl;
				temp->waccept();
				std::cout <<"read loop finished " << std::endl;
				// async lambda function that first establishes a websocket then 
				//recursively listens for any incoming message and provides reply
				//std::async([&](){temp->waccept();});
				//std::cout << "connection accepted current clients:"<<cs.size() << std::endl;
				i->accept();
				});
		}
};
int main()
{
	asio::io_context ioc_clients{9};
	std::vector<std::thread> th;

/*	asio::deadline_timer tt(ioc, boost::posix_time::seconds(5));
	tt.async_wait([](boost::system::error_code ec) {std::cout << "timer fin" << std::endl; });
	for (int i = 1; i <= 15; ++i)
	{
		std::cout << i << std::endl;
	}
*/
	std::make_shared<lolo>(ioc_clients,port_l,"127.0.0.1")->accept();

	ioc_clients.run();
	return 0;
}
