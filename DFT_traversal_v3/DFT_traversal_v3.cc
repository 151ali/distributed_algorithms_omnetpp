/*
 * DFT_traversal_v3.cc
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
    Visited  = 3
};

class Node : public cSimpleModule {
    private:
        int my_Id,
            entry,
            gates_num,

            next,
            sender;

        bool have_entry       = false,
             is_Initiator     = false,
             visited          = false,
             available        = false,
             done             = false;

        Set Unvisited;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
        virtual void visit();
        virtual void first_visit();

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

    int message_kind = msg->getKind();

    if(msg->isSelfMessage()){ // I am the initiator

        next = Unvisited.Pick();
        // send(T) to next
        Snail *smsg = new Snail("T");
        smsg->setKind(Token);
        send(smsg,"gate$o",next);

        //send(Visited) to N(x) - {next}
        for(int i=0;i<gates_num;i++){
            if(i == next) continue;
            Snail *smsg = new Snail("V");
            smsg->setKind(Visited);
            send(smsg,"gate$o",i);
        }
        visited = true;

        // cDisplayString& di = this->getDisplayString();
        // di.parse("i=block/routing,gold");


    }else{

        sender = msg->getArrivalGate()->getIndex();

        if(available){ // I am available

            switch (message_kind) {
                case Token:

                    first_visit();
                    break;
                case Visited:
                    Unvisited.Remove(sender);
                    break;
                default:
                    break;
            }
        }else{
            if(visited){ // I am visited ...

                switch (message_kind) {
                    case Return:
                        visit();
                        break;
                    case Token:
                        Unvisited.Remove(sender);
                        if(next == sender) visit();
                        break;
                    case Visited:
                        Unvisited.Remove(sender);
                        if(next == sender) visit();
                        break;
                    default:
                        break;
                }

            }else{      // I am idle

                switch (message_kind) {
                    case Token:
                        first_visit();
                        break;
                    case Visited:
                        Unvisited.Remove(sender);
                        available = true;
                        break;
                    default:
                        break;
                }
            }
        }

    }
}

void Node::first_visit(){
    entry = sender;

    Unvisited.Remove(sender);

    if(Unvisited.Empty() == false){
        next = Unvisited.Pick();

        // send(T) to next
        Snail *smsg = new Snail("T");
        smsg->setKind(Token);
        send(smsg,"gate$o",next);

        //send(Visited) to N(x)-{entry, next}
        for(int i=0;i<gates_num;i++){
            if(i == entry || i == next) continue;
            Snail *smsg = new Snail("V");
            smsg->setKind(Visited);
            send(smsg,"gate$o",i);
        }
        visited = true;

    }else{
        // send(R) to entry
        Snail *smsg = new Snail("R");
        smsg->setKind(Return);
        send(smsg,"gate$o",entry);
        //send(Visited) to N(x)-{entry}
        for(int i=0;i<gates_num;i++){
            if(i == entry) continue;
            Snail *smsg = new Snail("V");
            smsg->setKind(Visited);
            send(smsg,"gate$o",i);
        }
        done = true;
    }

}

void Node::visit(){
    if(Unvisited.Empty() == false){
        next = Unvisited.Pick();
        // send(T) to next
        Snail *smsg = new Snail("T");
        smsg->setKind(Token);
        send(smsg,"gate$o",next);

    }else{
        // send(R) to entry
        Snail *smsg = new Snail("R");
        smsg->setKind(Return);
        send(smsg,"gate$o",entry);
    }
    done = true;
}
