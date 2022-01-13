#include "../inc/uch_client.h"

void *sender_func(void *param) {
    t_client *cur_client = (t_client *)param;
    //char message[512];
    char *message = (char *)malloc(512);
    char buf[512 + 32];
    fseek(stdin,0,SEEK_END);
    while(1) {
        printf("> ");
        fflush(stdout);
        size_t len;
        getline(&message, &len, stdin);
        //scanf("%s", message);
        message = mx_strtrim(message);
        if (mx_strcmp(message, "users") == 0) {
            send(cur_client->cl_socket, message, mx_strlen(message) - 1, 0);
        }
        else if (mx_strcmp(message, "exit") == 0) {
            send(cur_client->cl_socket, message, 4, 0);
            break;
        }
        else {
            sprintf(buf, "%s: %s\n", cur_client->name, message);
            send(cur_client->cl_socket, buf, mx_strlen(buf) - 1, 0);
        }
        clear_message(message, 512);
        clear_message(buf, 512 + 32);

    }
    return NULL;
}

void send_jpeg(int socket, char *file) {
    int size, read_size;
    int stat;
    char send_buffer[10240], read_buffer[256];
    FILE *picture = fopen(file, "rb");
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    send(socket, &size, sizeof(int), 0);

    //Send Picture as Byte Array
    stat=recv(socket, &read_buffer , 255, 0);
    while (stat < 0) { //Read while we get errors that are due to signals.
       stat = recv(socket, &read_buffer , 255, 0);
    } 

    while(!feof(picture)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

       //Send data through our socket
       stat = send(socket, send_buffer, read_size, 0);
       while (stat < 0){
         stat = send(socket, send_buffer, read_size, 0);
       }

       //Zero out our send buffer
       clear_message(send_buffer, sizeof(send_buffer));
    }
}

void *rec_func(void *param) {
    int fd = *(int *)param;
    char message[512] = {0};
    while (1) {
		int receive = recv(fd, message, 512, 0);
        if (receive > 0) {
            printf("%s\n", message);
            printf("> ");
            fflush(stdout);
        } else if (receive == 0) {
            break;
        } else {
                // -1
		}
		clear_message(message, 512);
    }
    return NULL;

}


int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    
    // Переменные для авторизации
    char login[32];
    char password[16];
    char choise; // i - вход, u - регистрация

    printf("Sign in or sign up? (i / u)\n");

    scanf("%c", &choise); // все сканфы убрать, оставил для наглядности, как считывает в терминале
    switch(choise) {
        case 'u':
        case 'i':
            printf("Enter login: ");
            scanf("%s",login);

            printf("Enter password: ");
            scanf("%s",password);

            break;
        default:
            mx_printerr("Invalid choise\n");
            exit(-1);
            break;
    }

    // Подключение к серверу, тут ничего менять не надо
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[2]));
    connect(fd, (struct sockaddr *)&adr, sizeof(adr));
    inet_pton(AF_INET, argv[1], &adr.sin_addr); //"127.0.0.1"

    // Отправка данных для авторизации на сервер
    send(fd, &choise, 1, 0);
    send(fd, login, 32, 0);
    send(fd, password, 16, 0);
    printf("%c || %s || %s\n", choise, login, password);
    char *jpeg = "testp.png";//"test.jpg";
    //send_jpeg(fd, jpeg);
    bool err_aut;
    recv(fd, &err_aut, sizeof(bool), 0); // Ожидание ответа от сервера об успешности входа или регистрации

    // Обработка ошибок от сервера, нужно ввести заново поля
    while (err_aut) {
        switch (choise) {
            case 'u':
                printf("user with this login already exists\n");
                printf("Enter login: ");
                scanf("%s",login);

                printf("Enter password: ");
                scanf("%s",password);
                break;
            
            case 'i':
                printf("login or password is invalid\n");
                printf("Enter login: ");
                scanf("%s",login);

                printf("Enter password: ");
                scanf("%s",password);
                break;
        }
        // Снова отправляем данные на сервер
        send(fd, &choise, 1, 0);
        send(fd, login, 32, 0);
        send(fd, password, 16, 0);
        recv(fd, &err_aut, sizeof(bool), 0);
    }


    // Запуск потоков для приёма и отправки сообщений, будем смотреть. Может, придётся переделать под события из гтк
    pthread_t sender_th;
    pthread_t rec_th;
    t_client cur_client = {
        .adr = adr,
        .cl_socket = fd,
        .name = login
    };
    pthread_create(&sender_th, NULL, sender_func, &cur_client);
    pthread_create(&rec_th, NULL, rec_func, &fd);





    //ожидание завершения потоков, если нужно добавить код, то добавлять до этих функций
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(fd);
    
    return 0;
}
