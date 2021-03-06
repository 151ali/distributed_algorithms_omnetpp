/*
 * DFT_traversal_v1.cc
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
    Backedge = 2,
    Visited  = 3,
    Ack      = 4
};

class Node : public cSimpleModule {
    private:
        int my_Id,
            entry,
            gates_num,
            ack_num = 0;

        bool have_entry       = false,
             is_Initiator     = false,
             allowed_to_forward = false,
             visited          = false,
             available        = false,
             done             = false;

        Set Unvisited;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
        virtual void visit();

};


Define_Module(Node);

void Node::initialize(){
    gates_num = gateSize("gate$o");
    int initiator = par("initiator").intValue();
    my_Id = getIndex();

    // Unvisited == N(x)
    for (int i = 0; i < gates_num; i++) Unvisited.Add(i);
    Unvisited.Print();

    if(my_Id == initiator){
        is_Initiator = true;
        cMessage *msg = new cMessage("Message");
        scheduleAt(simTime() + 1, msg);   // a self message !
    }
}


void Node::handleMessage(cMessage* msg){

    int sender;
    Snail *smsg;

    if(msg->isSelfMessage()){   // I am the initiator

        for(int i = 0; i < gates_num; i++){
            smsg = new Snail("V");
            smsg->setKind(Visited);
            send(smsg,"gate$o",i);
        }
        //visit();

    }else{
        sender = msg->getArrivalGate()->getIndex();

        switch (msg->getKind()) {
            case Token:
                Unvisited.Remove(sender);
                if(have_entry == false){
                    entry = sender;
                    have_entry = true;
                }

                if(!is_Initiator)
                    for(int i = 0; i < gates_num; i++){
                        smsg = new Snail("V");
                        smsg->setKind(Visited);
                        send(smsg,"gate$o",i);
                    }
                else visit();

                break;

            case Visited:
                if(!visited && !available){
                    available = true;
                    bubble("I am available!");
                }
                Unvisited.Remove(sender);
                //Unvisited.Print();
                smsg = new Snail("Ack");
                smsg->setKind(Ack);
                send(smsg,"gate$o",sender);
                break;

            case Return:
                visit();
                break;

            case Ack:
                if(!visited){
                    ack_num++;
                    if(ack_num  == gates_num){
                        allowed_to_forward = true;
                        visit();
                    }
                }
                break;

            default:
                bubble("wait a minute !");
                break;

        }

    }
}


void Node::visit(){
    Unvisited.Print();
    int next;
    if(Unvisited.Empty() == false ){ // I have neighbor to talk to
        if(allowed_to_forward || ( this->is_Initiator && !this->visited)){

            next = Unvisited.Pick();
            Snail *smsg = new Snail("T");
            smsg->setKind(Token);
            send(smsg,"gate$o",next);

            ack_num = 0;
            //allowed_to_forward = false;
            visited = true;
            available = false;
        }else{
            // initiator && visited
            next = Unvisited.Pick();
            Snail *smsg = new Snail("T");
            smsg->setKind(Token);
            send(smsg,"gate$o",next);
        }

    }else{ // Unvisited.Empty() == true
        if(!is_Initiator){
            Snail *smsg = new Snail("R");
            smsg->setKind(Return);
            send(smsg,"gate$o",entry);
        }else{
            bubble("Done !");
        }
        done = true;
    }

}


