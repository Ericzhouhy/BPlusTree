#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

class Node
{
public:
    vector<int> Keys;
    Node* Parent;
    bool isLeaf;
    virtual ~Node() {}
};

class LeafNode: public Node
{
public:
    Node* Next;
    vector<string> Value;
    LeafNode(){isLeaf=true; Next=NULL; Parent=NULL;}
    ~LeafNode() {}
};

class In_Node:public Node
{
public:
    vector<Node*> Children;
    In_Node(){isLeaf=false; Parent=NULL; }
    ~In_Node() {
        for (Node* child : Children) {
            delete child;
        }
    }
};

class BPlusTree
{
private:
    Node* head;
    int n;
    Node* insertion(int, string);
    Node* deletion(int);
    void redistribution(Node* t);
    Node* copyNode(Node*, Node*);
public:
    //default constructor
    BPlusTree();
    // this constructor creates an empty tree; the parameter sets the number of keys that can be stored in each node (leaf and interior)
    BPlusTree(int);
    // inserts the key (int) and value (string) into the tree unless the key is in the tree; returns true if the insertion was successful and false otherwise
    bool insert(int, string);
    // searches the tree for the key (int); if found removes the key and associated value from the tree; returns true if the key was found and removed and false otherwise
    bool remove(int);
    // searches the tree for the key (int); if found returns the matching value (string), otherwise returns the empty string
    string find(int);
    // prints the keys in each node with nodes of the same level appearing on the same line
    void printKeys();
    // prints the values in key (not value) order
    void printValues();
    // destructor
    ~BPlusTree();
    //copy constructor
    BPlusTree(const BPlusTree& bpt);
	BPlusTree& operator=(const BPlusTree &bpt);
};