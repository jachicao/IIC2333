#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "MessageType.h"

#define PORT 8080

typedef uint8_t byte_t;

volatile int g_client_socket = -1;

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
    byte_t* message = NULL;
    switch (type) {
        case Heartbeat:
            message = (byte_t*) malloc(sizeof(byte_t) * 6);
            message[0] = type;
            message[1] = 4;
            int current_time = (int) time(NULL);
            memcpy(message + 2, &current_time, 4);
            send(socket, message, 6, 0);
            break;
        case Matchmaking:
            break;
        case MatchmackingList:
            break;
        case MatchrequestInvite:
            break;
        case MatchrequestInvitation:
            break;
        case Chat:
            break;
        case GameStart:
            break;
        case Move:
            break;
        case Disconnect:
            break;
        case ContrincantDisconnect:
            break;
        case Error:
            break;
        case GameSync:
            break;
        case BoardSetAuthority:
            break;
        case ServerInfo:
            break;
        case PacketSupport:
            break;
        case GameRequest:
            break;
        default:
            break;
    }
    free(payload);
}

void* recv_thread(void* p_socket) {
    int socket = *(int*) p_socket;
    ssize_t length;
    while(true) {
        byte_t* message = (byte_t*) malloc(sizeof(byte_t) * 2);
        length = recv(socket, message, 2, 0);
        if (length > 0) {
            enum MessageType type = (int) message[0];
            int payload_size = message[1];
            byte_t* payload = NULL;
            if (payload_size > 0) {
                payload = (byte_t*) malloc(sizeof(byte_t) * payload_size);
                read(socket, payload, payload_size);
            }
            on_receive(socket, type, payload_size, payload);
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

int create(int port) {
    int m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) {
        printf("socket failed\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);
    if (connect(m_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
        printf("connect failed\n");
        exit(EXIT_FAILURE);
    }
    return m_socket;
}

void on_destroy(int s) {
    if (g_client_socket != -1) {
        close(g_client_socket);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[]) {
    signal(SIGQUIT, on_destroy);
    signal(SIGINT, on_destroy);
    
    int socket = create(PORT);
    g_client_socket = socket;
    
    pthread_t recv_thread = create_recv_thread(socket);
    pthread_join(recv_thread, NULL);
    
    on_destroy(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
