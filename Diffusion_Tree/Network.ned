simple Node
{
    parameters:
        @display("i=block/routing");
        int initiator = default(1);
    gates:
        inout gate[];
}


network OneCenter
{
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100 ms;
        }
    submodules:
        node[11]: Node;
    connections:
        node[10].gate++ <--> Channel <--> node[9].gate++;
        node[6].gate++ <--> Channel <--> node[9].gate++;

        node[6].gate++ <--> Channel <--> node[8].gate++;
        node[6].gate++ <--> Channel <--> node[0].gate++;

        node[1].gate++ <--> Channel <--> node[3].gate++;
        node[1].gate++ <--> Channel <--> node[2].gate++;

        node[5].gate++ <--> Channel <--> node[4].gate++;
        node[5].gate++ <--> Channel <--> node[1].gate++;

        node[7].gate++ <--> Channel <--> node[5].gate++;
        node[7].gate++ <--> Channel <--> node[6].gate++;
}

network TwoCenters
{
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100 ms;
        }
    submodules:
        node[10]: Node;
    connections:

        node[6].gate++ <--> Channel <--> node[9].gate++;

        node[6].gate++ <--> Channel <--> node[8].gate++;
        node[6].gate++ <--> Channel <--> node[0].gate++;

        node[1].gate++ <--> Channel <--> node[3].gate++;
        node[1].gate++ <--> Channel <--> node[2].gate++;

        node[5].gate++ <--> Channel <--> node[4].gate++;
        node[5].gate++ <--> Channel <--> node[1].gate++;

        node[7].gate++ <--> Channel <--> node[5].gate++;
        node[7].gate++ <--> Channel <--> node[6].gate++;
}



network Tree
{
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100 ms;
        }
    submodules:
        node[7]: Node;
    connections:
        node[1].gate++ <--> Channel <--> node[0].gate++;
        node[1].gate++ <--> Channel <--> node[2].gate++;


        node[0].gate++ <--> Channel <--> node[3].gate++;
        node[0].gate++ <--> Channel <--> node[4].gate++;

        node[4].gate++ <--> Channel <--> node[5].gate++;
        node[4].gate++ <--> Channel <--> node[6].gate++;




}
network BTree
{
    int height;
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100 ms;
        }
    submodules:
        node[2^height+1]: Node;
    connections:

        for i=0..2^(height-1)-1 {
            node[i].gate++ <--> Channel <--> node[2*i+1].gate++;
            node[i].gate++ <--> Channel <--> node[2*i+2].gate++;
        }
}


network Random
{
    int n;
    double alpha;//=0.5;

    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100 ms;
        }
    submodules:
        node[n]: Node{
        	@display("p=",n,",",n%2);
        };
    connections:

        for i=0..n-1, for j=0..n-1 {
            node[i].gate++ <--> Channel <--> node[j].gate++ if i!=j && uniform(0,1)<alpha;
        }
}