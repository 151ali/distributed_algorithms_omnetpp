/*
 * ST_v2.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */

// TODO : !!!!


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;

enum {
    Question = 0,
    Yes      = 1,
    No       = 2
};

class Node : public cSimpleModule {
    private:
        int my_Id,
            parent_Id,
            gates_num,
            counter;

        bool root   = false,
             done   = false,
             active = false;

        Set Nx;         // my neighbors

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
        int neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        Nx.Add(neighbor);
    }

    Nx.Print();

    if(my_Id == initiator){

        root = true;

        // send Q to Nx
        for(int i=0;i<gates_num;i++){
            Snail *msg = new Snail("Q");
            msg->setZ(Nx);
            msg->setKind(Question);
            send(msg,"gate$o",i);
        }
        counter = 0;
        active = true;
    }
}


void Node::handleMessage(cMessage* msg){

    int message_king = msg->getKind();
    int sender_gate = msg->getArrivalGate()->getIndex();
    int sender_id   = gate("gate$o",sender_gate)->getPathEndGate()->getOwnerModule()->getIndex();


    if(active){

        if(message_king == Question){

            bubble("what !");
            // send no to sender
            //Snail *smsg = new Snail("No");
            //smsg->setKind(No);
            //send(smsg,"gate$o",sender_gate);


        }

        if(message_king == Yes){
            // make connection with my sun blue
            // displat stuff ...

            cDisplayString connDisplay = gate("gate$o",parent_Id)->getDisplayString();
            connDisplay.setTagArg("ls",0,"blue");
            connDisplay.setTagArg("ls",1,"3");
            gate("gate$o",sender_gate)->setDisplayString(connDisplay.str());

            counter +=1;
            if(counter == Nx.Size()) done = true;
        }

        //if(message_king == No){
        //    counter +=1;
        //    if(counter == Nx.Size()) done = true;
        //}







    }else{  // I am idle & I received Question


        parent_Id = sender_gate;

        // displat stuff ...
        cDisplayString connDisplay = gate("gate$o",parent_Id)->getDisplayString();
        connDisplay.setTagArg("ls",0,"blue");
        connDisplay.setTagArg("ls",1,"3");
        gate("gate$o",sender_gate)->setDisplayString(connDisplay.str());


        Snail *smsg = new Snail("Yes");
        smsg->setKind(Yes);
        send(smsg,"gate$o",sender_gate);

        counter = 1;

        if(counter == Nx.Size()) done = true;


        else{
            Snail *smsg = check_and_cast<Snail*>(msg);
            Set Zx = smsg->getZ();

            // Y = [ N(x)-Z(X) ] - sender
            Set Y = Nx - Zx ;
            Y.Remove(sender_id);
            // new Zx
            Zx = Y.Union(Zx) + sender_id;

            if(Y.Empty() == false){
                int neighbor;
                    for (int i = 0; i < gateSize("gate$o"); i++) {
                        // send the new Zx to Y
                        // get the neighbor
                        neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();

                        if(Y.Find(neighbor)){
                            Snail* smsg = new Snail("Q ... ");
                            smsg->setKind(Question);
                            smsg->setZ(Zx);
                            send(smsg,"gate$o",i);
                        }
                    }
            }
            active = true;
        }

    }
}
