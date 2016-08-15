#include "stdafx.h"
#include "xunfeiSDK.h"


/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr =
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{ 'W', 'A', 'V', 'E' },
	{ 'f', 'm', 't', ' ' },
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{ 'd', 'a', 't', 'a' },
	0
};


//char* CStringToChar(CString cstr, char* char_dst)
//{
//	int len_b = WideCharToMultiByte(CP_ACP, 0, cstr, cstr.GetLength(), NULL, 0, NULL, NULL);
//	char_dst = new char[len_b + 1];
//	WideCharToMultiByte(CP_ACP, 0, cstr, cstr.GetLength() + 1, char_dst, len_b + 1, NULL, NULL);
//	char_dst[len_b + 1] = '/0';
//
//	return char_dst;
//}

SoundTest::XunFeiSDK::XunFeiSDK()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char nowTime[255];
	sprintf_s(nowTime, "%d-%02d-%02d-%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	fopen_s(&out_file, "log.txt", "at+");
	if (out_file == NULL)
	{
		ret = -1;
		return;
	}
	fseek(out_file, 0, 2);
	fprintf(out_file, "begin Time:%s \n", nowTime);

	appid = "57a575d3";
	user = "";
	password = "";//若需要进一步开发请在科大讯飞官网注册账号
	pcm_path = "PCM_SPEED.pcm";
	voice_type = "xiaoyan";
	volunm = "7";
	voice_speed = "5";
	engin = "intp65";

	session_chat_params = "sub=iat,ssm=1,sch=1,auf=audio/L16;rate=16000,aue=speex,ent=sms16k,ptt=0,rst=json,rse=utf8,nlp_version=2.0, result_encoding = gb2312";

}

SoundTest::XunFeiSDK::~XunFeiSDK()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char nowTime[255];
	sprintf_s(nowTime, "%d-%02d-%02d-%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	fprintf(out_file, "Time:%s end\n", nowTime);
	fclose(out_file);
}

int SoundTest::XunFeiSDK::status()
{
	return ret;
}

bool SoundTest::XunFeiSDK::Login() //登录
{
	string logins = "appid = " + appid + ",work_dir =   .  ";
	ret = MSPLogin(user.c_str(), password.c_str(), logins.c_str());
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "MSPLogin failed , Error code %d.\n", ret);
		return false;
	}
	return true;
}

void SoundTest::XunFeiSDK::Logout()
{
	MSPLogout();//退出登录
}

int SoundTest::XunFeiSDK::TextToSpeech(std::string Ssrc_text)
{
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	const char* sessionID = NULL;
	unsigned int text_len = 0;
	char* audio_data = NULL;
	unsigned int audio_len = 0;
	int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	FILE* fp = NULL;
	string params = "voice_name = xiaoyan, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";//参数可参考可设置参数列表
	ret = -1;//失败
	//参数配置
	//params = "vcn=" + voice_type + ", spd = " + voice_speed + ", vol = " + volunm + ", ent = " + engin;

	const char* src_text = Ssrc_text.c_str();

	//src_text = CStringToChar(Ssrc_text, src_text);

	pcm_path = "tts_sample.wav";

	fprintf(out_file, "begin to synth source = %s\n", src_text);
	if (NULL == src_text)
	{
		fprintf(out_file, "params is null!\n");
		return ret;
	}
	text_len = strlen(src_text);//获取文本长度

	fopen_s(&fp, pcm_path.c_str(), "wb");//打开PCM文件
	//fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		fprintf(out_file, "open PCM file %s error\n", pcm_path);
		return ret;
	}

	sessionID = QTTSSessionBegin(params.c_str(), &ret);//开始一个会话				
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "QTTSSessionBegin: qtts begin session failed Error code %d.\n", ret);
		return ret;
	}
	fprintf(out_file, "sess_id = %s\n", sessionID);
	ret = QTTSTextPut(sessionID, src_text, text_len, NULL);//发送txt信息
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "QTTSTextPut: qtts put text failed Error code %d.\n", ret);
		QTTSSessionEnd(sessionID, "TextPutError");//异常，结束
		return ret;
	}
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp);//把开始文件写到最前面

	while (1)//循环读取音频文件并存储 
	{
		const void *data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len;//修正pcm数据的大小
		}
		if (synth_status == MSP_TTS_FLAG_DATA_END || ret != 0)
			break;
	}//合成状态synth_status取值可参考开发文档

	//修正pcm文件头数据的大小
	wav_hdr.size_8 += wav_hdr.data_size + 36;

	//将修正过的数据写回文件头部
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp);
	fseek(fp, 40, 0);
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp);
	fclose(fp);

	ret = QTTSSessionEnd(sessionID, NULL);
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "QTTSSessionEnd: qtts end failed Error code %d.\n", ret);
	}
	fprintf(out_file, "program end");
	return ret;
}

