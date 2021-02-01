/*
 * ST_DFT.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;

enum {
    Token    = 0,
    Return   = 1,
    Backedge = 2
};

class Node : public cSimpleModule {
    private:
        int my_Id,
            entry;

        bool have_entry   = false,
             is_Initiator = false,
             visited      = false,
             done         = false;

        Set Unvisited;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
        virtual void visit();
};


Define_Module(Node);

void Node::initialize(){

    int initiator = par("initiator").intValue();
    my_Id = getIndex();

    // Unvisited == N(x)
    for (int i = 0; i < gateSize("gate$o"); i++) Unvisited.Add(i);
    Unvisited.Print();

    if(my_Id == initiator){
        is_Initiator = true;
        cMessage *msg = new cMessage("Message");
        scheduleAt(simTime() + 1, msg);   // a self message !
    }
}


void Node::handleMessage(cMessage* msg){
    int sender;
    cDisplayString connDisplay;

    if(msg->isSelfMessage()){   // I am the initiator
        visit();
    } else {
        sender = msg->getArrivalGate()->getIndex();
        if(!visited){ // I am idle

            if(have_entry == false){
                entry = sender;
                have_entry = true;
            }

            cDisplayString connDisplay = gate("gate$o",sender)->getDisplayString();
            connDisplay.setTagArg("ls",0,"blue");
            connDisplay.setTagArg("ls",1,"3");
            gate("gate$o",sender)->setDisplayString(connDisplay.str());


            Unvisited.Remove(sender);
            visit();

        }else{ // I am visited
            switch (msg->getKind()) {
                Snail *smsg;
                case Token:
                    Unvisited.Remove(sender);
                    smsg = new Snail("B");
                    smsg->setKind(Backedge);
                    send(smsg,"gate$o",sender);
                    break;
                case Return :
                    connDisplay = gate("gate$o",sender)->getDisplayString();
                    connDisplay.setTagArg("ls",0,"blue");
                    connDisplay.setTagArg("ls",1,"3");
                    gate("gate$o",sender)->setDisplayString(connDisplay.str());
                    visit();
                    break;
                case Backedge :
                    visit();
                    break;

                default:
                    bubble("wait a minute !");
                    break;
            }
        }

    }
}


void Node::visit(){
    int next;
    if(Unvisited.Empty() == false){

        next = Unvisited.Pick();
        Snail *smsg = new Snail("T");
        smsg->setKind(Token);
        send(smsg,"gate$o",next);
        visited = true;
    }else{
        if(! is_Initiator){
            Snail *smsg = new Snail("R");
            smsg->setKind(Return);
            send(smsg,"gate$o",entry);
        }else{
            bubble("Done !");
        }
        done = !done;
    }
}


