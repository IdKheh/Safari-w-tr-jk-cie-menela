#ifndef UTILH
#define UTILH
#include "main.h"

/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (zegar lamporta */
    int src;  

    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
} packet_t;
/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 3

/* Typy wiadomości - TYPY PAKIETÓW  */
enum messages{
    START,  // start wycieczki
    REQUEST, // czeka na sekcję krytyczną
    ACK, // potwierdzenie
    END // koniec wycieczki
};

extern MPI_Datatype MPI_PAKIET_T;
void initTypePacket();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);
void sendPacketBroadcast(packet_t *pkt, int tag, int ts);

enum state{
    RUN, //proces jest uruchomiony
    WAIT, // czeka na sekcję krytyczną
    TRACE, //wycieczka
    HOSPITAL // pobyt w szpitalu
};

extern state stan;
extern pthread_mutex_t stateMut;
extern pthread_mutex_t lamportMut;
extern pthread_mutex_t globalMutex;

/* zmiana stanu, obwarowana muteksem */
void changeState(state newState);
#endif
