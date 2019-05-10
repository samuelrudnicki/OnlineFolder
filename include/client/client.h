struct inotyClient{
  char userName[64];
  int socket;
};

char lastFile[100];

void *listener(void *socket);

void synchronize(int sockfd,char* clientName);

void clientListServer(int sockfd);

void clientSyncServer(int sockfd, char* clientName);