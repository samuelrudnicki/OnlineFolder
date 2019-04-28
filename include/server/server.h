#define MAXNAME 64
#define MAXFILES 20
#define FREEDEV -1

#define INSERTDEVICE 0
#define REMOVEDEVICE 1

#define SUCESS 1

struct clientList *clientList; //Inicialização do nodo inicial da lista de clientes

void *handleConnection(void *socketDescriptor);
void appendNewClient(int socketNewClient, char* userName);
int updateNumberOfDevices(struct clientList *client_node, int socketNumber, int option);

struct client
{
  int devices[2];
  char userName[MAXNAME];
};

struct clientList
{
  struct client client;
  struct clientList *next;
};