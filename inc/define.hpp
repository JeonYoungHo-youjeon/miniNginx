#ifndef DEFINE_HPP
#define DEFINE_HPP

enum eType {
	SERVER = 0,
	CLIENT
};

enum eState {
	READY_REQUEST = 0,
	READ_REQUEST,
	DONE_REQUEST,
	READ_RESPONSE,
	WRITE_RESPONSE,
	DONE_RESPONSE
	// TODO: EXEC_RESPONSE
};
// FIXME : file인지 cgi인지 구분 => 분기를 통한 state 반환 값으로 처리
// FIXME : read 탈출 조건 => read의 정상적인 탈출 조건은 read가 읽은 길이가 0 일때
// write 질문
// enum eState {
// 	NONE = 0,
// 	READ_REQUEST,
// 	DONE_REQUEST,
// 	READ_RESPONSE,
// 	EXEC_RESPONSE,
// 	WRITE_RESPONSE,
// 	DONE_RESPONSE,
// 	DONE
// };

enum eProg
{
	READY,
	START_LINE,
	HEADER,
	HEADER_SET,
	BODY,
	PROG_DONE
};

static const time_t		TIMEOUT = 30;
static const int		MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
const int				BUFFER_SIZE = 1024;

#endif	//	DEFINE_HPP