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

#include "Set.h"
#include <omnetpp.h>

using namespace omnetpp;
Set::Set() {
    // TODO Auto-generated constructor stub

}

Set::~Set() {
    // TODO Auto-generated destructor stub
}

void Set::Add(int i){
    this->content.insert(i);
}

void Set::Remove(int i){
    this->content.erase(i);
}

bool Set::Empty(){
    return this->content.empty();
}

void Set::Clear(){
    this->content.clear();
}

Set Set::Union(Set& s2){
    Set tmp;
    set<int, greater<int> >::iterator itr;

    for (itr = s2.content.begin();itr != s2.content.end(); ++itr){
        tmp.content.insert(*itr);
    }

    for (itr = content.begin();itr != content.end(); ++itr){
            tmp.content.insert(*itr);
    }

    return tmp;
}

void Set::Print(){
   set<int, greater<int> >::iterator itr;
   for (itr = content.begin();itr != content.end(); ++itr){
       EV << *itr << "   ";
   }
   EV <<"\n";

}

