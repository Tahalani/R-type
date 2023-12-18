/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** udp
*/

#include <typeindex>
#include <random>
#include <unordered_map>

struct Position2 {
    float x;
    float y;
};

bool operator==(const Packet &lhs, const Packet &rhs)
{
    return lhs.magic_number == rhs.magic_number
            && lhs.packet_type == rhs.packet_type
            && lhs.timestamp == rhs.timestamp
            && lhs.entity_id == rhs.entity_id
            && lhs.type_index == rhs.type_index;
}

UDPServer::UDPServer(std::size_t port, std::string ip, registry &reg)
    : socket_(_io_context, asio::ip::udp::endpoint(asio::ip::make_address(ip), port)), _magic_number(4242), reg(reg)
{
    this->_port = socket_.local_endpoint().port();
    _thread = std::thread(&UDPServer::run, this);
    start_receive();
}

UDPServer::~UDPServer()
{
    std::cout << "Server is closing" << std::endl;
    // _thread.detach();
    _thread.join();
}

void UDPServer::start_receive()
{
    socket_.async_receive_from(
        asio::buffer(_recv_buffer), remote_endpoint_,
        std::bind(&UDPServer::handle_receive, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
}

std::unordered_map<uint32_t, std::type_index> _typeIndex = {
    {2, typeid(Position2)}};

template <typename T>
std::string UDPServer::pack(const T &component, uint32_t entity_id, PacketType packet_type)
{
    std::type_index targetType = typeid(T);
    int type_index = 2;

    for (const auto &entry : _typeIndex) {
        if (entry.second == targetType) {
            type_index = entry.first;
            break;
        }
    }
    if ((type_index) == -1) {
        std::cerr << "ERROR: type_index not found message not send" << std::endl;
        return "";
    } else {
        std::array<char, 37> uuid = generate_uuid();
        std::cout << "UUID SERVER du packet: " << uuid.data() << std::endl;
        Packet packet = {_magic_number, packet_type, std::time(nullptr), entity_id, static_cast<u_int32_t>(type_index), uuid};
        try {
            return std::string(reinterpret_cast<char *>(&packet),
            sizeof(packet)) + std::string(reinterpret_cast<const char *>(&component),
            sizeof(component));
        } catch (const std::exception &e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return "";
        }
    }
}

std::array<char, 37> UDPServer::generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::array<char, 37> uuid;
    std::string hexChars = "0123456789abcdef";


    for (int i = 0; i < 37; i++) {
        uuid[i] = hexChars[dis(gen)];
    }
    return uuid;
}

void UDPServer::handle_receive(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        std::cout << "bytes transferred to serv: " << bytes_transferred << std::endl;
        Packet receivedPacket;
        std::string receivedComponent = unpack(receivedPacket, _recv_buffer);
        std::cout << sizeof(receivedComponent) << std::endl;
        if (receivedComponent.empty()) {
            std::cout << "ERROR: receivedComponent empty" << std::endl;
            start_receive();
            return;
        }
        if (receivedPacket.packet_type == NEW_CONNECTION) {
            _clientsUDP[receivedPacket.entity_id] = remote_endpoint_;
            std::cout << "New player connected from " << remote_endpoint_.address() << " " << remote_endpoint_.port() << std::endl;
            sendToAll(receivedPacket, 0, NEW_CONNECTION);
            start_receive();
            return;
        }
        if (receivedPacket.magic_number != _magic_number) {
            std::cout << "ERROR: magic number not valid in received packet" << std::endl;
            start_receive();
            return;
        }
        if (receivedPacket.packet_type == DATA_PACKET) {
            Position2 pos = {12, 12};
            std::cout << "NEW Position2 pos.x: " << pos.x << " pos.y: " << pos.y << std::endl;

            auto type = reg._typeIndex[receivedPacket.type_index];
            std::cout << "type: " << type.name() << std::endl;
            
            // reg.registerPacket(receivedPacket.entity_id, receivedComponent.c_str(), type);
            // sendToAll(pos, receivedPacket.entity_id, DATA_PACKET);
            start_receive();
            return;
        }
        if (receivedPacket.packet_type == RESPONSE_PACKET) {
            for (const auto &query : _queries) {
                Packet queryPacket;
                std::memcpy(&queryPacket, query.second.data(), sizeof(Packet));
                if (receivedPacket.uuid == queryPacket.uuid) {
                    std::cout << "Query found" << std::endl;
                    // mtx.lock();
                    // _queries.erase(std::remove(_queries.begin(), _queries.end(), query), _queries.end());
                    // mtx.unlock();
                }
            }
        }
        start_receive();
    }
}

void UDPServer::run()
{
    std::cout << "Server is running on port " << this->_port << std::endl;
    this->_io_context.run();
}

void UDPServer::response(std::string message)
{
    std::shared_ptr<std::string> message2(new std::string(message));
    socket_.async_send_to(asio::buffer(*message2), remote_endpoint_,
                          std::bind(&UDPServer::handle_send, this, message2,
                                    std::placeholders::_1,
                                    std::placeholders::_2));
    std::cout << remote_endpoint_.address() << " " << remote_endpoint_.port() << std::endl;
}

void UDPServer::send(std::string message, asio::ip::udp::endpoint endpoint)
{
    try {
        socket_.send_to(asio::buffer(message), endpoint);
    } catch (const asio::system_error &ec) {
        std::cerr << "ERROR UDP sending message" << ec.what() << std::endl;
    }
}

template <typename T>
void UDPServer::sendToAll(const T &component, uint32_t entity_id, PacketType packet_type)
{
    std::string data = pack(component, entity_id, packet_type);

    if (data.empty())
        return;
    try {
        for (const auto &client : _clientsUDP) {
            std::cout << "Message sent to client UDP: " << client.first << std::endl;
            socket_.send_to(asio::buffer(data), client.second);
            if (packet_type == DATA_PACKET) {
                data[4] = RESPONSE_PACKET;
                mtx.lock();
                _queries.push_back(std::make_pair(client.second, data));
                mtx.unlock();
            }
        }
    } catch (const asio::system_error &ec) {
        std::cerr << "ERROR UDP sending message" << ec.what() << std::endl;
    }
}

std::string UDPServer::unpack(Packet &packet, std::array<uint8_t, 1024> query)
{
    try {
        std::memcpy(&packet, query.data(), sizeof(Packet));
        std::string component(query.data() + sizeof(Packet), query.data() + sizeof(Packet) + sizeof(component));
        return component;
    } catch (const std::exception &e) {
        std::cerr << "ERROR unpack: " << e.what() << std::endl;
        return nullptr;
    }
}

void UDPServer::handle_send(std::shared_ptr<std::string> message, const asio::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Message envoyé" << std::endl;
}

size_t UDPServer::getPort() const
{
    return this->_port;
}
