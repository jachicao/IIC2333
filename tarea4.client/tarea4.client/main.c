#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

int g_socket = -1;

void* recv_thread(void* p_socket) {
    int socket = *(int*) p_socket;
    ssize_t length;
    while(true) {
        char* message = (char*) malloc(sizeof(char) * 1024);
        length = recv(socket, message, 1024, 0);
        if (length > 0) {
            printf("message received: %s\n", message);
        } else {
            break;
        }
    }
    if (length == 0) {
        printf("server disconnected\n");
    } else if (length == -1) {
        printf("recv failed\n");
    }
    free(p_socket);
    return EXIT_SUCCESS;
}

pthread_t create_recv_thread(int socket) {
    pthread_t thread;
    int* p_socket = (int*) malloc(1);
    *p_socket = socket;
    if (pthread_create(&thread, NULL, recv_thread, (void*) p_socket) == -1) {
        printf("pthread_create failed\n");
        exit(EXIT_FAILURE);
    }
    return thread;
}

pthread_t create(int port) {
    g_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_socket == -1) {
        printf("socket failed\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);
    if (connect(g_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
        printf("connect failed\n");
        exit(EXIT_FAILURE);
    }
    return create_recv_thread(g_socket);
}

int main(int argc, const char * argv[]) {
    pthread_t recv_thread = create(PORT);
    pthread_join(recv_thread, NULL);
    return EXIT_SUCCESS;
}
