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

#ifndef SET_H_
#define SET_H_


#include <iterator>
#include <set>
#include <map>
#include <omnetpp.h>

using namespace omnetpp;

using namespace std;


class Set {
private:
    set<int, greater<int> > content;

public:
    Set();
    virtual ~Set();
    void Add(int i);
    void Remove(int i);
    bool Empty();
    void Clear();
    bool Find(int i){return (content.count(i)) == 1;}
    Set  Union(Set& s2);
    void Print();
    int Size(){
        int s = 0;
        set<int, greater<int> >::iterator itr;
        for (itr = this->content.begin();itr != this->content.end(); ++itr) s++;
        return s;

    }
    Set operator + (const int & x){
        this->Add(x);
        return *this;
    }
    Set operator - (Set & A){
        Set tmp;
        set<int, greater<int> >::iterator itr;
        // tmp = this;
        for (itr = this->content.begin();itr != this->content.end(); ++itr){
            tmp.Add(*itr);
        }
        // tmp = tmp - A
        for (itr = A.content.begin();itr != A.content.end(); ++itr){
            tmp.Remove(*itr);
        }
        return tmp;
    }

};

class Map {
    private:
        map<int, Set> m;

    public:
        Map(){

        }
        ~Map(){

        }

        void Add(int node,Set RT){
            m.insert(
                    pair<int, Set>(node, RT)
            );
        }
        void Print(){
            map<int, Set>::iterator itr;
            for(itr = m.begin(); itr != m.end(); ++itr) {
                EV << itr->first<<"  : ";itr->second.Print();
            }
        }


        int Find(int x){
            map<int, Set>::iterator itr;
            for(itr = m.begin(); itr != m.end(); ++itr) {

                if(itr->second.Find(x))
                    return itr->first;
            }
            return -1;
        }

};



#endif /* SET_H_ */
