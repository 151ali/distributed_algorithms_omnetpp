/*
 * Wflood.cc
 *
 *  Created on: Nov 14, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"

using namespace omnetpp;

class Node : public cSimpleModule {
    private:
        bool AWAKE = false;
        bool ASLEEP = true;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Node);

void Node::initialize(){


    // generate random satus 0 -> asleep , 1-> become awake
    int awake = intuniform(0, 1);
    if(awake == 1){
        AWAKE  = true;
        ASLEEP = false;
    }

    // send(I) to N(x)
    if(AWAKE){
        for (int i = 0; i < gateSize("gate$o"); i++) {
            Snail* msg = new Snail("MyMessage");
            send(msg,"gate$o",i);
        }
    }
}
void Node::handleMessage(cMessage* msg){
    int sender = msg->getArrivalGate()->getIndex();

    if(ASLEEP){
        ASLEEP = false;
        AWAKE  = true;

        Snail *smsg = check_and_cast<Snail*>(msg);
        for (int i = 0; i < gateSize("gate$o"); i++) {
            if(i == sender) continue;
            Snail* msg = new Snail("MyMessage");
            send(msg,"gate$o",i);
        }
    }

}


