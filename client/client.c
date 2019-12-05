// Ho ten: Nguyen Viet Hung
// Ma SV: 16021588
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main()
{
  const int _family = AF_INET;   // IPv4
  const int _type = SOCK_STREAM; // TCP
  const int _protocol = 0;

  int serverSocket;
  struct sockaddr_in serverAddr;
  char *serverIpAddr = (char *)malloc(100 * sizeof(char *));
  int serverPort = 6398;

  char *path = "SharedFolder";
  char fileName[1024];
  char receiveFileBuffer[1024];
  int nbytes = 0;

  serverSocket = socket(_family, _type, _protocol);
  if (serverSocket < 0)
  {
    perror("Server socket error");
    return 1;
  }

  bzero(&serverAddr, sizeof(serverAddr));
  printf("Server IP Address: ");
  scanf("%s", serverIpAddr);

  serverAddr.sin_addr.s_addr = inet_addr(serverIpAddr);
  serverAddr.sin_port = htons(serverPort);
  serverAddr.sin_family = _family;

  // Connect to server
  int connectResult = connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if (connectResult < 0)
  {
    perror("Connect error");
    return 1;
  }
  printf("Connected to server.\n");

  while (true)
  {
    // nhập lệnh và gửi cho server
    char buffer[1024];
    char command[1024];
    printf("Nhap cau lenh:\n");
    fflush(stdin);
    scanf("%s", command);
    fflush(stdin);
    
    write(serverSocket, command, sizeof(command));

    // nếu command là dơnload hoặc upload thì xử lý riêng
    //server và client giao tiếp lần lượt với nhau,
    // khi 1 bên gửi, nó sẽ chờ đến khi nhận được phẩn hồi của bên kia
    // thì mới thục hiện tiếp
    if (strcmp(command, "download") == 0)
    {
      read(serverSocket, buffer, sizeof(buffer));

      printf("nhap ten file can download: ");
      fflush(stdin);
      scanf("%s", command);

      write(serverSocket, command, sizeof(command));
      bzero(buffer, sizeof(buffer));
      read(serverSocket, buffer, 1024);

      if (strcmp(buffer, "send file") == 0)
      {
        write(serverSocket, "chuan bi nhan file", 18);

        int length = 0; //length of file
        int rec = 0;    // số lượng ký tự đã nhận được

        read(serverSocket, fileName, sizeof(fileName));

        printf("Filename: %s\n", fileName);
        write(serverSocket, "da nhan filename", 16);

        // nhận file length
        unsigned int fileSize = 0;
        unsigned int receivedSize = 0;
        nbytes = read(serverSocket, &fileSize, sizeof(fileSize));
        if (nbytes < 0)
        {
          perror("Read error");
          return 1;
        }
        fileSize = ntohl(fileSize);
        // Receive file
        FILE *file = fopen(fileName, "wb+");
        printf("Receiving file '%s'\n", fileName);
        printf("File size: %d\n", fileSize);

        // bắt đầu nhận file
        while (receivedSize < fileSize)
        {
          int currRcvSize = read(serverSocket, receiveFileBuffer, 1024);
          receivedSize += currRcvSize;
          fwrite(receiveFileBuffer, 1, currRcvSize, file);
        }
        fclose(file);
        printf("Received file successfully!\n");
      }
      else
      {
        printf("Server: %s\n", buffer);
      }
    }
    else if (strcmp(command, "upload") == 0)
    {
      read(serverSocket, buffer, sizeof(buffer));
      printf("nhap ten file can upload: ");
      fflush(stdin);
      bzero(command, sizeof(command));
      scanf("%s", command);

      write(serverSocket, command, sizeof(command));
      bzero(buffer, sizeof(buffer));
      read(serverSocket, buffer, 1024);

      //tạo đường dẫn tuyệt đối
      char fullPath[1024];
      strcpy(fullPath, path);
      strcat(fullPath, "/");
      strcat(fullPath, command);
      printf("%s\n", fullPath);
      FILE *file = fopen(fullPath, "rb");
      if (file == NULL)
      {
        printf("Cannot find file '%s'!\n", command);
        write(serverSocket, "file khong ton tai", 19);
      }
      else
      {
        write(serverSocket, "file ton tai", 13);
        read(serverSocket, buffer, sizeof(buffer));
        int size;
        printf("Sending file '%s'\n", fileName);
        // Send file length first
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
        printf("File size: %d\n", size);
        size = htonl(size);
        write(serverSocket, (void *)&size, sizeof(size));
        read(serverSocket, buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer));
        // Send file
        while (!feof(file))
        {
          int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
          write(serverSocket, buffer, readSize);
          bzero(buffer, sizeof(buffer));
        }
        fclose(file);
        printf("Sent file successfully!\n");
      }
    }
    else
    // các trường hopwj khác 
    // show ra thông báo từ phía server
    {
      read(serverSocket, buffer, 1024);
      printf("Server: %s\n", buffer);
      // khi người dùng gõ lệnh @LOGOUT, server guiwr lại message ok để đóng kết nối
      if (strcmp(buffer, ">> OK") == 0)
      {
        break;
      }
    }
    bzero(buffer, sizeof(buffer));
  }
  close(serverSocket);
  printf("Closed connection.\n");
  return 0;
}
