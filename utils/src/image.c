#include "../inc/utils.h"

void send_image(SSL *socket, char *file) {
    int size, read_size;
    int stat;
    char send_buffer[10240];
    FILE *picture = fopen(file, "rb");
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    SSL_write(socket, &size, sizeof(int));

    while(!feof(picture)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

       //Send data through our socket
       stat = SSL_write(socket, send_buffer, read_size);
       while (stat < 0){
         stat = SSL_write(socket, send_buffer, read_size);
       }

       //Zero out our send buffer
       clear_message(send_buffer, sizeof(send_buffer));
    }
}

int recv_image(SSL *socket, char *path) {
    int  recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char imagearray[10241];
    FILE *image;

    //Find the size of the image
    stat = SSL_read(socket, &size, sizeof(int));
    while (stat < 0) {
        stat = SSL_read(socket, &size, sizeof(int));
    }
    

    image = fopen(path, "wb");

    if(!image) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1; 
    }

    //Loop while we have not received the entire file yet
    while(recv_size < size) {
        read_size = SSL_read(socket,imagearray, 10241);
        while(read_size <0) {
            read_size = SSL_read(socket,imagearray, 10241);
        }

        //Write the currently read data into our image file
        write_size = fwrite(imagearray,1,read_size, image);
      
        if(read_size !=write_size) {
            printf("error in read write\n");    
        }

        recv_size += read_size;
        packet_index++;
    }

    fclose(image);
    
    return 1;
}

