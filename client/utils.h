#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <tr1/unordered_map>

using namespace std;

typedef tr1::unordered_map<string, string> stringMap;
bool ishex(char c);
void outhex(int fdes, char c);
char hextoc(char c1, char c2);
bool startsWith(const string& word, const string& Expr);
bool endsWith(const string& word, const string& Expr);
bool isBlank(const string& line, const string& WhiteSpace = " \t"); 
string chopWord(string&, const char* WhiteSpace = " \t");
string chopLine(string&);
string grabWord(string&, int whichWord = 1, const string& WhiteSpace = " \t");
string grabLastWord(string& , const string& WhiteSpace = " \t");
string chopWordReverse(string&, const string& WhiteSpace = " \t");
void removeMatch(string&, const string& Expr);
void removeSpaces(string&, const string& WhiteSpace = " \t\n");
bool containsWord(const string& word, const string& Expr);
bool startsWithWord(const string& word, const string& Expr);
string shell(const string& command);
string shell(const char* command);
string chopValue(const string&, string&);
string chopValue(const string&);
void replaceAll(const char*, const char*, string&);
void buildMap(const string&, const string&, stringMap&);
void toLower(string&);
void getParamValue(string&, string&, string&);          
void createPortMap(stringMap &, string, string);
void findArg(char* argv[], char* arg, int argc, string&);
void findArg(char* argv[], char* arg, int argc, int&);
bool isArg(char* argv[], char* arg, int argc);
int sendmail(const char *to, const char *from, const char *subject, const char *message);
float SItoFloat(const string&);
char* FloatToSI(float);
float evalString(const string&);

#endif

