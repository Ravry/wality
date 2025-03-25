#pragma once
#include <iostream>
#include <vector>
#include <string_view>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class GIF {
private:
    AVFormatContext* formatContext = nullptr;
    AVCodecContext* codecContext = nullptr;
    const AVCodec* codec = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    int videoStreamIndex = -1;
public:
    struct GIF_FrameData {
        std::vector<uint8_t> data;
        double duration;
    };

    std::vector<GIF_FrameData> frames;
    unsigned int width, height;

    GIF(std::string_view path);
    ~GIF();
};