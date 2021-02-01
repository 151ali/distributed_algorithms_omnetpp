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

}


network FullGraph {
   int n;
    
    types:
	   channel Channel extends ned.DelayChannel {
	   	delay = 100 ms;
	    }
    submodules:
       	node[n] : Node;
    connections:
        
        for i = 0..n-1, for j=0..n-1 {
            node[i].gate++ <--> Channel <--> node[j].gate++ if i!=j;
        }	
  }
  
  
  
network HyperCube {
   int dim;
    
    types:
	   channel Channel extends ned.DelayChannel {
	   	delay = 100 ms;
	    }
    submodules:
       	node[2^dim] : Node;
    connections:
        
        for i = 0..2^dim-1, for j=0..dim-1 {
            node[i].gate++ <--> Channel <--> node[i # 2^j].gate++ ;
        }	
  }
  
  
  
  
  network BTree {

        int height;
    types:
	   channel Channel extends ned.DelayChannel {
	   	delay = 100 ms;
	    }
    submodules:
       	node[2^height+1] : Node;
    connections:
        
        for i = 0..2^(height-1)-1 {
              node[i].gate++ <--> Channel <--> node[2*i+1].gate++;
              node[i].gate++ <--> Channel <-->  node[2*i+2].gate++;
        }	
  }
  
  
network Random {
	int n;
	double alpha;//=0.5;

    types:
    	channel Channel extends ned.DelayChannel {
    	   	delay = 100 ms;
    	}
    submodules:
       	node[n] : Node;
    connections:
        
        for i = 0..n-1 , for j = 0..n-1 {
            node[i].gate++ <--> Channel <-->  node[j].gate++ if i!=j && uniform(0,1)<alpha;
        }	
  }
  
  
network Chain {
	int n;
	
    types:
    	channel Channel extends ned.DelayChannel {
    	   	delay = 100 ms;
    	}
    submodules:
       	node[n] : Node;
    connections:
        
        for i = 0..n-2{
       		node[i].gate++ <--> Channel <-->  node[i+1].gate++;
         }	
  }
  
  
network Ring {
	int n;
	
    types:
    	channel Channel extends ned.DelayChannel {
    	   	delay = 100 ms;
    	}
    submodules:
       	node[n] : Node;
    connections:
        
        for i = 0..n-2{
       		node[i].gate++ <--> Channel <-->  node[i+1].gate++;
         }	
         node[n-1].gate++ <--> Channel <-->  node[0].gate++;
  }
  
  
  
  