extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

#include <stdio.h>
#include <Windows.h>

AVCodec* vCodec, *aCodec;
AVCodecContext* vCtx, *aCtx;
AVCodecParameters* vPara, *aPara;
AVFormatContext* fmtCtx;
AVStream* vStream, *aStream;
int vidx = -1, aidx = -1;

int main(void) {	
	// 동영상 파일 정보 가져오기
	int ret = avformat_open_input(&fmtCtx, "C:\\Users\\SHIN\\Desktop\\Wildlife.wmv", NULL, NULL);
	if (ret != 0) { return -1; }
	avformat_find_stream_info(fmtCtx, NULL);

	printf("스트림 개수 = %d\n", fmtCtx->nb_streams);
	printf("시간 = %I64d초\n", fmtCtx->duration / AV_TIME_BASE);
	printf("비트레이트 = %I64d\n", fmtCtx->bit_rate);
	printf("--------------------------------\n");

	// 스트림 찾기

	// 루프 돌면서 찾기
	/*for (unsigned i = 0; i < fmtCtx->nb_streams; i++) {
		if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			vidx = i;
		}

		if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			aidx = i;
		}
	}*/

	// av_find_best_stream 함수 사용하여 최적 스트림 찾기
	vidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	aidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, vidx, NULL, 0);

	printf("video = %d번, audio = %d번\n\n", vidx, aidx);
	printf("--------------------------------\n");

	// 스트림 정보 보여주기
	// av_dump_format(fmtCtx, vidx, "c:\\mpstudy\\fire.avi", 0);	

	// 스트림 주요 정보 출력
	vStream = fmtCtx->streams[vidx];
	printf("프레임 개수 = %I64d\n", vStream->nb_frames);
	printf("프레임 레이트 = %d / %d\n", vStream->avg_frame_rate.num, vStream->avg_frame_rate.den);
	printf("타임 베이스 = %d / %d\n", vStream->time_base.num, vStream->time_base.den);
	printf("--------------------------------\n");

	vPara = vStream->codecpar;
	printf("폭 = %d\n", vPara->width);
	printf("높이 = %d\n", vPara->height);
	printf("색상 포맷 = %d\n", vPara->format);
	printf("코덱 = %d\n", vPara->codec_id);

	printf("--------------------------------\n");

	aStream = fmtCtx->streams[aidx];
	printf("프레임 개수 = %I64d\n", aStream->nb_frames);
	printf("타임 베이스 = %d / %d\n", aStream->time_base.num, aStream->time_base.den);
	printf("--------------------------------\n");
	aPara = aStream->codecpar;
	printf("사운드 포맷 = %d\n", aPara->format);
	printf("코덱 = %d\n", aPara->codec_id);
	printf("채널 = %d\n", aPara->channels);
	printf("샘플 레이트 = %d\n", aPara->sample_rate);
	printf("--------------------------------\n");

	// 비디오 코덱 오픈
	vCodec = avcodec_find_decoder(vPara->codec_id);
	vCtx = avcodec_alloc_context3(vCodec);
	avcodec_parameters_to_context(vCtx, vPara);
	avcodec_open2(vCtx, vCodec, NULL);

	// 오디오 코덱 오픈
	aCodec = avcodec_find_decoder(aPara->codec_id);
	aCtx = avcodec_alloc_context3(aCodec);
	avcodec_parameters_to_context(aCtx, aPara);
	avcodec_open2(aCtx, aCodec, NULL);

	// 코덱 정보 조사

	printf("비디오 코덱 : %d, %s(%s)\n", vCodec->id, vCodec->name, vCodec->long_name);
	printf("능력치 : %x\n", vCodec->capabilities);
	printf("오디오 코덱 : %d, %s(%s)\n", aCodec->id, aCodec->name, aCodec->long_name);
	printf("능력치 : %x\n", aCodec->capabilities);

	avcodec_free_context(&vCtx);
	avcodec_free_context(&aCtx);
	avformat_close_input(&fmtCtx);

	return 0;
}