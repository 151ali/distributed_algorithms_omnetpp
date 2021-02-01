/*
 * Tree_center_finding_BFT.cc
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
    Center      = 2,

    BFT_Token   = 3,
    BFT_Yes     = 4,
    BFT_No      = 5

};

class Node : public cSimpleModule {
    private:
        int my_Index,
            parent_gate; // used while saturaion ..

        Set Nx,          // my neighbors
            Neighbours;  // for saturation

        Set BFT_Children;

        int max_value  = 0,
            max2_value = 0,
            max_neighbor = -1;



        bool center     = false,
             available  = true,
             active     = false,
             processing = false,
             saturated  = false;


        bool have_parent_bft = false;
        int  parent_id_bft;
        int  parent_gate_bft;
        int  level_bft;
        int  expected_msg;
        int d;



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

    Snail *yes,*no,*tok;




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
       // ***************************************************** //
       //           ********* *** BFT *** ********              //
       // ***************************************************** //
        case BFT_Token:

            d = check_and_cast<Snail*>(msg)->getDistance();

            if(have_parent_bft == false){

                parent_id_bft = sender_id;
                parent_gate_bft = sender_gate;

                BFT_Children.Clear();
                level_bft = d + 1;
                have_parent_bft = true;

                // displat stuff ...
                cDisplayString connDisplay = gate("gate$o",sender_gate)->getDisplayString();
                connDisplay.setTagArg("ls",0,"blue");
                connDisplay.setTagArg("ls",1,"3");
                gate("gate$o",sender_gate)->setDisplayString(connDisplay.str());

                Nx.Remove(sender_gate);
                expected_msg = Nx.Size();
                Nx.Add(sender_gate);


                if(expected_msg == 0){
                    // send(Yes, d + 1) to parent
                    yes = new Snail("Yes");
                    yes->setKind(BFT_Yes);
                    yes->setDistance(d + 1);
                    send(yes,"gate$o",parent_gate_bft);
                }else{
                    // send(T, d + 1) to Nx-sender_gate
                    for(int i=0;i<gateSize("gate$o");i++){
                        if(i == sender_gate) continue;
                        tok = new Snail("Token");
                        tok->setKind(BFT_Token);
                        tok->setDistance(d + 1);
                        send(tok,"gate$o",i);
                    }
                }

            }else{ // I have parent ...

                if(level_bft > d + 1){

                    parent_id_bft   = sender_id;
                    parent_gate_bft = sender_gate;

                    BFT_Children.Clear();
                    level_bft = d + 1;
                    have_parent_bft = true;


                    // displat stuff ...
                    cDisplayString connDisplay = gate("gate$o",parent_gate_bft)->getDisplayString();
                    connDisplay.setTagArg("ls",0,"blue");
                    connDisplay.setTagArg("ls",1,"3");
                    gate("gate$o",parent_gate_bft)->setDisplayString(connDisplay.str());


                    Nx.Remove(sender_gate);
                    expected_msg = Nx.Size();
                    Nx.Add(sender_gate);

                    if(expected_msg == 0){
                        // Send(yes, level) to Parent
                        yes = new Snail("Yes");
                        yes->setKind(BFT_Yes);
                        yes->setDistance(level_bft);
                        send(yes,"gate$o",parent_gate_bft);

                    }else{
                        // send(T, d + 1) to Nx-sender_gate
                        for(int i=0;i<gateSize("gate$o");i++){
                            if(i == sender_gate) continue;
                            yes = new Snail("Token");
                            yes->setKind(BFT_Token);
                            yes->setDistance(d + 1);
                            send(yes,"gate$o",i);
                        }
                    }
                }else{
                    // send (No, d + 1) to sender_gate
                    no = new Snail("No");
                    no->setKind(BFT_No);
                    no->setDistance(d + 1);
                    send(no,"gate$o",sender_gate);
                }
            }
            break;


        case BFT_Yes:
            d = check_and_cast<Snail*>(msg)->getDistance();

            bubble("Yess ");
            EV << "sender_gate : "<<sender_gate <<"  sender_id : "<< sender_id <<"\n";
            if(d == level_bft + 1){
                BFT_Children.Add(sender_gate);

                // displat stuff ...
                cDisplayString connDisplay = gate("gate$o",sender_gate)->getDisplayString();
                connDisplay.setTagArg("ls",0,"blue");
                connDisplay.setTagArg("ls",1,"3");
                gate("gate$o",sender_gate)->setDisplayString(connDisplay.str());
            }

            expected_msg--;

            if(expected_msg == 0){
                if(parent_id_bft != my_Index){
                    // send (yes, level) to parent
                    yes = new Snail("Yes");
                    yes->setKind(BFT_Yes);
                    yes->setDistance(level_bft);
                    send(yes,"gate$o",parent_gate_bft);

                }else{// I am the initiator
                    // the node learns that bft tree is built
                    bubble("Done !");
                }
            }

            break;
        case BFT_No:
            d = check_and_cast<Snail*>(msg)->getDistance();
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
        // center 02
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
            // center 01

                cDisplayString& dd = this->getDisplayString();
                dd.parse("i=block/routing,red");
                center = true;


                // ********************************************** //
                //              Starting BFT                      //
                // ********************************************** //
                have_parent_bft = true;
                parent_id_bft = my_Index; // !

                d = -1;
                level_bft = d + 1;


                if(Nx.Size() != 0){
                    for(int i=0;i<gateSize("gate$o");i++){
                        Snail *m = new Snail("BFT_T");
                        m->setKind(BFT_Token);
                        m->setDistance(d + 1);
                        send(m,"gate$o",i);
                    }
                }else bubble("idle mutation :{ ");

        }
    }

}

