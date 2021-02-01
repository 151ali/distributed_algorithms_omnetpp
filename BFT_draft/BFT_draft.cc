/*
 * BFT_draft.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */

/*
 * Breadth-First Spanning Tree
 * Built Without Centralized Control
 */

#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;

enum {
    Token = 0,
    Yes   = 1,
    No    = 2
};

class Node : public cSimpleModule {
    private:
        int my_Id,d,
            gates_num;
        int level;
        int expected_msg;
        int parent_Id;

        bool have_parent = false;
        Set Nx,
            children;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;



};


Define_Module(Node);

void Node::initialize(){
    gates_num = gateSize("gate$o");
    int initiator = par("initiator").intValue();
    my_Id = getIndex();

    // initialize N(x)
    for (int i = 0; i < gateSize("gate$o"); i++) {
        int neighbor = i;//gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        Nx.Add(neighbor);
    }

    Nx.Print();

    if(my_Id == initiator){

        Snail *msg = new Snail("Message");
        msg->setKind(Token);
        msg->setDistance(-1);
        scheduleAt(simTime() + 1, msg);   // a self message !

    }
}

void Node::handleMessage(cMessage* msg){
    int sender_gate;
    Snail *smsg = check_and_cast<Snail*>(msg);
    d = smsg->getDistance();
    int message_kind = smsg->getKind();
    Snail *yes,*no,*tok;

    if(msg->isSelfMessage()){

        parent_Id = my_Id;
        have_parent = true;
        level = 0;

        if(Nx.Size() != 0){
            for(int i=0;i<gates_num;i++){
                Snail *m = new Snail("Token");
                m->setKind(Token);
                m->setDistance(d + 1);
                send(m,"gate$o",i);
            }
        }else bubble("idle mutation :{ ");


    }else {
        /// ***********
        sender_gate  = smsg->getArrivalGate()->getIndex();

        if(message_kind == Token){

            if(have_parent == false){

                parent_Id = sender_gate;
                children.Clear();
                level = d + 1;
                have_parent = true;


                // displat stuff ...
                cDisplayString connDisplay = gate("gate$o",parent_Id)->getDisplayString();
                connDisplay.setTagArg("ls",0,"blue");
                connDisplay.setTagArg("ls",1,"3");
                gate("gate$o",parent_Id)->setDisplayString(connDisplay.str());


                //  expected_msg <- | N(x) - {sender_gate }|;
                Nx.Remove(sender_gate);
                expected_msg = Nx.Size();
                Nx.Add(sender_gate);
                // EV << "expected_msg "<<expected_msg <<"\n";

                if(expected_msg == 0){
                    // send(Yes, d + 1) to parent
                    yes = new Snail("Yes");
                    yes->setKind(Yes);
                    yes->setDistance(d + 1);
                    send(yes,"gate$o",parent_Id);
                }else{
                    // send(T, d + 1) to Nx-sender_gate
                    for(int i=0;i<gates_num;i++){
                        if(i == sender_gate) continue;
                        tok = new Snail("Token");
                        tok->setKind(Token);
                        tok->setDistance(d + 1);
                        send(tok,"gate$o",i);
                    }
                }

            }else{ //I  have parent
                if(level > d + 1){
                    parent_Id   = sender_gate;
                    children.Clear();
                    level = d +1;
                    have_parent = true;


                    // displat stuff ...
                    cDisplayString connDisplay = gate("gate$o",parent_Id)->getDisplayString();
                    connDisplay.setTagArg("ls",0,"blue");
                    connDisplay.setTagArg("ls",1,"3");
                    gate("gate$o",parent_Id)->setDisplayString(connDisplay.str());


                    Nx.Remove(sender_gate);
                    expected_msg = Nx.Size();
                    Nx.Add(sender_gate);

                    if(expected_msg == 0){
                        // Send(yes, level) to Parent
                        yes = new Snail("Yes");
                        yes->setKind(Yes);
                        yes->setDistance(level);
                        send(yes,"gate$o",parent_Id);

                    }else{
                        // send(T, d + 1) to Nx-sender_gate
                        for(int i=0;i<gates_num;i++){
                            if(i == sender_gate) continue;
                            yes = new Snail("Token");
                            yes->setKind(Token);
                            yes->setDistance(d + 1);
                            send(yes,"gate$o",i);
                        }
                    }
                }else{
                    // send (No, d + 1) to sender_gate
                    no = new Snail("No");
                    no->setKind(No);
                    no->setDistance(d + 1);
                    send(no,"gate$o",sender_gate);
                }
            }
        }else{
            // response = either Yes or No
            if(d == level +1){
                if(message_kind == Yes){
                    children.Add(sender_gate);

                    // displat stuff ...
                    cDisplayString connDisplay = gate("gate$o",sender_gate)->getDisplayString();
                    connDisplay.setTagArg("ls",0,"blue");
                    connDisplay.setTagArg("ls",1,"3");
                    gate("gate$o",sender_gate)->setDisplayString(connDisplay.str());
                }
                expected_msg--;

                if(expected_msg == 0){
                    if(parent_Id != my_Id){

                        // send (yes, level) to parent
                        yes = new Snail("Yes");
                        yes->setKind(Yes);
                        yes->setDistance(level);
                        send(yes,"gate$o",parent_Id);

                    }else{// I am the initiator
                        // the node learns that bft tree is built
                        EV << "I am : "<<my_Id <<"\n";
                        bubble("Done !");
                    }
                }
            }
        }
    }

//    delete(smsg);
//    delete(yes);
//    delete(no);
//    delete(tok);


}
