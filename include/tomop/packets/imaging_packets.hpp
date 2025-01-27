#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <boost/hana/define_struct.hpp>

using namespace std;

#include "../packets.hpp"

namespace tomop {


struct MicroscopeConnectionPacket
    : public PacketBase<MicroscopeConnectionPacket> {
    static const auto desc = packet_desc::microscope_connection;
    MicroscopeConnectionPacket() = default;
    MicroscopeConnectionPacket(int32_t a)
        : scene_id(a) {}
    BOOST_HANA_DEFINE_STRUCT(MicroscopeConnectionPacket, (int32_t, scene_id))
};

struct MicroscopeSpecificationsPacket
    : public PacketBase<MicroscopeSpecificationsPacket> {
    static const auto desc = packet_desc::microscope_specifications;
    MicroscopeSpecificationsPacket() = default;
    MicroscopeSpecificationsPacket(int32_t a, int32_t b, int32_t c, int32_t d, float e,
                                   std::vector<string> f, std::vector<float> g, std::vector<int32_t> h)
        : scene_id(a), num_magnifications(b), num_detectors(c), num_frame_sizes(d),
          pixel_size(e), detectors(f), magnifications(g), frame_sizes(h) {}
    BOOST_HANA_DEFINE_STRUCT(MicroscopeSpecificationsPacket, (int32_t, scene_id),
                             (int32_t, num_magnifications),
                             (int32_t, num_detectors),
                             (int32_t, num_frame_sizes),
                             (float, pixel_size),
                             (std::vector<string>, detectors),
                             (std::vector<float>, magnifications),
                             (std::vector<int32_t>, frame_sizes));
};

struct ImagingSpecificationsPacket
    : public PacketBase<ImagingSpecificationsPacket> {
    static const auto desc = packet_desc::imaging_specifications;
    ImagingSpecificationsPacket() = default;
    ImagingSpecificationsPacket(int32_t a, int32_t b, std::array<string, 10> c, float d,
                                int32_t e, float f, float g, int32_t h, float i)
        : scene_id(a), num_detectors(b), detectors(c), acquisition_dwell_time(d),
          acquisition_frame_size(e), acquisition_frame_time(f), scanning_dwell_time(g),
          scanning_frame_size(h), scanning_frame_time(i) {}
    BOOST_HANA_DEFINE_STRUCT(ImagingSpecificationsPacket, (int32_t, scene_id),
                             (int32_t, num_detectors),
                             (std::array<string, 10>, detectors),
                             (float, acquisition_dwell_time),
                             (int32_t, acquisition_frame_size),
                             (float, acquisition_frame_time),
                             (float, scanning_dwell_time),
                             (int32_t, scanning_frame_size),
                             (float, scanning_frame_time));
};

} // namespace tomop