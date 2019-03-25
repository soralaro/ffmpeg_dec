#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
#include <libavutil/file.h>
#include <libavutil/imgutils.h>
}

int main(int argc, char* argv[])
{
	printf("star!\n");
	char* filename = "/projects/video/t1019.h264";
	AVFormatContext* FormatContext=0;
	AVCodec* codec=0;
	AVCodecContext* CodecContext=0;
	AVPacket* pkt=0;
	AVFrame* frame=0;
	int ret=-1;

	// 初始化解封装
	av_register_all();
	// 初始化网络
	avformat_network_init();

	FormatContext= avformat_alloc_context();
	if(avformat_open_input(&FormatContext, filename, 0, 0) <0)
	{
		printf("%s avformat_open_input erro\n",filename);
	    exit(1);
	}
	int st_idx= -1;

	for(int i=0; i<FormatContext->nb_streams; ++i)
	{
	    if(FormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
	    {
	        st_idx= i;
	        break;
	    }
	}
	if(-1 == st_idx)
	{
	    avformat_close_input(&FormatContext);
	    exit(1);
	}

	avformat_find_stream_info(FormatContext, 0);
	av_dump_format(FormatContext, 0, "", 0);

	codec= avcodec_find_decoder((AVCodecID)FormatContext->streams[st_idx]->codecpar->codec_id);
	CodecContext= avcodec_alloc_context3(codec);
	if(!CodecContext)
	{
	    avformat_close_input(&FormatContext);
	    exit(1);
	}
	if(avcodec_open2(CodecContext, codec, 0) <0)
	{
	    goto end;
	}



	pkt= av_packet_alloc();
	frame=av_frame_alloc();
	AVFrame* frameYUV= av_frame_alloc();

	//!
	FILE* fp= fopen("./stream.yuv", "wb");
	//    if(!fp)
	//        goto end;
	//

	AVCodecParameters* cpar= FormatContext->streams[st_idx]->codecpar;
	av_image_alloc(frameYUV->data, frameYUV->linesize, cpar->width, cpar->height, AV_PIX_FMT_YUV420P, 1);
	SwsContext* swsc= sws_getContext(cpar->width, cpar->height, (AVPixelFormat)cpar->format,
	                                 cpar->width, cpar->height, AV_PIX_FMT_YUV420P,
	                                 SWS_FAST_BILINEAR, 0,0,0);
	int got_pict= 0;
	while(av_read_frame(FormatContext, pkt) >=0)
	{
	    if(pkt->stream_index == st_idx)
	    {
	        if(avcodec_decode_video2(CodecContext, frame, &got_pict, pkt) >=0)
	        {
	            if(got_pict)
	            {
	                ret= sws_scale(swsc,(const uint8_t* const*)frame->data, frame->linesize, 0, frame->height,(uint8_t* const*)frameYUV->data, frameYUV->linesize);
	                if(ret != 0)
	                {
	                    fwrite(frameYUV->data[0], CodecContext->width*CodecContext->height, 1, fp);
	                    fwrite(frameYUV->data[1], CodecContext->width*CodecContext->height/4, 1, fp);
	                    fwrite(frameYUV->data[2], CodecContext->width*CodecContext->height/4, 1, fp);
	                    printf( "write frame_number %d \n", CodecContext->frame_number);
	                }
	            }
	        }
	    }
	}

	end:
	avformat_close_input(&FormatContext);
	avcodec_free_context(&CodecContext);
	av_packet_free(&pkt);
	av_frame_free(&frame);
	av_frame_free(&frameYUV);
	fclose(fp);
	sws_freeContext(swsc);
	av_free(&frameYUV->data[0]);

	return 0;
}
