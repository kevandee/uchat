#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

t_list *users_list;

void *client_work(void *param) {
    t_client *cur = (t_client *)param;
    char name[NAME_LEN];
    char buff_out[MAX_LEN + NAME_LEN];
    bool is_run = true;
    // Name
	if(recv(cur->cl_socket, name, NAME_LEN, 0) <= 0 || mx_strlen(name) <  2 || mx_strlen(name) >= 32){
		printf("Didn't enter the name.\n");
	} else{
		cur->name=mx_strdup(name);
        sprintf(buff_out, "%s has joined\n", cur->name);
		printf("%s", buff_out);
		send_message(buff_out,name);
	}

    char message[MAX_LEN + NAME_LEN];
    while (is_run) {
        int mes_stat = recv(cur->cl_socket, message, MAX_LEN + NAME_LEN, 0);

        if (mes_stat == 0 || (mx_strcmp(message, "exit") == 0)) {
            sprintf(buff_out, "%s has left\n", cur->name);
			printf("%s", buff_out);
            is_run = false;
        }
        else if (mx_strcmp(message, "users") == 0) {
            char *table = NULL;
            t_list *users_tmp = users_list;
            int i = 1;
            while (users_tmp) {
                t_client *cl_tmp = (t_client *)(users_tmp->data);
                if (cl_tmp->name) {
                    char *num = mx_itoa(i);
                    table = mx_strjoin(table, num);
                    mx_strdel(&num);
                    table = mx_strjoin(table, ". ");
                    table = mx_strjoin(table, cl_tmp->name);
                    table = mx_strjoin(table, "\n");
                    i++;
                }

                users_tmp = users_tmp->next;
            }
            send(cur->cl_socket, table, mx_strlen(table), 0);
            clear_message(message, MAX_LEN + NAME_LEN);
            continue;
        }
        else if (mes_stat > 0) {
            printf("Message Received from %s\n", name);
		    send_message(message, name);

            clear_message(message, MAX_LEN + NAME_LEN);
        }
        
    }
    close(cur->cl_socket);
    mx_strdel(&cur->name);
    free(cur);
    cur = NULL;
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2) {
        mx_printerr("usage: ./uchat_server <port>\n");
        return -1;
    }
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[1]));
    if (bind(serv_fd, (struct sockaddr *) &adr, sizeof(struct sockaddr_in)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    if (listen(serv_fd, 5) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
	}

    socklen_t adrlen = sizeof(struct sockaddr_in);
    int client_fd;
    pthread_t thread;
    users_list = NULL;
    pthread_mutex_init(&send_mutex, NULL);
    while(1) {
        client_fd = accept(serv_fd, (struct sockaddr *) &adr, &adrlen);
        
        t_client *new_client = (t_client *)malloc(sizeof(t_client));
        new_client->adr = adr;
        new_client->cl_socket = client_fd;
        new_client->name = NULL;

        mx_push_back(&users_list, new_client);

        pthread_create(&thread, NULL, client_work, (void *)new_client);
        sleep(1);
    }

    close(client_fd);
    close(serv_fd);
    return 0;
}
