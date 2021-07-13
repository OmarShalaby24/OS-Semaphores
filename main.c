#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define counters  50

sem_t mess, empty, full;
// three semaphores are initialized one for the messages, one for the empty slots in the buffer and the last for the the full slots in the buffer
int data=0;
int buffer[10];


    /*
     *locks the counter of the mess semaphore add the message then unlock the semaphore for another message to be recieved
     *then wait for the message to be written in the buffer
    */
void * message (void * arg)
{
    sleep(rand()% 100);
    sem_wait(&mess);
    data =data + 1;
    sem_post(&mess);
    printf("recieved a message %d\n",data);
    printf("waiting to write\n\n");
}

/*
 *monitor is the responsible thread for writing the message in the buffer
*/

void * monitor (void * arg)
{
    int i =0;
    while(1)
    {
        sleep(rand()%100);

        sem_wait(&full);
        int message_content =data;
        data = 0;
        buffer[i]=message_content;

        printf("waiting to read message\n");
        printf("reading message of value %d\n",message_content);
        printf("writing to buffer at position %d\n\n",i++);

        if (i>9)
            i=0;

        sem_post(&full);
        sem_post(&empty);
    }
}

/*
 *collector is responsible for fetching(read) the message form the buffer and print it
*/
void * collector (void * arg)
{
    int i=0;
    while (1)
    {
        sleep(rand() % 100);

        sem_wait(&empty);
        sem_wait(&full);
        int message_content = buffer[i];
        printf("buffer reads %d from position %d\n",message_content,i);
        i++;
        if (i>9)
            i=0;
        sem_post(&full);
    }
}

int main()
{
    pthread_t monitort,collectort;
    pthread_t messages[counters];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    sem_init(&mess,1,1);
    sem_init(&full,1,1);
    sem_init(&empty,1,0);

    int i;
    for(i=0; i<counters; i++)
        pthread_create(&messages[i],&attr,message,NULL);

    pthread_create(&monitort,&attr,monitor,NULL);
    pthread_create(&collectort,&attr,collector,NULL);

    for(i=0; i<counters; i++)
        pthread_join(messages[i],NULL);

    pthread_join(monitort,NULL);
    pthread_join(collectort,NULL);

    return 0;
}
