#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define IP "127.0.0.1"
#define BUFFER_MAX 80
#define PORT 6378
#define MAX_CLIENTS 2
#define MAX_CONNECTION_REQUESTS 10

extern int server_socket;
extern fd_set readfds;
extern int max_sd;
extern int client_sockets[MAX_CLIENTS];
extern char buffer[BUFFER_MAX];

extern int activeClientCount();

extern void clearClientList();
extern void clearBuffer();

extern void createServerSocket();
extern void createServerAddress();

extern void setSocketAsReuseAddr(int socket);

extern void bindServerSocketAddress();
extern void listenForServerConnections();

extern void setServerReadFDs();
extern void selectFDs();

extern int addToClientList(int fd);
extern int acceptConnection();

extern void disconnectClientIndex(int index);
extern void disconnectClientSD(int sd);

extern void handleNewServerConnection();
extern void handleClient(int sd);

#endif // TCP_SERVER_H