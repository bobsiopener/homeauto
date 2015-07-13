#include "xml.h"
#include "../utils.h"

node::node(const string &createMe) { theName = createMe; }
node::~node() {
  node *toRemove;
  nodeMap::iterator thisNode;
  for (thisNode = childNodes.begin(); thisNode != childNodes.end();
       thisNode++) {
    toRemove = thisNode->second;
    delete toRemove;
  }
  childNodes.clear();
}
node *node::child(const string &tagName) {
  if (childNodes.find(tagName) != childNodes.end()) {
    return (childNodes[tagName]);
  } else {
    return (NULL);
  }
}
node *node::createChild(const string &entry) {
  node *addMe;
  addMe = new node(entry);
  childNodes[entry] = addMe;
  return (addMe);
}
void node::setValue(const string &newValue) { theValue = newValue; }
string node::value() { return (theValue); }
string node::name() { return (theName); }
node *node::getParent() { return (parent); }
void node::setParent(node *parentToBe) { parent = parentToBe; }
xml::~xml() {
  node *toRemove;
  nodeMap::iterator thisNode;
  for (thisNode = topLevelNodes.begin(); thisNode != topLevelNodes.end();
       thisNode++) {
    toRemove = thisNode->second;
    delete toRemove;
  }
  topLevelNodes.clear();
}
void xml::load(string &document) {
  string word;
  node *currentLevel, *topLevel, *nextLevel;
  // remove any precedding white space
  removeMatch(document, " \t\n");
  word = chopWord(document, ">");
  removeMatch(word, "<");
  topLevel = new node(word);
  topLevelNodes[word] = topLevel;
  currentLevel = topLevel;
  while (document != "") {
    removeMatch(document, " \t\n");
    if (startsWith(document, "<")) {
      // This is a tag of open or close
      word = chopWord(document, ">");
      removeMatch(word, "<");
      if (startsWith(word, "/")) {
        // if it started with </ then move up a lvel
        // we should confirm that we are closing the correct level
        removeMatch(word, "/");
        if (currentLevel->name() != word) {
          fprintf(stderr, "Error: Trying to close %s, found %s\n",
                  (currentLevel->name()).c_str(), word.c_str());
          return;
        }
        // if we just close the top level then we can leave
        if (currentLevel == topLevel)
          return;
        // close this level and move back up a level
        currentLevel = currentLevel->getParent();
      } else {
        // if it starts with < and no / then create a new child
        nextLevel = currentLevel->createChild(word);
        nextLevel->setParent(currentLevel);
        currentLevel = nextLevel;
      }
    } else {
      // if none of these then it is the value
      // grab the line up to the next <
      word = chopWord(document, "<");
      // chopWord will remove the "<" from the begining of document, so lets put
      // it back
      document.insert(0, "<");
      removeSpaces(word);
      currentLevel->setValue(word);
    }
  }
}
node *xml::child(const string &tagName) {
  if (topLevelNodes.find(tagName) != topLevelNodes.end()) {
    return (topLevelNodes[tagName]);
  } else {
    return (NULL);
  }
}
