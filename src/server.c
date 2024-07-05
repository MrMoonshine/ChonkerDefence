#include <server.h>

static const char* TAG = "Server";
static sem_t sParamCpy;

void *server_main(void *portNumPtr)
{
    server_initial_params sip;
    //server_initial_params *s_i = (server_initial_params*)portNumPtr;
    memcpy(&sip, (server_initial_params*)portNumPtr, sizeof(server_initial_params));

    int server_fd, new_socket;
    struct sockaddr_in6 address;
    int opt = 1;
    int addrlen = sizeof(address);

    //Setup Poller
    int fdCount = 0;
    int fdSize = SERVER_CLIENT_COUNT_MAX;
    struct pollfd *pfds = malloc(sizeof *pfds * fdSize);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        LOGERRNO(TAG, "socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        LOGERRNO(TAG, "setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin6_family = AF_INET6;
    if(sip.port > 0){
        address.sin6_addr = in6addr_any;
        printf("[INFO] %s: Starting server on port %d\n", TAG, sip.port);
    }else{
        address.sin6_addr = in6addr_loopback;
        printf("[INFO] %s: Starting server on localhost:%d\n", TAG, SERVER_PORT_DEFAULT);
        sip.port = SERVER_PORT_DEFAULT;
    }
    address.sin6_port = htons(sip.port);

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        LOGERRNO(TAG, "bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        LOGERRNO(TAG, "listen failed");
        exit(EXIT_FAILURE);
    }

    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN | POLLPRI | POLLERR;
    fdCount = 1;
    char buffer[SERVER_RCV_BUFF_SIZE] = {0};

    bool server_running = true, firstRun = true;
    while(server_running){
        //LOGI(TAG,"Poll Start");
        if(firstRun){
            firstRun = false;
            sem_post(&sParamCpy);          // Allow main thread to return
        }
        int pollCount = poll(pfds, fdCount, -1);
        if (pollCount < 0) {
            LOGERRNO(TAG, "poll");
            exit(1);
        }
        //LOGI(TAG,"Poll finished");

        for(int i = 0; i < fdCount; i++){
            // is one ready?
            if (pfds[i].revents & POLLIN){
                // is the listener ready to read?
                if(pfds[i].fd == server_fd){
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
                        LOGE(TAG, "accept");
                    }else{
                        //If there is not enough space, the buffer for the other descriptors gets doubled in size
                        if(fdCount == fdSize){
                            fdSize *= 2;
                            pfds = realloc(pfds, sizeof(*pfds) * fdSize);
                            if(!pfds)
                                exit(EXIT_FAILURE);
                        }

                        pfds[fdCount].fd = new_socket;
                        pfds[fdCount].events = POLLIN;
                        fdCount++;
                        //Print some infos
                        char astring[INET6_ADDRSTRLEN];
                        inet_ntop(AF_INET6, &(address.sin6_addr), astring, INET6_ADDRSTRLEN);
                        printf("[INFO] %s: Added %s to the Pollserver\n", TAG, astring);
                    }
                }else{
                    //Standard client procedure...
                    int byteCount = recv(pfds[i].fd, buffer, sizeof buffer, 0);
                    if(byteCount <= 0){
                        if(byteCount == 0)
                            /*
                                Test with telnet:
                                ctrl + ]
                                telnet> quit
                            */
                            printf("Socket hung up: %d\n", pfds[i].fd);
                        else
                            LOGE(TAG, "recv");

                        close(pfds[i].fd);

                        pfds[i] = pfds[fdCount-1];
                        fdCount--;
                    }else{
                        //Got some Data!
                        LOGI(TAG, "Got some Data");
                        switch((uint8_t)buffer[0]){
                            case PROTO_SCOPE_LEVELS:{
                                if(byteCount > 1){
                                    switch(buffer[1]){
                                        case PROTO_CMD_SHOW: {
                                            //printf("Server: Bytecount is %d\n", byteCount);
                                            switch(byteCount){
                                                case PROTO_CMD_LEVEL_SHOW_LEN:{
                                                    // pfds[i].fd
                                                    srvlevel_show(pfds+i, 1, NULL);
                                                }break;
                                                case PROTO_CMD_LEVEL_SHOW_INSTANCE_LEN:{
                                                    // All FDs except server
                                                    srvlevel_show(pfds+1, fdCount -1, (uint8_t*)(buffer + 2));
                                                }break;
                                                default: break;
                                            }
                                        } break;
                                        default: break;
                                    }
                                }
                            }break;
                            default:{   // PROTO_SCOPE_TOP
                                if(byteCount > 1){
                                    switch((uint8_t)buffer[1]){
                                        case PROTO_CMD_STOP_SERVER: {
                                            server_running = false;
                                            break;
                                        } break;
                                        default: break;
                                    }
                                }
                            }break;
                        }
                        /*for(int j = 0; j < fdCount; j++){
                            int dest_fd = pfds[j].fd;
                            //send to everybody except the sender and the server:
                            if (dest_fd != server_fd && dest_fd != pfds[i].fd){
                                if(send(dest_fd, buffer, byteCount, 0) == -1)
                                    LOGE(TAG, "send");
                            }
                        }*/
                    }
                }
            }
        }
    }
    free(pfds);
    pfds = NULL;
    close(server_fd);
    LOGS(TAG, "Stop!");
    return NULL;
}

int server_start(server_initial_params *params){
    params->sessionkey = time(NULL) % UINT32_MAX;

    sem_init(&sParamCpy, 0, 1);     // Params: semaphore, 0 = share between threads, 1 = value
    sem_wait(&sParamCpy);           // Take fisrt semaphore

    pthread_create(&params->thrid, NULL, server_main, (void*)params); // Strat server
    sem_wait(&sParamCpy);       // Wait for server to copy params into its own thread
    sem_destroy(&sParamCpy);    // no more use for it
    return 0;
}

int server_stop(){
    return 0;
}
