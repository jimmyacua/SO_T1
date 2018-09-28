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
#include <map>
#include <iterator>

#include "Semaforo.h"
//#include "Buzon.h"

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
            while (cont <= f.totalEtq()) {
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

    //char * AP[n];  //cambiar a diccionario
    AC ap;
    struct my_msgbuf r;
    for(int i=1; i<n; i++){
        cout << "Proceso padre:" << endl;
        sem.Wait();
        st = msgrcv(idB, &r, MSGSIZE, i, IPC_NOWAIT);
        strncpy(ap.Etiquetas[i].etq,r.label, MSGSIZE);
        ap.Etiquetas[i].Veces = r.times;
        cout << "fin" << endl;
    }
    bool listo[n];
    for(int i=1; i<=n; i++){
        listo[i] = false;
    }
    int numListos = 0;
    while(numListos<n){
        int menor = 1;
        for(int i = 1; i<n; i++){
            if(ap.Etiquetas[i].etq < ap.Etiquetas[menor].etq && !listo[i]){
                menor = i;
            }
        }
        strncpy(area->Etiquetas[area->numEtq].etq, ap.Etiquetas[menor].etq, MSGSIZE);
        area->Etiquetas[area->numEtq].Veces = ap.Etiquetas[menor].Veces;
        area->numEtq++;

        st = msgrcv(idB, &r, MSGSIZE, menor, IPC_NOWAIT);
        if(r.label != finalTag) {
            strncpy(ap.Etiquetas[menor].etq, r.label, MSGSIZE);
            ap.Etiquetas[menor].Veces = r.times;
        } else{
            listo[menor] = true;
            numListos++;
        }
    }
    cout << area->numEtq << endl;
    for(int i = 1; i<area->numEtq;i++){
        cout << area->Etiquetas[i].etq << " : " << area->Etiquetas[i].Veces << endl;
    }

    /*
     * FALTA: ED para guardar en la mem compartida las etiquetas sin que se repitan
     */

    msgctl(idB, IPC_RMID, NULL);
    shmdt(area);
    shmctl(id, IPC_RMID, NULL);

    return 0;
}