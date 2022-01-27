#include "../inc/utils.h"

void send_image(int socket, char *file) {
    int size, read_size;
    int stat;
    char send_buffer[10240];
    FILE *picture = fopen(file, "rb");
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    send(socket, &size, sizeof(int), 0);

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

int recv_image(int socket, char *path) {
    int  recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char imagearray[10241];
    FILE *image;

    //Find the size of the image
    stat = recv(socket, &size, sizeof(int), 0);
    while (stat < 0) {
        stat = recv(socket, &size, sizeof(int), 0);
    }

    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf("\n");

    printf("Reply sent\n");

    image = fopen(path, "wb");

    if(!image) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1; 
    }

    //Loop while we have not received the entire file yet
    while(recv_size < size) {
        read_size = recv(socket,imagearray, 10241, 0);
        while(read_size <0) {
            read_size = recv(socket,imagearray, 10241, 0);
        }

        printf("Packet number received: %i\n",packet_index);
        printf("Packet size: %i\n",read_size);

        //Write the currently read data into our image file
        write_size = fwrite(imagearray,1,read_size, image);
        printf("Written image size: %i\n",write_size); 

        if(read_size !=write_size) {
            printf("error in read write\n");    
        }

        recv_size += read_size;
        packet_index++;
        printf("Total received image size: %i\n",recv_size);
        printf(" \n");
        printf(" \n");

    }

    fclose(image);
    
    printf("Image successfully Received!\n");
    char buf[512 + 32] = {0};
    sprintf(buf, "<image loaded>");
    printf("%d", send_all(socket, buf, 512 + 32)); 
    
    return 1;
}

