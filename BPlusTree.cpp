#include "BPlusTree.h"
#include <math.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;


//public:
//default constructor
BPlusTree::BPlusTree()
{
    this->n = 1;
    this->head = new LeafNode();
}
//Constructor: creates an empty tree, the parameter sets the number of keys that can be stored in each node (leaf and interior)
BPlusTree::BPlusTree(int value)
{
    this->n = value;
    this->head = new LeafNode();
}

//inserts the key (int) and value (string) into the tree
bool BPlusTree::insert(int key, string value)
{
    if (find(key)!=""){
        cout<<"You cannot insert duplicate keys!"<<endl;
        return false;
    }
    Node* t= insertion(key,value);

    //Redistributing
    redistribution(t);
    return true;
}

// searches the tree for the key (int), if found removes the key and associated value from the tree
bool BPlusTree::remove(int key)
{
    if (find(key) == ""){
        cout<<"You don't have that key in your tree!"<<endl;
        return false;
    }

    Node* t = deletion(key);

    if(((LeafNode*)t)->Value.size()<ceil((n+1)/2) && t->Parent!=NULL){
        
        Node* Lsibling = NULL;
        Node* Rsibling = NULL;
        
        int Child_num = -1;
        for(int i=0;i<((In_Node*)t->Parent)->Children.size();++i){
            //Find the index
            if(((In_Node*)t->Parent)->Children[i]==t){
                Child_num=i;
                break;
            }
        }
        if(Child_num >= 1){
            Lsibling = ((In_Node*)t->Parent)->Children[Child_num-1];
        }
        
        if(Child_num+1<((In_Node*)t->Parent)->Children.size()){
            Rsibling = ((In_Node*)t->Parent)->Children[Child_num+1];
        }

        //If value is on the left and the right sibling is full
        if(Rsibling!=NULL && ((LeafNode*)Rsibling)->Value.size()-1>=ceil((n+1)/2)){
            //move the front of right sibling key at the end of mine
            t->Keys.push_back(Rsibling->Keys.front());
            ((LeafNode*)t)->Value.push_back(((LeafNode*)Rsibling)->Value.front());
            Rsibling->Keys.erase(Rsibling->Keys.begin());
            ((LeafNode*)Rsibling)->Value.erase(((LeafNode*)Rsibling)->Value.begin());
            t->Parent->Keys[Child_num]=Rsibling->Keys.front();
            return true;
        }
        //If value is in the middle and the left sibling is full
        else if(Lsibling!=NULL && ((LeafNode*)Lsibling)->Value.size()-1>=ceil((n+1)/2)){
            //move the end of left sibling to right
            t->Keys.insert(t->Keys.begin(), Lsibling->Keys.back());
            ((LeafNode*)t)->Value.insert(((LeafNode*)t)->Value.begin(), ((LeafNode*)Lsibling)->Value.back());
            Lsibling->Keys.erase(Lsibling->Keys.end()-1);
            ((LeafNode*)Lsibling)->Value.erase(((LeafNode*)Lsibling)->Value.end()-1);
            t->Parent->Keys[Child_num-1]=t->Keys.front();
            return true;
        }
        else {
            if(Rsibling!=NULL){
                t->Keys.insert(t->Keys.end(), Rsibling->Keys.begin(), Rsibling->Keys.end());
                ((LeafNode*)t)->Value.insert(((LeafNode*)t)->Value.end(),((LeafNode*)Rsibling)->Value.begin(), ((LeafNode*)Rsibling)->Value.end());
                ((LeafNode*)t)->Next=((LeafNode*)Rsibling)->Next;
                t->Parent->Keys.erase(t->Parent->Keys.begin()+Child_num);
                ((In_Node*)t->Parent)->Children.erase(((In_Node*)t->Parent)->Children.begin()+Child_num+1);
            }
            else if(Lsibling!=NULL){
                Lsibling->Keys.insert(Lsibling->Keys.end(), t->Keys.begin(), t->Keys.end());
                ((LeafNode*)Lsibling)->Value.insert(((LeafNode*)Lsibling)->Value.end(),((LeafNode*)t)->Value.begin(), ((LeafNode*)t)->Value.end());
                ((LeafNode*)Lsibling)->Next=((LeafNode*)t)->Next;
                t->Parent->Keys.erase(t->Parent->Keys.begin()+Child_num-1);
                ((In_Node*)t->Parent)->Children.erase(((In_Node*)t->Parent)->Children.begin()+Child_num);
                t = Lsibling;
            }
            
            while(t->Parent!=this->head){
                Rsibling=Lsibling=NULL;
                t=t->Parent;
                if(((In_Node*)t)->Children.size()>=floor((n+2)/2)){
                    return true;
                }
                int Child_num = -1;
                for(int i=0;i<((In_Node*)t->Parent)->Children.size();++i){
                    if(((In_Node*)t->Parent)->Children[i]==t){
                        Child_num=i;
                        break;
                    }
                }
                if(Child_num >= 1){
                    Lsibling = ((In_Node*)t->Parent)->Children[Child_num-1];
                }
                
                if(Child_num+1<((In_Node*)t->Parent)->Children.size()){
                    Rsibling = ((In_Node*)t->Parent)->Children[Child_num+1];
                }

                if(Rsibling!=NULL){
                    if(((In_Node*)Rsibling)->Children.size()-1>=floor((n+2)/2)){
                        ((In_Node*)t)->Children.push_back(((In_Node*)Rsibling)->Children.front());
                        t->Keys.push_back(t->Parent->Keys[Child_num]);
                        t->Parent->Keys[Child_num]=Rsibling->Keys.front();
                        ((In_Node*)Rsibling)->Children.erase(((In_Node*)Rsibling)->Children.begin());
                        Rsibling->Keys.erase(Rsibling->Keys.begin());
                        ((In_Node*)t)->Children.back()->Parent=t;
                        return true;
                    }
                    else {
                        ((In_Node*)Rsibling)->Children.insert(((In_Node*)Rsibling)->Children.begin(),((In_Node*)t)->Children.begin(),((In_Node*)t)->Children.end());
                        Rsibling->Keys.insert(Rsibling->Keys.begin(), t->Parent->Keys[Child_num]);
                        Rsibling->Keys.insert(Rsibling->Keys.begin(), t->Keys.begin(), t->Keys.end());
                        for(int i=0;i<((In_Node*)t)->Children.size();++i){
                            ((In_Node*)t)->Children[i]->Parent=Rsibling;
                        }
                        t->Parent->Keys.erase(t->Parent->Keys.begin()+Child_num);
                        ((In_Node*)t->Parent)->Children.erase(((In_Node*)t->Parent)->Children.begin()+Child_num);
                        t = Rsibling;
                    }
                }
                else if(Lsibling!=NULL){
                    if(((In_Node*)Lsibling)->Children.size()-1>=floor((n+2)/2)){
                        ((In_Node*)t)->Children.insert(((In_Node*)t)->Children.begin(),((In_Node*)Lsibling)->Children.back());
                        t->Keys.insert(t->Keys.begin(), t->Parent->Keys[Child_num-1]);
                        t->Parent->Keys[Child_num]=t->Keys.front();
                        ((In_Node*)Lsibling)->Children.erase(((In_Node*)Lsibling)->Children.end()-1);
                        Lsibling->Keys.erase(Lsibling->Keys.end()-1);
                        ((In_Node*)t)->Children.front()->Parent=t;
                        return true;
                    }
                    else {
                        ((In_Node*)Lsibling)->Children.insert(((In_Node*)Lsibling)->Children.end(), ((In_Node*)t)->Children.begin(), ((In_Node*)t)->Children.end());
                        Lsibling->Keys.insert(Lsibling->Keys.end(), t->Parent->Keys[Child_num-1]);
                        Lsibling->Keys.insert(Lsibling->Keys.end(), t->Keys.begin(), t->Keys.end());
                        for(int i=0;i<((In_Node*)t)->Children.size();++i){
                            ((In_Node*)t)->Children[i]->Parent=Lsibling;
                        }
                        t->Parent->Keys.erase(t->Parent->Keys.begin()+Child_num-1);
                        ((In_Node*)t->Parent)->Children.erase(((In_Node*)t->Parent)->Children.begin()+Child_num);
                        t = Lsibling;
                    }
                }

            }
            if(t->Parent==this->head && this->head->Keys.size()==0){
                this->head=t;
                return true;
            }
            
        }
        
    }
    return true;
    
}

