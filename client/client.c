#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<pthread.h>
#include<errno.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>

const char *_terminateChar = "@exit";

char fileName[1024];

pthread_mutex_t mptr_clientCount = PTHREAD_MUTEX_INITIALIZER;

int gobalSize = -1;

bool received = false;
bool fullyReceived = false;

//void showTime() {
//    time_t mytime = time(NULL);
//    char * time_str = ctime(&mytime);
//    time_str[strlen(time_str)-1] = '\0';
//    printf("Current Time : %s\n", time_str);
//}

//void *responeRequest(void *arg) {
//    int clientSocket = *((int *) arg);
//    free(arg);
//    pthread_detach(pthread_self());
//
//    // Receive file name from client and send file back
//    char buffer[1024];
//    while(1) {
//        if (received) break;
//    }
//
//    FILE *file = fopen(fileName, "rb+");
//    int size;
//    // Handle error
//    if (file == NULL) {
//        size = -1;
//        printf("Cannot find file '%s'!\n", fileName);
//        size = htonl(size);
//        write(clientSocket, (void *) &size, sizeof(size));
//    }
//    else {
//        printf("Sending file '%s'\n", fileName);
//        // Send file length first
//        //fseek(file, 0, SEEK_END);
//        //size = ftell(file);
//        size = gobalSize;
//        fseek(file, 0, SEEK_SET);
//        printf("File size: %d\n", size);
//        size = htonl(size);
//        write(clientSocket, (void *) &size, sizeof(size));
//        int totalSent = 0;
//        // Send file
//        while (totalSent < gobalSize) {
//            // Read file to buffer
//            int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
//            write(clientSocket, buffer, readSize);
//            totalSent += readSize;
//            // Zero out buffer after writing
//            bzero(buffer, sizeof(buffer));
//            //if (fullyReceived && feof(file)) break;
//        }
//        fclose(file);
//        printf("Send: %d\n", totalSent);
//        pthread_mutex_lock(&mptr_clientCount);
//        clientReceived++;
//        pthread_mutex_unlock(&mptr_clientCount);
//        //printf("Sent file successfully!\n");
//        //printf("Sent: %d!\n", totalSent);
//    }
//    close(clientSocket);
//    return NULL;
//}

