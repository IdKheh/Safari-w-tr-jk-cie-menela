#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
#include <algorithm>

// odpowiedź na komunikat ACK
void sendACK(packet_t & pakiet){
    int& sender = pakiet.src;
    sendPacket(&pakiet,sender,messages::ACK);
}

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;

    while(true) {
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &lamportMut );
        lamport= std::max(lamport,pakiet.ts)+1;
        pthread_mutex_unlock( &lamportMut );

                switch (status.MPI_TAG) {
                    case messages::START:
                        global.lock();
                        // potwierdzenie wycieczki - powinien być jeszcze warunek bycia na czele kolejki na razie idzie każdy kto dostał ACK
                        if (global.numberOfACK == size-1) {
                            changeState(state::TRACE);
                            global.numberOfACK = 0;
                        }
                        else {
                            if (stan == state::WAIT) debug("nie dostałem od wszystkich ACK, nie idę :(");
                        }
                        global.unlock();
                        break;
                    case messages::REQUEST:
                        global.lock();
                        global.addProcess(pakiet.ts, pakiet.src);
                        global.unlock();
                        // for (const auto& p : global.kolejka) {
                        //     std::cout << "ID: " << p.id << std::endl;
                        // }
                        sendACK(pakiet);
                        break;
                    case messages::ACK:
                        global.lock();
                        global.numberOfACK++;
                        debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, global.numberOfACK);
                        global.unlock();
                        break;
                    case messages::END:
                        debug("Cos poszlo nie tak");
                        break;
                    default:
                        debug("Nie powinno być takiego typu kominikatu (??)");
                        break;
                }
                

        
    }
}
