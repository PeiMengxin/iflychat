#include "stdafx.h"
#include "xunfeiSDK.h"


/* wav��Ƶͷ����ʽ */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = ��һ���ṹ��Ĵ�С : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = ͨ���� : 1
	int				samples_per_sec;        // = ������ : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = ÿ���ֽ��� : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = ÿ�������ֽ��� : wBitsPerSample / 8
	short int       bits_per_sample;        // = ����������: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = �����ݳ��� : FileSize - 44 
} wave_pcm_hdr;

/* Ĭ��wav��Ƶͷ������ */
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
	password = "";//����Ҫ��һ���������ڿƴ�Ѷ�ɹ���ע���˺�
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

bool SoundTest::XunFeiSDK::Login() //��¼
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
	MSPLogout();//�˳���¼
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
	string params = "voice_name = xiaoyan, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";//�����ɲο������ò����б�
	ret = -1;//ʧ��
	//��������
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
	text_len = strlen(src_text);//��ȡ�ı�����

	fopen_s(&fp, pcm_path.c_str(), "wb");//��PCM�ļ�
	//fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		fprintf(out_file, "open PCM file %s error\n", pcm_path);
		return ret;
	}

	sessionID = QTTSSessionBegin(params.c_str(), &ret);//��ʼһ���Ự				
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "QTTSSessionBegin: qtts begin session failed Error code %d.\n", ret);
		return ret;
	}
	fprintf(out_file, "sess_id = %s\n", sessionID);
	ret = QTTSTextPut(sessionID, src_text, text_len, NULL);//����txt��Ϣ
	if (ret != MSP_SUCCESS)
	{
		fprintf(out_file, "QTTSTextPut: qtts put text failed Error code %d.\n", ret);
		QTTSSessionEnd(sessionID, "TextPutError");//�쳣������
		return ret;
	}
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp);//�ѿ�ʼ�ļ�д����ǰ��

	while (1)//ѭ����ȡ��Ƶ�ļ����洢 
	{
		const void *data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len;//����pcm���ݵĴ�С
		}
		if (synth_status == MSP_TTS_FLAG_DATA_END || ret != 0)
			break;
	}//�ϳ�״̬synth_statusȡֵ�ɲο������ĵ�

	//����pcm�ļ�ͷ���ݵĴ�С
	wav_hdr.size_8 += wav_hdr.data_size + 36;

	//��������������д���ļ�ͷ��
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

int SoundTest::XunFeiSDK::StartRecord() //��ʼ¼��
{
	return 0;
}

int SoundTest::XunFeiSDK::EndRecord() //����¼��
{
	return 0;
}

std::string SoundTest::XunFeiSDK::SpeechToText(std::string text)
{
	string Sys_value("No data return");
	const char* src_wav_filename = text.c_str();//textת��
	//src_wav_filename = CStringToChar(text, src_wav_filename);
	//test = Marshal::PtrToStringAnsi((IntPtr)(char *)src_text);
	//return test;
	char rec_result[1024] = { 0 };//��ŷ��ؽ��
	const char *sessionID = NULL;
	FILE *f_pcm = NULL;//
	char *pPCM = NULL;//�����Ƶ�ļ�����
	int lastAudio = 0;
	int audStat = MSP_AUDIO_SAMPLE_CONTINUE;
	int epStatus = MSP_EP_LOOKING_FOR_SPEECH;
	int recStatus = MSP_REC_STATUS_SUCCESS;
	long pcmCount = 0;
	long pcmSize = 0;//��Ƶ�ļ���С
	int errCode = 10;
	string param = "sub=iat,auf=audio/L16;rate=16000,aue=speex-wb,ent=sms16k,rst=plain,rse=gb2312";

	fprintf(out_file, "Start iat...\n");
	sessionID = QISRSessionBegin(NULL, param.c_str(), &errCode);//��ʼһ·�Ự
	fopen_s(&f_pcm, src_wav_filename, "rb");
	if (NULL != f_pcm) {
		fseek(f_pcm, 0, SEEK_END);
		pcmSize = ftell(f_pcm);//��ȡ��Ƶ��С
		fseek(f_pcm, 0, SEEK_SET);
		pPCM = (char *)malloc(pcmSize);//�����ڴ�����Ƶ
		fread((void *)pPCM, pcmSize, 1, f_pcm);
		fclose(f_pcm);
		f_pcm = NULL;
	}//��ȡ��Ƶ�ļ�,����pPCM��
	else
	{
		fprintf(out_file, "media %s not found\n", src_wav_filename);
		return Sys_value;
	}

	while (1) {//��ʼ��������д��Ƶ����
		unsigned int len = 6400;
		int ret = 0;
		if (pcmSize < 12800) {
			len = pcmSize;
			lastAudio = 1;//��Ƶ����С��12800
		}
		audStat = MSP_AUDIO_SAMPLE_CONTINUE;//�к����Ƶ
		if (pcmCount == 0)
			audStat = MSP_AUDIO_SAMPLE_FIRST;
		if (len <= 0)
		{
			break;
		}
		fprintf(out_file, "csid=%s,count=%d,aus=%d,", sessionID, pcmCount / len, audStat);
		ret = QISRAudioWrite(sessionID, (const void *)&pPCM[pcmCount], len, audStat, &epStatus, &recStatus);//д��Ƶ
		fprintf(out_file, "eps=%d,rss=%d,ret=%d\n", epStatus, recStatus, errCode);
		if (ret != 0)
			break;
		pcmCount += (long)len;
		pcmSize -= (long)len;
		if (recStatus == MSP_REC_STATUS_SUCCESS) {
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);//������Ѿ���ʶ���������Ի�ȡ
			fprintf(out_file, "csid=%s,rss=%d,ret=%d\n", sessionID, recStatus, errCode);
			if (NULL != rslt)
				strcat_s(rec_result, rslt);
		}
		if (epStatus == MSP_EP_AFTER_SPEECH)
			break;
		Sleep(10);//ģ����˵��ʱ���϶
	}
	QISRAudioWrite(sessionID, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &epStatus, &recStatus);//д�����
	free(pPCM);
	pPCM = NULL;
	while (recStatus != MSP_REC_STATUS_COMPLETE && 0 == errCode) {
		const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);//��ȡ���
		fprintf(out_file, "csid=%s,rss=%d,ret=%d\n", sessionID, recStatus, errCode);
		if (NULL != rslt)
		{
			strcat_s(rec_result, rslt);
		}
		Sleep(10);
	}
	QISRSessionEnd(sessionID, NULL);
	fprintf(out_file, "The result is: %s\n", rec_result);
	if (NULL != rec_result)//��Ϊ��ʱ������ȷֵ
		Sys_value = rec_result;//��ֵת��

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
	volunm = oss1.str();//����
	oss2 << e_speed;
	voice_speed = oss2.str();//����
}

