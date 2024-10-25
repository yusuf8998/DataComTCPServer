#include "TCPServer.h"
#include "GameLogic.h"

int game()
{
    clearBoard();

    printBoard();
    (void) makeMove(0, 0); // X
    (void) makeMove(0, 2); // O
    (void) makeMove(1, 0); // X
    (void) makeMove(1, 1); // O
    (void) makeMove(2, 1); // X
    (void) makeMove(2, 0); // O
    printBoard();
    cell_t winner = checkBoard();
    printf("winner: %s\n", winner == X_CELL ? "X" : winner == O_CELL ? "O" : "Tie");
    return 0;
}

int main(int argc, char const *argv[])
{
    // return game();
    clearBoard();
    clearClientList();
    clearPacket(buffer);

    createServerSocket();
    createServerAddress();

    setSocketAsReuseAddr(server_socket);

    bindServerSocketAddress();
    listenForServerConnections();

    int sd, i;
    while (1)
    {
        setServerFDs(&readfds);
        setServerFDs(&writefds);
        selectFDs();

        if (FD_ISSET(server_socket, &readfds))
            handleNewServerConnection();

        setServerFDs(&readfds);
        setServerFDs(&writefds);
        selectFDs();

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (FD_ISSET((sd = client_sockets[i]), &readfds))
                readFromClient(i);
        }

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (FD_ISSET((sd = client_sockets[i]), &writefds))
                writeToClient(i);
        }

        if (game_start_event)
        {
            printf("%s", start_msg);
            game_start_event = 0;
        }
        game_tick_event = 0;
    }
    return 0;
}