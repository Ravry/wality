#pragma once
#include <iostream>
#include <vector>
#include <string_view>
#include <thread>
#include <mutex>
#include <queue>
#include <optional>
#include <atomic>
#include <condition_variable>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

struct Media_FrameData {
    std::vector<uint8_t> data;
    unsigned int width;
    unsigned int height;
};

class FrameDataQueue {
private:
    std::queue<Media_FrameData> queue;
    std::mutex mutex;
    std::condition_variable condition;
public:
    FrameDataQueue() = default;
    ~FrameDataQueue() = default;

    void push(Media_FrameData&& data) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(data));
        condition.notify_one();
    }

    std::optional<Media_FrameData> pop() {
        std::unique_lock<std::mutex> lock(mutex);
        Media_FrameData data = queue.front();
        queue.pop();
        return data;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

class Media {
private:
    AVFormatContext* formatContext = nullptr;
    AVCodecContext* codecContext = nullptr;
    SwsContext* swsContext = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    int videoStreamIndex = -1;
public:
    FrameDataQueue frameQueue;

    Media(std::string_view path);
    ~Media();
    void decodeFrame();
};