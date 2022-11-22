#ifndef DEFINE_HPP
#define DEFINE_HPP

enum eType
{
	SERVER = 0,
	CLIENT
};

enum eState
{
	READ_REQUEST,
	END_REQUEST,
	READ_RESPONSE,
	WRITE_RESPONSE,
	SEND_RESPONSE,
	END_RESPONSE,
	REPEAT_REQUEST,
};

static const std::string HEAD[] =
{
	"transfer-encoding",
	"content-length",
	"connection",
	"cookie"
};

enum eHeader
{
	TRANSFER_ENCODING,
	CONTENT_LENGTH,
	CONNECTION,
	COOKIE
};


enum eProg
{
	READY,
	START_LINE,
	HEADER,
	CRLF,
	BODY,
	LENGTH_BODY,
	CHUNK_SIZE,
	CHUNK_DATA,
	DONE
};

static const time_t		TIMEOUT = 60;
static const int		MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
const int				BUFFER_SIZE = 8192;
const int				MAX_BODY_SIZE = 8192;
const int				SESSION_TIMEOUT = 120; // second

#endif	//	DEFINE_HPP