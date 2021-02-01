//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
simple Node
{
    int id = default(0);
    bool isPeer = default(true);
    @display("i=device/pc4"); // add a default icon
    gates:
        inout gate[];
}
network Network
{
    //@display("bgb=487,276;i=maps/world,blue,19;is=n;i2=status/");
    submodules:
        service: Node {
            @display("i=device/server, chartreuse;p=216,125");
            isPeer = false;
           
        }
        peer1: Node{
        	id = 1;
        };
        peer2: Node{
        	id = 2;
        };
        peer3: Node{
        	id = 3;
        };
        peer4: Node{
        	id = 4;
        };
        
    connections:

        service.gate++ <--> {  delay = 100ms; } <--> peer1.gate++;
        service.gate++ <--> {  delay = 100ms; } <--> peer2.gate++;
        service.gate++ <--> {  delay = 100ms; } <--> peer3.gate++;
        service.gate++ <--> {  delay = 100ms; } <--> peer4.gate++;
         
        
        
        peer2.gate++ <--> {  delay = 100ms; } <--> peer1.gate++;

        peer3.gate++ <--> {  delay = 100ms; } <--> peer1.gate++;
        peer2.gate++ <--> {  delay = 100ms; } <--> peer3.gate++;

        
        peer4.gate++ <--> {  delay = 100ms; } <--> peer1.gate++;
        peer4.gate++ <--> {  delay = 100ms; } <--> peer2.gate++;
        peer4.gate++ <--> {  delay = 100ms; } <--> peer3.gate++;
}