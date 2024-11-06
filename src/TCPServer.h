#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "GameLogic.h"

#define IP "127.0.0.1"
#define PORT 6378
#define BUFFER_MAX 1024
#define MAX_CLIENTS 2
#define MAX_CONNECTION_REQUESTS 10

typedef char packet_t[BUFFER_MAX];

extern void clearPacket(packet_t p);
extern void loadPacket(packet_t p, const char *msg);
extern int comparePacket(packet_t p, const char *msg);
extern void appendPacket(packet_t p, const char *msg);

extern void loadBoardToPacket(packet_t p);

extern int server_socket;
extern fd_set readfds, writefds;
extern int max_sd;
extern int client_sockets[MAX_CLIENTS];
extern int client_count;
extern packet_t buffer;

extern int game_start_event, game_tick_event;
extern int turn_index;

extern const int X_index, O_index;

extern const char *terminate_msg;
extern const char *assign_x_msg;
extern const char *assign_o_msg;
extern const char *start_msg;
extern const char *turn_msg;
extern const char *turn_wait_msg;
extern const char *mid_disconnection_msg;
extern const char *winner_msg;
extern const char *loser_msg;
extern const char *tie_msg;

extern int activeClientCount();

extern void clearClientList();

extern void createServerSocket();
extern void createServerAddress();

extern void setSocketAsReuseAddr(int socket);

extern void bindServerSocketAddress();
extern void listenForServerConnections();

extern void setServerFDs(fd_set *set);
extern void selectFDs();

extern int addToClientList(int fd);
extern int acceptConnection();

extern void disconnectClientIndex(int index);
// extern void disconnectClientSD(int sd);

extern void sendPacket(int sd, packet_t p);
extern int receivePacket(int sd, packet_t p);

extern void sendPacketToAll(packet_t p);

extern void handleNewServerConnection();
// extern void handleClient(int sd);

extern void readFromClient(int index);
extern void writeToClient(int index);

#endif // TCP_SERVER_H