#include <vector>
#include <iostream>

struct proces{
    int id;
    int ts;
    proces(int ts, int id): ts(ts), id(id){}
};
struct data{
    std::vector<proces> kolejka; //id zgloszonych procesow
    std::vector<int> usunKolejka; //usuwa G procesow
    int przewodnicy;
    void lock();
    void unlock();
    void addProcess(int ts, int id);
    data();

    int numberOfACK;
    int numberOfEnd;
};