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

#define PACKET_SIZE 1024

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

typedef struct packet {
    uint16_t type; // Tipo do pacote ( DATA | CMD )
    uint16_t seqn; // Numero de sequencia
    uint32_t total_size; // Numero total de fragmentos
    uint16_t length; // Comprimento do payload
    char* fileName;
    const char* _payload; // Dados do pacote
} packet;

void serializePacket(packet inPacket, char* serialized, int size);

void deserializePacket(packet outPacket, char* serialized, int size);

/*
  Lança uma thread para ficar no watcher no path de argumento
*/
void *inotifyWatcher(void *pathToWatch);

/*
  Verifica se o usuario já tem o diretorio criado na pasta User com seu nome, e se não tiver já o cria
*/
int checkAndCreateDir(char *pathName);