std::string SoundTest::XunFeiSDK::GetPcmName()
{
	return pcm_path;
}

int SoundTest::XunFeiSDK::Play(std::string text)
{
	if (text == "") return -1;
	PlaySound(CString(text.c_str()), NULL, SND_FILENAME | SND_ASYNC);
	return 0;
}

int SoundTest::XunFeiSDK::StartRecord() //开始录音
{
	return 0;
}

int SoundTest::XunFeiSDK::EndRecord() //结束录音
{
	return 0;
}

std::string SoundTest::XunFeiSDK::SpeechToText(std::string text)
{
	string Sys_value("No data return");
	const char* src_wav_filename = text.c_str();//text转换
	//src_wav_filename = CStringToChar(text, src_wav_filename);
	//test = Marshal::PtrToStringAnsi((IntPtr)(char *)src_text);
	//return test;
	char rec_result[1024] = { 0 };//存放返回结果
	const char *sessionID = NULL;
	FILE *f_pcm = NULL;//
	char *pPCM = NULL;//存放音频文件缓存
	int lastAudio = 0;
	int audStat = MSP_AUDIO_SAMPLE_CONTINUE;
	int epStatus = MSP_EP_LOOKING_FOR_SPEECH;
	int recStatus = MSP_REC_STATUS_SUCCESS;
	long pcmCount = 0;
	long pcmSize = 0;//音频文件大小
	int errCode = 10;
	string param = "sub=iat,auf=audio/L16;rate=16000,aue=speex-wb,ent=sms16k,rst=plain,rse=gb2312";

	fprintf(out_file, "Start iat...\n");
	sessionID = QISRSessionBegin(NULL, param.c_str(), &errCode);//开始一路会话
	fopen_s(&f_pcm, src_wav_filename, "rb");
	if (NULL != f_pcm) {
		fseek(f_pcm, 0, SEEK_END);
		pcmSize = ftell(f_pcm);//获取音频大小
		fseek(f_pcm, 0, SEEK_SET);
		pPCM = (char *)malloc(pcmSize);//分配内存存放音频
		fread((void *)pPCM, pcmSize, 1, f_pcm);
		fclose(f_pcm);
		f_pcm = NULL;
	}//读取音频文件,读到pPCM中
	else
	{
		fprintf(out_file, "media %s not found\n", src_wav_filename);
		return Sys_value;
	}

	while (1) {//开始往服务器写音频数据
		unsigned int len = 6400;
		int ret = 0;
		if (pcmSize < 12800) {
			len = pcmSize;
			lastAudio = 1;//音频长度小于12800
		}
		audStat = MSP_AUDIO_SAMPLE_CONTINUE;//有后继音频
		if (pcmCount == 0)
			audStat = MSP_AUDIO_SAMPLE_FIRST;
		if (len <= 0)
		{
			break;
		}
		fprintf(out_file, "csid=%s,count=%d,aus=%d,", sessionID, pcmCount / len, audStat);
		ret = QISRAudioWrite(sessionID, (const void *)&pPCM[pcmCount], len, audStat, &epStatus, &recStatus);//写音频
		fprintf(out_file, "eps=%d,rss=%d,ret=%d\n", epStatus, recStatus, errCode);
		if (ret != 0)
			break;
		pcmCount += (long)len;
		pcmSize -= (long)len;
		if (recStatus == MSP_REC_STATUS_SUCCESS) {
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);//服务端已经有识别结果，可以获取
			fprintf(out_file, "csid=%s,rss=%d,ret=%d\n", sessionID, recStatus, errCode);
			if (NULL != rslt)
				strcat_s(rec_result, rslt);
		}
		if (epStatus == MSP_EP_AFTER_SPEECH)
			break;
		Sleep(10);//模拟人说话时间间隙
	}
	QISRAudioWrite(sessionID, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &epStatus, &recStatus);//写入结束
	free(pPCM);
	pPCM = NULL;
	while (recStatus != MSP_REC_STATUS_COMPLETE && 0 == errCode) {
		const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);//获取结果
		fprintf(out_file, "csid=%s,rss=%d,ret=%d\n", sessionID, recStatus, errCode);
		if (NULL != rslt)
		{
			strcat_s(rec_result, rslt);
		}
		Sleep(10);
	}
	QISRSessionEnd(sessionID, NULL);
	fprintf(out_file, "The result is: %s\n", rec_result);
	if (NULL != rec_result)//不为空时返回正确值
		Sys_value = rec_result;//数值转换

	return Sys_value;
}

