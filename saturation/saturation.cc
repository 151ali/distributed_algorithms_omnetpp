/*
 * saturation.cc
 *
 *  Created on: Jan 31, 2020
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;

enum {

    Activate    = 0,
    Saturation  = 1,
    Center      = 2

};

class Node : public cSimpleModule {
    private:
        int my_Index,
            parent_gate; // used while saturaion ..

        Set Nx,          // my neighbors
            Neighbours;  // for saturation

        int max_value  = 0,
            max2_value = 0,
            max_neighbor = -1;



        bool center     = false,
             available  = true,
             active     = false,
             processing = false,
             saturated  = false;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;

        virtual Snail * prepare_message();
        virtual void process_message(Snail *msg);
        virtual void resolve();
};


Define_Module(Node);

void Node::initialize(){
    my_Index = getIndex();
    int initiator = 5;// par("initiator").intValue();

    // initialize N(x)
    for (int i = 0; i < gateSize("gate$o"); i++) {
        //int neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        //Nx.Add(neighbor);
        Nx.Add(i);
    }
    Nx.Print();


    if(my_Index == initiator){

         // send Activate to N(x)
         for (int i = 0; i < gateSize("gate$o"); i++) {
             if(Nx.Find(i) == false) continue;
             Snail *msg = new Snail("Activate");
             msg->setKind(Activate);
             send(msg,"gate$o",i);
         }

         Neighbours = Nx;

         if(Neighbours.Size() == 1){ // I am leaf
             Snail* M = prepare_message();
             parent_gate = Neighbours.Pick();
             send(M,"gate$o",parent_gate);
             processing = true;

             cDisplayString& dd = this->getDisplayString();
             dd.parse("i=block/routing,yellow");

         }else{
             cDisplayString& dd = this->getDisplayString();
             dd.parse("i=block/routing,green");
             active = true;
         }

     }
}


void Node::handleMessage(cMessage* msg){
    int message_kind = msg->getKind();
    int sender_gate = msg->getArrivalGate()->getIndex();
    int sender_id   = gate("gate$o",sender_gate)->getPathEndGate()->getOwnerModule()->getIndex();


    switch (message_kind) {
        case Activate:
            if(available){
                // send Activate to N(x) - sender
                for(int i = 0; i < gateSize("gate$o"); i++) {
                    if(Nx.Find(i) == false || i == sender_gate) continue;
                    Snail *msg = new Snail("Activate");
                    msg->setKind(Activate);
                    send(msg,"gate$o",i);
                }

                Neighbours = Nx;

                if(Neighbours.Size() == 1){ // I am leaf
                    Snail* M = prepare_message();
                    parent_gate = Neighbours.Pick();
                    send(M,"gate$o",parent_gate);
                    processing = true;

                    cDisplayString& dd = this->getDisplayString();
                    dd.parse("i=block/routing,yellow");

                }else{
                    cDisplayString& dd = this->getDisplayString();
                    dd.parse("i=block/routing,green");
                    active = true;
                }
            }else{

            }
            break;

        case Saturation:



            process_message(check_and_cast<Snail*>(msg));



            if(active){
                Neighbours.Remove(sender_gate);
                Neighbours.Print();


                if(Neighbours.Size() == 1){ // I am leaf
                    Snail* M = prepare_message();
                    parent_gate = Neighbours.Pick();
                    send(M,"gate$o",parent_gate);


                    cDisplayString& dd = this->getDisplayString();
                    dd.parse("i=block/routing,yellow");
                    processing = true;
                    active     = false;
                }
            }else{
                if(processing){
                    resolve();
                    //cDisplayString& dd = this->getDisplayString();
                    //dd.parse("i=block/routing,red");

                }
            }
            break;

        case Center :
            process_message(check_and_cast<Snail*>(msg));
            resolve();
            break;
        default:
            break;
    }

}

// plug in  ...
Snail * Node::prepare_message(){
    Snail *msq = new Snail("Saturation");
    msq->setValue(max_value + 1);
    msq->setKind(Saturation);
    return msq;
}

void Node::process_message(Snail *msg){

    int received_value = msg->getValue();
    int sender = msg->getArrivalGate()->getIndex();


    if(max_value <= received_value){
        max2_value = max_value;
        max_value = received_value;
        max_neighbor = sender;

        char buff[50];
        sprintf(buff, "mmax_neighbor = %d",max_neighbor);
        bubble(buff);


    }else{
        if(max2_value <= received_value) max2_value = received_value;
    }

}

void Node::resolve(){
    bubble("RESOLVE .. ");

    if(max_value - max2_value == 1){
        if(max_neighbor != parent_gate){
            // send(Center,max2_value) to max_neighbor
            Snail *msq = new Snail("Center");
            msq->setKind(Center);
            msq->setValue(max2_value+1);
            send(msq,"gate$o",max_neighbor);

        }
        // become center
        cDisplayString& dd = this->getDisplayString();
        dd.parse("i=block/routing,red");
        center = true;

    }else{
        if(max_value - max2_value > 1){
            // send(Center,max2_value) to max_neighbor
            Snail *msq = new Snail("Center");
            msq->setKind(Center);
            msq->setValue(max2_value+1);
            send(msq,"gate$o",max_neighbor);
        }else{
            // become center
            cDisplayString& dd = this->getDisplayString();
            dd.parse("i=block/routing,red");
            center = true;
        }
    }

}

