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
                // for(int i=0; i<ARRAYSIZE; i++){
                //     debug("Nr w kolejce:%d, proces:%d", i, pakiet.dataArray[i]);
                // }
                global.przewodnicy -= 1;
                for(int i=0; i<SIZE; i++){
                    if (pakiet.dataArray[i] == rank) {
                        global.going = true;
                    }
                    for(int j=0; j<global.kolejka.size(); j++) {
                        if (pakiet.dataArray[i] == global.kolejka[j].id) {
                            global.kolejka.erase(global.kolejka.begin() + j);
                        }
                    }
                }
                // potwierdzenie wyjścia na wycieczkę
                if (global.numberOfACK == size-1) {
                    if (global.going) {
                        global.numberOfACK = 0;
                        global.going = false;
                        debug("Idziemy na wycieczke, bierzemy misia w teczke...");
                        changeState(state::TRACE);
                    }
                    else {
                        debug("Nie jestem na początku kolejki, nie idę jeszcze");
                    }
                }
                global.unlock();
                break;
            case messages::REQUEST:
                global.lock();
                global.addProcess(pakiet.ts, pakiet.src);
                global.unlock();
                sendACK(pakiet);
                break;
            case messages::ACK:
                global.lock();
                global.numberOfACK++;
                // debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, global.numberOfACK);
                global.unlock();
                break;
            case messages::END:
                global.lock();
                global.numberOfEnd++;
                // debug("Dostałem END od %d, mam już %d", status.MPI_SOURCE, global.numberOfEnd);
                if (global.numberOfEnd == SIZE) {
                    global.numberOfEnd = 0;
                    global.przewodnicy += 1;
                }
                global.unlock();
                break;
            default:
                debug("Nie powinno być takiego typu komunikatu (??)");
                break;
        }
    }
}
