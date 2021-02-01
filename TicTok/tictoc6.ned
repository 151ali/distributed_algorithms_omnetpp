simple Txc6 {
    parameters:
       
        // volatile double delayTime @unit(s);   // delay before sending back message
       volatile double delayTime @unit(s);
        
        @display("i=block/routing");
	gates:
        input in;output out;
}
	
network Tictoc6 {
    submodules:
        tic: Txc6 {
            parameters:
                @display("i=,cyan");
        }
        toc: Txc6 {
            parameters:
                @display("i=,gold");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}