/*
 * RR_v1.cc
 *
 *  Created on: Yennayer 1, 2971
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"

using namespace omnetpp;

class Node : public cSimpleModule {
    private:
    //
    int my_Index;
    long numSent;
    long numReceived;
    cHistogram hopCountStats; // cLongHistogram is deprecated
    cOutVector hopCountVector;
    protected:
        virtual Snail* generateMessage();
        virtual void forwardMessage(Snail* msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;

        virtual void refreshDisplay() const override;
        virtual void finish() override;
};


Define_Module(Node);

void Node::initialize(){

    // Initialize variables
    my_Index = getIndex();
    int initiator = par("initiator").intValue();
    numSent     = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    hopCountStats.setName("hopCountStats");
    //hopCountStats.setRangeAutoUpper(0, 10, 1.5);// cLongHistogram::setRangeAutoUpper is deprecated
    hopCountStats.setRange(0, 10);
    hopCountVector.setName("HopCount");


    if(getIndex() == initiator ){
        Snail *msg = generateMessage();
        scheduleAt(0.1, msg);
    }
}
void Node::handleMessage(cMessage* msg){
    Snail *ttmsg = check_and_cast<Snail*>(msg);

    if(ttmsg -> getDestination() == getIndex()){
        int hopcount = ttmsg->getHopCount();


        EV << "Message" << ttmsg << " arrived after "<< ttmsg->getHopCount() <<"hopes. \n";
        bubble("ARRIVED , Starting new one !");


        // update statistics.
        numReceived++;
        hopCountVector.record(hopcount);
        hopCountStats.collect(hopcount);

        delete ttmsg;

        EV << "generating another message: ";
        Snail *newmsg = generateMessage();
        EV << newmsg <<endl;

        numSent++;
        forwardMessage(newmsg);
    }else{
        forwardMessage(ttmsg);
    }

}

Snail* Node::generateMessage(){
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);


    if(dest >= src)
        dest++;


    char msgname[20];
    sprintf(msgname, "from  %d  to  %d",src,dest);


    Snail *msg = new Snail(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;

}
void Node::forwardMessage(Snail* msg){
    msg->setHopCount(msg->getHopCount()+1);

    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "forwarding messae"<< msg << "on gate["<<k<<"]\n";
    send(msg, "gate$o",k);

}
void Node::refreshDisplay() const{
        char buf[40];
        sprintf(buf, "rcvd: %ld sent: %ld", numReceived, numSent);

        getDisplayString().setTagArg("t", 0, buf);
}

void Node::finish(){

    EV << "===============  === Report ===  ===============\n";
    EV <<"Node : "<< my_Index <<"\n";

    // This function is called by OMNeT++ at the end of the simulation.
    EV << "Sent:              " << numSent << endl;
    EV << "Received:          " << numReceived << endl;
    EV << "Hop count, min:    " << hopCountStats.getMin() << endl;
    EV << "Hop count, max:    " << hopCountStats.getMax() << endl;
    EV << "Hop count, mean:   " << hopCountStats.getMean() << endl;
    EV << "Hop count, stddev: " << hopCountStats.getStddev() << endl;
    EV << "===============  === ====== ===  ===============\n";
    recordScalar("#sent", numSent);
    recordScalar("#received", numReceived);

    hopCountStats.recordAs("hop count");
}
