// include header files for a simple socket server
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    const int PORT = 9099;
    const int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int other_descriptor_bounce = 0;
    struct sockaddr_in server_address;
    struct sockaddr* server_as_sockaddr_casted = &server_address;
    bzero(&server_address, sizeof(server_address));
    const char* bindTo = "127.0.0.1";
    if(serverSocketDescriptor < 0)
    {
        fprintf(stderr, "Could not create socket\n");
        exit(1);
    }
    server_address.sin_addr.s_addr = inet_addr(bindTo);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    if(bind(serverSocketDescriptor, server_as_sockaddr_casted, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "Could not bind at %s:%d\n", bindTo, PORT);
        exit(1);
    }
    if(listen(serverSocketDescriptor, 5) < 0)
    {
        fprintf(stderr, "Could not listen\n");
        exit(1);
    }
    printf("Currently listening at %s:%d\n", bindTo, PORT);
    for(;;)
    {
         int serverSize = sizeof(server_address);
        char buffer[255];
        int buffer_incoming_descriptor = accept(serverSocketDescriptor, server_as_sockaddr_casted, &serverSize);
        fprintf(stdout, "Accepting..\n");
        if (other_descriptor_bounce != 0 && buffer_incoming_descriptor > 0)
        {
            // other_descriptor_bounce = buffer_incoming_descriptor;
            // switch
            if(read(buffer_incoming_descriptor, buffer, 255) < 0)
            {
                fprintf(stderr, "Could not write inside\n");
                break;
            }
            else
            {
                buffer[strlen(buffer)] = '\0';
                printf("message: %s\n", buffer);
            }
            if(send(other_descriptor_bounce, buffer, 255, 0) < 0)
            {
                fprintf(stderr, "Could not bounce back\n");
                break;
            }
            else
            {
                fprintf(stdout, "Successfully sent!\n");
            }
        }
        else if (other_descriptor_bounce == 0 && buffer_incoming_descriptor > 0)
        {
            other_descriptor_bounce = buffer_incoming_descriptor;
            if(send(other_descriptor_bounce, "notifier=1", 12, 0) < 0)
            {
                fprintf(stderr, "Couldn't send bounce-back message");
            }
            else
            {
                fprintf(stdout,"Elevated role\n");
            }
        }

    }
    close(serverSocketDescriptor);
    printf("Safely closed everything\n");

}
