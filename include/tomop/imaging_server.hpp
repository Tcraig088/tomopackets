#pragma once

#include <zmq.hpp>
#include <string>
#include <thread>
#include <atomic>

namespace tomop {

class imaging_server {
  public:
    imaging_server(std::string rep_address = "tcp://*:5555",
                   std::string pub_address = "tcp://*:5556")
        : context_(1), rep_socket_(context_, ZMQ_REP),
          pub_socket_(context_, ZMQ_PUB), running_(false) {
        rep_socket_.bind(rep_address);
        pub_socket_.bind(pub_address);
    }

    ~imaging_server() {
        stop();
        rep_socket_.close();
        pub_socket_.close();
        context_.close();
    }

    void start() {
        running_ = true;
        server_thread_ = std::thread(&imaging_server::run, this);
    }

    void stop() {
        running_ = false;
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
    }

  private:
    void run() {
        while (running_) {
            zmq::message_t request;
            rep_socket_.recv(&request);
            std::string reply_message = "Reply from server";
            zmq::message_t reply(reply_message.size());
            memcpy(reply.data(), reply_message.data(), reply_message.size());
            rep_socket_.send(reply);

            while (running_) {
                std::string update_message = "Update from server";
                zmq::message_t update(update_message.size());
                memcpy(update.data(), update_message.data(), update_message.size());
                pub_socket_.send(update);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    zmq::context_t context_;
    zmq::socket_t rep_socket_;
    zmq::socket_t pub_socket_;
    std::thread server_thread_;
    std::atomic<bool> running_;
};

} // namespace tomop