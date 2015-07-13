#ifndef XML_H_
#define XML_H_
#include <string>
#include <tr1/unordered_map>
using namespace std;

class node;
typedef tr1::unordered_map<string, node*> nodeMap;

class node{
	public:
		node(const string&);
		~node();
		node* child(const string&);
		node* createChild(const string&);
		node* getParent();
		void setParent(node*);
		void setValue(const string&);
		string value();
		string name();
	private:
		nodeMap childNodes;
		node *parent;
		string theName;
		string theValue;
};

class xml{
	public:
		~xml();
		void load(string&);
		node* child(const string&);
	private:
		nodeMap topLevelNodes;
};

#endif
