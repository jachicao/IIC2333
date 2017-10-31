#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

#include "ListMessage.h"

#define PORT 8080
#define MAX_CLIENTS 80
#define HEARTBEAT_SLEEP 10
#define HEARTBEAT_TIMEOUT 30
typedef uint8_t byte_t;

volatile int g_server_socket = -1;
ListMessage* g_list = NULL;

pthread_mutex_t lock;

void on_destroy(int s) {
    if (g_server_socket != -1) {
        close(g_server_socket);
    }
    if (g_list != NULL) {
        list_message_destroy(g_list);
    }
    pthread_mutex_destroy(&lock);
    exit(EXIT_SUCCESS);
}

void send_heartbeat(int socket) {
    byte_t* message = (byte_t*) malloc(sizeof(byte_t) * 3);
    message[0] = Heartbeat;
    message[1] = 1;
    message[2] = 1;
    send(socket, message, 3, 0);
    free(message);
}

void on_receive(int socket, enum MessageType type, int payload_size, byte_t* payload) {
    printf("Received socket: %d, type: %d, size: %d, payload: ", socket, type, payload_size);
    if (payload_size > 0) {
        printf("[");
        for (int i = 0; i < payload_size; i++) {
            printf("%d, ", payload[i]);
        }
        printf("]");
    }
    printf("\n");
}

void* send_thread(void* p_socket) {
    int socket = *(int*) p_socket;
    time_t last_heartbeat_sent = 0;
    time_t last_heartbeat_received = time(NULL);
    while(socket > 0) {
        time_t current_time = time(NULL);
        if (last_heartbeat_sent - last_heartbeat_received >= HEARTBEAT_TIMEOUT) {
            printf("Timeout\n");
            socket = -1;
            break;
        }
        if (current_time - last_heartbeat_sent >= HEARTBEAT_SLEEP) {
            send_heartbeat(socket);
            last_heartbeat_sent = current_time;
        }
        pthread_mutex_lock(&lock);
        Message* message = g_list -> head;
        while (message != NULL) {
            Message* next = message -> next;
            if (message -> socket == socket) {
                on_receive(message -> socket, message -> type, message -> payload_size, message -> payload);
                switch (message -> type) {
                    case Heartbeat:
                        last_heartbeat_received = current_time;
                        break;
                    default:
                        break;
                }
                list_message_remove(g_list, message);
                message_destroy(message);
            }
            message = next;
        }
        pthread_mutex_unlock(&lock);
    }
    free(p_socket);
    return EXIT_SUCCESS;
}

pthread_t create_send_thread(int socket) {
    pthread_t thread;
    int* p_socket = (int*) malloc(sizeof(int));
    *p_socket = socket;
    if (pthread_create(&thread, NULL, send_thread, (void*) p_socket) == -1) {
        printf("pthread_create failed\n");
        exit(EXIT_FAILURE);
    }
    return thread;
}

void* recv_thread(void* p_socket) {
    int socket = *(int*) p_socket;
    printf("new client: %d\n", socket);
    ssize_t recv_length = 0;
    while(socket > 0) {
        byte_t* recv_message = (byte_t*) malloc(sizeof(byte_t) * 2);
        recv_length = recv(socket, recv_message, 2, 0);
        if (recv_length > 0) {
            enum MessageType type = (int) recv_message[0];
            int recv_payload_size = recv_message[1];
            byte_t* recv_payload = NULL;
            if (recv_payload_size > 0) {
                recv_payload = (byte_t*) malloc(sizeof(byte_t) * recv_payload_size);
                read(socket, recv_payload, recv_payload_size);
            }
            Message* message = message_create(socket, type, recv_payload_size, recv_payload);
            pthread_mutex_lock(&lock);
            list_message_insert(g_list, message);
            pthread_mutex_unlock(&lock);
        } else {
            break;
        }
    }
    if (recv_length == 0) {
        printf("client disconnected: %d\n", socket);
    } else if (recv_length == -1) {
        printf("recv failed\n");
    }
    free(p_socket);
    return EXIT_SUCCESS;
}

pthread_t create_recv_thread(int socket) {
    pthread_t thread;
    int* p_socket = (int*) malloc(sizeof(int));
    *p_socket = socket;
    if (pthread_create(&thread, NULL, recv_thread, (void*) p_socket) == -1) {
        printf("pthread_create failed\n");
        exit(EXIT_FAILURE);
    }
    return thread;
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
            create_recv_thread(client_socket);
            create_send_thread(client_socket);
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
    int* p_socket = (int*) malloc(sizeof(int));
    *p_socket = socket;
    if (pthread_create(&thread, NULL, accept_thread, (void*) p_socket) == -1) {
        printf("pthread_create failed\n");
        exit(EXIT_FAILURE);
    }
    return thread;
}

int create(int port) {
    int m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) {
        printf("socket failed\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    memset(&address, '0', sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(m_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
        printf("bind failed\n");
        exit(EXIT_FAILURE);
    }
    if (listen(m_socket, MAX_CLIENTS) == -1) {
        printf("listen failed\n");
        exit(EXIT_FAILURE);
    }
    return m_socket;
}

int main(int argc, const char * argv[]) {
    signal(SIGQUIT, on_destroy);
    signal(SIGINT, on_destroy);
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("mutex_init failed \n");
        exit(EXIT_FAILURE);
    }
    
    g_list = list_message_create();
    
    int socket = create(PORT);
    g_server_socket = socket;
    pthread_t accept_thread = create_accept_thread(socket);
    pthread_join(accept_thread, NULL);
    
    on_destroy(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
