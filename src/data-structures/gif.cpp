#include "gif.hpp"

GIF::GIF(std::string_view path) {
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
    codec = avcodec_find_decoder(formatContext->streams[videoStreamIndex]->codecpar->codec_id);
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

    // Prepare conversion context
    SwsContext* swsContext = nullptr;
    AVFrame* rgbFrame = av_frame_alloc();
    uint8_t* buffer = nullptr;

    // Read frames
    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            // Send packet to decoder
            int response = avcodec_send_packet(codecContext, packet);
            if (response < 0) {
                std::cerr << "Error while sending a packet to the decoder" << std::endl;
                break;
            }

            // Receive frames
            while (response >= 0) {
                response = avcodec_receive_frame(codecContext, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    std::cerr << "Error while receiving a frame from the decoder" << std::endl;
                    break;
                }

                // Initialize conversion context on first frame
                if (!swsContext) {
                    width = frame->width;
                    height = frame->height;
                    
                    swsContext = sws_getContext(
                        width, height, codecContext->pix_fmt,
                        width, height, AV_PIX_FMT_RGBA,
                        SWS_BILINEAR, nullptr, nullptr, nullptr);
                    
                    if (!swsContext) {
                        throw std::runtime_error("Could not initialize conversion context");
                    }

                    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
                    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
                    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, 
                                       AV_PIX_FMT_RGBA, width, height, 1);
                }

                // Convert to RGBA
                sws_scale(swsContext, 
                         frame->data, frame->linesize, 0, height,
                         rgbFrame->data, rgbFrame->linesize);

                // Copy frame data
                std::vector<uint8_t> frameData(width * height * 4);
                for (int y = 0; y < height; y++) {
                    memcpy(
                        frameData.data() + (height - 1 - y) * width * 4, 
                        rgbFrame->data[0] + y * rgbFrame->linesize[0], 
                        width * 4
                    );
                }

                AVRational timeBase = formatContext->streams[videoStreamIndex]->time_base;
                double frameDurationInSeconds = av_q2d(timeBase) * frame->duration;

                frames.push_back(GIF_FrameData {
                    .data = frameData,
                    .duration = frameDurationInSeconds
                });
            }
        }
        av_packet_unref(packet);
    }

    // Cleanup
    if (swsContext) sws_freeContext(swsContext);
    if (buffer) av_free(buffer);
    av_frame_free(&rgbFrame);

    std::cout << "Loaded GIF with " << frames.size() << " frames (" 
              << width << "x" << height << ")" << std::endl;
}

GIF::~GIF()
{
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    avcodec_free_context(&codecContext);
    av_packet_free(&packet);
    av_frame_free(&frame);
} 