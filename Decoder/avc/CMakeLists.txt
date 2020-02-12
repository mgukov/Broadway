cmake_minimum_required(VERSION 3.8)

project(avc)

set(SDL2_DIR /usr/lib/x86_64-linux-gnu/cmake/SDL2)

message("CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}")
message("CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

find_package(SDL2 REQUIRED)

message("SDL2_LIBRARIES  ${SDL2_LIBRARIES}")
message("SDL2_INCLUDE_DIRS  ${SDL2_INCLUDE_DIRS}")


link_directories(
        ${SDL2_LIBRARIES}
)

include_directories(
        inc
        src
        ${SDL2_INCLUDE_DIRS}
)


add_library(
        avc
        "src/h264bsd_transform.c"
        "src/h264bsd_util.c"
        "src/h264bsd_byte_stream.c"
        "src/h264bsd_seq_param_set.c"
        "src/h264bsd_pic_param_set.c"
        "src/h264bsd_slice_header.c"
        "src/h264bsd_slice_data.c"
        "src/h264bsd_macroblock_layer.c"
        "src/h264bsd_stream.c"
        "src/h264bsd_vlc.c"
        "src/h264bsd_cavlc.c"
        "src/h264bsd_nal_unit.c"
        "src/h264bsd_neighbour.c"
        "src/h264bsd_storage.c"
        "src/h264bsd_slice_group_map.c"
        "src/h264bsd_intra_prediction.c"
        "src/h264bsd_inter_prediction.c"
        "src/h264bsd_reconstruct.c"
        "src/h264bsd_dpb.c"
        "src/h264bsd_image.c"
        "src/h264bsd_deblocking.c"
        "src/h264bsd_conceal.c"
        "src/h264bsd_vui.c"
        "src/h264bsd_pic_order_cnt.c"
        "src/h264bsd_decoder.c"
        "src/H264SwDecApi.c"
        "src/extraFlags.c"
        "src/Decoder.c"
        )

target_link_libraries(
        avc
#        ${SDL2_LIBRARIES}
        /usr/lib/x86_64-linux-gnu/libSDL2.so
)