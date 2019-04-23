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

#define PACKET_SIZE 1024

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

