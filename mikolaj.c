#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// według wikipedii, zaprzęg Świętego Mikołaja liczy 9 reniferów
// "Jest Waleczny, i Walczyk, i Strzała, i Swarek
// Zefir, Kupidyn, Kometa i Śmiałek
// Sławą przyćmił ich
// Rudolf z nosem co w mroku lśni"
//                              Zbigniew Wodecki

pthread_t santaClausID, gnomeID[10], reindeerID[9];
pthread_mutex_t santaMutex, gnomeMutex, reindeerMutex;
pthread_cond_t santaCond, countCond;

int gnomeCount = 0, reindeerCount = 0;
bool gnomeReady = false, reindeerReady = false;

void init();
void join();
void *santa(void *arg);
void *gnome(void *arg);
void *reindeer(void *arg);

int main()
{
    init();
    join();

}

void init()
{
    if(pthread_mutex_init(&santaMutex, NULL) != 0)
    {
        perror("Utworzenie mutexa mikolaja");
        exit(1);
    }

    if(pthread_mutex_init(&gnomeMutex, NULL) != 0)
    {
        perror("Utworzenie mutexa skrzata");
        exit(1);
    }

    if(pthread_mutex_init(&reindeerMutex, NULL) != 0)
    {
        perror("Utworzenie mutexa renifera");
        exit(1);
    }

    if(pthread_cond_init(&santaCond, NULL) != 0)
    {
        perror("Utworzenie zmiennej warunkowej mikolaja");
        exit(1);
    }

    if(pthread_cond_init(&countCond, NULL) != 0)
    {
        perror("Utworzenie zmiennej warunkowej liczacej");
        exit(1);
    }

    if(pthread_create(&santaClausID, NULL, santa, NULL) != 0)
    {
        perror("Utworzenie watku mikolaja");
        exit(1);
    }

    for(int i = 0; i < 10; i++)
        if(pthread_create(&gnomeID[i], NULL, gnome, NULL) != 0)
        {
            perror("Utowrzenie watku skrzata");
            exit(1);
        }
    for(int i = 0; i < 9; i++)
        if(pthread_create(&reindeerID[i], NULL, reindeer, NULL) != 0)
        {
            perror("Utworzenie watku renifera");
            exit(1);
        }
}

void join()
{
    pthread_join(santaClausID, NULL);

    for(int i = 0; i < 10; i++)
        pthread_join(gnomeID[i], NULL);

    for(int i = 0; i < 9; i++)
        pthread_join(reindeerID[i], NULL);
}

void *santa(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    for(int i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&santaMutex);
        printf("Mikolaj spi\n");
        pthread_cond_broadcast(&countCond);
        pthread_cond_wait(&santaCond, &santaMutex);
        if(reindeerReady)
        {
            pthread_mutex_lock(&reindeerMutex);
            printf("Mikolaja budza renifery\n");
            printf("Mikolaj zaprzega renifery\n");
            printf("Mikolaj dostarcza zabawki\n");
            sleep(1);
            printf("Mikolaj wyprzega renifery\n");
            reindeerCount = 0;
            reindeerReady = false;
            pthread_mutex_unlock(&reindeerMutex);
        }
        if(gnomeReady)
        {
            pthread_mutex_lock(&gnomeMutex);
            printf("Mikolaja budza skrzaty\n");
            printf("Mikolaj wpuszcza do biura %d skrzatow\n", gnomeCount);
            printf("Mikolaj obsluguje skrzaty\n");
            printf("Mikolaj wypuszcza skrzaty\n");
            gnomeCount = 0;
            gnomeReady = false;
            pthread_mutex_unlock(&gnomeMutex);
        }
        pthread_mutex_unlock(&santaMutex);
    }
}

void *gnome(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1)
    {
        pthread_mutex_lock(&gnomeMutex);
        pthread_cond_wait(&countCond, &gnomeMutex);
        if(gnomeCount < 10)
            gnomeCount++;
        if(gnomeCount >= 3)
        {
            gnomeReady = true;
            pthread_cond_signal(&santaCond);
        }
        printf("Czeka %d skrzatow\n", gnomeCount);
        pthread_mutex_unlock(&gnomeMutex);
    }
    
}

void *reindeer(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1)
    {
        pthread_mutex_lock(&reindeerMutex);
        pthread_cond_wait(&countCond, &reindeerMutex);
        if(reindeerCount < 9)
            reindeerCount++;     
        else
        {
            reindeerReady = true;
            pthread_cond_signal(&santaCond);
        }
        printf("Czeka %d reniferow\n", reindeerCount);
        pthread_mutex_unlock(&reindeerMutex); 
    }
}


