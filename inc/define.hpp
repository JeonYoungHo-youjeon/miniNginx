#ifndef DEFINE_HPP
#define DEFINE_HPP

enum eType {
	SERVER = 0,
	CLIENT
};

enum eState {
	NONE = 0,
	READ_REQUEST,
	DONE_REQUEST,
	READ_RESPONSE,
	EXEC_RESPONSE,
	WRITE_RESPONSE,
	DONE_RESPONSE,
	DONE
};

namespace REQ
{
	enum eProgress
	{
		START_LINE,
		HEADER,
		HEADER_SET,
		BODY,
		DONE
	};
}

namespace RES
{
	enum eProgress
	{
		READY,
		START_LINE,
		HEADER,
		BLANK_LINE,
		BODY,
		DONE
	};
}
static const time_t		TIMEOUT = 30;
static const int		MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
const int				BUFFER_SIZE = 1024;

#endif	//	DEFINE_HPP