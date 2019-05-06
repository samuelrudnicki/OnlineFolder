/**
 * 
 * 
 * Colocar structs, defines comuns a clientes e servidor aqui
 * 
 * 
 * */

#define PORT 4000

#define TRUE 1

#define FALSE 0

#define WAITING 2

#define PAYLOAD_SIZE 1024

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#define CLIENT_NAME_SIZE 64

#define FILENAME_SIZE 512

typedef struct packet {
    uint16_t type; // Tipo do pacote ( DATA | CMD )
    uint16_t seqn; // Numero de sequencia
    uint16_t length; // Comprimento do payload
    uint32_t total_size; // Numero total de fragmentos
    char clientName[CLIENT_NAME_SIZE];
    char fileName[FILENAME_SIZE];
    char _payload[PAYLOAD_SIZE]; // Dados do pacote
} packet;


#define PACKET_SIZE (sizeof (struct packet))

void serializePacket(packet* inPacket, char* serialized);

void deserializePacket(packet* outPacket, char* serialized);

/*
  Lança uma thread para ficar no watcher no path de argumento
*/
void *inotifyWatcher(void *pathToWatch);

/*
  Verifica se o usuario já tem o diretorio criado na pasta User com seu nome, e se não tiver já o cria
*/
int checkAndCreateDir(char *pathName);

