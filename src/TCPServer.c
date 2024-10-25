#include "TCPServer.h"

// TCPServer.h extern variables
int server_socket;
fd_set readfds, writefds;
int max_sd;
int client_sockets[MAX_CLIENTS];
int client_count = 0;
packet_t buffer;
struct sockaddr_in server_addr = {};
socklen_t len = sizeof(server_addr);
int game_start_event = 0, game_tick_event = 0;
int turn_index = -1;

// pre-allocation of frequently used variables
int valread = 0, sd = 0, cli = 0, new_socket = 0, i = 0, n = 0, select_status = 0;

// messages
const char *terminate_msg = "terminate";
const char *assign_x_msg = "You are assigned to 'X'!\n";
const char *assign_o_msg = "You are assigned to 'O'!\n";
const char *start_msg = "All players are here. Game is starting!\nYou make a move by entering '(x,y)'\n";
const char *turn_msg = "It is your turn!\n";
const char *turn_wait_msg = "Waiting for other players move...\n";
const char *mid_disconnection_msg = "The other player has disconnected! Pausing game...\n";
const char *winner_msg = "You win!\n";
const char *loser_msg = "You lose!\n";
const char *tie_msg = "It's a tie!\n";

void clearPacket(packet_t p)
{
    memset(p, 0, sizeof(packet_t));
}
void loadPacket(packet_t p, const char *msg)
{
    size_t msg_len = strlen(msg);
    msg_len = msg_len > BUFFER_MAX ? BUFFER_MAX : msg_len;
    memcpy(p, msg, msg_len);
}
int comparePacket(packet_t p, const char *msg)
{
    return strncmp(p, msg, strlen(msg));
}
void appendPacket(packet_t p, const char *msg)
{
    char *it = &p[0];
    while (*it++ != '\0');
    it--;
    size_t msg_len = strlen(msg);
    msg_len = msg_len + (it - &p[0]) > BUFFER_MAX ? BUFFER_MAX : msg_len;
    memcpy(it, msg, msg_len);
}

void loadBoardToPacket(packet_t p)
{
    int offset = 0;
    char brd_buff[5];
    for (int y = 2; y >= 0; y--)
    {
        for (int x = 0; x < 3; x++)
        {
            cellToString(brd_buff, x, y);
            offset += sprintf(p + offset, "%s ", brd_buff);
        }
        offset += sprintf(p + offset, "\n");
    }
    sprintf(p + offset, "-----------------\n");
}

int activeClientCount()
{
    return client_count;
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
void setServerFDs(fd_set *set)
{
    FD_ZERO(set);
    FD_SET(server_socket, set);
    max_sd = server_socket;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        sd = client_sockets[i];
        if (sd > 0)
            FD_SET(sd, set);
        if (sd > max_sd)
            max_sd = sd;
    }
}
void selectFDs()
{
    select_status = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
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
        client_count++;
        return i;
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
    if (addToClientList(new_socket) >= 0)
        return new_socket;
    printf("Max client count reached!\n");
    close(new_socket);
    return -1;
}
void disconnectClientIndex(int index)
{
    getpeername(client_sockets[index], (struct sockaddr *)&server_addr, &len);
    printf("Client disconnected, fd: %d, ip : %s, port : %d\n", client_sockets[index], inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    clearPacket(buffer);
    loadPacket(buffer, terminate_msg);
    sendPacket(client_sockets[index], buffer);
    clearPacket(buffer);
    close(client_sockets[index]);
    client_sockets[index] = 0;
    client_count--;
    if (activeClientCount() == MAX_CLIENTS - 1 && turn_index != -1)
    {
        loadPacket(buffer, mid_disconnection_msg);
        clearBoard();
        sendPacketToAll(buffer);
        clearPacket(buffer);
    }
}
// void disconnectClientSD(int sd)
// {
//     getpeername(sd, (struct sockaddr *)&server_addr, &len);
//     printf("Client disconnected, fd : %d, ip : %s, port : %d\n", sd, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
//     close(sd);
//     for (i = 0; i < MAX_CLIENTS; i++)
//     {
//         if (client_sockets[i] != sd)
//             continue;
//         client_sockets[i] = 0;
//         break;
//     }
//     if (activeClientCount() == MAX_CLIENTS - 1)
//     {
//         loadPacket(buffer, mid_disconnection_msg);
//         sendPacketToAll(buffer);
//         clearPacket(buffer);
//     }
// }
void sendPacket(int sd, packet_t p)
{
    if (send(sd, p, BUFFER_MAX, 0) < 0)
        printf("Packet send error!\n");
}
int receivePacket(int sd, packet_t p)
{
    if ((valread = read(sd, p, BUFFER_MAX)) < 0)
        printf("Packet receive error!\n");
    return valread;
}
void sendPacketToAll(packet_t p)
{
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (FD_ISSET((sd = client_sockets[i]), &writefds))
            sendPacket(sd, p);
    }
}
void handleNewServerConnection()
{
    if ((new_socket = acceptConnection()) < 0)
        return;
    printf("New connection, fd : %d, ip : %s, port : %d\n", new_socket,
           inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port));
    // clearPacket(buffer);
    // loadBoardToPacket(buffer);
    // sendPacket(new_socket, buffer);
    if (activeClientCount() == MAX_CLIENTS)
    {
        game_tick_event = 1;
        game_start_event = 1;
    }
}
// void handleClient(int cli)
// {
//     clearPacket(buffer);
//     if (game_tick_event)
//     {
//         loadPacket(buffer, start_msg);
//         sendPacket(cli, buffer);
//         printf("%s", start_msg);
//     }
//     if ((valread = receivePacket(cli, buffer)) == 0 || comparePacket(buffer, terminate_msg) == 0)
//         disconnectClientSD(cli);
//     else
//     {
//         buffer[valread] = '\0';
//         printf("%s", buffer);
//         sendPacket(cli, buffer);
//     }
// }

