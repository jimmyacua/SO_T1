#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include "Diccionario.h"
#include "Fichero.h"
#include <wait.h>
#include <sys/sem.h>

#include "Buzon.h"
#include "Semaforo.h"


struct AreaCompartida {
    int numEtq;
    struct Etiqueta {
        char etq[MSGSIZE];
        int Veces;
    }
            Etiquetas[50];
};

typedef struct AreaCompartida AC;

struct my_msgbuf {
    long mtype;     // message type, must be > 0
    int times;	// Times that label appears
    char label[MSGSIZE];  // Label to send to mailbox
};

struct my_msgbuf B;

string finalTag = "finalTag";

int main(int argc, char** argv) {

    //--------Memoria compartida----------------
    AC * area;
    int id = shmget(KEY,1024,0600|IPC_CREAT);
    area = (AC *) shmat(id, NULL, 0);
    //------------------------------------------
    Semaforo sem;
    Fichero f;
    //Buzon b;

    int idB = msgget(KEY,0600|IPC_CREAT);
    if ( -1 == idB) {
        perror("Constuctor buzon: ");
        exit(1);
    }
    int st = 0;
    if(!fork()) { //proceso hijo
        string archivo = "/home/jimmy/GitHub/SO_T1/ejemploXML.xml";
        f.leerArchivo(archivo);
        int cont = 1;
        while (cont <= f.totalEtq()) {
            area->numEtq++;
            //strncpy(area->Etiquetas[cont].etq ,f.getEtq(cont).c_str(),MSGSIZE);
            strncpy(B.label ,f.getEtq(cont).c_str(),MSGSIZE);
            B.times = f.getTimes(cont);
            B.mtype = 1;
            ssize_t len = sizeof(B.label)-sizeof(long);
            st = msgsnd(idB, &B, len, IPC_NOWAIT);
            area->Etiquetas[cont].Veces = f.getTimes(cont);
            //b.Enviar(f.getEtq(cont).c_str(), f.getTimes(cont), 1);
            cont++;
        }
        strncpy(B.label ,finalTag.c_str(),MSGSIZE);
        ssize_t len = sizeof(B.label)-sizeof(long);
        st = msgsnd(idB, &B, len, IPC_NOWAIT);

        sem.Signal();
        _exit(0);
    }
        struct my_msgbuf r;
        cout << "Proceso padre:" << endl;
        sem.Wait();
        //int cont = b.Recibir(1);
        st = msgrcv(idB, &r, MSGSIZE, 1, IPC_NOWAIT);
        int cont = 0;
        while (st > 0) {
            area->numEtq++;
            strncpy(area->Etiquetas[cont].etq, r.label, MSGSIZE);
            area->Etiquetas[cont].Veces = r.times;
            cout << "Etiq: " << r.label << ", veces: " << r.times << endl;
            st = msgrcv(idB, &r, MSGSIZE, 1, IPC_NOWAIT);
            cont++;
        }

        cout << "fin" << endl;

        msgctl(idB, IPC_RMID, NULL);
        shmdt(area);
        shmctl(id, IPC_RMID, NULL);
        _exit(0);

}