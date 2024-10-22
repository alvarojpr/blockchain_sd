#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace std;

void server() {
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_service);
    acceptor.accept(socket);
    string message = "Block received!";
    boost::asio::write(socket, boost::asio::buffer(message));
}

void client() {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("localhost", "1234");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    string msg = "New block data";
    boost::asio::write(socket, boost::asio::buffer(msg));
}
