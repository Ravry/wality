#include "media.hpp"

Media::Media(std::string_view path) {
    avformat_network_init();

    // Open input file
    if (avformat_open_input(&formatContext, path.data(), nullptr, nullptr) != 0) {
        throw std::runtime_error("Could not open input file");
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        throw std::runtime_error("Could not find stream information");
    }

    // Find video stream
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        throw std::runtime_error("Could not find video stream");
    }

    // Get codec and create codec context
    const AVCodec* codec = avcodec_find_decoder(formatContext->streams[videoStreamIndex]->codecpar->codec_id);
    if (!codec) {
        throw std::runtime_error("Unsupported codec");
    }

    codecContext = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0) {
        throw std::runtime_error("Could not copy codec context");
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        throw std::runtime_error("Could not open codec");
    }

    // Allocate packet and frame
    packet = av_packet_alloc();
    frame = av_frame_alloc();

    std::thread decodeThread(&Media::decodeFrame, this);
    decodeThread.detach();
}

Media::~Media()
{
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    avcodec_free_context(&codecContext);
    av_packet_free(&packet);
    av_frame_free(&frame);
}

void Media::decodeFrame() {
    // Preallocate conversion context and frame outside the loop
    SwsContext* swsContext = nullptr;
    
    // Use a single, preallocated vector to reduce memory allocations
    std::vector<uint8_t> frameBuffer;
    
    // Minimize locking and synchronization overhead
    while (true) {
        int readResult = av_read_frame(formatContext, packet);
        if (readResult < 0) {
            av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
            continue;
        }

        if (packet->stream_index == videoStreamIndex) {
            // Reduce function call overhead
            int sendResult = avcodec_send_packet(codecContext, packet);
            if (sendResult < 0) {
                av_packet_unref(packet);
                continue;
            }

            while (avcodec_receive_frame(codecContext, frame) == 0) {
                // Lazy initialization of conversion context
                if (!swsContext) {
                    swsContext = sws_getContext(
                        frame->width, frame->height,
                        codecContext->pix_fmt,
                        frame->width, frame->height,
                        AV_PIX_FMT_RGBA,
                        SWS_FAST_BILINEAR, // More performant scaling
                        nullptr, nullptr, nullptr
                    );

                    if (!swsContext) {
                        throw std::runtime_error("Could not initialize conversion context");
                    }
                }

                // Resize buffer only if dimensions change
                if (frameBuffer.size() != frame->width * frame->height * 4) {
                    frameBuffer.resize(frame->width * frame->height * 4);
                }

                Media_FrameData currentFrame;
                currentFrame.width = frame->width;
                currentFrame.height = frame->height;
                
                // Use preallocated buffer
                currentFrame.data = std::move(frameBuffer);

                // Vertical flip with optimized destination planes
                uint8_t* destPlanes[4] = { 
                    currentFrame.data.data() + (frame->height - 1) * frame->width * 4,
                    nullptr, nullptr, nullptr 
                };

                // Negative stride to flip image vertically
                int destStrides[4] = { -frame->width * 4, 0, 0, 0 };

                // Perform scaling
                sws_scale(
                    swsContext,
                    frame->data, frame->linesize, 0, frame->height,
                    destPlanes, destStrides
                );

                // Use move semantics to avoid deep copy
                frameQueue.push(std::move(currentFrame));

                // More flexible frame rate control
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(static_cast<int>((1.0 / 60.0) * 1000))
                );
            }
        }
        
        // Always unref packet to prevent memory leaks
        av_packet_unref(packet);
    }

    // Cleanup (though this will never be reached in the current implementation)
    if (swsContext) sws_freeContext(swsContext);
}