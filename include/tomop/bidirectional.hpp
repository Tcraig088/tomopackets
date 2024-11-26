#pragma once

#include <string>
#include <zmq.hpp>
#include "packets.hpp"

namespace tomop {

class BidirectionalMessenger {
  public:
    BidirectionalMessenger(std::string hostname = "localhost", int32_t req_port = 5555, int32_t rep_port = 5556)
        : context_(1), req_socket_(context_, ZMQ_REQ), rep_socket_(context_, ZMQ_REP) {
        using namespace std::string_literals;

        // Set up the REQ socket
        req_address_ = "tcp://"s + hostname + ":"s + std::to_string(req_port);
        req_socket_.setsockopt(ZMQ_LINGER, 200);
        req_socket_.connect(req_address_);

        // Set up the REP socket
        rep_address_ = "tcp://"s + hostname + ":"s + std::to_string(rep_port);
        rep_socket_.setsockopt(ZMQ_LINGER, 200);
        rep_socket_.bind(rep_address_);
    }

    ~BidirectionalMessenger() {
        req_socket_.close();
        rep_socket_.close();
        context_.close();
    }

    void send_request(const Packet& packet) {
        packet.send(req_socket_);
        zmq::message_t reply;
        req_socket_.recv(&reply);
        process_reply(reply);
    }

    void receive_request() {
        zmq::message_t request;
        rep_socket_.recv(&request);
        process_request(request);
        zmq::message_t reply("Reply from server");
        rep_socket_.send(reply);
    }

  private:
    void process_request(const zmq::message_t& request) {
        // Process the received request
        std::cout << "Received request: " << request.to_string() << std::endl;
        // Add your request processing logic here
    }

    void process_reply(const zmq::message_t& reply) {
        // Process the received reply
        std::cout << "Received reply: " << reply.to_string() << std::endl;
        // Add your reply processing logic here
    }

    zmq::context_t context_;
    zmq::socket_t req_socket_;
    zmq::socket_t rep_socket_;
    std::string req_address_;
    std::string rep_address_;
};

} // namespace tomop