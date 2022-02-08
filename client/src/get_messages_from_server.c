#include "../inc/uch_client.h"

void get_messages_from_server(int c_id, int mes_id) {
    char buf[512 + 32] = {0};

    sprintf(buf, "<get messages chat_id=%d, last_mes=%d>", c_id, mes_id);
    swiftchat_send(cur_client.ssl, buf, 512+32);
}
