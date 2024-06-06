#include "main.h"
#include "watek_glowny.h"
#include <memory.h>
#include <random>

void mainLoop()
{
	std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 100);
	changeState(state::RUN);
	while(true){
		auto* pkt = new packet_t();
		global.lock();
		
		if(global.kolejka.size() >= SIZE && global.przewodnicy > 0){
			debug("Kolejka ma rozmiar: %d. Wysyłam START. Ilość wolnych przewodników: %d", global.kolejka.size(), global.przewodnicy);
			pthread_mutex_lock(&lamportMut);
			// inicjalizacja tablicy
			int values[ARRAYSIZE];
			for (int i = 0; i < ARRAYSIZE; ++i) {
        		values[i] = -1;
   		    }
			// przepisanie danych z kolejki
			for (size_t i = 0; i < global.kolejka.size(); i++) {
				values[i] = global.kolejka[i].id;
			}
			sendPacketBroadcast(pkt,messages::START,lamport++,values);
			// czyszczenie pierwszych elementów
			global.kolejka.erase(global.kolejka.begin(), global.kolejka.begin() + SIZE);
			global.przewodnicy -= 1;
			pthread_mutex_unlock(&lamportMut);
		}
		
		global.unlock();
		switch (stan){
			case state::RUN:
				debug("Zyje");
				if(distribution(generator) > 40) {
					sleep(2);
					break; // po jakims losowym czasie...
				}
				debug("Chce isc na wycieczke!");
				// dodanie siebie do swojej kolejki
				global.lock();
				global.addProcess(lamport, rank);
				global.unlock();
				// prośba do reszty
				pthread_mutex_lock(&lamportMut);
				sendPacketBroadcast(pkt,messages::REQUEST,lamport++,nullptr);
				pthread_mutex_unlock(&lamportMut);
				changeState(state::WAIT);
				delete pkt;
				break;
			case state::WAIT:
				// czekaj na wycieczkę
				break;
			case state::TRACE:
				sleep(TIMETRACE); //jestesmy na wycieczce i nas nie ma :(
				pthread_mutex_lock(&lamportMut);
				sendPacketBroadcast(pkt,messages::END,lamport++,nullptr);
				pthread_mutex_unlock(&lamportMut);

				if(distribution(generator) > 50) changeState(state::HOSPITAL);
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
}