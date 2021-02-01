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
	    node[2] : Node;
	    
	connections:
	    node[0].gate++ <--> Channel <--> node[1].gate++;
   
}