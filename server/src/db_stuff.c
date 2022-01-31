#include "../inc/uch_server.h"

void sqlite3_create_db() {
    struct stat buffer;
    int exist = stat("server.db", &buffer);
    if (exist != 0) {
        sqlite3 *db;
        int rc = sqlite3_open("server.db", &db);
        char *sql = NULL, *err_msg = 0;
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        sql = mx_strrejoin(sql, "PRAGMA encoding = \"UTF-8\";");
        sql = mx_strrejoin(sql, "CREATE TABLE users (id INTEGER PRIMARY KEY, login TEXT NOT NULL, password TEXT NOT NULL, name TEXT DEFAULT \".clear\", surname TEXT DEFAULT \".clear\", bio TEXT DEFAULT \".clear\", avatar TEXT DEFAULT \"default\", theme TEXT DEFAULT dark);");
        sql = mx_strrejoin(sql, "CREATE TABLE chats (id INTEGER PRIMARY KEY, name TEXT NOT NULL, members INTEGER NOT NULL);");
        sql = mx_strrejoin(sql, "CREATE TABLE members (id INTEGER PRIMARY KEY, chat_id INT NOT NULL, user_id INT NOT NULL, admin BOOLEAN NOT NULL DEFAULT FALSE);");
        sql = mx_strrejoin(sql, "CREATE TABLE messages (id INTEGER PRIMARY KEY, chat_id INT NOT NULL, user_id INT NOT NULL, text TEXT DEFAULT NULL, type TEXT DEFAULT text);");
        sql = mx_strrejoin(sql, "INSERT INTO users (login, password) VALUES ('Dima123', 'Dimapassword');");
        sql = mx_strrejoin(sql, "INSERT INTO users (login, password) VALUES ('Fibbs123', 'Mafilirkan');");
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        mx_strdel(&sql);
        sqlite3_close(db);
    }
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    (void)azColName;
    t_list **list_data = (t_list**)data;

    if (argc == 0)
        return 0;
    for (int i = 0; i < argc; i++) {
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if(argv[i] == NULL)
            mx_push_back(*(&list_data), strdup("NULL"));
        else
            mx_push_back(*(&list_data), strdup(argv[i]));
    }
    return 0;
}

void *sqlite3_exec_db(char *query, int type) {
    sqlite3 *db;
    int rc = sqlite3_open("server.db", &db);
    char *err_msg = 0;
    t_list *list = NULL;
    int auto_inc;

    //printf("database_request: %s\n", query);
    if (type == 1) {
        rc = sqlite3_exec(db, query, callback, &list, &err_msg);
        sqlite3_close(db);
        return list;
    }
    if (type == 2) {
        rc = sqlite3_exec(db, query, 0, 0, &err_msg);
        auto_inc = sqlite3_last_insert_rowid(db);
        sqlite3_close(db);
        int *p = &auto_inc;
        return p;
    }
    return NULL;
}

int get_user_id(char *login) {
    char *query = NULL;
    char *sql_pattern = "SELECT id FROM users WHERE login = ('%s');";
    asprintf(&query, sql_pattern, login);
    t_list *list = sqlite3_exec_db(query, 1);
    int u_id = mx_atoi(list->data);
    return u_id;
}

t_list  *get_user_names(int id) {
    char *query = NULL;
    char *sql_pattern = "SELECT name, surname FROM users WHERE id = (%d);";
    asprintf(&query, sql_pattern, id);
    t_list *list = sqlite3_exec_db(query, 1);
    return list;
}

char  *get_user_login(int id) {
    char *query = NULL;
    char *sql_pattern = "SELECT login FROM users WHERE id = (%d);";
    asprintf(&query, sql_pattern, id);
    t_list *list = sqlite3_exec_db(query, 1);
    char *login = list->data;
    return login;
}

char  *get_user_bio(int id) {
    char *query = NULL;
    char *sql_pattern = "SELECT bio FROM users WHERE id = (%d);";
    asprintf(&query, sql_pattern, id);
    t_list *list = sqlite3_exec_db(query, 1);
    char *login = list->data;
    return login;
}

char  *get_user_avatar(int id) {
    char *query = NULL;
    char *sql_pattern = "SELECT avatar FROM users WHERE id = (%d);";
    asprintf(&query, sql_pattern, id);
    t_list *list = sqlite3_exec_db(query, 1);
    char *login = list->data;
    return login;
}

int get_chat_id(char *name) {
    char *query = NULL;
    char *sql_pattern = "SELECT id FROM chats WHERE name = ('%s');";
    asprintf(&query, sql_pattern, name);
    t_list *list = sqlite3_exec_db(query, 1);
    int c_id = mx_atoi(list->data);
    return c_id;
}

