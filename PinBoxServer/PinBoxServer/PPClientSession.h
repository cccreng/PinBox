#pragma once
#ifndef _PP_CLIENTSESISON_H_
#define _PP_CLIENTSESISON_H_

#include <fstream>
#include "PPMessage.h"
// evpp
#include <glog/config.h>
#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>




enum PPSession_Type { PPSESSION_NONE, PPSESSION_MOVIE, PPSESSION_SCREEN_CAPTURE, PPSESSION_INPUT_CAPTURE };

#define MSG_COMMAND_SIZE 9

#define PPREQUEST_NONE 0
#define PPREQUEST_HEADER 10
#define PPREQUEST_BODY 15
// authentication code
#define MSG_CODE_REQUEST_AUTHENTICATION_SESSION 1

#define MSG_CODE_RESULT_AUTHENTICATION_SUCCESS 5
#define MSG_CODE_RESULT_AUTHENTICATION_FAILED 6
// screen capture code
#define MSG_CODE_REQUEST_START_SCREEN_CAPTURE 10
#define MSG_CODE_REQUEST_STOP_SCREEN_CAPTURE 11
#define MSG_CODE_REQUEST_CHANGE_SETTING_SCREEN_CAPTURE 12
#define MSG_CODE_REQUEST_NEW_SCREEN_FRAME 15
#define MSG_CODE_REQUEST_SCREEN_RECEIVED_FRAME 16
#define MSG_CODE_REQUEST_NEW_AUDIO_FRAME 18
#define MSG_CODE_REQUEST_RECEIVED_AUDIO_FRAME 19

// input capture
#define MSG_CODE_REQUEST_START_INPUT_CAPTURE 40
#define MSG_CODE_REQUEST_STOP_INPUT_CAPTURE 41
#define MSG_CODE_SEND_INPUT_CAPTURE 42
#define MSG_CODE_SEND_INPUT_CAPTURE_IDLE 44

// 8Mb default size just enough
#define DEFAULT_CONTINUOUS_BUFFER_SIZE 1024 * 1024 *8 
// 24Kb for each step
#define DEFAULT_CONTINUOUS_BUFFER_STEP 1024*24 
#define CHOP_N(BUFFER, COUNT, LENGTH) memmove(BUFFER, BUFFER + COUNT, LENGTH)

class PPServer;
class PPClientSession
{
private:
	evpp::TCPConnPtr			_connection;
	bool						_authenticated = false;

	u8*							_continuousBuffer = nullptr;
	u32							_bufferWriteIndex = 0;
	u32							_bufferSize = 0;

	u8							_currentReadState = PPREQUEST_NONE;
	u8							_currentMsgCode = 0;
	u32							_waitForSize = 0;
private:
	void						sendMessageWithCode(u8 code);
	void						sendMessageWithCodeAndData(u8 code, u8* buffer, size_t bufferSize);
	void						preprocessMessageCode(u8 code);
	void						processMessageBody(u8* buffer, u8 code);

	void						ProcessIncommingMessage();
	void						ProcessAuthentication();
public:
	PPServer*					_server;

	void						InitSession(evpp::TCPConnPtr conn, PPServer* parent);
	void						ProcessMessage(evpp::Buffer* msg);
	void						DisconnectFromServer();

	bool						IsAuthenticated() const { return _authenticated; }

	void						PrepareVideoPacketAndSend(u8* buffer, int bufferSize);
	void						PrepareAudioPacketAndSend(u8* buffer, int bufferSize, uint64_t pts);
};

#endif