/*
 * RT_v0.cc
 *
 *  Created on: Jan 20, 2021
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "ND_m.h"

#include "Set.h"

using namespace omnetpp;

enum{
    ND_ACK     = 2,
    ND_REQUEST = 3
};

class Node : public cSimpleModule {
    private:
    //

    Set RT0;
    int my_Index;


    //
    long numSent;
    long numReceived;
    cHistogram hopCountStats; // cLongHistogram is deprecated
    cOutVector hopCountVector;
    protected:
        virtual int getGateByDest(int dest);
        virtual Snail* generateMessage();
        virtual ND* generateND();


        virtual void forwardMessage(Snail* msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;

        virtual void refreshDisplay() const override;
        virtual void finish() override;
};


Define_Module(Node);

void Node::initialize(){

    // Initialize variables

    int initiator = par("initiator").intValue();
    my_Index = getIndex();
    numSent     = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    hopCountStats.setName("hopCountStats");
    //hopCountStats.setRangeAutoUpper(0, 10, 1.5);// cLongHistogram::setRangeAutoUpper is deprecated
    hopCountStats.setRange(0, 10);
    hopCountVector.setName("HopCount");


    // Start Constructing routing table
    /*
    for(int i=0;i<gateSize("gate$o");i++){
        // send ND_Request
        ND* msg = generateND();
        send(msg,"gate$o",i);
    }
    */
    // initialize routing table statically
    switch (my_Index) {
        case 0:
            RT0.Add(1);
            break;
        case 1:
            RT0.Add(0);
            RT0.Add(2);
            RT0.Add(4);
            break;
        case 2:
            RT0.Add(1);
            break;
        case 3:
            RT0.Add(4);
            break;
        case 4:
            RT0.Add(5);
            RT0.Add(3);
            RT0.Add(1);
            break;
        case 5:
            RT0.Add(4);
            break;
        default:
            break;
    }
    RT0.Print();

    // Start the protocole
    if(getIndex() == initiator ){
        Snail *msg = generateMessage();

        scheduleAt(0.1, msg);
    }



}
void Node::handleMessage(cMessage* msg){

    if(msg->getKind() == ND_REQUEST){
        ND *ndmsg = check_and_cast<ND*>(msg);
        //
        int sender_gate = ndmsg->getArrivalGate()->getIndex();

        ND* m = ndmsg->dup();
        m->setNeighbor(my_Index);
        m->setKind(ND_ACK);

        send(m,"gate$o",sender_gate);

    }else{

        if(msg->getKind() == ND_ACK){
            ND *ndmsg = check_and_cast<ND*>(msg);
            //
            int neighbor = ndmsg->getNeighbor();

            RT0.Add(neighbor);
            RT0.Print();

        }else{
            // I recieved a packet !
            Snail *ttmsg = check_and_cast<Snail*>(msg);

            if(ttmsg->getDestination() == getIndex()){
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


                int dest =  newmsg->getDestination();

                if(RT0.Find(dest) == false ){
                    forwardMessage(newmsg);
                }else{
                    send(newmsg, "gate$o",getGateByDest(dest));
                }

            }else{
                int dest =  ttmsg->getDestination();

                if(RT0.Find(dest) == false ){
                    forwardMessage(ttmsg);
                }else{
                    send(ttmsg, "gate$o",getGateByDest(dest));
                }

            }
        }
    }
}

// *********************************************************************
int Node::getGateByDest(int dest){
    for (int i = 0; i < gateSize("gate$o"); i++) {
        int neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        if(neighbor == dest)
            return i;
    }
    // otherwise
    return -1;
}
ND* Node::generateND(){
    int src = getIndex();

    ND *msg = new ND("ND Request");
    msg->setSrc(src);
    msg->setKind(ND_REQUEST);
    return msg;
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
    EV <<"Node : "<< my_Index <<" routing table\n";
    RT0.Print();
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
