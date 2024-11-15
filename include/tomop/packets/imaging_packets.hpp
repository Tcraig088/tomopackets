#pragma once

#include <cstdint>

#include "../packets.hpp"

namespace tomop {

struct MicroscopeSpecificationsPacket
    : public PacketBase<MicroscopeSpecificationsPacket> {
    static const auto desc = packet_desc::microscope_scpecifications;
    MicroscopeSpecificationsPacket() = default;
    MicroscopeSpecificationsPacket(int32_t a, 
                                int32_t b,
                                int32_t c,
                                int32_t d,
                                float e,
                                std::array<string> f,
                                std::array<float> g,
                                std::array<int32_t> h
                                )
        : scene_id(a),
        num_magnifications(b), 
        num_detectors(c), 
        num_frame_sizes(d),
        pixel_size(e),
        detectors(f)
        magnifications(g)
        frame_sizes(h) {}
    BOOST_HANA_DEFINE_STRUCT(MicroscopeSpecificationsPacket, (int32_t, scene_id),
                             (int32_t, num_magnifications),
                             (int32_t, num_detectors),
                             (int32_t, num_frame_sizes),
                             (float, pixel_size),
                             (std::array<string>, detectors),
                             (std::array<float>, magnifications),
                             (std::array<int32_t>, frame_sizes));
};


struct ImagingSpecificationsPacket
    : public PacketBase<ImagingSpecificationsPacket> {
    static const auto desc = packet_desc::imaging_scpecifications;
    ImagingSpecificationsPacket() = default;
    ImagingSpecificationsPacket(int32_t a, 
                                int32_t b,  
                                std::array<string> c,
                                float d,
                                int32_t e,
                                float f,
                                float g,
                                int32_t h,
                                float i
                                )
        : scene_id(a),
        num_detectors(b),
        detectors(c),
        acquisition_dwell_time(d), 
        acquisition_frame_size(e), 
        acquisition_frame_time(f),
        scanning_dwell_time(g), 
        scanning_frame_size(h), 
        scanning_frame_time(i) {}
    BOOST_HANA_DEFINE_STRUCT(ImagingSpecificationsPacket, (int32_t, scene_id),
                             (int32_t, num_magnifications),
                             (int32_t, num_detectors),
                             (int32_t, num_frame_sizes),
                             (float, pixel_size),
                             (std::array<string>, detectors),
                             (std::array<float>, magnifications),
                             (std::array<int32_t>, frame_sizes));
};