#include "../inc/uch_client.h"

void create_user_db(char *login) {

    sqlite3 *db;
    char *db_name = get_db_name(login);
    struct stat u_buffer;
    int exist = stat(db_name, &u_buffer);
    if (exist != 0) {
        int rc = sqlite3_open(db_name, &db);
        char *sql = NULL, *err_msg = 0;
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        sql = mx_strrejoin(sql, "PRAGMA encoding = \"UTF-8\";");
        sql = mx_strrejoin(sql, "CREATE TABLE user (id INTEGER PRIMARY KEY, login TEXT NOT NULL, password TEXT NOT NULL, name TEXT DEFAULT \".clear\", surname TEXT DEFAULT \".clear\", bio TEXT DEFAULT \".clear\", avatar TEXT DEFAULT \"default\", theme TEXT DEFAULT dark);");
        sql = mx_strrejoin(sql, "CREATE TABLE chats (id INTEGER PRIMARY KEY, name TEXT NOT NULL, members INTEGER NOT NULL);");
        sql = mx_strrejoin(sql, "CREATE TABLE members (id INTEGER PRIMARY KEY, chat_name TEXT NOT NULL, user_name TEXT NOT NULL);");
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

void *user_exec_db(char *login, char *query, int type) {
    sqlite3 *db;
    char *db_name = get_db_name(login);
    int rc = sqlite3_open(db_name, &db);
    char *err_msg = 0;
    t_list *list = NULL;
    int auto_inc;

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

char *get_db_name(char *login) {
    char *db_name = NULL;
    char *sql_pattern = "%s/%s.db";
    asprintf(&db_name, sql_pattern, login, login);
    return db_name;
}

void insert_user_db(t_client cur_client) {
    char *query = NULL;
    char *sql_pattern = "INSERT INTO user (login, password, name, surname, bio) VALUES ('%s', '%s', '%s', '%s', '%s');";
    asprintf(&query, sql_pattern, cur_client.login, cur_client.passwd, cur_client.name, cur_client.surname, cur_client.bio);
    user_exec_db(cur_client.login, query, 2);
    t_list *temp = cur_client.chats;
    while(temp != NULL && temp->data != NULL) {
        char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
        sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
        asprintf(&query, sql_pattern, chat_name, ((t_chat *)(temp->data))->count_users);
        user_exec_db(cur_client.login, query, 2);
        t_list *users = ((t_chat *)(temp->data))->users;
        while(users != NULL && users->data != NULL) {
            
            sql_pattern = "INSERT INTO members (chat_name, user_name) VALUES ('%s', '%s');";
            asprintf(&query, sql_pattern, chat_name, users->data);
            user_exec_db(cur_client.login, query, 2);
 
            users = users->next;
        }

        temp = temp->next;
    }

}

void insert_user_db(t_client cur_client) {
    char *query = NULL;
    char *sql_pattern = "INSERT INTO user (login, password, name, surname, bio) VALUES ('%s', '%s', '%s', '%s', '%s');";
    asprintf(&query, sql_pattern, cur_client.login, cur_client.passwd, cur_client.name, cur_client.surname, cur_client.bio);
    user_exec_db(cur_client.login, query, 2);
    t_list *temp = cur_client.chats;
    while(temp != NULL && temp->data != NULL) {
        char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
        sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
        asprintf(&query, sql_pattern, chat_name, ((t_chat *)(temp->data))->count_users);
        user_exec_db(cur_client.login, query, 2);
        t_list *users = ((t_chat *)(temp->data))->users;
        while(users != NULL && users->data != NULL) {
            
            sql_pattern = "INSERT INTO members (chat_name, user_name) VALUES ('%s', '%s');";
            asprintf(&query, sql_pattern, chat_name, users->data);
            user_exec_db(cur_client.login, query, 2);
 
            users = users->next;
        }

        temp = temp->next;
    }

}

/*void insert_user_db(t_client cur_client) {
    char *query = NULL;
    char *sql_pattern = "UPDATE user SET login = '%s', password = '%s', name = '%s', surname = '%s', bio = '%s';";
    asprintf(&query, sql_pattern, cur_client.login, cur_client.passwd, cur_client.name, cur_client.surname, cur_client.bio);
    user_exec_db(cur_client.login, query, 2);
    t_list *temp = cur_client.chats;
    while(temp != NULL && temp->data != NULL) {
        char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
        sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
        asprintf(&query, sql_pattern, chat_name, ((t_chat *)(temp->data))->count_users);
        user_exec_db(cur_client.login, query, 2);
        t_list *users = ((t_chat *)(temp->data))->users;
        while(users != NULL && users->data != NULL) {
            
            sql_pattern = "INSERT INTO members (chat_name, user_name) VALUES ('%s', '%s');";
            asprintf(&query, sql_pattern, chat_name, users->data);
            user_exec_db(cur_client.login, query, 2);
 
            users = users->next;
        }

        temp = temp->next;
    }

}*/

