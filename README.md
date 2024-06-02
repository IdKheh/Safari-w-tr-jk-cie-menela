# Temat: Safari w trójkącie menela
Poszukujący mocnych wrażeń turyści pokochali nowy sposób spędzania wolnego czasu: safari w Poznaniu, nocą na Dębcu. Po założeniu koszulek "kochamy Legia Warszawa" turyści, pod opieką przewodnika, ruszają na Dębiec.

Turyści najpierw rezerwują przewodnika. Jest P nierozróżnialnych przewodników, każdy opiekuje się grupą turystów o rozmiarze G. Turystów musi być co najmniej 2*G.
Wycieczka rusza, gdy grupa osiąga rozmiar G.
W czasie wycieczki turysta może zostać pobity. W takim wypadku trafia do szpitala i przez pewien czas nie bierze udziału w wycieczce.
Zaimplementować procesy T turystów. T >> P

## Autorzy: Dawid Drożdżyński 151867, Wiktoria Dębowska 151874


Opis algorytmu:
* Założenia: kanały FIFO, niezawodne
* Działanie:
    - T procesów turystów inicjalizuje działanie z zerowym zegarem. Każdy z nich posiada kolejkę (wektor) ubiegania się o przewodnika oraz liczbę dostępnych w danym czasie przewodników ustawioną początkowo na P
    - Każdy proces Pi po losowym czasie ubiega się o dostęp do przewodnika wysyłając wszystkim pozostałym procesom wiadomość REQUEST 
    - Proces Pj po otrzymaniu od Pi wiadomości REQUEST dopisuje jego identyfikator oraz zegar do kolejki ubiegającej się o przewodnika zgodnie z kolejnością zegara (jeżeli zegary są takie same to na podstawie id procesu) i odsyła wiadomość ACK
    - Jeśli proces Pi otrzyma od wszystkich innych potwierdzenia wie że znalazł się w kolejce i cierpliwie czeka na przewodnika
    - Jeżeli kolejka osiągnie rozmiar G i liczba zajętych przewodników jest mniejsza od liczby P, to rusza wycieczka. Proces który wykryje tę sytuację ustala losowy czas wycieczki, usuwa również z kolejki G procesów oraz zmniejsza liczbę dostępnych przewodników i wysyła START do wszystkich procesów.
    - Turysta który otrzyma wiadomość START a wcześniej otrzymał ACK od wszystkich procesów odczekuje zadany czas trwania wycieczki. Następnie losuje czy został pobity w czasie wycieczki. Wysyła do wszystkich procesów END informując o końcu wycieczki. Jeśli został pobity trafia do szpitala (nie ubiega się o przewodnika przez losowy czas). Jeśli wyszedł cały z wycieczki ubiega się od razu ponownie o dostęp do przewodnika rozsyłając wiadomość REQUEST.
    - Po otrzymaniu START proces usuwa pierwsze G procesów z kolejki i zmniejsza liczbę przewodników o 1. Po otrzymaniu END od G procesów liczba dostępnych przewodników jest inkrementowana o 1.
* Złożoność:
	- czasowa: 4
	- komunikacyjna: 4n - 4