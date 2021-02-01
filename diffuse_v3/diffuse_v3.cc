/*
 * diffuse_v3.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"

#include "Set.h"

using namespace omnetpp;

int num_edges = 0;

class Node : public cSimpleModule {
    private:
        bool done = false;
        Set Nx;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Node);

void Node::initialize(){


    int myIndex = getIndex();
    int initiator = par("initiator").intValue();


    // initialize N(x)
    for (int i = 0; i < gateSize("gate$o"); i++) {
        int neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        Nx.Add(neighbor);
    }
    num_edges += Nx.Size();

    // send(I, N(x)) to N(x)
    if(myIndex == initiator ){
        for (int i = 0; i < gateSize("gate$o"); i++) {
            Snail* msg = new Snail("M");

            // noticed probleme :
            // As I am the sender I should send my Id too to my neighbors
            msg->setZ(Nx + myIndex);
            send(msg,"gate$o",i);
        }
    // become done
    done = true;
    }

    printf("*%d  \n",num_edges);
}

void Node::handleMessage(cMessage* msg){
    int Index = getIndex();
    //EV <<"I am "<< Index <<"\n";

    // Process(I) and get Z(x)
    Snail *smsg = check_and_cast<Snail*>(msg);
    Set Zx = smsg->getZ();

    // Calculate Y = N(x)\Z(x)
    Set Y = Nx - Zx;

    // Calculate the new Zx
    Zx = Y.Union(Zx) + Index;

    if(!done && !Y.Empty()){
        // become done
        done = true;

        int neighbor;
        for (int i = 0; i < gateSize("gate$o"); i++) {
            // send the new Zx to Y
            // get the neighbor
            neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();

            if(Y.Find(neighbor)){
                Snail* msg = new Snail("M");
                msg->setSource(Index);
                msg->setZ(Zx);
                send(msg,"gate$o",i);
            }
        }
    }

    delete(msg);
}
/*
 * Since M[Bcast/Ri+] >= m and M[diff_v3] = m
 * so we reache the minimun
 * so diff_v3 is optimal in term of messages
 */