// searches the tree for the key (int), if found returns the matching value
string BPlusTree::find(int target)
{
    Node* t=this->head;
    while (!t->isLeaf){ //find the proper location
        int flag = 0;
        for (int i=0;i<t->Keys.size();++i){
            if(t->Keys[i]>target){
                t = ((In_Node*)t)->Children[i];
                flag = 1;
                break;
            }
        }
        if(!flag){
            t = ((In_Node*)t)->Children[t->Keys.size()];
        }
    }
    if(t->Keys.size()==0||target>t->Keys.back()){
        return "";
    }
    else{
        for(int i=0;i<t->Keys.size();++i){ 
            if(t->Keys[i]==target){
                return ((LeafNode*)t)->Value[i];
            }
        }
        return "";
    }
}

//prints the keys in each node with nodes of the same level appearing on the same line
void BPlusTree::printKeys()
{
    if(head->Keys.size()==0){
        cout<<"[]"<<endl;
        return;
    }
    vector<Node*> q;
    q.push_back(head);
    while(q.size()){
        int size = q.size();
        for(int i=0;i<size;++i){
            if(!q[i]->isLeaf){
                for(int j=0;j<((In_Node*)q[i])->Children.size();++j){
                        q.push_back(((In_Node*)q[i])->Children[j]);
                }
            }
            cout<<"[";
            int nk=0;
            for(nk=0;nk<q[i]->Keys.size()-1;++nk){
                cout<<q[i]->Keys[nk]<<",";
            }
            cout<<q[i]->Keys[nk]<<"] ";
        }
        q.erase(q.begin(),q.begin()+size);
        cout<<endl;
    }
}