int main() {
	const int _family = AF_INET;    // IPv4
	const int _type = SOCK_STREAM;  // TCP
	const int _protocol = 0;
	const int _listenQueue = 1024;
	const int _port = 9863;

	int serverSocket;
	int serverSocket2;
	int serverSocket3;
	int* clientSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_in serverAddr2, connClientAddr;
	struct sockaddr_in serverAddr3;

	//int downloadType;
	char receiveBuffer[1024];

	int nbytes = 0;

	char* serverIpAddr = (char*)malloc(100 * sizeof(char*));
	int serverPort = 8080;

	serverSocket = socket(_family, _type, _protocol);
	if (serverSocket < 0) {
		perror("Server socket error");
		return 1;
	}

	bzero(&serverAddr, sizeof(serverAddr));

	printf("Server IP Address: ");
	scanf("%s", serverIpAddr);

	serverAddr.sin_addr.s_addr = inet_addr(serverIpAddr);
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_family = _family;

	// Connect to server by using serverSocket
	int connectResult = connect(serverSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr));
	if (connectResult < 0) {
		perror("Connect error");
		return 1;
	}

	printf("Connected to server.\n");
	while (true) {
		// nhập lệnh và gửi cho server
		char buffer[1024], command[1024];
		printf("Nhap cau lenh:\n");
		scanf("%s", command);
		write(serverSocket, &command, sizeof(command));

		// nhận response từ server
		//if ((responseLine = is.readLine()) != null) {
		read(serverSocket, buffer, 1024);
		printf("Server: %s", buffer);
		// khi người dùng gõ lệnh @LOGOUT, server guiwr lại message ok để đóng kết nối
		if (strstr(buffer, "OK") != null) {
			break;
		}

		// khi người dùng nhập lệnh download file và file đấy tồn tại thì server guiwr message send file

		else if (strstr(buffer, "send file") != null) {
			write(serverSocket, "chuan bi nhan file", 18);

			int length = 0;            //length of file 
			int rec = 0;                // số lượng ký tự đã nhận được
			//server guiwr cho client ten file và kích thước file
			//String fileName = is.readLine();
			//System.out.println("fileName: " + fileName);
			//os.write("da nhan fileName");
			//os.newLine();
			//os.flush();

			//length = is.read();
			//is.readLine();    // đọc dấu xuống dòng còn thừa trong socket
			//System.out.println("length of file: " + length);
			//os.write("da nhan length");
			//os.newLine();
			//os.flush();
			////tạo file mới trong mục download của client và ghi file
			//Formatter nf = new Formatter("./download/" + fileName);
			//while (rec < length) {
			//	responseLine = is.readLine();
			//	System.out.println(responseLine);
			//	rec += responseLine.length() + 1;
			//	nf.format("%s\n", responseLine);
			//}
			//nf.close();


			read(serverSocket, fileName, sizeof(fileName));
			//printf("nbytes: %d\n", nbytes);

			printf("Filename: %s\n", fileName);
			write(serverSocket, "da nhan filename", 16);

			unsigned int fileSize = 0;
			unsigned int receivedSize = 0;
			nbytes = read(serverSocket, &fileSize, sizeof(fileSize));
			if (nbytes < 0) {
				perror("Read error");
				return 1;
			}
			fileSize = ntohl(fileSize);
			gobalSize = fileSize;
			// Receive file
			else {
				FILE* file = fopen(fileName, "wb+");
				printf("Receiving file '%s'\n", fileName);
				printf("File size: %d\n", fileSize);
				received = true;
				while (receivedSize < fileSize) {
					int currRcvSize = read(serverSocket, receiveBuffer, 1024);
					receivedSize += currRcvSize;
					fwrite(receiveBuffer, 1, currRcvSize, file);
					//printf ("currRcvSize: %d\n", currRcvSize);
				}
				//fseek(file, 0, SEEK_END);
				//fileSize = ftell(file);
				//(file, 0, SEEK_SET);
				//printf("Received size: %d\n", receivedSize);
				//printf("File size: %d\n", fileSize);
				fclose(file);
				fullyReceived = true;
				printf("Received file successfully!\n");
				//showTime();
				int done = 1;
				//write(serverSocket, &done, sizeof(done));
			}
		}
	}
	close(serverSocket);
	printf("Closed connection.\n");

	return 0;
}
		//}
    //while (1) {
    //    bzero(fileName, sizeof(fileName));
    //    //printf("Still here\n");
    //    //nbytes =
    //    read(serverSocket, fileName, sizeof(fileName));

    //    printf("Filename: %s\n", fileName);
		

    //    read (serverSocket, &downloadType, sizeof(downloadType));
    //    printf("DownloadType: %d\n", downloadType);
    //    if (downloadType == 1) {

    //    printf("\nAct like server now\n");

    //        serverSocket2 = socket(_family, _type, _protocol);
    //        if (serverSocket2 < 0) {
    //            perror("Server socket error");
    //            return 1;
    //        }

    //        int option = 1;
    //        setsockopt(serverSocket2, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    //        bzero(&serverAddr, sizeof(serverAddr));

    //        serverAddr2.sin_addr.s_addr = htonl(INADDR_ANY);
    //        serverAddr2.sin_port = htons(_port);
    //        serverAddr2.sin_family = _family;

    //        int bindCheck = bind(serverSocket2, (struct sockaddr *) &serverAddr2, sizeof(serverAddr2));
    //        if (bindCheck < 0) {
    //            perror("Bind error");
    //            return 1;
    //        }

    //        int listenCheck = listen(serverSocket2, _listenQueue);
    //        if (listenCheck < 0) {
    //            perror("Listen error");
    //            return 1;
    //        }

    //        printf("Server is listening at port %d. Waiting for connection...\n", _port);
    //        unsigned int addrLength = sizeof(connClientAddr);

    //        while (1) {
    //            if (clientCount == 2)
    //            {
    //                break;
    //            }
    //            clientSocket = malloc(sizeof(int));
    //            *clientSocket = accept(serverSocket2, (struct sockaddr *) &connClientAddr, &addrLength);
    //            if (clientSocket < 0) {
    //                if (errno == EINTR) continue;
    //                else {
    //                    perror("Accept error");
    //                    return 1;
    //                }
    //            }
    //            // Get client info
    //            char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
    //            int clientPort = ntohs(connClientAddr.sin_port);

    //            printf("Client address: %s:%d\n", clientIpAddr, clientPort);

    //            // Create a thread to handle requests of client
    //            pthread_t tid;
    //            pthread_create(&tid, NULL, &responeRequest, (void *) clientSocket);
    //            clientCount++;
    //        }
    //        clientCount = 0;
    //        close(serverSocket2);

    //        // Receive file size
    //        unsigned int fileSize = 0;
    //        unsigned int receivedSize = 0;
    //        nbytes = read(serverSocket, &fileSize, sizeof(fileSize));
    //        if (nbytes < 0) {
    //            perror("Read error");
    //            return 1;
    //        }
    //        fileSize = ntohl(fileSize);
    //        gobalSize = fileSize;
    //        // Handle error
    //        if (fileSize == -1) {
    //            printf("Cannot download file '%s'!\n", fileName);
    //            continue;
    //        }
    //        // Receive file
    //        else {
    //            FILE *file = fopen(fileName, "wb+");
    //            printf("Receiving file '%s'\n", fileName);
    //            printf("File size: %d\n", fileSize);
    //            received = true;
    //            while (receivedSize < fileSize) {
    //                int currRcvSize = read(serverSocket, receiveBuffer, 1024);
    //                receivedSize += currRcvSize;
    //                fwrite(receiveBuffer, 1, currRcvSize, file);
    //                //printf ("currRcvSize: %d\n", currRcvSize);
    //            }
    //            //fseek(file, 0, SEEK_END);
    //            //fileSize = ftell(file);
    //            //(file, 0, SEEK_SET);
    //            //printf("Received size: %d\n", receivedSize);
    //            //printf("File size: %d\n", fileSize);
    //            fclose(file);
    //            fullyReceived = true;
    //            printf("Received file successfully!\n");
    //            showTime();
    //            int done = 1;
    //            write (serverSocket, &done, sizeof(done));
    //        }
    //        while (1) {
    //            if (clientReceived == 2) break;
    //        }

    //        received = false;
    //        fullyReceived = false;
    //        clientReceived = 0;
    //        printf("Closed\n\n");
    //    }
    //    else if (downloadType == 2) {
    //        read (serverSocket, receiveBuffer, sizeof(receiveBuffer));

    //        printf("Download from %s\n", receiveBuffer);

    //        serverSocket3 = socket(_family, _type, _protocol);
    //        if (serverSocket3 < 0) {
    //            perror("Server socket error");
    //            return 1;
    //        }
    //        serverAddr3.sin_addr.s_addr = inet_addr(receiveBuffer);
    //        serverAddr3.sin_port = htons(_port);
    //        serverAddr3.sin_family = _family;

    //        // Connect to server by using serverSocket
    //        int connCheck = -1;
    //        while (connCheck < 0) {
    //            connCheck = connect(serverSocket3, (struct sockaddr *) &serverAddr3, sizeof(serverAddr3));
    //        }
    //        if (connCheck < 0) {
    //            perror("Connect error");
    //            return 1;
    //        }

    //        printf("Connected to server.\n");

    //        // Receive file size
    //        unsigned int fileSize = 0;
    //        unsigned int receivedSize = 0;
    //        nbytes = read(serverSocket3, &fileSize, sizeof(fileSize));
    //        if (nbytes < 0) {
    //            perror("Read error");
    //            return 1;
    //        }
    //        fileSize = ntohl(fileSize);
    //        // Handle error
    //        if (fileSize == -1) {
    //            printf("Cannot download file '%s'!\n", fileName);
    //        }
    //        // Receive file
    //        else {
    //            FILE *file = fopen(fileName, "wb");
    //            printf("Receiving file '%s'\n", fileName);
    //            printf("File size: %d\n", fileSize);
    //            while (receivedSize < fileSize) {
    //                int currRcvSize = read(serverSocket3, receiveBuffer, 1024);
    //                receivedSize += currRcvSize;
    //                fwrite(receiveBuffer, 1, currRcvSize, file);
    //            }
    //            fclose(file);
    //            printf("Received file successfully!\n");
    //            showTime();
    //        }
    //        close(serverSocket3);
    //        printf("Closed\n\n");
    //        int done = 1;
    //        write (serverSocket, &done, sizeof(done));
    //    }


