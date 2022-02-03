#include "../inc/uch_client.h"

void create_user_db(t_client cur_client) {
    sqlite3 *db;
    char *db_name = get_db_name(cur_client.login);
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
        sql = mx_strrejoin(sql, "CREATE TABLE user (id INTEGER NOT NULL, login TEXT NOT NULL, password TEXT NOT NULL, name TEXT DEFAULT \".clear\", surname TEXT DEFAULT \".clear\", bio TEXT DEFAULT \".clear\", avatar TEXT DEFAULT \"default\", theme TEXT DEFAULT dark);");
        sql = mx_strrejoin(sql, "CREATE TABLE chats (id INTEGER NOT NULL, name TEXT NOT NULL, members INTEGER NOT NULL);");
        sql = mx_strrejoin(sql, "CREATE TABLE members (id INTEGER PRIMARY KEY AUTOINCREMENT,  chat_id INTEGER NOT NULL, user_name TEXT NOT NULL);");
        sql = mx_strrejoin(sql, "CREATE TABLE messages (id INTEGER NOT NULL, chat_id INTEGER NOT NULL, user_name TEXT NOT NULL, text TEXT DEFAULT NULL, type TEXT DEFAULT text);");
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }

        insert_user_db(cur_client);


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
    struct stat st = {0};
    if (stat(login, &st) == -1) {
        mkdir(login, 0777);
    }
    char *db_name = NULL;
    char *sql_pattern = "%s/%s.db";
    asprintf(&db_name, sql_pattern, login, login);
    
    return db_name;
}


void insert_user_db(t_client cur_client) {
    user_exec_db(cur_client.login, "DELETE FROM user; DELETE FROM chats; DELETE FROM members;", 2);
    char *query = NULL;
    char *buff = NULL;
    asprintf(&buff, "path=%s scaled_w=%f scaled_h=%f x=%f y=%f ", cur_client.avatar.path, cur_client.avatar.scaled_w, cur_client.avatar.scaled_h,cur_client.avatar.x, cur_client.avatar.y);
    char *sql_pattern = "INSERT INTO user (id, login, password, name, surname, bio) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s');";
    asprintf(&query, sql_pattern, cur_client.id, cur_client.login, cur_client.passwd, cur_client.name, cur_client.surname, cur_client.bio, buff);
    user_exec_db(cur_client.login, query, 2);
    t_list *temp = cur_client.chats;
    while(temp != NULL && temp->data != NULL) {
        char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
        sql_pattern = "INSERT INTO chats (id, name, members) VALUES (%d, '%s', %d);";
        asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id, chat_name, ((t_chat *)(temp->data))->count_users);
        user_exec_db(cur_client.login, query, 2);
        t_list *users = ((t_chat *)(temp->data))->users;
        while(users != NULL && users->data != NULL) {
            sql_pattern = "INSERT INTO members (chat_id, user_name) VALUES (%d, '%s');";
            asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id, users->data);
            user_exec_db(cur_client.login, query, 2);
 
            users = users->next;
        }
        temp = temp->next;
    }
}

/*void update_user_db(t_client cur_client) {
    char *query = NULL;
    //USER INFO UPDATE
    char *sql_pattern = "UPDATE user SET login = '%s', password = '%s', name = '%s', surname = '%s', bio = '%s';";
    asprintf(&query, sql_pattern, cur_client.login, cur_client.passwd, cur_client.name, cur_client.surname, cur_client.bio);
    user_exec_db(cur_client.login, query, 2);
    //USER CHATS INFO UPDATE OR INSERT
    t_list *temp = cur_client.chats;
    while(temp != NULL && temp->data != NULL) {
        sql_pattern = "SELECT EXISTS (SELECT id FROM chats WHERE id = (%d));";
        asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id);
        t_list *list = sqlite3_exec_db(query, 1);
        if (strcmp(list->data, "1") == 0) {
            //USER CHATS INFO UPDATE
            char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
            sql_pattern = "UPDATE chats SET name = '%s', members = (%d) WHERE id = (%d);";
            asprintf(&query, sql_pattern, chat_name, ((t_chat *)(temp->data))->count_users, ((t_chat *)(temp->data))->id);
            user_exec_db(cur_client.login, query, 2);
        }
        else {
            //USER CHATS INFO INSERT
            char *chat_name = mx_strdup(((t_chat *)(temp->data))->name);
            sql_pattern = "INSERT INTO chats (id, name, members) VALUES (%d, '%s', %d);";
            asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id, chat_name, ((t_chat *)(temp->data))->count_users);
            user_exec_db(cur_client.login, query, 2);
        }

        t_list *users = ((t_chat *)(temp->data))->users;
            while(users != NULL && users->data != NULL) {
                //USER CHAT MEMBERS INFO UPDATE
                sql_pattern = "SELECT EXISTS (SELECT id FROM members WHERE chat_id = (%d) AND user_name = ('%s'));";
                asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id);
                t_list *list = sqlite3_exec_db(query, 1);
                if (strcmp(list->data, "0") == 0) {
                    //USER CHAT MEMBERS INFO INSERT
                    sql_pattern = "INSERT INTO members (chat_id, user_name) VALUES (%d, '%s');";
                    asprintf(&query, sql_pattern, ((t_chat *)(temp->data))->id, users->data);
                    user_exec_db(cur_client.login, query, 2);
                }
                
                DELETE FROM members
                users = users->next;
            }
        temp = temp->next;
    }

}*/

