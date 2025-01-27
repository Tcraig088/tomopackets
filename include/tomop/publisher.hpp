#pragma once

#include <cstddef>
#include <string>
#include <zmq.hpp>
#include "exceptions.hpp"
#include "packets.hpp"

namespace tomop {

class publisher {
  public:
    publisher(std::string hostname = "localhost", int32_t port = 5555,
              int32_t push_port = 5556)
        : context_(1), rep_socket_(context_, ZMQ_REP), push_socket_(context_, ZMQ_PUSH) {
        using namespace std::string_literals;
        using namespace std::chrono_literals;

        rep_address_ = "tcp://"s + hostname + ":"s + std::to_string(port);
        push_address_ = "tcp://"s + hostname + ":"s + std::to_string(push_port);

        rep_socket_.setsockopt(ZMQ_LINGER, 200);
        rep_socket_.bind(rep_address_);

        push_socket_.setsockopt(ZMQ_LINGER, 200);
        push_socket_.connect(push_address_);
    }

    ~publisher() {
        rep_socket_.close();
        push_socket_.close();
        context_.close();
    }

    void send(const Packet& packet) {
        packet.send(push_socket_);
    }

    void listen() {
        auto listen_thread = std::thread([&] {
          while(true){
            zmq::message_t update;
            bool kill = false;
            rep_socket_.recv(&update);
            auto desc = ((packet_desc*)update.data())[0];
            auto buffer = memory_buffer(update.size(), (char*)update.data());
            
            switch (desc)
            {
            case packet_desc::microscope_connection:
              auto packet = std::make_unique<MicroscopeConnectionPacket>();
              scene_id_ = packet->scene_id;
              break;
            
            default:
              break;
            }

          }
        zmq::message_t request;
        rep_socket_.recv(&request);

        std::string request_str(static_cast<char*>(request.data()), request.size());
        if (request_str == "READY") {
            zmq::message_t reply(9);
            memcpy(reply.data(), "CONFIRMED", 9);
            rep_socket_.send(reply);
        } else {
            throw server_error("Unexpected request received");
        }
    }

  void set_microscope_settings(int32_t num_magnifications, int32_t num_detectors, int32_t num_frame_sizes, float pixel_size, std::array<std::string> detectors, std::array<float> magnifications, std::array<int32_t> frame_sizes) {
    num_magnifications_ = num_magnifications;
    num_detectors_ = num_detectors;
    num_frame_sizes_ = num_frame_sizes;
    pixel_size_ = pixel_size;
    detectors_ = detectors;
    magnifications_ = magnifications; 
  }

  void send_microscope_settings() {
    MicroscopeSpecificationsPacket packet(scene_id_, num_magnifications_, num_detectors_, num_frame_sizes_, pixel_size_, detectors_, magnifications_, frame_sizes_);
    packet.send(push_socket_);
  }

  
  private:
    // publisher connection
    zmq::context_t context_;
    zmq::socket_t rep_socket_;
    zmq::socket_t push_socket_;

    std::string rep_address_;
    std::string push_address_;

    int32_t scene_id_ = -1;
    int32_t num_magnifications_;
    int32_t num_detectors_;
    int32_t num_frame_sizes_;
    float pixel_size_;
    std::array<std::string> detectors_;
    std::array<float> magnifications_;
    std::array<int32_t> frame_sizes_;
};

} // namespace tomop
