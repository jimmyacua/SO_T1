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
#include <unistd.h>
#include <list>
#include <iterator>

#include "Semaforo.h"


#define MSGSIZE 80

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
    area->numEtq = 0;
    //------------------------------------------
    Semaforo sem;
    Fichero f;

    int idB = msgget(KEY,0600|IPC_CREAT);
    if ( -1 == idB) {
        perror("Constuctor buzon: ");
        exit(1);
    }

    int st = 0;
    int n = argc;
    for(int i = 1; i< n; i++){
        if(!fork()) { //proceso hijo
            //string archivo = "ejemploXML.xml";
            string archivo = argv[i];
            f.leerArchivo(archivo);
            int cont = 1;
            while (cont < f.totalEtq()) {
                strncpy(B.label ,f.getEtq(cont).c_str(),MSGSIZE);
                B.times = f.getTimes(cont);
                B.mtype = i;
                ssize_t len = sizeof(B.label)-sizeof(long);
                st = msgsnd(idB, &B, len, IPC_NOWAIT);
                cont++;
            }
            strncpy(B.label ,finalTag.c_str(),MSGSIZE);
            ssize_t len = sizeof(B.label)-sizeof(long);
            B.times = -13;
            st = msgsnd(idB, &B, len, IPC_NOWAIT);

            sem.Signal();
            _exit(0);
        }
    }

    AC ap; //arreglo de padre
    struct my_msgbuf r;
    for(int i=1; i<n; i++){ //recibe un mensaje de cada hijo
        sem.Wait();
        st = msgrcv(idB, &r, MSGSIZE, i, IPC_NOWAIT);
        strncpy(ap.Etiquetas[i].etq,r.label, MSGSIZE);
        ap.Etiquetas[i].Veces = r.times;
    }

    //----------------------------------------------------------------------------------------
    bool listo[n];
    for(int i=1; i<n; i++){
        listo[i] = false;
    }

    int numListos = 0;
    list<int> iguales;
    list<int>::iterator it;
    Semaforo s2;

    if(fork()) {
        iguales.clear();
        while (numListos < n) {
            int menor = 1;
            for (int i = 1; i < n; i++) {
                if (ap.Etiquetas[i].etq < ap.Etiquetas[menor].etq && !listo[i]) {
                    menor = i;
                }
            }
            iguales.push_back(menor);
            for (int i = 1; i < n; i++) {
                if (ap.Etiquetas[i].etq == ap.Etiquetas[menor].etq) {
                    iguales.push_back(i);
                }
            }

            strncpy(area->Etiquetas[area->numEtq].etq, ap.Etiquetas[menor].etq, MSGSIZE);
            for (it = iguales.begin(); it != iguales.end(); it++) {
                //area->Etiquetas[area->numEtq].Veces = ap.Etiquetas[menor].Veces;
                area->Etiquetas[area->numEtq].Veces += ap.Etiquetas[*it].Veces;
                st = msgrcv(idB, &r, MSGSIZE, *it, IPC_NOWAIT);
                if (r.label != finalTag) {
                    strncpy(ap.Etiquetas[*it].etq, r.label, MSGSIZE);
                    ap.Etiquetas[*it].Veces = r.times;
                } else {
                    listo[menor] = true;
                    numListos++;
                }
            }
            area->numEtq++;
            iguales.clear();

            /*st = msgrcv(idB, &r, MSGSIZE, menor, IPC_NOWAIT);
            if(r.label != finalTag) {
                strncpy(ap.Etiquetas[menor].etq, r.label, MSGSIZE);
                ap.Etiquetas[menor].Veces = r.times;
            } else{
                listo[menor] = true;
                numListos++;
            }*/
        }
        s2.Signal();
        _exit(0);
    }
    //-------------------------------------------------------------------------------------
    else {
        s2.Wait();
        cout << "HIJO IMPRESOR " << area->numEtq << endl;
        for (int i = 1; i < area->numEtq; i++) {
            cout << area->Etiquetas[i].etq << " : " << area->Etiquetas[i].Veces << endl;
        }
        //_exit(0);
    }

    msgctl(idB, IPC_RMID, NULL);
    shmdt(area);
    shmctl(id, IPC_RMID, NULL);
    _exit(0);

    return 0;
}