std::string SoundTest::XunFeiSDK::Iat(std::string audio_file)
{
	const char*		session_id = NULL;
	char			rec_result[BUFFER_SIZE] = { NULL };
	char			hints[HINTS_SIZE] = { NULL }; //hintsΪ�������λỰ��ԭ�����������û��Զ���
	unsigned int	total_len = 0;
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//��Ƶ״̬
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//�˵���
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//ʶ��״̬
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
	pcm_size = ftell(f_pcm); //��ȡ��Ƶ�ļ���С 
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory! \n");
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //��ȡ��Ƶ�ļ�����
	if (read_size != pcm_size)
	{
		printf("\nread [%s] error!\n", audio_file);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	printf("\n��ʼ������д ...\n");
	session_id = QISRSessionBegin(NULL, session_chat_params.c_str(), &errcode); //��д����Ҫ�﷨����һ������ΪNULL
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
		QISRSessionEnd(session_id, hints);

		return string(rec_result);
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; // ÿ��д��200ms��Ƶ(16k��16bit)��1֡��Ƶ20ms��10֡=200ms��16k�����ʵ�16λ��Ƶ��һ֡�Ĵ�СΪ640Byte
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

		if (MSP_REC_STATUS_SUCCESS == rec_stat) //�Ѿ��в�����д���
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
		Sleep(200); //ģ����˵��ʱ���϶��200ms��Ӧ10֡����Ƶ
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
		Sleep(150); //��ֹƵ��ռ��CPU
	}
	printf("\n������д����\n");
	printf("=============================================================\n");
	printf("%s\n", rec_result);
	printf("=============================================================\n");

	QISRSessionEnd(session_id, hints);

	return string(rec_result);

}

SoundTest::SoundType::SoundType()
{
	engin = "intp65"; voice_type = "xiaoyan"; voice = "��ͨ��Ů��";
}

SoundTest::SoundType::SoundType(string e_voice) //switch case ��֧��string������
{
	voice = e_voice;

	if (e_voice == "��ͨ��Ů��") { engin = "intp65"; voice_type = "xiaoyan"; }
	else if (e_voice == "��ͨ������") { engin = "intp65"; voice_type = "xiaoyu"; }
	else if (e_voice == "Ӣ��Ů��") { engin = "intp65_en"; voice_type = "Catherine"; }
	else if (e_voice == "Ӣ������") { engin = "intp65_en"; voice_type = "henry"; }
	else if (e_voice == "����") { engin = "vivi21"; voice_type = "vixm"; }
	else if (e_voice == "̨�廰") { engin = "vivi21"; voice_type = "vixl"; }
	else if (e_voice == "�Ĵ���") { engin = "vivi21"; voice_type = "vixr"; }
	else if (e_voice == "������") { engin = "vivi21"; voice_type = "vixyun"; }
	else { engin = "intp65"; voice_type = "xiaoyan"; voice = "��ͨ��Ů��"; }
}
