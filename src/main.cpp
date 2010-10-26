#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <pion/net/WebServer.hpp>
#include <ShutdownManager.hpp>
#include <EchoService.hpp>
#include <sstream>

using namespace std;
using namespace pion;
using namespace pion::net;


/// simple TCP server that just sends "Hello there!" to each connection
class HelloServer : public TCPServer {
public:
    HelloServer(const unsigned int tcp_port) : TCPServer(tcp_port) {}
    virtual ~HelloServer() {}
    virtual void handleConnection(TCPConnectionPtr& tcp_conn)
    {
        static const std::string HELLO_MESSAGE("Hello there!\x0D\x0A");
        tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);	// make sure it will get closed
        tcp_conn->async_write(boost::asio::buffer(HELLO_MESSAGE),
							  boost::bind(&TCPConnection::finish, tcp_conn));
    }
};


int main(int argc, char **argv)
{
std::string output;
#if defined(BOOST_ASIO_HAS_IOCP)
  output = "iocp" ;
#elif defined(BOOST_ASIO_HAS_EPOLL)
  output = "epoll" ;
#elif defined(BOOST_ASIO_HAS_KQUEUE)
  output = "kqueue" ;
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
  output = "/dev/poll" ;
#else
  output = "select" ;
#endif
    std::cout << output << std::endl;

#ifdef PION_HAVE_SSL 
        std::cout << "HAVE_SSL" << std::endl; 
#else 
        std::cout << "DO NOT HAVE_SSL" << std::endl; 
#endif
   std::cout << PION_VERSION << std::endl;


    static const unsigned int DEFAULT_PORT = 8080;

    // initialize log system (use simple configuration)
    PionLogger main_log(PION_GET_LOGGER("PionHelloServer"));
    PionLogger pion_log(PION_GET_LOGGER("pion"));
    PION_LOG_SETLEVEL_INFO(main_log);
    PION_LOG_SETLEVEL_INFO(pion_log);
    PION_LOG_CONFIG_BASIC;

    try {
        size_t numThreads;
        istringstream stream(argv[1]);
        stream >> numThreads;
        cout << "Number of threads: " << numThreads << endl;

        PionOneToOneScheduler scheduler;
        scheduler.setNumThreads(numThreads);

        WebServer web_server(scheduler, DEFAULT_PORT);
        plugins::EchoService echo_service;
        web_server.addService("/", dynamic_cast<WebService *>(&echo_service));
        web_server.start();
        main_shutdown_manager.wait();
    } catch (std::exception& e) {
        PION_LOG_FATAL(main_log, e.what());
    }

    cout << "Hello" << endl;
}
