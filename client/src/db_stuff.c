#include "../inc/uch_client.h"

void create_user_db(char *login, char *password) {
    struct stat buffer;
    char *db_name = NULL;
    char *sql_pattern = "%s/%s.db";
    asprintf(&db_name, sql_pattern, login, login);
    int exist = stat(db_name, &buffer);
    if (exist != 0) {
        sqlite3 *db;
        int rc = sqlite3_open(db_name, &db);
        char *sql = NULL, *err_msg = 0;
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }

        char *query = NULL;
        sql_pattern = "INSERT INTO user (login, password) VALUES ('%s', '%s');";
        asprintf(&query, sql_pattern, login, password);

        sql = mx_strrejoin(sql, "PRAGMA encoding = \"UTF-8\";");
        sql = mx_strrejoin(sql, "CREATE TABLE user (login TEXT NOT NULL, password TEXT NOT NULL);");
        //sql = mx_strrejoin(sql, "CREATE TABLE chats (id INTEGER PRIMARY KEY, name TEXT NOT NULL, members INTEGER NOT NULL);");
        //sql = mx_strrejoin(sql, "CREATE TABLE members (id INTEGER PRIMARY KEY, chat_id INT NOT NULL, user_id INT NOT NULL, admin BOOLEAN NOT NULL DEFAULT FALSE);");
        //sql = mx_strrejoin(sql, "CREATE TABLE messages (id INTEGER PRIMARY KEY, chat_id INT NOT NULL, user_id INT NOT NULL, date DATETIME NOT NULL, text TEXT DEFAULT NULL);");
        sql = mx_strrejoin(sql, query);

        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        mx_strdel(&sql);
        mx_strdel(&query);
        mx_strdel(&sql_pattern);
        sqlite3_close(db);
    }
}