void readFromClient(int index)
{
    cli = client_sockets[index];
    clearPacket(buffer);
    if ((valread = receivePacket(cli, buffer)) == 0 || comparePacket(buffer, terminate_msg) == 0)
        disconnectClientIndex(index);
    if (activeClientCount() != MAX_CLIENTS)
        return;
    if (turn_index == index)
    {
        if (buffer[0] != '(' || buffer[4] != ')') return;
        int x = strtol(&buffer[1], NULL, 0);
        int y = strtol(&buffer[3], NULL, 0);
        if (x < 0 || x > 2 || y < 0 || y > 2) return;
        if (makeMove(x, y) < 0) return;
        clearPacket(buffer);
        loadBoardToPacket(buffer);
        sendPacketToAll(buffer);
        cell_t winner = checkBoard();
        if (winner == BOARD_FULL)
        {
            clearBoard();
            clearPacket(buffer);
            loadPacket(buffer, tie_msg);
            sendPacketToAll(buffer);

            printBoard("It's a tie!\n");
            turn_index = -1;
            return;

        }
        if (winner != EMPTY_CELL)
        {
            clearBoard();

            clearPacket(buffer);
            loadPacket(buffer, winner_msg);
            sendPacket(client_sockets[winner], buffer);
            clearPacket(buffer);
            loadPacket(buffer, loser_msg);
            sendPacket(client_sockets[(winner+1)%2], buffer);

            printf("%s wins!\n", winner == X_CELL ? "X" : "O");

            turn_index = -1;
            // disconnectClientIndex(0);
            // disconnectClientIndex(1);
            return;
        }
        turn_index = (turn_index+1)%2;
        game_tick_event = 1;
    }
}

void writeToClient(int index)
{
    cli = client_sockets[index];
    if (cli == 0)
        return;
    if (!game_tick_event && !game_start_event)
        return;
    clearPacket(buffer);
    if (game_start_event)
    {
        loadBoardToPacket(buffer);
        // sendPacket(cli, buffer);
        // clearPacket(buffer);
        //
        appendPacket(buffer, start_msg);
        // sendPacket(cli, buffer);
        // clearPacket(buffer);
        if (index == 0)
            appendPacket(buffer, assign_x_msg);
        else if (index == 1)
            appendPacket(buffer, assign_o_msg);
        else
            exit(-1);
        // sendPacket(cli, buffer);
        // clearPacket(buffer);
        turn_index = 0;
    }
    if (turn_index == index)
    {
        appendPacket(buffer, turn_msg);
        // sendPacket(cli, buffer);
    }
    else
    {
        appendPacket(buffer, turn_wait_msg);
        // sendPacket(cli, buffer);
    }

    sendPacket(cli, buffer);
}