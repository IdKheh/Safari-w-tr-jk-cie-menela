#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

state stan=state::WAIT;
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;


struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = {{"start procesu", messages::START },
                {"prośbę o sekcję krytyczną", messages::REQUEST},
                {"potwierdzenie", messages::ACK},
                {"koniec wycieczki", messages::END}};

const char *const tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	    if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}

/* tworzy typ MPI_PAKIET_T  analogia do MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE ...) */
void initTypePacket()
{
    int blocklengths[NITEMS] = {1,1,1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt==nullptr) { 
        pkt = (packet_t*)malloc(sizeof(packet_t)); 
        freepkt=1;
    }

    pkt->src = rank;
    pthread_mutex_lock( &lamportMut );
    pkt->ts = ++lamport;
    pthread_mutex_unlock( &lamportMut );
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string( tag), destination);
    if (freepkt) free(pkt);

}

void sendPacketBroadcast(packet_t *pkt, int tag, int ts)
{
    int freepkt=0;
    if (pkt==nullptr) { 
        pkt = (packet_t*)malloc(sizeof(packet_t)); 
        freepkt=1;
    }
    pkt->src = rank;
    pkt->ts = ts;

    for (int i = 0; i < size; i++){
        if (i == rank) continue; // nie wysyłamy sami do siebie
        
        MPI_Send( pkt, 1, MPI_PAKIET_T, i, tag, MPI_COMM_WORLD);
        debug("Wysyłam %s do %d\n", tag2string(tag), i);
    }
    if (freepkt) free(pkt);
}

void changeState(state newState )
{
    pthread_mutex_lock( &stateMut );
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}
