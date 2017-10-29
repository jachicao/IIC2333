#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 50

int g_socket = -1;

void* recv_thread(void* p_socket) {
    int client_socket = *(int*) p_socket;
    printf("new client: %d\n", client_socket);
    ssize_t length;
    while(true) {
        char* message = (char*) malloc(sizeof(char) * 1024);
        length = recv(client_socket, message, 1024, 0);
        if (length > 0) {
            printf("message received: %s\n", message);
        } else {
            break;
        }
    }
    if (length == 0) {
        printf("client disconnected\n");
    } else if (length == -1) {
        printf("recv failed\n");
    }
    free(p_socket);
    return EXIT_SUCCESS;
}

void* accept_thread(void* p_socket) {
    int server_socket = *(int*) p_socket;
    int client_socket;
    while(true) {
        struct sockaddr_in client_address;
        memset(&client_address, '0', sizeof(client_address));
        socklen_t client_address_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_size);
        if (client_socket > 0) {
            pthread_t client_thread;
            int* p_client_socket = (int*) malloc(1);
            *p_client_socket = client_socket;
            if (pthread_create(&client_thread, NULL, recv_thread, (void*) p_client_socket) == -1) {
                printf("pthread_create failed\n");
                break;
            }
        } else {
            break;
        }
    }
    if (client_socket == -1) {
        printf("accept failed\n");
    }
    free(p_socket);
    return EXIT_SUCCESS;
}

pthread_t create_accept_thread(int socket) {
    pthread_t thread;
    int* p_socket = (int*) malloc(1);
    *p_socket = socket;
    if (pthread_create(&thread, NULL, accept_thread, (void*) p_socket) == -1) {
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
    memset(&address, '0', sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(g_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
        printf("bind failed\n");
        exit(EXIT_FAILURE);
    }
    if (listen(g_socket, MAX_CLIENTS) == -1) {
        printf("listen failed\n");
        exit(EXIT_FAILURE);
    }
    return create_accept_thread(g_socket);
}

int main(int argc, const char * argv[]) {
    pthread_t accept_thread = create(PORT);
    pthread_join(accept_thread, NULL);
    return EXIT_SUCCESS;
}
