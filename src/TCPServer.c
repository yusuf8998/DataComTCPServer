#include "TCPServer.h"

// TCPServer.h extern variables
int server_socket;
fd_set readfds;
int max_sd;
int client_sockets[MAX_CLIENTS];
char buffer[BUFFER_MAX];
struct sockaddr_in server_addr = {};
socklen_t len = sizeof(server_addr);

// pre-allocation of frequently used variables
int valread = 0, sd = 0, new_socket = 0, i = 0, n = 0, select_status = 0;

int activeClientCount()
{
    n = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
        if (client_sockets[i] != 0)
            n++;
    return n;
}

void clearClientList()
{
    memset(client_sockets, 0, sizeof(int) * MAX_CLIENTS);
}

void clearBuffer()
{
    memset(buffer, 0, sizeof(char) * BUFFER_MAX);
}

void createServerSocket()
{
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        exit(-1);
    printf("Socket created!\n");
}
void createServerAddress()
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
}
void setSocketAsReuseAddr(int master_socket)
{
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        exit(-1);
}
void bindServerSocketAddress()
{
    if (bind(server_socket, (struct sockaddr *)&server_addr, len) < 0)
        exit(-1);
    printf("Socket bound!\n");
}
void listenForServerConnections()
{
    if (listen(server_socket, MAX_CONNECTION_REQUESTS) < 0)
        exit(-1);
    printf("Listening...\n");
}
void setServerReadFDs()
{
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);
    max_sd = server_socket;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        sd = client_sockets[i];
        if (sd > 0)
            FD_SET(sd, &readfds);
        if (sd > max_sd)
            max_sd = sd;
    }
}
void selectFDs()
{
    select_status = select(max_sd + 1, &readfds, NULL, NULL, NULL);
    if (select_status < 0 && (errno != EINTR))
        exit(-1);
}
int addToClientList(int fd)
{
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0)
            continue;
        client_sockets[i] = fd;
        return 0;
    }
    return -1;
}
int acceptConnection()
{
    new_socket = accept(server_socket, (struct sockaddr *)&server_addr, &len);
    if (new_socket < 0)
    {
        printf("Failed to accept client!\n");
        return -1;
    }
    if (addToClientList(new_socket) == 0)
        return new_socket;
    printf("Max client count reached!\n");
    close(new_socket);
    return -1;
}
void disconnectClientIndex(int index)
{
    getpeername(client_sockets[index], (struct sockaddr *)&server_addr, &len);
    printf("Client disconnected, fd: %d, ip : %s, port : %d\n", client_sockets[index], inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    close(client_sockets[index]);
    client_sockets[index] = 0;
}
void disconnectClientSD(int sd)
{
    getpeername(sd, (struct sockaddr *)&server_addr, &len);
    printf("Client disconnected, fd : %d, ip : %s, port : %d\n", sd, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != sd)
            continue;
        client_sockets[i] = 0;
        break;
    }
    close(sd);
}
void handleNewServerConnection()
{
    if ((new_socket = acceptConnection()) < 0)
        return;
    printf("New connection, fd : %d, ip : %s, port : %d\n", new_socket,
           inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port));
}
void handleClient(int cli)
{
    if ((valread = read(cli, buffer, BUFFER_MAX)) == 0 || strncmp(buffer, "terminate", 9) == 0)
        disconnectClientSD(cli);
    else
    {
        buffer[valread] = '\0';
        send(cli, buffer, strlen(buffer), 0);
    }
}