//prints the values in key (not value) order
void BPlusTree::printValues()
{
    Node* t = this->head;
    while(!t->isLeaf){
        t = ((In_Node*)t)->Children[0];
    }
    while(t!=NULL){
        for(int i=0;i<t->Keys.size();i++){
            cout<<((LeafNode*)t)->Value[i]<<endl;
        }
        t=((LeafNode*)t)->Next;
    }
}

//destructor
BPlusTree::~BPlusTree()
{
    if(this->head){
        delete this->head;
    }
}

BPlusTree::BPlusTree(const BPlusTree& bpt) {
    this->n = bpt.n;
    this->head = copyNode(bpt.head, nullptr);
}

BPlusTree& BPlusTree::operator=(const BPlusTree & bpt)
{
    if(this->head){
        delete this->head;
    }
    this->n = bpt.n;
    this->head = copyNode(bpt.head, nullptr);

    return *this;
}

Node* BPlusTree::copyNode(Node* t, Node* parent) {
    if (!t) {
        return nullptr;
    }

    Node* newNode = nullptr;
    //copy leaf node
    if (t->isLeaf) {
        LeafNode* tar = static_cast<LeafNode*>(t);
        LeafNode* res = new LeafNode();
        res->Keys = tar->Keys;
        res->Value = tar->Value;
        res->Parent = parent;
        newNode = res;
    } 
    //copy in-node
    else {
        In_Node* tar = static_cast<In_Node*>(t);
        In_Node* res = new In_Node();
        res->Parent = parent;
        res->Keys = t->Keys;
        for (Node* child : tar->Children) {
            Node* newChild = copyNode(child, res);
            if (newChild) {
                res->Children.push_back(newChild);
            }
        }
        newNode = res;
    }

    return newNode;
}

