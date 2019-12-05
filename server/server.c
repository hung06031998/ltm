#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

pthread_mutex_t mptr_clientCount = PTHREAD_MUTEX_INITIALIZER;

void *responeRequest(void *arg)
{
    int clientSocket = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());

    char buffer[1024];
    bool end = false;
    char *path = "SharedFolder";
    char receiveBuffer[1024];
    char fileName[1024];
    while (!end)
    {
        //đọc các lệnh từ user
        bzero(buffer, sizeof(buffer));
        read(clientSocket, buffer, sizeof(buffer));
        printf("%s\n", buffer);

        // server guiwr lại cho client OK nếu client muốn LOGOUT
        if (strcmp("@LOGOUT", buffer) == 0)
        {
            write(clientSocket, ">> OK", 6);
            end = true;
            break;
        }
        else
        {
            // trả về các file hiện có
            if (strcmp("ls", buffer) == 0)
            {
                struct dirent *de;
                DIR *dr = opendir(path);
                if (dr == NULL)
                {
                    printf("loi mở folder\n");
                    return 0;
                }
                bzero(buffer, sizeof(buffer));
                strcat(buffer, "Danh sach cac file:\n");
                while ((de = readdir(dr)) != NULL)
                {
                    if (de->d_type == DT_REG)
                    {
                        strcat(buffer, strcat(de->d_name, "\n"));
                    }
                }
                closedir(dr);
                int nbytes;
                if (strcmp(buffer, "Danh sach cac file:\n") == 0)
                {
                    strcat(buffer, "\nEmpty");
                }

                if ((nbytes = write(clientSocket, buffer, sizeof(buffer))) < 0)
                {
                    printf("write to server fail \n");
                    return NULL;
                }
                bzero(buffer, sizeof(buffer));
            }
            else
            {
                //lấy ra fileName mà client muốn download
                if (strcmp(buffer, "download") == 0)
                {
                    write(clientSocket, "hay gui ten file", 17);
                    read(clientSocket, fileName, sizeof(fileName));
                    char fullPath[1024];
                    strcpy(fullPath, path);
                    strcat(fullPath, "/");
                    strcat(fullPath, fileName);
                    printf("%s\n", fullPath);
                    FILE *file = fopen(fullPath, "rb");
                    if (file == NULL)
                    {
                        printf("Cannot find file '%s'!\n", fileName);
                        write(clientSocket, "file khong ton tai", 19);
                    }
                    else
                    {
                        write(clientSocket, "send file", 10);
                        bzero(buffer, sizeof(buffer));
                        read(clientSocket, buffer, sizeof(buffer));
                        write(clientSocket, fileName, sizeof(fileName));
                        bzero(buffer, sizeof(buffer));
                        read(clientSocket, buffer, sizeof(buffer));
                        int size;
                        printf("Sending file '%s'\n", fileName);
                        // Send file length
                        fseek(file, 0, SEEK_END);
                        size = ftell(file);
                        fseek(file, 0, SEEK_SET);
                        printf("File size: %d\n", size);
                        size = htonl(size);
                        write(clientSocket, (void *)&size, sizeof(size));
                        // Send file
                        while (!feof(file))
                        {
                            int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
                            write(clientSocket, buffer, readSize);
                            bzero(buffer, sizeof(buffer));
                        }
                        fclose(file);
                        printf("Sent file successfully!\n");
                    }
                }
                else
                {
                    // khi client nhập lệnh upload
                    if (strcmp(buffer, "upload") == 0)
                    {
                        write(clientSocket, "hay gui ten file", 17);
                        read(clientSocket, fileName, sizeof(fileName));
                        printf("Ten file: %s\n", fileName);
                        write(clientSocket, "file ton tai k", 15);
                        bzero(buffer, sizeof(buffer));
                        read(clientSocket, buffer, sizeof(buffer));
                        if (strcmp(buffer, "file khong ton tai") == 0)
                        {
                            printf("upload fail: file k ton tai\n");
                        }
                        else
                        {
                            // nhận file length
                            write(clientSocket, "hay gui file length", 20);
                            unsigned int fileSize = 0;
                            unsigned int receivedSize = 0;
                            int nbytes = read(clientSocket, &fileSize, sizeof(fileSize));
                            if (nbytes < 0)
                            {
                                perror("Read error");
                                return NULL;
                            }
                            fileSize = ntohl(fileSize);
                            // Receive file
                            FILE *file = fopen(fileName, "wb+");
                            printf("Receiving file '%s'\n", fileName);
                            printf("File size: %d\n", fileSize);
                            write(clientSocket, "bat dau nhan", 13);
                            while (receivedSize < fileSize)
                            {
                                int currRcvSize = read(clientSocket, receiveBuffer, 1024);
                                receivedSize += currRcvSize;
                                fwrite(receiveBuffer, 1, currRcvSize, file);
                            }
                            fclose(file);
                            printf("Received file successfully!\n");
                        }
                    }
                    else
                    {
                        write(clientSocket, ">> lenh khong ro", 17);
                    }
                }
            }
        }
    }
    return NULL;
}

int main()
{
    const int _family = AF_INET;   // IPv4
    const int _type = SOCK_STREAM; // TCP
    const int _protocol = 0;
    const int _port = 6398;

    int serverSocket;
    int *clientSocket;
    struct sockaddr_in serverAddr, connClientAddr;

    // Create a socket to listen
    serverSocket = socket(_family, _type, _protocol);
    if (serverSocket < 0)
    {
        perror("Server socket error");
        return 1;
    }

    int option = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    bzero(&serverAddr, sizeof(serverAddr));

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_family = _family;

    int bindResult = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindResult < 0)
    {
        perror("Bind error");
        return 1;
    }

    int listenResult = listen(serverSocket, 10);
    if (listenResult < 0)
    {
        perror("Listen error");
        return 1;
    }

    printf("Waiting for connection...\n");

    unsigned int addrLength = sizeof(connClientAddr);

    while (1)
    {
        clientSocket = malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr *)&connClientAddr, &addrLength);
        if (clientSocket < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                perror("Accept error");
                return 1;
            }
        }
        // Get client info
        char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
        int clientPort = ntohs(connClientAddr.sin_port);

        printf("Client address: %s:%d\n", clientIpAddr, clientPort);

        // Create a thread to handle requests of client
        pthread_t tid;
        if (pthread_create(&tid, NULL, &responeRequest, (void *)clientSocket) != 0)
        {
            printf("looix rooif");
        }
    }

    close(serverSocket);
    return 0;
}
