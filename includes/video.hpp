#pragma once
#include <string_view>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class Video {
private:
public:
    Video(std::string_view filepath);
    ~Video();
};