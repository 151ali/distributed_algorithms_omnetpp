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
    int Pick(){
        // TODO : change it to uniforme

        // Pick the first element a erase it
        int picked = *content.begin();
        content.erase(picked);
        return picked;
    }
};

#endif /* SET_H_ */
