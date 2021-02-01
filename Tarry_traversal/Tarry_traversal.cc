/*
 * Tarry_traversal.cc
 *
 *  Created on: Nov 9, 2020
 *      Author: lina
 */

/*
 * =====================================================
 * la complexite de terry's traversal est 2m
 * du fait que chaque noeud envoie 1 mesg vers les voisins + 1 msg vers le parent
 * dans chaque arete
 * ======================================================
 */



#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"


using namespace omnetpp;

class Node : public cSimpleModule {
    private:
        int num_Gates,
            num_Closed_Gates = 0,
            my_Id,
            parent_Id;

        bool done = false,
             have_Parent = false;

        Set already_Informed_Nghd;
        cDisplayString Disply; // TODO : some visualizing stuffs
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
};


Define_Module(Node);

void Node::initialize(){

    my_Id = getIndex();
    num_Gates = gateSize("gate$o");
    int initiator = par("initiator").intValue();

    if(my_Id == initiator){
        cMessage *msg = new cMessage("Message");
        scheduleAt(simTime() + 1, msg);   // a self message !
    }
}


void Node::handleMessage(cMessage* msg){

    int neighbor;
    int senderId;

    if(msg->isSelfMessage()){// I am the initiator ...
        if(num_Gates == 0)  bubble("Idle mutation !");

        parent_Id = this->my_Id; // convention
        senderId = -1;
        have_Parent = true;
    }else{
        bubble("recieved Message :)");
        if(have_Parent == false){
            int port=  msg->getArrivalGate()->getIndex();
            parent_Id = gate("gate$o",port)->getPathEndGate()->getOwnerModule()->getIndex();
            EV << " my parent :  " << parent_Id << "\n";
            have_Parent = true;
        }
    }

    //********************************************************************************

    for(int i=0;i<num_Gates;i++){
        neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();


        if(neighbor != parent_Id){
            if(num_Closed_Gates < num_Gates){
                if(already_Informed_Nghd.Find(neighbor)){
                    // never forwards the token throught the same channel twice
                    continue;
                }else{

                    num_Closed_Gates ++;

                    send(msg->dup(),"gate$o",i);
                    already_Informed_Nghd.Add(neighbor);
                    break; // send to one
                           // visit sequentialy ...

                }
            } else {
                //
                if(my_Id == par("initiator").intValue()){
                    bubble("Correct!");
                }
            }
        }else{
            if(my_Id != par("initiator").intValue()){

                if(num_Closed_Gates == num_Gates - 1){ // I send to all except my parent
                    // forwards the token to its parent when there is no other option
                    num_Closed_Gates++;
                    send(msg->dup(),"gate$o",i);
                    already_Informed_Nghd.Add(neighbor);
                    break;
                }

            }
        }
    }






}


