#include "../inc/uch_server.h"

void change_chat_by_id(int id, t_client *cur) {
    char *query = NULL;
    char *sql_pattern = NULL;
    t_list *list = NULL;
    sql_pattern = "SELECT EXISTS (SELECT name FROM chats WHERE id=('%d'));";
    asprintf(&query, sql_pattern, id);
    list = sqlite3_exec_db(query, 1);
    if (strcmp(list->data, "1") == 0) {
        int c_id = id;
        sql_pattern = "SELECT name FROM chats WHERE id=('%d');";
        asprintf(&query, sql_pattern, id);
        list = sqlite3_exec_db(query, 1);
        char *c_name = mx_strdup(list->data);
        int u_id = get_user_id(cur->login);
        sql_pattern = "SELECT EXISTS (SELECT id FROM members WHERE chat_id=(%d) AND user_id=(%d));";
        asprintf(&query, sql_pattern, c_id, u_id);
        list = sqlite3_exec_db(query, 1);
        printf("check %s\n", list->data);
        if (strcmp(list->data, "1") == 0) {
            //
            mx_strcpy(cur->cur_chat.name, c_name);
            cur->cur_chat.id = c_id;
            cur->cur_chat.count_users = get_chat_members(cur->cur_chat.id);
            cur->cur_chat.users = get_chat_users(cur->cur_chat.id);
            //
            printf("cur name: %s\n", cur->cur_chat.name);
            printf("1 user: %s\n", cur->cur_chat.users->data);
            printf("2 user: %s\n", cur->cur_chat.users->next->data);
        }
        else {
            //NOT CHAT MEMBER
        }
    }
    else {
        //NO SUCH CHAT
    }
}
