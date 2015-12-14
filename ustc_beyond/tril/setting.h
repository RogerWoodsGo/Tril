#ifndef SETTING_H_
#define SETTING_H_

namespace ustc_beyond {
namespace tril {
#define BV(x) (1 << x)

typedef enum { HANDLER_UNSET,
HANDLER_GO_ON,
HANDLER_FINISHED,
HANDLER_COMEBACK,
HANDLER_WAIT_FOR_EVENT,
HANDLER_ERROR,
HANDLER_WAIT_FOR_FD
} handler_t;

typedef enum {
	CON_STATE_CONNECT,
	CON_STATE_REQUEST_START,
	CON_STATE_READ,
	CON_STATE_REQUEST_END,
	CON_STATE_READ_POST,
	CON_STATE_HANDLE_REQUEST,
	CON_STATE_RESPONSE_START,
	CON_STATE_WRITE,
	CON_STATE_RESPONSE_END,
	CON_STATE_ERROR,
	CON_STATE_CLOSE
} connection_state_t;

}
}
#endif
