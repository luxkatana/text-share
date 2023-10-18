// include header files for a simple socket server
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

struct ThreadArgs
{
    struct sockaddr* casted_server;
    int serverSocketDescriptor;
    int *bouncing_descriptor;
    int buffer_descriptor;
    int new_one;
};
void *accept_thread(void *args)
{
    struct ThreadArgs *arguments = (struct ThreadArgs*) args;
    struct sockaddr* server = arguments->casted_server;
    int sizeofserver = sizeof(*server);
    arguments->new_one = 0;
    while(1)
    {
        int descripter = accept(arguments->serverSocketDescriptor, server, &sizeofserver);
        printf("%d\n", descripter);
        if(descripter < 0)
        {
            fprintf(stderr, "Could not Accept\n");
        }
    arguments->buffer_descriptor = descripter;
    arguments->new_one = 1;
        

    }

    
}
typedef struct {
    int send_on_success; // file descriptor
    int me;
}BroadcastingStruct ;
void* broadcast_thread(void* args)
{
    BroadcastingStruct *data = (BroadcastingStruct*) args;
    for(;;)
    {
        char buffer[256];
        int received_bytes = read((*data).me, buffer, sizeof(buffer));
        if(received_bytes < 0)
        {
            fprintf(stderr, "Couldn't receive\n");
            continue;
        }
        buffer[received_bytes] = '\0';
        printf("received %s\n", buffer);
        if(send((*data).send_on_success, buffer, strlen(buffer), 0) < 0)
        {
            fprintf(stderr, "Couldn't send data back\n");
        }
        else
        {
            printf("Sent %s\n", buffer);
        }
    }


}
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
    listen(serverSocketDescriptor, 5);
    printf("Currently listening at %s:%d\n", bindTo, PORT); 
    pthread_t threading;
    struct ThreadArgs ThreadingArguments;
    ThreadingArguments.bouncing_descriptor = &other_descriptor_bounce;
    ThreadingArguments.casted_server = server_as_sockaddr_casted;
    ThreadingArguments.serverSocketDescriptor = serverSocketDescriptor;

    pthread_create(&threading,NULL, accept_thread, (void*)&ThreadingArguments);
    printf("Daemon started\n");
    for(;;)
    {
        char buffer[255];
        int serverSize = sizeof(server_address);
        bzero(buffer, sizeof(buffer));
        int buffer_incoming_descriptor = ThreadingArguments.buffer_descriptor;
        // int buffer_incoming_descriptor = 0;
        // printf("%d\n", buffer_incoming_descriptor);
        if (ThreadingArguments.new_one == 1 && other_descriptor_bounce != 0 && buffer_incoming_descriptor > 0)
        {
            // other_descriptor_bounce = buffer_incoming_descriptor;
            // switch
            int bytes_recv = read(buffer_incoming_descriptor, buffer, 255);
            if(bytes_recv < 0)
            {
                fprintf(stderr, "Could not write inside\n");
                break;
            }
            else
            {
                buffer[bytes_recv] = '\0';
                printf("message: %s\n", buffer);
            }
            if(send(buffer_incoming_descriptor, buffer, strlen(buffer), 0) < 0)
            {
                fprintf(stderr, "Could not echo back for verification\n");
                break;
            }
            printf("Echo success %s\n", buffer);
            if(send(other_descriptor_bounce, buffer, strlen(buffer), 0) < 0)
            {
                fprintf(stderr, "Could not bounce back\n");
                break;
            }
            else
            {
                fprintf(stdout, "Successfully sent!\n");
                ThreadingArguments.new_one = 0;
                BroadcastingStruct receive_this;
                receive_this.me = buffer_incoming_descriptor;
                receive_this.send_on_success = *(ThreadingArguments.bouncing_descriptor);
                pthread_t thread_for_broadcasting;

                pthread_create(&thread_for_broadcasting, NULL, broadcast_thread, (void*)&receive_this);
            }
        }
        else if (ThreadingArguments.new_one == 1 && other_descriptor_bounce == 0 && buffer_incoming_descriptor > 0)
        {
            other_descriptor_bounce = buffer_incoming_descriptor;
            if(send(other_descriptor_bounce, "notifier=1", 11, 0) < 0)
            {
                fprintf(stderr, "Couldn't send bounce-back message\n");
            }
            else
            {
                fprintf(stdout,"Elevated role\n");
                ThreadingArguments.new_one = 0;
            }
        }

    }
    close(serverSocketDescriptor);
    printf("Safely closed everything\n");

}