//Helper function insert the node without redistribute
Node* BPlusTree::insertion(int key, string value)
{
    Node* t=this->head;
    while (!t->isLeaf){ 
        //find the proper location
        int flag = 0;
        for (int i=0;i<t->Keys.size();i++){
            if(t->Keys[i]>key){
                t = ((In_Node*)t)->Children[i];
                flag = 1;
                break;
                //Found a value in node that greater than n, break before move to next
            }
        }
        if(flag == 0 ){
            //there is no value in the node greater than n
            t = ((In_Node*)t)->Children.back();
        }
    }
    if(t->Keys.size()==0||key>t->Keys.back()){
        // Found the right location
        t->Keys.push_back(key);
        //Insert
        ((LeafNode*)t)->Value.push_back(value);
    }
    else{
        for(int i=0;i<t->Keys.size();i++){ 
            //insert into the leaf node
            if(t->Keys[i]>key){
                t->Keys.insert(t->Keys.begin() + i, key);
                ((LeafNode*)t)->Value.insert(((LeafNode*)t)->Value.begin() + i, value);
                break;
            }
        }
    }
    return t;
}

//Helper function: delete the node without redistribute 
Node* BPlusTree::deletion(int key)
{
    Node* t=this->head;
    while (!t->isLeaf){ //find the proper location
        int flag = 0;
        for (int i=0;i<t->Keys.size();++i){
            if(t->Keys[i]>key){
                t = ((In_Node*)t)->Children[i];
                flag = 1;
                break;
            }
        }
        if(!flag){
            t = ((In_Node*)t)->Children[t->Keys.size()];
        }
    }
    for(int i=0;i<t->Keys.size();++i){
        if(t->Keys[i]==key){
            t->Keys.erase(t->Keys.begin()+i);
            ((LeafNode*)t)->Value.erase(((LeafNode*)t)->Value.begin()+i);
            break;
        }
    }
    return t;
}

//Helper Funtion: Redistribute the tree when insertion
void BPlusTree::redistribution(Node* t){
    if(t->Keys.size()>this->n){
        // when the key size exceed the max size after insertion
        //split the leaf node
        Node* tnew = new LeafNode();
        //New node has same parent with current one
        tnew->Parent = t->Parent;
        tnew->Keys.insert(tnew->Keys.begin(), t->Keys.begin()+ceil((n+2)/2), t->Keys.end());
        ((LeafNode*)tnew)->Value.insert(((LeafNode*)tnew)->Value.begin(), 
            ((LeafNode*)t)->Value.begin()+ceil((n+2)/2), 
            ((LeafNode*)t)->Value.end());

        //erase the previous node
        t->Keys.erase(t->Keys.begin()+ceil((n+2)/2), t->Keys.end());
        ((LeafNode*)t)->Value.erase(((LeafNode*)t)->Value.begin()+ceil((n+2)/2), 
            ((LeafNode*)t)->Value.end());

        //Switch the next node
        ((LeafNode*)tnew)->Next=((LeafNode*)t)->Next;
        ((LeafNode*)t)->Next=tnew;
        int key = t->Keys[floor((n+2)/2)];
        //check its parent
        while(t->Parent!=NULL){
            t=t->Parent;
            for(int i=0;i<t->Keys.size();++i){
                if(key>t->Keys.back()){
                    t->Keys.push_back(key);
                    ((In_Node*)t)->Children.push_back(tnew);
                    break;
                }
                //Find the place
                else if(t->Keys[i]>key){
                    t->Keys.insert(t->Keys.begin() + i, key);
                    ((In_Node*)t)->Children.insert(((In_Node*)t)->Children.begin()+i+1, tnew);
                    break;
                }
            }
            if(t->Keys.size()>this->n){
                //If the value in a key exceed the max size
                Node* nright = new In_Node();
                nright->Parent = t->Parent;
                nright->Keys.insert(nright->Keys.begin(), t->Keys.begin()+floor((n+1)/2), t->Keys.end());
                ((In_Node*)nright)->Children.insert(((In_Node*)nright)->Children.begin(), 
                    ((In_Node*)t)->Children.begin()+floor((n+1)/2+1), ((In_Node*)t)->Children.end());
                for(int i=floor((n+1)/2+1);i<((In_Node*)t)->Children.size();++i){
                    ((In_Node*)t)->Children[i]->Parent = nright;
                }
                t->Keys.erase(t->Keys.begin()+floor((n+1)/2), t->Keys.end());
                ((In_Node*)t)->Children.erase(((In_Node*)t)->Children.begin()+floor((n+1)/2+1), ((In_Node*)t)->Children.end());
                tnew = nright;
                key = tnew->Keys[0];
                tnew->Keys.erase(tnew->Keys.begin());
            }
            else{
                tnew->Parent = t;
                return;
            }
        }
        if(t->Parent==NULL){
            t->Parent = new In_Node();
            ((In_Node*)t->Parent)->Children.insert(((In_Node*)t->Parent)->Children.begin(), t);
            ((In_Node*)t->Parent)->Children.insert(((In_Node*)t->Parent)->Children.begin()+1,tnew);
            if(t->isLeaf){
                (t->Parent)->Keys.insert((t->Parent)->Keys.begin(),tnew->Keys.front());
            }
            else{
                (t->Parent)->Keys.insert((t->Parent)->Keys.begin(),((In_Node*)tnew)->Children.front()->Keys.front());
                
            }
            tnew->Parent=t->Parent;
            head = t->Parent;
        }
    }
    return;
    
}

