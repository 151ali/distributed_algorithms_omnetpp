/*
 * Processus_Noir.cc
 *
 *  Created on: Y y, 2021
 *      Author: lina
 */


#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;
enum{
    Black =7,
    White = 3
};

class Node : public cSimpleModule {
    private:
        //
        int myIndex,
            my_color,
            num_black_node = 0;

        bool done;

        Set Black_nodes,  // contains black node id's
            White_nodes,  // contains white node id's
            Unknown;      // contains the id's of unknow color's node
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;

        virtual void refreshDisplay() const override;
};


Define_Module(Node);

void Node::initialize(){
    myIndex = getIndex();

    // initialize Unknown to N(x) & Known to {x}

    for (int i = 0; i < gateSize("gate$o"); i++) {
        int neighbor = gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getIndex();
        Unknown.Add(neighbor);
    }


    // initialize Colors =================================
    if(intuniform(0, 1) == 0){
        my_color = Black;
        Black_nodes.Add(myIndex);
    }else{
        my_color = White;
        White_nodes.Add(myIndex);
    }


    // display stuff ..
    if(my_color == Black) {
        cDisplayString& dd = this->getDisplayString();
        dd.parse("i=block/routing,black");
    }


    // start the protocole  ===============================
    for (int i = 0; i < gateSize("gate$o"); i++) {

        // send to n
        Snail* msg = new Snail("MyMessage");
        msg->setBlackNodes(Black_nodes);
        msg->setWhiteNodes(White_nodes);
        msg->setUnknown(Unknown);


        send(msg,"gate$o",i);

    }

}
void Node::handleMessage(cMessage* msg){

    Snail *smsg = check_and_cast<Snail*>(msg);
    Set b,w,u;

    b = smsg->getBlackNodes();
    w = smsg->getWhiteNodes();
    u = smsg->getUnknown();



    // get new colors information
    Black_nodes = Black_nodes.Union(b);
    White_nodes = White_nodes.Union(w);

    // update unknown set
    Unknown = Unknown.Union(u);
    Unknown = Unknown.operator -(Black_nodes);
    Unknown = Unknown.operator -(White_nodes);



    if(!done){

        if(Unknown.Size() == 0 ) done = true;

        // start the protocole  ===============================
        for (int i = 0; i < gateSize("gate$o"); i++) {

            Snail* msg = new Snail("MyMessage");
            msg->setBlackNodes(Black_nodes);
            msg->setWhiteNodes(White_nodes);
            msg->setUnknown(Unknown);


            send(msg,"gate$o",i);

        }

    }

    num_black_node = Black_nodes.Size();
}

void Node::refreshDisplay() const{
        char buf[40];
        sprintf(buf, "num_black_node : %d ", num_black_node);

        getDisplayString().setTagArg("t", 0, buf);
}



