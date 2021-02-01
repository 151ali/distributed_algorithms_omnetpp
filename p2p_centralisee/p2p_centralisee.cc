/*
 * p2p_centralisee.cc
 *
 *  Created on: Jan 8, 2021
 *      Author: lina
 */

#include <string.h>
#include <omnetpp.h>
#include "Snail_m.h"
#include "Set.h"

using namespace omnetpp;

enum{
    Request = 0,  // Search request to the server
    Result  = 1,  // Search result from the server

    Conn    = 2,  // connexion between peers
    Part    = 3   // this message is a file part
};


class Node : public cSimpleModule {

public:
    bool is_peer;
    int service_port,
        my_id;

    Set File_Parts;
    int file_size;


protected:

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
// The module class needs to be registered with OMNeT++
Define_Module(Node);


void Node::initialize() {
    is_peer= par("isPeer").boolValue();
    my_id  = par("id").intValue();



    if(is_peer){
        // initilize server port
        for (int i = 0; i < gateSize("gate$o"); i++) {
            if(strcmp(gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getName(),
                            "service") == 0)
                service_port = i;
        }

    }else{
        // I am service
        // initialize where the file is

        getDisplayString().setTagArg("t", 0, "file at\npeer1\nand peer4");

        File_Parts.Add(4);
        File_Parts.Add(1);

    }


    if(strcmp(getName() , "peer3") == 0 || strcmp(getName() , "peer4") == 0){

            cDisplayString& dd = this->getDisplayString();
            dd.parse("i=device/pc4,blue");

            cMessage *msg = new cMessage("Request");
            send(msg,"gate$o",service_port);
    }

}

void Node::handleMessage(cMessage *msg) {
    int message_kind = msg->getKind();
    int sender_gate  = msg->getArrivalGate()->getIndex();





    if(! is_peer && message_kind == Request ){
        Snail *resp = new Snail("Results");
        resp->setKind(Result);
        resp->setFP(File_Parts);

        send(resp,"gate$o",sender_gate);
    }


    if(is_peer && message_kind == Result){
        Snail *result = check_and_cast<Snail*>(msg);
        Set file_parts = result->getFP();
        file_parts.Remove(my_id);

        file_size = file_parts.Size();

        while(file_parts.Size()!=0){
            char peerName[6];
            sprintf(peerName, "peer%d", file_parts.Pick());

            for (int i = 0; i < gateSize("gate$o"); i++) {
                if(strcmp(gate("gate$o",i)->getPathEndGate()->getOwnerModule()->getName(),peerName) == 0){
                    Snail *conn = new Snail("File Request");
                    conn->setKind(Conn);
                    send(conn,"gate$o",i);
                }
            }
        }

    }

    if(is_peer && message_kind == Conn){

       Snail *conn = new Snail("File part");
       conn->setKind(Part);
       send(conn,"gate$o",sender_gate);
    }

    if(is_peer && message_kind == Part){
       file_size--;
       if(file_size == 0){
           bubble("I have recieved all parts");
           cDisplayString& dd = this->getDisplayString();
           dd.parse("i=device/pc4,red");
       }
    }














}
