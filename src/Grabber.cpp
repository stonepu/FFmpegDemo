#include "Grabber.h"

Grabber::Grabber()
{
	av_register_all();
	avformat_network_init();
	videoIndex = audioIndex = -1;
	fctx = avformat_alloc_context();
	if (!fctx) {
		throw std::runtime_error("formatContext init error");
	}
}

int Grabber::open(const std::string& filename)
{
	int ret = 0;
	ret = avformat_open_input(&fctx, filename.c_str(), 0, 0);
	if (ret < 0) {
		throw std::runtime_error("open file error");
	}

	ret = avformat_find_stream_info(fctx, 0);
	if (ret) {
		avformat_close_input(&fctx);
		throw std::runtime_error("find stream  info error");
	}

	for (int i = 0; i < fctx->nb_streams; ++i) {
		if (fctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioIndex = i;
			std::cout << "aIndex: " << audioIndex << std::endl;
		}
		if (fctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			std::cout << "vIndex: " << videoIndex << std::endl;
		}
	}

	//此处需要文件既有视频又有音频，否则抛出异常
	if (videoIndex == -1 || audioIndex == -1) {
		avformat_close_input(&fctx);
		throw std::runtime_error("not find vidoe or audio");
	}
	actx = fctx->streams[audioIndex]->codec;
	vctx = fctx->streams[videoIndex]->codec;
	auto vcodec = avcodec_find_decoder(vctx->codec_id);
	auto acodec = avcodec_find_decoder(actx->codec_id);
	ret = avcodec_open2(vctx, vcodec, 0);
	if (ret) {
		throw std::runtime_error("vctx open error");
	}

	ret = avcodec_open2(actx, acodec, 0);
	if (ret) {
		throw std::runtime_error("actx open error");
	}
	frameRate = av_q2d(fctx->streams[videoIndex]->avg_frame_rate);
	return 0;
}

AVFrame* Grabber::decode(AVPacket * pkt)
{
	int ret = 0;
	if (pkt->stream_index == videoIndex) {
		ret = avcodec_send_packet(vctx, pkt);
		if (ret < 0) {
			if (ret == AVERROR(EAGAIN)) {

			}
			else {
				av_free_packet(pkt);
				return 0;
			}
		}
		AVFrame* frame = av_frame_alloc();
		ret = avcodec_receive_frame(vctx, frame);
		if (ret < 0) {
			if (ret == AVERROR(EAGAIN)) {

			}
			else {
				av_frame_free(&frame);
				return 0;
			}
		}
		return frame;
	}
	else if( pkt->stream_index == audioIndex) {
		ret = avcodec_send_packet(actx, pkt);
		if (ret < 0) {
			if (ret == AVERROR(EAGAIN)) {
				
			}
			else {
				av_free_packet(pkt);
				return 0;
			}
		}
		AVFrame* frame = av_frame_alloc();
		ret = avcodec_receive_frame(actx, frame);
		if (ret < 0) {
			if (ret == AVERROR(EAGAIN)) {
				return 0;
			}
			else {
				av_frame_free(&frame);
				return 0;
			}
		}
		return frame;
	}
	return 0;
}

int Grabber::receiveFrame(AVFrame * frame)
{
	return 0;
}

AVPacket* Grabber::grab()
{
	int ret = 0;
	AVPacket* pkt = av_packet_alloc();
	av_init_packet(pkt);
	ret = av_read_frame(fctx, pkt);
	if (ret < 0) {
		av_free_packet(pkt);
		std::cout << "grab fai" << std::endl;
		//throw std::runtime_error("can not read frame of the file");
	}
	return pkt;
	//if (pkt->stream_index == videoIndex) {
	//	ret = avcodec_send_packet(vctx, pkt);
	//	if (ret < 0) {
	//		if (ret == AVERROR(EAGAIN)) {
	//			//TODO 待优化，此处直接丢掉
	//		}
	//		else {
	//			av_free_packet(pkt);
	//			throw std::runtime_error("send packet error");
	//		}
	//	}
	//	AVFrame *frameTemp = av_frame_alloc();
	//	ret = avcodec_receive_frame(vctx, frameTemp);
	//	if (ret < 0) {
	//		if (ret == AVERROR(EAGAIN)) {
	//		}
	//		else {
	//			av_frame_free(&frameTemp);
	//			throw std::runtime_error("receive frame error");
	//		}
	//	}
	//	av_packet_unref(pkt);
	//	return frameTemp;
	//}
	//else if (pkt->stream_index == audioIndex) {
	//	ret = avcodec_send_packet(actx, pkt);
	//	if (ret < 0) {
	//		if (ret == AVERROR(EAGAIN)) {
	//			//TODO 待优化，此处直接丢掉
	//		}
	//		else {
	//			av_free_packet(pkt);
	//			throw std::runtime_error("send packet error");
	//		}
	//	}
	//	AVFrame *frameTemp = av_frame_alloc();
	//	ret = avcodec_receive_frame(actx, frameTemp);
	//	if (ret < 0) {
	//		if (ret == AVERROR(EAGAIN)) {
	//		}
	//		else {
	//			av_frame_free(&frameTemp);
	//			throw std::runtime_error("receive frame error");
	//		}
	//	}
	//	av_packet_unref(pkt);
	//	return frameTemp;
	//}
	//return nullptr;
}
