#include "srv.h"
/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */


void servidor(int mi_cliente)
{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int rank;
    int cantidadDeServersOriginal;
    int cantidadDeServers;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeServers);
    cantidadDeServers = cantidadDeServersOriginal = cantidadDeServers/2;
    int maxSeqNum = 0;
    int seqNum = 0;
    int buf;
    int servEnEspera;                                   // Cuando mando mensajes, servEnEspera indica la cantidad de servers que todavia no respondieron
    char serverStatus[cantidadDeServers];               // ServerStatus guarda el estado de cada server, para saber cuales se fueron, cuales estan esperando un reply, etc
    char loEstoyEsperando[cantidadDeServers];           // Este arreglo guarda de cuáles servidores estoy esperando respuesta, porque si termina un servidor del cuál estoy esperando respuesta, no quiero quedarme esperando un mensaje que nunca va a llegar
    int i;
    for (i = 0; i < cantidadDeServers; i++) {
        serverStatus[i] = STAT_NORMAL;
        loEstoyEsperando[i] = 0;
    }


    while( ! listo_para_salir ) {
        MPI_Recv(&buf, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);   // Primero recibe un mensaje
        
        origen = status.MPI_SOURCE;                                             // Ahora debe analizar el origen y tag del mensaje para saber
        tag = status.MPI_TAG;                                                   // que clase de mensaje es
        
        if (tag == TAG_PEDIDO) {                                                // Caso en el que el cliente pide el mutex
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            debug("Dándole permiso");
            servEnEspera = cantidadDeServers - 1;                               // Estoy a punto de mandar los mensajes, ninguno de los n-1 servidores me respondio todavia
            seqNum = ++maxSeqNum;
            printf("cso: %d\n", cantidadDeServers);
            if (cantidadDeServers != 1) {                                       // -separo el caso en que soy el único servidor: no hay exclusión mutua
                for(i = 0; i < cantidadDeServersOriginal*2; i = i + 2){
                    if(i != rank && serverStatus[i/2] != STAT_SE_FUE){          // Excluye aquellos que ya se fueron
                        MPI_Send(&seqNum, 1, MPI_INT, i, TAG_ME_REQ, COMM_WORLD); // Envia un pedido de permiso para obtener un mutex
                        loEstoyEsperando[i/2]=1;
                    }
                }
            } else {
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
            debug("Di permiso");
        }
        
        else if (tag == TAG_LIBERO) {                                           // Caso en el que el cliente le indica que libere el mutex
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (i=0; i<cantidadDeServersOriginal; i++) {                       // Indica a los que estaban esperando este mutex que ya tienen el permiso de su cliente
                if (serverStatus[i] == STAT_WAITING) {
                    MPI_Send(&rank, 1, MPI_INT, i*2, TAG_ME_REP, COMM_WORLD);
                    serverStatus[i] = STAT_NORMAL;
                } 
            }
        }

        else if (tag == TAG_TERMINE) {                                          // Caso en el que el cliente le indica que terminó de ejecutarse
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;

            for (i=0; i<cantidadDeServersOriginal*2; i+=2) {                    // Indica a los servidores que siguen activos que el server está por terminar
                if (i != rank && serverStatus[i/2] != STAT_SE_FUE) {
                    MPI_Send(&rank, 1, MPI_INT, i, TAG_SRV_EXIT, COMM_WORLD);
                } 
            }
            return;
        }

        else if (tag == TAG_ME_REQ) {                                          // Caso en el que un servidor le pide permiso para acceder a un mutex           
            maxSeqNum = (maxSeqNum < buf)? buf : maxSeqNum;                    // actualizo maxSeqNum para mantener la coherencia del timestamp
            if (hay_pedido_local && (seqNum < buf || (seqNum == buf && rank < origen))) { // Si mi cliente pidió acceso exclusivo y tiene más prioridad que el sevidor que estoy procesando, dejo el servidor en espera
                serverStatus[origen/2] = STAT_WAITING;
            } else {                                                            
                MPI_Send(&rank, 1, MPI_INT, origen, TAG_ME_REP, COMM_WORLD);    // en caso contrario, le mando el OK al servidor para que acceda al mutex
            }
        }

        else if (tag == TAG_ME_REP) {                                           // Caso en el que un servidor le otorga permiso para acceder a un mutex  
            servEnEspera--;
            loEstoyEsperando[origen/2]=0;
            if (servEnEspera == 0) {
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD); // Si ya recibí respuesta de todos los servidores, le otorgo el acceso exclusivo a mi cliente
            }
        }

        else if (tag == TAG_SRV_EXIT) {                                         // Caso en el que el servidor le informa que está por terminar
            if (loEstoyEsperando[origen/2] == 1) servEnEspera--;                // Si estaba esperando su respuesta para acceder al mutex, considero que ya me respondió
            serverStatus[origen/2] = STAT_SE_FUE;
            cantidadDeServers--;
        }
        
    }
    
}

