/*
 * diffuse_v1.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"

using namespace omnetpp;


class Node : public cSimpleModule {
    private:
        bool done = false;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Node);

void Node::initialize(){

    int initiator = par("initiator").intValue();
    EV << "initiator is : "<< initiator << " ! \n";

    // send(I) to N(x)
    if(getIndex() == initiator ){
        for (int i = 0; i < gateSize("gate$o"); i++) {
            Snail* msg = new Snail("MyMessage");
            send(msg,"gate$o",i);
        }
        done = true;
        cDisplayString& dd = this->getDisplayString();
        dd.parse("i=block/routing,gold");
    }
}
void Node::handleMessage(cMessage* msg){
    // Precess(I)
    Snail *smsg = check_and_cast<Snail*>(msg);


    if(!done){
        for (int i = 0; i < gateSize("gate$o"); i++) {
            Snail* msg = new Snail("MyMessage");
            send(msg,"gate$o",i);
        }
        done = true;
        cDisplayString& dd = this->getDisplayString();
        dd.parse("i=block/routing,gold");
    }else{

    }

}


