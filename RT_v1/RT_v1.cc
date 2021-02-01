/*
 * RT_v1.cc
 *
 *  Created on: Jan 30, 2021
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "ND_m.h"
#include "NDt_m.h"

#include "Set.h"

using namespace omnetpp;

enum{
    ND_ACK     = 2,
    ND_REQUEST = 3,
    ND2        = 4
};

class Node : public cSimpleModule {
    private:
    //


    Set RT0;
    Set RT1;
    Map RT;



    int my_Index;
    int nd_ack_num;

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
    my_Index = getIndex();
    nd_ack_num = gateSize("gate$o");
    int initiator = par("initiator").intValue();

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


    // initialize routing table-- I hard coded this part--
    Set r0;r0.Add(4);r0.Add(2); // depuis 1
    Set r1;r1.Add(5);r1.Add(3); // depuis 4
    Set r2;r2.Add(4);r2.Add(0); // depuis 1
    Set r3;r3.Add(5);r3.Add(1); // depuis 4
    Set r4;r4.Add(0);r4.Add(2); // depuis 1
    Set r5;r5.Add(3);r5.Add(1); // depuis 4

    switch (my_Index) {
        case 0:
            RT0.Add(1);
            RT1.Add(4);
            RT1.Add(2);
            RT.Add(1, r0);
            break;
        case 1:
            RT0.Add(0);
            RT0.Add(2);
            RT0.Add(4);
            RT1.Add(5);
            RT1.Add(3);
            RT.Add(4, r1);
            break;
        case 2:
            RT0.Add(1);
            RT1.Add(4);
            RT1.Add(0);
            RT.Add(1, r2);
            break;
        case 3:
            RT0.Add(4);
            RT1.Add(5);
            RT1.Add(1);
            RT.Add(4, r3);
            break;
        case 4:
            RT0.Add(5);
            RT0.Add(3);
            RT0.Add(1);
            RT1.Add(2);
            RT1.Add(0);
            RT.Add(1, r4);
            break;
        case 5:
            RT0.Add(4);
            RT1.Add(3);
            RT1.Add(1);
            RT.Add(4, r5);
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
    int message_kind = msg->getKind();


    if(message_kind == ND_REQUEST){
        ND *ndmsg = check_and_cast<ND*>(msg);
        //
        int sender_gate = ndmsg->getArrivalGate()->getIndex();

        ND* m = ndmsg->dup();
        m->setNeighbor(my_Index);
        m->setKind(ND_ACK);
        send(m,"gate$o",sender_gate);

    }else{

        if(message_kind == ND_ACK){
            nd_ack_num--;

            ND *ndmsg = check_and_cast<ND*>(msg);
            //
            int neighbor = ndmsg->getNeighbor();

            RT0.Add(neighbor);
            //RT0.Print();
            if(nd_ack_num == 0){
                for(int i=0;i<gateSize("gate$o");i++){
                    //
                    NDt* m = new NDt("myRT");
                    m->setKind(ND2);
                    m->setSrc(my_Index);
                    m->setRT(RT0);

                    send(m,"gate$o",i);
                }
            }

        }else{
            if(message_kind == ND2){
                // configure RT1
                NDt *ndtmsg = check_and_cast<NDt*>(msg);
                Set Z = ndtmsg->getRT();
                Z.Remove(my_Index);
                RT1 = RT1.Union(Z);
                RT.Add(ndtmsg->getSrc(), Z);


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
                        if(RT1.Find(dest)==false){
                            forwardMessage(newmsg);
                        }else{
                            send(newmsg, "gate$o",getGateByDest(RT.Find(dest)));
                        }

                    }else{
                        send(newmsg, "gate$o",getGateByDest(dest));
                    }

                }else{
                    int dest =  ttmsg->getDestination();

                    if(RT0.Find(dest) == false ){
                        if(RT1.Find(dest)==false){
                            forwardMessage(ttmsg);
                        }else{
                            EV <<" =  "<< RT.Find(dest)<<"\n";
                            send(ttmsg, "gate$o",getGateByDest(RT.Find(dest)));  ///////////////////////
                        }

                    }else{
                        send(ttmsg, "gate$o",getGateByDest(dest));
                    }
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
    RT1.Print();
    EV << "maaaaaaaaaaaa3\n";
    RT.Print();
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