t_list *get_chats_id (int u_id) {
    char *query = NULL;
    char *sql_pattern = "SELECT chat_id FROM members WHERE user_id = (%d);";
    asprintf(&query, sql_pattern, u_id);
    t_list *list = sqlite3_exec_db(query, 1);
    return list;
}

int get_chat_members(int c_id) {
    char *query = NULL;
    char *sql_pattern = "SELECT members FROM chats WHERE id = (%d);";
    asprintf(&query, sql_pattern, c_id);
    t_list *list = sqlite3_exec_db(query, 1);
    int mc = mx_atoi(list->data);
    return mc;
}

t_list *get_chat_users(int c_id) {
    char *query = NULL;
    char *sql_pattern = "SELECT login FROM users INNER JOIN members ON users.id=members.user_id WHERE members.chat_id = (%d);";
    asprintf(&query, sql_pattern, c_id);
    t_list *list = sqlite3_exec_db(query, 1);
    return list;
}

t_chat *chat_info (int c_id) {
    t_chat *chat = (t_chat *)malloc(sizeof(t_chat));
    chat->id = c_id;
    char *query = NULL;
    char *sql_pattern = "SELECT name, members FROM chats WHERE id = (%d);";
    asprintf(&query, sql_pattern, c_id);
    t_list *list = sqlite3_exec_db(query, 1);
    char *temp = mx_strdup(list->data);
    mx_strcpy(chat->name, temp);
    chat->count_users = mx_atoi(list->next->data);
    chat->users = get_chat_users(c_id);
    return chat;
}

t_client *get_user_info(int id) {
    t_client *user = (t_client *)malloc(sizeof(t_client));

    user->login = get_user_login(id);
    t_list *names = get_user_names(id);
    char *bio = get_user_bio(id);
    if (names){
        mx_strcpy(user->name, names->data);
    }
    else {
        mx_strcpy(user->name, ".clear");
    }
    if (names->next) {
        mx_strcpy(user->surname, names->next->data);
    }
    else {
        mx_strcpy(user->surname, ".clear");
    }
    if (!bio) {
        mx_strcpy(user->bio, ".clear");
    }
    else {
        mx_strcpy(user->bio, bio);
    }

    char *avatar_info = get_user_avatar(id);
    if (mx_strcmp (avatar_info, "default") == 0) {
        user->avatar.path = mx_strdup(avatar_info);
    } 
    else {
        char *temp = avatar_info + 5;
        int len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        user->avatar.path = mx_strndup(avatar_info + 5, len);
        temp = mx_strstr(avatar_info, "scaled_w=") + 9;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        //char *num = mx_strndup(temp, len);
        char *end = temp + len;
        user->avatar.scaled_w = strtod(temp, &end);
        //mx_strdel(&num);

        temp = mx_strstr(avatar_info, "scaled_h=") + 9;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        char *num = mx_strndup(temp, len);
        end = temp + len;
        user->avatar.scaled_h = strtod(temp, &end);
        mx_strdel(&num);

        temp = mx_strstr(avatar_info, "x=") + 2;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        num = mx_strndup(temp, len);
        end = temp + len;
        user->avatar.x = strtod(temp, &end);
        mx_strdel(&num);

        temp = mx_strstr(avatar_info, "y=") + 2;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        num = mx_strndup(temp, len);
        end = temp + len;
        user->avatar.y = strtod(temp, &end);
        mx_strdel(&num);
    }
    t_list *chats_id = get_chats_id(id);
    t_list *chats_info = NULL;
    int i = 0;
    while(chats_id != NULL && chats_id->data != NULL) {
        i++;
        mx_push_back(&chats_info, chat_info(mx_atoi(chats_id->data)));
        chats_id = chats_id->next;
    
    }
    user->chat_count = i;
    user->chats = chats_info;
    return user;
 }


t_list *db_messages_sender(int c_id) {
    char *query = NULL;
    char *sql_pattern = "SELECT * FROM messages WHERE chat_id = (%d) ORDER BY id DESC LIMIT 250;";
    asprintf(&query, sql_pattern, c_id);
    t_list *list = sqlite3_exec_db(query, 1);
    t_list *temp = NULL;
    while (list->data != NULL && list != NULL) {
        
        t_message *mess = (t_message *)malloc(sizeof(t_message));
        mess->id = mx_atoi(list->data);
        list = list->next;
        mess->c_id = mx_atoi(list->data);
        list = list->next;
        mx_strcpy(mess->sender, get_user_login(mx_atoi(list->data)));
        list = list->next;
        mx_strcpy(mess->data, list->data);
        list = list->next;
        mx_strcpy(mess->type, list->data);
        mx_push_back(&temp, mess);
        if (list->next == NULL) {
            break;
        }
        list = list->next;
    }
    return temp;
}
