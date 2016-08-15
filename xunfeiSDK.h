#ifndef __xunfeiSDK_H__
#define __xunfeiSDK_H__



#include "stdafx.h"

#include "stdlib.h"
#include "stdio.h"
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <string>

using namespace std;

#include "include/qisr.h"
#include "include/qtts.h"
#include "include/msp_cmn.h"
#include "include/msp_errors.h"

//#ifdef _WIN64
//#pragma comment(lib,"libs/msc_x64.lib")//x64
//#else
//#pragma comment(lib,"libs/msc.lib")//x86
//#endif

#include <mmsystem.h>  
#pragma comment(lib, "winmm.lib")

typedef int SR_DWORD;
typedef short int SR_WORD;

#define	BUFFER_SIZE	4096
#define FRAME_LEN	640 
#define HINTS_SIZE  100

namespace SoundTest{

	class XunFeiSDK
	{
	public:
		XunFeiSDK();
		~XunFeiSDK();

	private:
		FILE* out_file;//���log�ļ�
		string appid;
		int ret;
		string pcm_path;//�洢��Ƶ�ļ����ļ���
		//string des_path;//�洢��Ƶ�ļ����ļ���
		string user;
		string password;
		string voice_type;//��������
		string volunm;//���� 0-10
		string engin;//����
		string voice_speed;//����0-10

		string session_chat_params;

	public:
		int status();

		bool Login();//��¼

		void Logout();

		int TextToSpeech(std::string Ssrc_text);//�ַ���ת��Ƶ����Ƶ�����PCM_SPEED.pcm��

		std::string GetPcmName();//��ȡ��Ƶ�ļ�·��

		int Play(std::string text);//������Ƶ�ļ�

		int StartRecord();//��ʼ¼��

		int EndRecord();//����¼��

		std::string SpeechToText(std::string text);//����ת���֣����������ļ���������������Ϣ

		void set_tts_params(string e_voice_type, string e_engin, int e_volunm, int e_speed);

		std::string Iat(std::string audio_path);

	};

	class SoundType
	{
	public:
		string engin;//��������
		string voice_type;//˵������
		string voice;//��ʾ

		SoundType(string e_voice);//switch case ��֧��string������
		SoundType();

	};
}

#endif // __xunfeiSDK_H__
