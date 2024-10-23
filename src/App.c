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
    clearClientList();
    clearBuffer();

    createServerSocket();
    createServerAddress();

    setSocketAsReuseAddr(server_socket);

    bindServerSocketAddress();
    listenForServerConnections();

    int sd;
    while (1)
    {
        setServerReadFDs();
        selectFDs();

        if (FD_ISSET(server_socket, &readfds))
            handleNewServerConnection();

        for (int i = 0; i < MAX_CLIENTS; i++)
            if (FD_ISSET((sd = client_sockets[i]), &readfds))
                handleClient(sd);
    }
    return 0;
}