void SoundTest::XunFeiSDK::set_tts_params(string e_voice_type, string e_engin, int e_volunm, int e_speed)
{
	const char* src_text = e_voice_type.c_str();
	voice_type = src_text;
	src_text = e_engin.c_str();
	engin = src_text;
	ostringstream oss1;
	ostringstream oss2;
	oss1 << e_volunm;
	volunm = oss1.str();//音量
	oss2 << e_speed;
	voice_speed = oss2.str();//语速
}

std::string SoundTest::XunFeiSDK::Iat(std::string audio_file)
{
	const char*		session_id = NULL;
	char			rec_result[BUFFER_SIZE] = { NULL };
	char			hints[HINTS_SIZE] = { NULL }; //hints为结束本次会话的原因描述，由用户自定义
	unsigned int	total_len = 0;
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//音频状态
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//识别状态
	int				errcode = MSP_SUCCESS;

	FILE*			f_pcm = NULL;
	char*			p_pcm = NULL;
	long			pcm_count = 0;
	long			pcm_size = 0;
	long			read_size = 0;


	if (audio_file.empty())
	{
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	f_pcm = fopen(audio_file.c_str(), "rb");
	if (NULL == f_pcm)
	{
		printf("\nopen [%s] failed! \n", audio_file);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm); //获取音频文件大小 
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory! \n");
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //读取音频文件内容
	if (read_size != pcm_size)
	{
		printf("\nread [%s] error!\n", audio_file);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	printf("\n开始语音听写 ...\n");
	session_id = QISRSessionBegin(NULL, session_chat_params.c_str(), &errcode); //听写不需要语法，第一个参数为NULL
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		printf(">");
		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			QISRSessionEnd(session_id, hints);

			return string(rec_result);
		}

		pcm_count += (long)len;
		pcm_size -= (long)len;

		if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
		{
			const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
			if (MSP_SUCCESS != errcode)
			{
				printf("\nQISRGetResult failed! error code: %d\n", errcode);
				QISRSessionEnd(session_id, hints);

				return string(rec_result);
			}
			if (NULL != rslt)
			{
				unsigned int rslt_len = strlen(rslt);
				total_len += rslt_len;
				if (total_len >= BUFFER_SIZE)
				{
					printf("\nno enough buffer for rec_result !\n");
					QISRSessionEnd(session_id, hints);

					return string(rec_result);
				}
				strncat(rec_result, rslt, rslt_len);
			}
		}

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		Sleep(200); //模拟人说话时间间隙。200ms对应10帧的音频
	}
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			printf("\nQISRGetResult failed, error code: %d\n", errcode);
			QISRSessionEnd(session_id, hints);

			return string(rec_result);
		}
		if (NULL != rslt)
		{
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				printf("\nno enough buffer for rec_result !\n");
				QISRSessionEnd(session_id, hints);

				return string(rec_result);
			}
			strncat(rec_result, rslt, rslt_len);
		}
		Sleep(150); //防止频繁占用CPU
	}
	printf("\n语音听写结束\n");
	printf("=============================================================\n");
	printf("%s\n", rec_result);
	printf("=============================================================\n");

	QISRSessionEnd(session_id, hints);

	return string(rec_result);

}

SoundTest::SoundType::SoundType()
{
	engin = "intp65"; voice_type = "xiaoyan"; voice = "普通话女声";
}

SoundTest::SoundType::SoundType(string e_voice) //switch case 不支持string的输入
{
	voice = e_voice;

	if (e_voice == "普通话女声") { engin = "intp65"; voice_type = "xiaoyan"; }
	else if (e_voice == "普通话男声") { engin = "intp65"; voice_type = "xiaoyu"; }
	else if (e_voice == "英文女声") { engin = "intp65_en"; voice_type = "Catherine"; }
	else if (e_voice == "英文男声") { engin = "intp65_en"; voice_type = "henry"; }
	else if (e_voice == "粤语") { engin = "vivi21"; voice_type = "vixm"; }
	else if (e_voice == "台湾话") { engin = "vivi21"; voice_type = "vixl"; }
	else if (e_voice == "四川话") { engin = "vivi21"; voice_type = "vixr"; }
	else if (e_voice == "东北话") { engin = "vivi21"; voice_type = "vixyun"; }
	else { engin = "intp65"; voice_type = "xiaoyan"; voice = "普通话女声"; }
}
