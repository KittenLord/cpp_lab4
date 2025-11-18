#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "protocol.cpp"

#define null ((void *)0)

#define PORT 6969

int main(void) {
    int result = 0;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("ERROR: socket\n");
        return 1;
    }

    int reuseAddrOptions = 1;
    result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOptions, sizeof(reuseAddrOptions));
    if (result == -1) {
        printf("ERROR: reuse\n");
        return 1;
    }

    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = (struct in_addr){
            .s_addr = htonl(INADDR_ANY),
        },
    };

    result = bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result == -1) {
        printf("ERROR: bind\n");
        return 1;
    }

    result = listen(sock, 10);
    if (result == -1) {
        printf("ERROR: listen\n");
        return 1;
    }

    printf("SERVER STARTED\n");

    while (1) {
        struct sockaddr_in clientAddr    = {0};
        socklen_t          clientAddrLen = sizeof(clientAddr);

        int clientSock = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSock == -1) {
            printf("Accept failed\n");
            continue;
        }

        printf("CLIENT CONNECTED\n");

        // NOTE: guaranteed to be able to hold all 3 headers
        union PacketMax headerBuffer;

        PacketMode packetMode;
        read(clientSock, &packetMode, sizeof(packetMode));

        switch(packetMode) {
            case PACKET_1:
                {
                    printf("Received message from Client 1\n");

                    read(clientSock, ((uint8_t *)&headerBuffer) + sizeof(PacketMode), sizeof(struct Packet1) - sizeof(PacketMode));
                    struct Packet1 p = headerBuffer.p1;

                    printf("Client's RAM size: %d\n", p.ramSize);
                    printf("Client's external drive availability: %d\n", p.diskAvailable);
                    printf("Client's screen width: %d\n", p.screenWidth);
                }
                break;
            case PACKET_2:
                {
                    printf("Received message from Client 2\n");

                    read(clientSock, ((uint8_t *)&headerBuffer) + sizeof(PacketMode), sizeof(struct Packet2) - sizeof(PacketMode));
                    struct Packet2 p = headerBuffer.p2;

                    printf("Client's line height: %d\n", p.lineHeight);
                    printf("Client's notification panel width: %d\n", p.notifWidth);
                    printf("Client's horizontal DPI: %d\n", p.dpiHorizontal);
                }
                break;
            case PACKET_3:
                {
                    printf("Received message from Client 3\n");

                    read(clientSock, ((uint8_t *)&headerBuffer) + sizeof(PacketMode), sizeof(struct Packet3) - sizeof(PacketMode));
                    struct Packet3 p = headerBuffer.p3;

                    char *buffer = (char *)calloc(sizeof(char), p.length);
                    read(clientSock, buffer, p.length);

                    printf("Client's message: %s\n", buffer);

                    free(buffer);
                }
                break;
            default:
                printf("INVALID PACKET TYPE: %d\n", packetMode);
                break;
        }

        close(clientSock);
    }

    close(sock);
    return 0;
}
