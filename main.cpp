#include <QtWidgets/QApplication>
#include "widget.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <ctime>

boost::array<char, 1000> buf;
size_t len;
std::string ipAddr = "127.0.0.1";

void receiveData()
{
    while(true){
        try{
            // socket receive
            boost::asio::io_service io;
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipAddr), 3200);
            boost::asio::ip::tcp::socket socket(io);
            socket.connect(endpoint);
            boost::system::error_code ec;
            len = socket.read_some( boost::asio::buffer(buf), ec);
            //std::string data(buf.data(), len);
            //std::cout << data << std::endl;
        }
        catch(std::exception& e){
            std::cerr << e.what() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    std::thread m_thread(receiveData);
    //m_thread.join();

    QApplication a(argc, argv);
    TimeWidget w;
    w.setData(&buf, &len);
    w.show();

    return a.exec();
}
