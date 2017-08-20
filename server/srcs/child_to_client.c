#include "libft.h"
#include "server.h"
#include "websocket.h"

void			send_text_msg(wslay_event_context_ptr ctx, char *msg)
{
	struct wslay_event_msg msgarg;

	msgarg.opcode = WSLAY_TEXT_FRAME;
	msgarg.msg = (const uint8_t*)msg;
	msgarg.msg_length = strlen((const char*)msgarg.msg);
	wslay_event_queue_msg(ctx, &msgarg);
}
