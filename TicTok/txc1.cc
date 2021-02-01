/*
 * txc1.cc
 *
 *  Created on: Nov 5, 2020
 *      Author: lina
 */

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule {
    private:
        // adding state variable
        int counter;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Txc1);

void Txc1::initialize(){
    counter = 10;
    WATCH(counter);

    if(strcmp("tic", getName()) == 0 ){
        cMessage *msg = new cMessage("tictokMSG");
        // adding logging
        EV << "Sending initial Message\n";
        send(msg, "out");
    }
}

void Txc1::handleMessage(cMessage* msg){
    counter -=1;
    if(counter !=0){
        EV << "Received message  > "<< msg->getName() << "< ,sending it agin ...\n";
        send(msg,"out");
    }else{
        EV << "Done ...\n";
    }
}
