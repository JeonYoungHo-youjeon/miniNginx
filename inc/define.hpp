#ifndef DEFINE_HPP
#define DEFINE_HPP

enum eType
{
	SERVER = 0,
	CLIENT
};

enum eState
{
	READY_REQUEST = 0,
	READ_REQUEST,
	DONE_REQUEST,
	READ_RESPONSE,
	WRITE_RESPONSE,
	DONE_RESPONSE
};

static const std::string HEAD[] =
{
	"transfer-encoding",
	"content-length",
	"connection"
};

enum eHeader
{
	TRANSFER_ENCODING,
	CONTENT_LENGTH,
	CONNECTION
};


enum eProg
{
	READY,
	START_LINE,
	HEADER,
	HEADER_SET,
	CRLF,
	BODY,
	CHUNK_SIZE,
	CHUNK_DATA,
	PROG_DONE
};

static const time_t		TIMEOUT = 30;
static const int		MAX_EVENT = 1024; // TODO: I don't know optimal MAX_EVENT yet.
const int				BUFFER_SIZE = 8192;
const int				MAX_BODY_SIZE = 8192;

#endif	//	DEFINE_HPP