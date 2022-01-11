#include "../inc/uch_server.h"

static void sqlite3_create_db() {
    sqlite3 *db;
    int rc = sqlite3_open("my.db", &db);
    char *sql = NULL, *err_msg = 0;
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sql = mx_strrejoin(sql, "PRAGMA encoding = \"UTF-8\";");
    sql = mx_strrejoin(sql, "CREATE TABLE `users` ( `id` INTEGER PRIMARY KEY AUTOINCREMENT , `name` VARCHAR(10) NOT NULL, `password` VARCHAR(16) NOT NULL);");
    sql = mx_strrejoin(sql, "INSERT INTO 'users' (name, password) VALUES ('Dima', 'test1test');");
    //sql = mx_strrejoin(sql, "CREATE TABLE `members` ( `id` INTEGER PRIMARY KEY AUTOINCREMENT , `chat_id` INT NOT NULL , `user_id` INT NOT NULL , `admin` BOOLEAN NOT NULL DEFAULT FALSE );");
    //sql = mx_strrejoin(sql, "CREATE TABLE `messages` ( `id` INTEGER PRIMARY KEY AUTOINCREMENT , `message_id` INT NOT NULL , `chat_id` INT NOT NULL , `user_id` INT NOT NULL , `date` VARCHAR(10) NOT NULL , `time` VARCHAR(5) NOT NULL , `text` VARCHAR(1024) NULL DEFAULT NULL , `sticker_id` INT NULL DEFAULT NULL , `photo_id` INT NULL DEFAULT NULL );");
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

static int callback(void *data, int argc, char **argv, char **azColName) {
    (void)azColName;
    t_list **list_data = (t_list**)data;

    if (argc == 0)
        return 0;
    for (int i = 0; i < argc; i++) {
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if(argv[i] == NULL)
            mx_push_back(*(&list_data), strdup("NULL"));
        else
            mx_push_back(*(&list_data), strdup(argv[i]));
    }
    return 0;
}

static void sqlite3_open_db() {
    struct stat buffer;
    int exist = stat("my.db", &buffer);
    if (exist != 0)
        sqlite3_create_db();
}

void *sqlite3_exec_db(char *query, int type) {
    sqlite3_open_db();
    sqlite3 *db;
    int rc = sqlite3_open("my.db", &db);
    char *err_msg = 0;
    t_list *list = NULL;
    int auto_inc;

    // printf("database_request: %s\n", query);
    if (type == 2) {
        rc = sqlite3_exec(db, query, callback, &list, &err_msg);
        sqlite3_close(db);
        return list;
    }
    if (type == 1) {
        rc = sqlite3_exec(db, query, 0, 0, &err_msg);
        auto_inc = sqlite3_last_insert_rowid(db);
        sqlite3_close(db);
        int *id = &auto_inc;
        return id;
    }
    return NULL;
}
