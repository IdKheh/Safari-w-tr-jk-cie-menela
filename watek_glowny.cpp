#include "main.h"
#include "watek_glowny.h"
#include <memory.h>

void mainLoop()
{
    srandom(rank);

	while(true){
		auto* pkt = new packet_t();
		switch (stan){
			case state::RUN:
				debug("Zyje");
				if(std::rand()%1000 >=700) break; // po jakims losowym czasie...
				
				pthread_mutex_lock(&lamportMut);
				sendPacketBroadcast(pkt,messages::REQUEST,lamport++);
				pthread_mutex_unlock(&lamportMut);

				changeState(WAIT);
				delete pkt;
				break;
			case state::WAIT:
				debug("Chce isc na wycieczke!");
				global.lock();
				if(global.kolejka.size()>SIZE && global.numberOfACK == size-1){		// jeśli wykryjemy przepełnienie to wysyłamy wektor z kolejką
					for(int i=0;i<GUIDE;i++){
						if(global.kolejka.size()>=SIZE) { //istnieje mozliwosc ze wiecej niz jeden proces wysle informacje o przepelnieniu
							pthread_mutex_lock(&lamportMut);


							// TODO: wysłanie wektora z procesami oraz ustawienie nr przewodnika
							/*
							if(global.numberOfACK ==size-1){
					int i=0;
					for(const auto& p: global.kolejka){ 
						if(p.id==rank && i<SIZE){
							changeState(state::TRACE);
						}
					}
				}
							*/


							sendPacketBroadcast(pkt,messages::START,lamport++);
							pthread_mutex_unlock(&lamportMut);
						}
					}
				}
				global.unlock();
				break;
			case state::TRACE:
				debug("Idziemy na wycieczke, bierzemy misia w teczke...");
				sleep(TIMETRACE); //jestesmy na wycieczce i nas nie ma :(

				pthread_mutex_lock(&lamportMut);
				sendPacketBroadcast(pkt,messages::END,lamport++);
				pthread_mutex_unlock(&lamportMut);

				// TODO: Usuwanie wektora kolejki i wysłanie takiego wektora

				if(std::rand()%1000 >=900) changeState(state::HOSPITAL);
				else changeState(state::RUN);
				break;
			case state::HOSPITAL:
				debug("Pobito mnie!");
				sleep(3*TIMETRACE);
				changeState(state::RUN);
				break;
			default:
				debug("Niepoprawny stan");
				break;
		}
	}
	sleep(SEC_IN_STATE);
}