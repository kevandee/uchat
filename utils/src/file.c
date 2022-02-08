#include "../inc/utils.h"

void send_file(SSL *socket, char *path, const char *mode) {
    int size, read_size;
    int stat;
    char send_buffer[10240];
    FILE *file = fopen(path, mode);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    SSL_write(socket, &size, sizeof(int));

    while(!feof(file)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, file);

       //Send data through our socket
       stat = SSL_write(socket, send_buffer, read_size);
       while (stat < 0){
         stat = SSL_write(socket, send_buffer, read_size);
       }

       //Zero out our send buffer
       clear_message(send_buffer, sizeof(send_buffer));
    }
}

int recv_file(SSL *socket, char *path, const char *mode) {
    
    int  recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char bitearray[10241];
    FILE *file;

    //Find the size of the file
    stat = SSL_read(socket, &size, sizeof(int));
    while (stat < 0) {
        stat = SSL_read(socket, &size, sizeof(int));
    }

    file = fopen(path, mode);

    if(!file) {
        printf("Error has occurred. File could not be opened\n");
        return -1; 
    }

    //Loop while we have not received the entire file yet
    while(recv_size < size) {
        read_size = SSL_read(socket,bitearray, 10241);
        while(read_size <0) {
            read_size = SSL_read(socket,bitearray, 10241);
        }

        //Write the currently read data into our file file
        write_size = fwrite(bitearray,1,read_size, file);
      
        if(read_size !=write_size) {
            printf("error in read write\n");    
        }

        recv_size += read_size;
        packet_index++;
    }

    fclose(file);
    
    return 1;
}

