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
#include <map>
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

    Semaforo sem(0);
    Fichero f;

    //--------------Buzon--------------------
    int idB = msgget(KEY,0600|IPC_CREAT);
    if ( -1 == idB) {
        perror("Constuctor buzon: ");
        exit(2);
    }
    //----------------------------------------

    int st = 0;
    int n = argc; //numero de archivos
    for(int i = 1; i< n; i++){
        if(!fork()) { //proceso hijo
            string archivo = argv[i];
            f.leerArchivo(archivo);
            int cont = 1;
            while (cont < f.totalEtq()) { //envia los mensajes al buzon
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
    ap.numEtq = 0;
    struct my_msgbuf r;
    sem.Wait();
    for(int i=1; i< n; i++){ //recibe un mensaje de cada hijo
        st = msgrcv(idB, &r, MSGSIZE, i, IPC_NOWAIT);
        strncpy(ap.Etiquetas[i].etq,r.label, MSGSIZE);
        ap.Etiquetas[i].Veces = r.times;
        ap.numEtq++;
    }

    //----------------------------------------------------------------------------------------
    bool listo[n];
    for(int i=1; i<n; i++){
        listo[i] = false;
    }

    int numListos = 1;

    Semaforo s2(0); //semaforo para imprimir

    char primerLetra;

    if (fork()) {
        while (numListos < n) {
            bool pasar = false;

            //while (!pasar) {
                int menor = 1;
                for (int i = 1; i < n; i++) { //busca la menor etiqueta
                    if (ap.Etiquetas[menor].etq > ap.Etiquetas[i].etq && ap.Etiquetas[i].etq != finalTag) {
                        menor = i;
                    }
                }

                if (ap.Etiquetas[menor].etq[0] == '/') { //ej: /hola, se guarda la h como primer letra
                    primerLetra = ap.Etiquetas[menor].etq[1];
                } else {
                    primerLetra = ap.Etiquetas[menor].etq[0];
                }

                for (int i = 1; i < n; i++) { //cuenta las apariciones de la etiq menor
                    if (menor != i) {
                        if (strcasecmp(ap.Etiquetas[i].etq, ap.Etiquetas[menor].etq) == 0) {
                            ap.Etiquetas[menor].Veces += ap.Etiquetas[i].Veces;
                        }
                    }
                }
                strncpy(area->Etiquetas[area->numEtq].etq, ap.Etiquetas[menor].etq, MSGSIZE);
                area->Etiquetas[area->numEtq].Veces += ap.Etiquetas[menor].Veces;
                area->numEtq++;
                //s2.Signal();
                for (int i = 1; i < n; i++) {//pide etiquetas nuevas tipo i
                    if (strcasecmp(ap.Etiquetas[i].etq, ap.Etiquetas[menor].etq) == 0 && menor != i) {
                        st = msgrcv(idB, &r, MSGSIZE, i, IPC_NOWAIT);
                        if (r.label != finalTag) {
                            strncpy(ap.Etiquetas[i].etq, r.label, MSGSIZE);
                            ap.Etiquetas[i].Veces = r.times;
                        } else {
                            listo[i] = true;
                            numListos++;
                        }
                    }
                }
                //pide una etiqueta nueva de tipo menor
                st = msgrcv(idB, &r, MSGSIZE, menor, IPC_NOWAIT);
                if (r.label != finalTag) {
                    strncpy(ap.Etiquetas[menor].etq, r.label, MSGSIZE);
                    ap.Etiquetas[menor].Veces = r.times;
                } else {
                    listo[menor] = true;
                    numListos++;
                }
                bool igual = false;
                int i = 1;
                while(i < n && !igual){
                    char prim = ap.Etiquetas[i].etq[0];
                    if(prim == '/'){
                        prim = ap.Etiquetas[i].etq[1];
                    }
                    if(prim == primerLetra){
                        igual = true;
                    } else { ;
                        i++;
                    }
                }
                if(!igual){
                    pasar = true;
                    s2.Signal();
                }
           // }
        }

        //_exit(0);
    }
    else {
        s2.Wait();
        cout << "HIJO IMPRESOR " << endl;
        for (int i = 0; i < area->numEtq; i++) {
            cout << area->Etiquetas[i].etq << " : " << area->Etiquetas[i].Veces << endl;
        }
        //exit(0);
    }

    msgctl(idB, IPC_RMID, NULL);
    shmdt(area);
    shmctl(id, IPC_RMID, NULL);
    //_exit(0);

    return 0;
}