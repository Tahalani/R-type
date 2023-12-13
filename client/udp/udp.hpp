/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** udp
*/

#ifndef UDP_HPP_
#define UDP_HPP_

#include <asio.hpp>
#include <iostream>
#include <array>
#include <functional>
#include <memory>
#include <any>
#include <typeindex>

struct Packet
{
    uint32_t magic_number;
    uint32_t entity_id;
    uint32_t type_index;
};

class UDPClient
{
public:
    UDPClient(std::size_t port, std::string ip);
    void start_receive();
    void handle_receive(const asio::error_code &error, std::size_t bytes_transferred);
    void handle_send(std::shared_ptr<std::string> message, const asio::error_code &error, std::size_t bytes_transferred);
    void send(std::string message, asio::ip::udp::endpoint endpoint);
    void run();
    std::string unpack(Packet &packet);

private:
    std::size_t _port;
    asio::ip::udp::endpoint _endpointServer;
    asio::io_context _io_context;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> _recv_buffer;
};

#endif /* !UDP_HPP_ */
