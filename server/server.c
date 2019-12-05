#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<pthread.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>

const int _listenQueue = 1024;  // Backlog in listen()
const char *_terminateChar = "@logout";

bool isFileSent = false;

int connectedClient = 0;
int clientReceived = 0;
int nameCount = 0;
bool allReceived = true;
pthread_mutex_t mptr_clientCount = PTHREAD_MUTEX_INITIALIZER;

//void showTime() {
//    time_t mytime = time(NULL);
//    char * time_str = ctime(&mytime);
//    time_str[strlen(time_str)-1] = '\0';
//    printf("Current Time : %s\n", time_str);
//}

void *responeRequest(void *arg) {
    int clientSocket = *((int *) arg);
    free(arg);
    pthread_detach(pthread_self());

    // Receive file name from client and send file back
    char buffer[1024];
	char listFile[20][1024];
	char fileName[1024] = "";
    //int downloadType = 0;

    //if (connectedClient == 3) {
    //    printf("\nEnter file name to send: ");
    //    scanf("%s", fileName);
    //    //showTime();
    //}
    /*while (1) {
        if (connectedClient == 3 && strcmp(fileName, "") != 0) break;
    }*/
	boolean end = false;
	while (!end) {

		//đọc các lệnh từ user
		buffer = read(clientSocket, &buffer, sizeof(buffer);

		// server guiwr lại cho client OK nếu client muốn LOGOUT
		if (strcmp("@LOGOUT", buffer)) {
			write(clientSocket, ">> OK", 6);
			end = true;
			break;
		}
		else {
			// trả về các file hiện có
			if (strcmp("ls", buffer)) {
				// gui danh sach file
			}
			else {
				//lấy ra fileName mà client muốn download
				if (strstr(buffer, "download") != null) {
					fileName = strchr(buffer, 9);

					// kiểm tra fileName đó có tồn tại hay k
					boolean tonTaiFile = true;
					////chwa ktra
					//for ( file : listFile) {
					//	if (fileName.equals(file)) {
					//		tonTaiFile = true;
					//		break;
					//	}
					//}
					if (tonTaiFile) {
						write(clientSocket, "send file ", 10);
						//xác nhận của client
						bzero(buffer, sizeof(buffer));
						read(clientSocket, &buffer, sizeof(buffer));
						printf("Sending\n");
						//guiwr file
						write(clientSocket, fileName, sizeof(fileName));
						bzero(buffer, sizeof(buffer));
						read(clientSocket, &buffer, 1024);
						FILE* file = fopen(fileName, "rb");
						int size;
						printf("Sending file '%s'\n", fileName);
						// Send file length first
						fseek(file, 0, SEEK_END);
						size = ftell(file);
						fseek(file, 0, SEEK_SET);
						printf("File size: %d\n", size);
						size = htonl(size);
						write(clientSocket, (void*)&size, sizeof(size));
						// Send file
						while (!feof(file)) {
							// Read file to buffer
							int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
							write(clientSocket, buffer, readSize);
							// Zero out buffer after writing
							bzero(buffer, sizeof(buffer));
						}
						fclose(file);
						printf("Sent file successfully!\n");
					}
					else {
						write(clientSocket, ">> file khong ton tai", 21);
					}
				}
				else {
					write(clientSocket, ">> lenh khong ro", 16);
				}
			}
		}
	}
    //while (1) {
    //    //if (!allReceived) continue;
    //    //write(clientSocket, fileName, sizeof(fileName));
    //    //int nbytes = write(clientSocket, fileName, sizeof(fileName));
    //    //printf("Nbytes: %d\n", nbytes);
    //    pthread_mutex_lock(&mptr_clientCount);
    //    //nameCount++;
    //    pthread_mutex_unlock(&mptr_clientCount);
    //    FILE *file = fopen(fileName, "rb");
    //    int size;/*
    //    if (file == NULL) {
    //        size = -1;
    //        printf("Cannot find file '%s'!\n", fileName);
    //        size = htonl(size);
    //        write(clientSocket, (void *) &size, sizeof(size));
    //    }*/
    //    //else {
    //    printf("Sending file '%s'\n", fileName);
    //    // Send file length first
    //    fseek(file, 0, SEEK_END);
    //    size = ftell(file);
    //    fseek(file, 0, SEEK_SET);
    //    printf("File size: %d\n", size);
    //    size = htonl(size);
    //    write(clientSocket, (void *) &size, sizeof(size));
    //    // Send file
    //    while (!feof(file)) {
    //        // Read file to buffer
    //        int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
    //        write(clientSocket, buffer, readSize);
    //        // Zero out buffer after writing
    //        bzero(buffer, sizeof(buffer));
    //        //printf ("readSize: %d\n", readSize);
    //    //isFileSent = true;
    //    }
    //    fclose(file);
    //    printf("Sent file successfully!\n");
    //    //}
    //    }
    //    //else {
    //    //    downloadType = 2;
    //    //    write(clientSocket, &downloadType, sizeof(downloadType));
    //    //    write(clientSocket, firstClientAddr, 1024);
    //    //    while (1) {
    //    //        //DO NOTHING
    //    //        if (isFileSent) break;
    //    //    }
    //    //}

    //    //if (nameCount == 3) {
    //    //    allReceived = false;
    //    //}
    //    //int done = 0;
    //    //read (clientSocket, &done, sizeof(done));
    //    //if (done == 1) {
    //    //    pthread_mutex_lock(&mptr_clientCount);
    //    //    clientReceived++;
    //    //    //printf("Total Clients Received: %d\n", clientReceived);
    //    //    pthread_mutex_unlock(&mptr_clientCount);
    //    //}

    //    //if (clientReceived == 3) {
    //    //    bzero(fileName, sizeof(fileName));
    //    //    printf("\nEnter file name to send: ");
    //    //    scanf("%s", fileName);
    //    //    showTime();
    //    //    //firstClientAddr = NULL;
    //    //    isFileSent = false;
    //    //    clientReceived = 0;
    //    //    nameCount = 0;
    //    //    allReceived = true;
    //    //}
    //}
    //close(clientSocket);
    return NULL;
}

int main() {
    const int _family = AF_INET;    // IPv4
    const int _type = SOCK_STREAM;  // TCP
    const int _protocol = 0;
    const int _port = 6398;

    int serverSocket;
    int *clientSocket;
    struct sockaddr_in serverAddr, connClientAddr;

    // Create a socket to listen
    serverSocket = socket(_family, _type, _protocol);
    if (serverSocket < 0) {
        perror("Server socket error");
        return 1;
    }

    int option = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    bzero(&serverAddr, sizeof(serverAddr));

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_family = _family;

    int bindResult = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (bindResult < 0) {
        perror("Bind error");
        return 1;
    }

    int listenResult = listen(serverSocket, _listenQueue);
    if (listenResult < 0) {
        perror("Listen error");
        return 1;
    }

    printf("Waiting for connection...\n");

    unsigned int addrLength = sizeof(connClientAddr);

    while (1) {
        clientSocket = malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr *) &connClientAddr, &addrLength);
        if (clientSocket < 0) {
            if (errno == EINTR) continue;
            else {
                perror("Accept error");
                return 1;
            }
        }
        // Get client info
        char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
        firstClientAddrTmp = inet_ntoa(connClientAddr.sin_addr);
        int clientPort = ntohs(connClientAddr.sin_port);

        printf("Client address: %s:%d\n", clientIpAddr, clientPort);
        connectedClient++;

        // Create a thread to handle requests of client
        pthread_t tid;
        pthread_create(&tid, NULL, &responeRequest, (void *) clientSocket);
    }

    close(serverSocket);
    return 0;
}