void insert_test(BPlusTree & bp1){
    cout<<"---------Insert Test---------"<<endl;
    bp1.insert(2, "two");
    bp1.insert(21, "twenty-one");
    bp1.insert(11, "eleven");
    bp1.insert(8, "eight");
    bp1.insert(64, "sixty-four");
    bp1.insert(5, "five");
    bp1.insert(23, "twenty-three");
    bp1.insert(6, "six");
    bp1.insert(19, "nineteen");
    bp1.insert(9, "nine");
    bp1.insert(7, "seven");
    bp1.insert(97, "ninety-seven");
    bp1.insert(1, "one");
    bp1.insert(15, "fifteen");
    bp1.insert(3, "three");
    bp1.insert(33, "thirty-three");
    bp1.insert(44, "forty-four");
    bp1.insert(55, "fifty-five");
    bp1.insert(77, "seventy-seven");
    bp1.insert(88, "eighty-eight");
    bp1.insert(10, "ten");
    bp1.insert(12, "twelve");
    bp1.insert(14, "fourteen");
    bp1.insert(16, "sixteen");
    bp1.insert(18, "eighteen");
    bp1.insert(20, "twenty");
    bp1.insert(22, "twenty-two");
    bp1.insert(24, "twenty-four");
    bp1.insert(26, "twenty-six");
    bp1.printKeys();
}

void copy_test(BPlusTree & bp1){
    cout<<"-------- = operation Test--------"<<endl;
    BPlusTree bp2 = bp1;
    bp2.printKeys();
    cout<<"-------- copy constructor Test--------"<<endl;
    BPlusTree bp3 (bp1);
    bp3.printKeys();
}

void remove_test(BPlusTree & bp1){
    cout<<"-------- remove Test--------"<<endl;
    bp1.remove(24);
    bp1.remove(14);
    bp1.remove(8);
    bp1.remove(44);
    bp1.remove(55);
    bp1.remove(64);
    bp1.remove(77);
    bp1.remove(2);
    bp1.remove(5);
    bp1.remove(3);
    bp1.remove(9);
    bp1.remove(10);

    bp1.printKeys();
}

void value_print_test(BPlusTree & bp1){
    cout<<"-------- print value Test--------"<<endl;
    bp1.printValues();
}

int main() {
    BPlusTree bp1(3);
    // Insertions
    insert_test(bp1);

    copy_test(bp1);

    remove_test(bp1);

    value_print_test(bp1);

    BPlusTree bp4(4);

    cout<<"------Test for n=4 ------"<<endl;
    insert_test(bp4);

    return 0;
}