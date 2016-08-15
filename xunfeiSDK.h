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
		FILE* out_file;//输出log文件
		string appid;
		int ret;
		string pcm_path;//存储音频文件的文件名
		//string des_path;//存储音频文件的文件名
		string user;
		string password;
		string voice_type;//语言类型
		string volunm;//音量 0-10
		string engin;//引擎
		string voice_speed;//语速0-10

		string session_chat_params;

	public:
		int status();

		bool Login();//登录

		void Logout();

		int TextToSpeech(std::string Ssrc_text);//字符串转音频，音频存放在PCM_SPEED.pcm下

		std::string GetPcmName();//获取音频文件路径

		int Play(std::string text);//播放音频文件

		int StartRecord();//开始录音

		int EndRecord();//结束录音

		std::string SpeechToText(std::string text);//语音转文字，输入语音文件名，返回文字信息

		void set_tts_params(string e_voice_type, string e_engin, int e_volunm, int e_speed);

		std::string Iat(std::string audio_path);

	};

	class SoundType
	{
	public:
		string engin;//语音引擎
		string voice_type;//说话类型
		string voice;//显示

		SoundType(string e_voice);//switch case 不支持string的输入
		SoundType();

	};
}

#endif // __xunfeiSDK_H__
