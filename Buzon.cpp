//
// Created by jimmy on 11/09/18.
//



#include <printf.h>
#include "Buzon.h"
#include <iostream>

using namespace std;

struct my_msgbuf {
    long mtype;       // message type, must be > 0
    int times;
    char msj[MSGSIZE];    // message data
};


Buzon::Buzon(){
    id = msgget(KEY,IPC_CREAT|0600);
    if(-1 == id){ //hubo error
        perror("Buzon::constructor");
        exit(2);
    }
}

Buzon::~Buzon(){
    int st = msgctl(id, IPC_RMID, NULL);
    printf("Destructor \n");
}

int Buzon::Enviar(const char * nmsj, int i, long type){
    struct my_msgbuf sender;
    ssize_t len = sizeof(sender.msj)-sizeof(long);
    sender.mtype = type;
    sender.times = i;
    strncpy(sender.msj, nmsj , MSGSIZE);
    int st = msgsnd(id,&sender,len, IPC_NOWAIT);
    if(-1 == st){ //hubo error
        perror("Buzon::enviar falló");
        exit(2);
    }
}

int Buzon::Recibir(long type) {
    struct my_msgbuf receiver;
    int st = msgrcv(id,&receiver, MSGSIZE,type,MSG_NOERROR | IPC_NOWAIT);
    if(-1 == st){ //hubo error
        perror("Buzon::recibir falló");
        exit(2);
    } else{
        char mensaje[MSGSIZE];
        int times = receiver.times;
        strncpy(mensaje, receiver.msj , MSGSIZE); //si se hace asignación no funciona correctamente.
        //printf("El mensaje recibido es:%s\n", mensaje, " aparece :", times);
        cout << "El mensaje es: "  << mensaje << " y aparece: " << times << " veces." << endl;
    }
    return  st;
}