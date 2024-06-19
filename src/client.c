#include <client.h>

int client_init(Client *client){
    struct sockaddr_in6 addr;
    client->socket = socket(AF_INET6, SOCK_STREAM, 0);
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(42069);
    inet_pton(AF_INET6, "::1", &addr.sin6_addr);
    connect(client->socket, (struct sockaddr *)&addr, sizeof(addr));
    return 0;
}

int client_disconnect(Client *client){
    close(client->socket);
    return 0;
}

int client_server_kill(Client *client, uint32_t key, pthread_t tid){
    uint8_t cmd[] = {PROTO_SCOPE_TOP, PROTO_CMD_STOP_SERVER};
    int ret = send(client->socket, cmd, sizeof(cmd), 0);
    pthread_join(tid, NULL);
    return ret;
}
