simple Node{
    parameters:
        @display("i=block/routing");
        int initiator = default(1);
    gates:
        inout gate[];
}



network Network {

	types:
	    channel Channel extends ned.DelayChannel {
	    	delay = 100 ms;
	    }
	submodules:
	    node[6] : Node;
	    
	connections:
	    node[0].gate++ <--> Channel <--> node[1].gate++;
        node[1].gate++ <--> Channel <--> node[2].gate++;
        node[1].gate++ <--> Channel <--> node[4].gate++;
        node[3].gate++ <--> Channel <--> node[4].gate++;
        node[4].gate++ <--> Channel <--> node[5].gate++;
        
        node[3].gate++ <--> Channel <--> node[5].gate++;
        
        // node[2].gate++ <--> Channel <--> node[0].gate++;
   
}