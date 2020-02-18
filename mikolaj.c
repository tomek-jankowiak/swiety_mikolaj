#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

pthread_t santaClausID, gnomeID[10], reindeerID[9];
pthread_mutex_t santaMutex;
pthread_cond_t santaCond;

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

    printf("Koniec\n");
}

void init()
{
    if(pthread_mutex_init(&santaMutex, NULL) != 0)
    {
        perror("Utworzenie mutexa mikolaja");
        exit(1);
    }

    if(pthread_cond_init(&santaCond, NULL) != 0)
    {
        perror("Utworzenie zmiennej warunkowej mikolaja");
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
        pthread_cond_wait(&santaCond, &santaMutex);
        if(reindeerReady)
        {
            printf("Mikolaja budza renifery\n");
            printf("Mikolaj zaprzega renifery\n");
            printf("Mikolaj dostarcza zabawki\n");
            sleep(2);
            printf("Mikolaj wyprzega renifery\n");
            reindeerCount = 0;
            reindeerReady = false;
            if(gnomeReady)
                printf("Mikolaj spi\n");
        }
        if(gnomeReady)
        {
            printf("Mikolaja budza skrzaty\n");
            printf("Mikolaj wpuszcza do biura %d skrzatow\n", gnomeCount);
            printf("Mikolaj obsluguje skrzaty\n");
            sleep(1);
            printf("Mikolaj wypuszcza skrzaty\n");
            gnomeCount = 0;
            gnomeReady = false;
            if(reindeerReady)
                printf("Mikolaj spi");
        }
        pthread_mutex_unlock(&santaMutex);
    }
    pthread_mutex_lock(&santaMutex);
    for(int i = 0; i < 9; i++)
        pthread_cancel(reindeerID[i]);
    
    for(int i = 0; i < 10; i++)
        pthread_cancel(gnomeID[i]);
    pthread_mutex_unlock(&santaMutex);

    pthread_exit(0);
}

void *gnome(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1)
    {
        pthread_mutex_lock(&santaMutex);
        if(gnomeCount < 10)
        {
            gnomeCount++;
            printf("Czeka %d skrzatow\n", gnomeCount);
        }
        if(gnomeCount >= 3)
        {
            gnomeReady = true;
            pthread_cond_signal(&santaCond);
        }
        pthread_mutex_unlock(&santaMutex);
    }
    
}

void *reindeer(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1)
    {
        pthread_mutex_lock(&santaMutex);
        if(reindeerCount < 9)
        {
            reindeerCount++;
            printf("Czeka %d reniferow\n", reindeerCount);
        }     
        else
        {
            reindeerReady = true;
            pthread_cond_signal(&santaCond);
        }
        pthread_mutex_unlock(&santaMutex); 
    }
}
