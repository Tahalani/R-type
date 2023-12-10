/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** socket
*/

#ifndef TCP_SERVER_HPP_
    #define TCP_SERVER_HPP_

    #include <asio.hpp>
    #include <iostream>
    #include <unordered_map>
    #include <functional>
    #include <memory>

class TCPServer {
    public:
        TCPServer(std::size_t port);
        ~TCPServer() = default;
        int createSocket();
        void run();
        void startAccept();
        void handleRead(std::shared_ptr<asio::ip::tcp::socket> client);
        void sendMessageToAllClients(const std::string &message);
        void sendMessageToAClient(const std::string &message, std::shared_ptr<asio::ip::tcp::socket> client);
        std::unordered_map<size_t, std::shared_ptr<asio::ip::tcp::socket>> _clientsInfo;

    private:
        std::size_t _port;
        asio::io_context _ioContext;
        asio::ip::tcp::endpoint _endpoint;
        asio::ip::tcp::acceptor _acceptor;
        asio::streambuf buffer;
        asio::error_code _ec;
    protected:
};

#endif /* !TCP_SERVER_HPP_ */
