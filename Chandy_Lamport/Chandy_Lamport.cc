/*
 * Chandy_Lamport.cc
 *
 *  Created on: Dec 23, 2020
 *      Author: lina
 */

/*
 * Breadth-First Spanning Tree
 * Built Without Centralized Control
 */

#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"


using namespace omnetpp;

int counter = 0,
    max_counter = 5;

enum {
    Token  = 1,
    Marker = 2,
    White  = 3,
    Red    = 4
};

class Node : public cSimpleModule {
    private:
        int my_Id,
            k, // K incoming channels
            color = White;

            cQueue *chan;
            bool  *closed;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
        virtual void turn_red();
        virtual void save_local_state();
        virtual void sendMessage();



};


Define_Module(Node);

void Node::initialize(){
    k = gateSize("gate$o");
    int initiator = par("initiator").intValue();
    my_Id = getIndex();

    // initialize chan
    chan   = new cQueue[k];


    // initialize closed channels
    closed = new bool[k];

    for (int i = 0; i < k; i++) {
        closed[i] = false;
    }

    if(my_Id == initiator){

        sendMessage();


        // send the marker and turn to Red
        Snail *mrq = new Snail("M");
        mrq->setKind(Marker);
        send(mrq,"gate$o",0);
        color = Red;


    }
}

void Node::handleMessage(cMessage* msg){

    Snail *smsg = check_and_cast<Snail*>(msg);

    int sender_gate = smsg->getArrivalGate()->getIndex();
    int message_kind = smsg->getKind();


    if(message_kind == Marker){
        if(color == White) turn_red();
        closed[sender_gate] = true;

    }else{
        // recieve program message from the incomming channel "sender_gate"

        if(color ==  Red && !closed[sender_gate]){
            // append the message
            EV << smsg->getName() <<"\n";

            chan[sender_gate].insert(smsg);
        }
        // toc
        if(counter < max_counter){
            sendMessage();
        }
  }

}

void Node::sendMessage(){
    char T[20];
    sprintf(T, "Token-%d", counter);
    Snail *tic = new Snail(T);

    tic->setKind(Token);
    tic->setId(counter);
    send(tic,"gate$o",0);
    counter++;

}





void Node::save_local_state(){
    // TODO:
}

void Node::turn_red(){
    EV << "turned Red\n";
    save_local_state();
    color = Red;

    // send marker to all neighbor
    for(int i=0;i<k;i++){
        Snail *marq = new Snail("Marker");
        marq->setKind(Marker);

        send(marq,"gate$o",i);
    }
}
