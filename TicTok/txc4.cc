/*
 * txc4.cc
 *
 *  Created on: Nov 6, 2020
 *      Author: lina
 */

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Txc4 : public cSimpleModule {
    private:
        // adding state variable
        int counter;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Txc4);

void Txc4::initialize(){
    counter = par("limit");


    WATCH(counter);

    if (par("sendMsgOnInit").boolValue() == true) {
        cMessage *msg = new cMessage("tictokMSG");
        // adding logging
        EV << "Sending initial Message\n";
        send(msg, "out");
    }
}

void Txc4::handleMessage(cMessage* msg){
    counter -=1;
    if(counter !=0){
        EV << "Received message  > "<< msg->getName() << "< ,sending it agin ...\n";
        send(msg,"out");
    }else{
        EV << "Done ...\n";
    }
}


