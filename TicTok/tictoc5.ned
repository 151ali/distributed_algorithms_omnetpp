


// NED inheritance 

// base 
simple Txc5 {
    parameters:
       	bool sendMsgOnInit = default(false);
        int limit = default(2);
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

simple Tic5 extends Txc5 {
	parameters:
	    @display("i=,cyan");
	    sendMsgOnInit = true;
}

simple Toc5 extends Txc5 {
	parameters:
	    @display("i=,gold");
	    sendMsgOnInit = false;  // Toc module should not send a message on init 
	    						// try to to make it `true`
}

network Tictoc5 {
    submodules:
        tic: Tic5;
        toc:Toc5;
    connections:
        tic.out --> {delay = 100ms;} --> toc.in;
	    tic.in <-- {delay = 100ms;} <-- toc.out;
}
        