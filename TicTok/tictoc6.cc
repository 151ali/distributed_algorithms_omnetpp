/*
 * tictoc6.cc
 *
 *  Created on: Nov 6, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Txc6 : public cSimpleModule {
    private:
        cMessage* event;         // used for timing
        cMessage* tictocMessage; // remember the message until it will be send back

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;

    public:
        Txc6();
        ~Txc6();
};


Define_Module(Txc6);

Txc6::Txc6(){
    event = tictocMessage = nullptr;
}

Txc6::~Txc6(){
    cancelAndDelete(event);
    delete tictocMessage;
}




void Txc6::initialize(){

    // create event object used for timing
    event = new cMessage("event");
    tictocMessage = nullptr;

    if (strcasecmp("tic", getName()) == 0) {
        EV << "Waiting 5.0s to start ... \n";
        tictocMessage = new cMessage("tictocMSG");
        // wait 5 sec to send message
        scheduleAt(5.0, event);
    }
}

void Txc6::handleMessage(cMessage* msg){

    // Random numbers and parameters ..
    simtime_t delay = par("delayTime");

    if(msg == event){
        EV << "Processing before sending ...\n";
        send(tictocMessage,"out");
        tictocMessage = nullptr;

    }else{

        if(uniform(0,1) < 0.1){
            // lose message
            EV<< "Message Lost :'( \n";
            bubble("message lost");  // making animation more informative...
            delete msg;
        }else{

        EV << "Messege Arrived , waiting for "<< delay <<"  sec ...\n";
        tictocMessage = msg;
        scheduleAt(simTime(), event);
        EV << "cbn\n";
        }
    }
}
/*
void Txc6::forwardMessage(cMessage *msg){

    int n = gateSize("*"); // replace * withgate name,
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on port out[" << k << "]\n";
    send(msg, "out", k);
}
*/

