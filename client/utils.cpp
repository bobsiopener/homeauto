#include <cstring>
#include <cstdio>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"

bool startsWith(const string& word, const string& Expr) {
   return(word.find_first_of(Expr) == 0);
}
bool endsWith(const string& word, const string& Expr) {
   return((word.find_last_of(Expr) + 1) == word.length());
}
bool isBlank(const string& line, const string& WhiteSpace){
   return(line.find_first_not_of(WhiteSpace) == string::npos);
}
void replaceAll(const char* find, const char* replace, string& source) {
   size_t location;
   location = source.find(find);
   while(location != string::npos) {
      source.replace(location,strlen(find),replace);
      location = source.find(find);
   }
}
void createPortMap(stringMap& portMap, string subcktPorts, string instancePorts){
   while(subcktPorts != "" || instancePorts != "")
   {
   portMap[chopWord(subcktPorts)]=chopWord(instancePorts);
   }
}
string chopWord(string& Line, const char* WhiteSpace) {
   string word;
   size_t spaceLoc,endofLine,nextWord,firstLetter;
   endofLine = Line.length();
   firstLetter = Line.find_first_not_of(WhiteSpace);
   spaceLoc = Line.find_first_of(WhiteSpace, firstLetter);
   if(spaceLoc != string::npos) {
      word = Line.substr(firstLetter,spaceLoc-firstLetter);
      Line = Line.substr(spaceLoc,endofLine);
      nextWord = Line.find_first_not_of(WhiteSpace);
      if(nextWord != string::npos) {
         endofLine = Line.length();
         Line = Line.substr(nextWord,endofLine);
      } else {
         //when nextWord == npos then the line is just spaces
         Line = "";
      }
   } else {
	// no whitespace after the word.. so the line is empty when we're done.
	// just need to get rid of any leading whitespace..
	if (firstLetter == string::npos) {
		// no firstLetter either?  That means the string empty..
		word = "";
	} else {
      		word = Line.substr(firstLetter,endofLine - firstLetter);
	}
      Line = "";
   }
   return(word);
}
string chopLine(string& Line) {
   return(chopWord(Line,"\n"));
}
string grabWord(string& Line, int whichWord, const string& WhiteSpace) {
   string word="";
   size_t spaceLoc=0,firstLetter=0;
   for(int i=0; i<whichWord; i++) {
      firstLetter = Line.find_first_not_of(WhiteSpace, spaceLoc);
      spaceLoc = Line.find_first_of(WhiteSpace, firstLetter);
      if(spaceLoc != string::npos) {
         word = Line.substr(firstLetter,spaceLoc - firstLetter);
      } else {
         word = Line;
      }
   }
   return(word);
}
string grabLastWord(string& Line, const string& WhiteSpace) {
   string word;
   int spaceLoc,endofWord;
   spaceLoc = Line.find_last_of(WhiteSpace);
   endofWord = Line.find_last_not_of(WhiteSpace);
   while(spaceLoc > endofWord) {
      Line = Line.substr(0,endofWord+1);
      spaceLoc = Line.find_last_of(WhiteSpace);
   }
   word = Line.substr(spaceLoc+1,endofWord + 1);
   return(word);
}
string chopWordReverse(string& Line, const string& WhiteSpace) {
   string word;
   int spaceLoc,endofWord;
   spaceLoc = Line.find_last_of(WhiteSpace);
   endofWord = Line.find_last_not_of(WhiteSpace);
   while(spaceLoc > endofWord) {
      Line = Line.substr(0,endofWord+1);
      spaceLoc = Line.find_last_of(WhiteSpace);
   }
   word = Line.substr(spaceLoc+1,endofWord + 1);
   Line = Line.substr(0,spaceLoc);
   return(word);
}
float evalString(const string& Line) {
//need to add in support for ( and )
//we'll assume it goet number expr number expr ...
//and we'll add error checking in later
   string internal;
   internal = Line;
   removeMatch(internal,"()");
   float result,number;
   char character;
   string word;
   word = chopWord(internal," \t()");
   result = atof(word.c_str());
   while(internal != "") {
      word = chopWord(internal," \t()");
      character=word[0];
      word = chopWord(internal," \t()");
      number = atof(word.c_str());
   switch(character) {
      case '+': result=result + number; break;
      case '-': result=result - number; break;
      case '*': result=result * number; break;
      case '/': result=result / number; break;
   }
   }
   return(result);
}
void removeSpaces(string& Line, const string& WhiteSpace) {
   removeMatch(Line,WhiteSpace);
}
void removeMatch(string& Line, const string& Expr) {
   size_t beginStr = Line.find_first_not_of(Expr);
   if(beginStr != string::npos)
   {
   size_t endStr = Line.find_last_not_of(Expr);
   Line = Line.substr(beginStr,endStr - beginStr + 1);
   }
}
string chopValue(const string& Line, string& key) {
   string word;
   size_t equalLoc,endofWord, beginWord;
   endofWord = Line.length();
   equalLoc = Line.find_first_of("=");
   if(equalLoc == string::npos) {
      //No equal sign, just return a ""
      key = Line;
      return("");
   } else {
      beginWord = Line.find_first_not_of("= \t",equalLoc);
      if(beginWord == string::npos) beginWord = equalLoc;   
      endofWord = Line.find_first_of("= \t\n", beginWord);
      if(endofWord == string::npos) endofWord = Line.length();
      key = Line.substr(0,equalLoc);
      word = Line.substr(beginWord,endofWord - beginWord + 1);
      return(word);
   }
}
void getParamValue(string& Line, string& key, string& value) {
   size_t equalLoc,endofValue, beginValue, beginKey, nextKey;
   equalLoc = Line.find_first_of("=");
   beginKey = Line.find_first_not_of(" \t");
   if(equalLoc == string::npos) {
      //No equal sign, just return a ""
      key = Line;
      value = "";
      Line = "";
   } else {
      //get rid of whitespace after equal.
      beginValue = Line.find_first_not_of("= \t",equalLoc);      
      if(beginValue == string::npos) beginValue = equalLoc; 
      //find the next key=value pair then go back from there  
      nextKey = Line.find_first_of("=\n", beginValue);
      if(nextKey == string::npos) {
         //If there are no more keys then return the rest of string as the value
         endofValue = Line.length();
         nextKey = endofValue;
      } else {
         //find the last space between the value and the next key
         endofValue = Line.find_last_of(" \t", nextKey);
         nextKey = endofValue;
         //Chop off all the whitespace at the end of the value
         endofValue = Line.find_last_not_of(" \t", endofValue);   
      }
      key = Line.substr(beginKey,equalLoc - beginKey);
      value = Line.substr(beginValue,endofValue - beginValue + 1);
      Line = Line.substr(nextKey);
   }   
}
string chopValue(const string& Line) {
   string word;
   size_t equalLoc,endofWord, beginWord;
   equalLoc = Line.find_first_of("=");
   if(equalLoc == string::npos) {
      //No equal sign, just return a ""
      return("");
   } else {
      beginWord = Line.find_first_not_of("= \t",equalLoc);
      if(beginWord == string::npos) beginWord = equalLoc;   
      endofWord = Line.find_first_of("= \t\n", beginWord);
      if(endofWord == string::npos) endofWord = Line.length(); 
      word = Line.substr(beginWord,endofWord - beginWord + 1);
      return(word);
   }
}
bool containsWord(const string& line, const string& word) {
   size_t position; 
   position =   line.find(word);
   if(position == string::npos) {
      return(false);
   } else {
      if((position == 0 || line[position-1]==' ' || line[position-1]=='\n') && (line[position + word.length()]==' ' || line[position + word.length()]=='\n' || line.length() == position + word.length())) 
      {
         return(true);
      } else { 
         return(false);
      }
   }
}
bool startsWithWord(const string& line, const string& word) {
   size_t position; 
   position =   line.find(word);
   if(position == string::npos) {
      return(false);
   } else {
      if((position == 0) && (line[position + word.length()]==' ' || line.length() == position + word.length())) 
      {
         return(true);
      } else { 
         return(false);
      }
   }
}
void toLower(string& word){
   transform(word.begin(), word.end(), word.begin(), ::tolower);
}
float SItoFloat(const string& word) {
   float value;
   char unit;
   unit = tolower(word[word.length() - 1]);
   value = atof(word.c_str());
   switch(unit) {
      case 'd': value=value * 1e-1; break;
      case 'c': value=value * 1e-2; break;
      case 'm': value=value * 1e-3; break;
      case 'u': value=value * 1e-6; break;
      case 'n': value=value * 1e-9; break;
      case 'p': value=value * 1e-12; break;
      case 'f': value=value * 1e-15; break;
      case 'a': value=value * 1e-18; break;
   }
   return(value);  
}
char* FloatToSI(float number) {
   char *result;
   size_t eLoc;
   string resultStr, prenum, postfix;
   int exp;
   float num;
   result = new char[32];
   sprintf(result, "%e", number);
   resultStr = result;   
   eLoc = resultStr.find_first_of("e");
   if(eLoc != string::npos) {
      prenum = resultStr.substr(0,eLoc);
      num = atof(prenum.c_str());
      postfix = resultStr.substr(eLoc+1);
      exp = atoi(postfix.c_str());
      bool inUnit=false;
      while(!inUnit){
         switch(exp) {
            case -1:  sprintf(result, "%.4fd", num); inUnit= true; break;
            case -2:  sprintf(result, "%.4fc", num); inUnit= true; break;
            case -3:  sprintf(result, "%.4fm", num); inUnit= true; break;
            case -6:  sprintf(result, "%.4fu", num); inUnit= true; break;
            case -9:  sprintf(result, "%.4fn", num); inUnit= true; break;
            case -12: sprintf(result, "%.4fp", num); inUnit= true; break;
            case -15: sprintf(result, "%.4ff", num); inUnit= true; break;
            case -18: sprintf(result, "%.4fa", num); inUnit= true; break;
            default: if(exp < -18 ) {
               exp += 1;
               num = num / 10;
            } else {
               exp = exp - 1;
               num = num * 10;
            } 
         }//end switch
      }//end while
   }   
   return(result); 
}
string shell(const string& command) {  
   string returnVal; 
   FILE *p = popen(command.c_str(), "r");
   char results[100];
   if(p != NULL) { 
      while(fgets(results, sizeof(results),p) != NULL) {
         returnVal.append(results);
      }
   }
   return(returnVal);
}
string shell(const char* command) {  
   string returnVal; 
   FILE *p = popen(command, "r");
   char results[100];
   if(p != NULL) { 
      while(fgets(results, sizeof(results),p) != NULL) {
         returnVal.append(results);
      }
   }
   return(returnVal);
}
void buildMap(const string& origNets, const string& newNets, stringMap& netMap) {
  string key, value;
  while(origNets != "" && newNets != "")
    {
      key = chopValue(origNets);
      value = chopValue(newNets);
      netMap[key] = value;
    }
}
void findArg(char* argv[], char* arg, int argc, string& results) {
   for(int i=0; i<argc; i++)
   {
      if(strcmp(arg, argv[i]) == 0 && i+1<argc)
      {
         results = argv[i+1];
      }
   }
}
void findArg(char* argv[], char* arg, int argc, int& result) {
   
   for(int i=0; i<argc; i++)
   {
      if(strcmp(arg, argv[i]) == 0 && i+1<argc)
      {
         result = atoi(argv[i+1]);
      }
   }
}
bool isArg(char* argv[], char* arg, int argc) {
   for(int i=0; i<argc; i++)
   {
      if(strcmp(arg, argv[i]) == 0)
      {
         return(true);
      }
   }
   return(false);
}
int sendmail(const char *to, const char *from, const char *subject, const char *message) {
   FILE *mailpipe = popen("/usr/lib/sendmail -t", "w");
   if(mailpipe != NULL)
   {
      fprintf(mailpipe, "To: %s\n", to);
      fprintf(mailpipe, "From: %s\n", from);
      fprintf(mailpipe, "Subject: %s\n\n", subject);
      fwrite(message, 1, strlen(message), mailpipe);
      fwrite(".\n",1,2, mailpipe);
      pclose(mailpipe);
      return(0);
   }
   return(-1);
}
bool ishex(char c) {
	char uppers;
	uppers = toupper(c);
	if (((uppers >= '0') && (uppers <= '9')) || ((uppers >= 'A') && (uppers <= 'F')))
		return true;
	else
		return false;
}
void outhex(int fdes, char c) {
	unsigned char uppers;
	char tc;
	uppers = *(unsigned char *)(&c);
	tc = ((uppers / 16) % 16);
	tc = ((tc >= 0) && (tc <= 9)) ? (tc + '0') : (tc - 10 + 'A');
	write(fdes,&tc,1);
	tc = (uppers % 16);
	tc = ((tc >= 0) && (tc <= 9)) ? (tc + '0') : (tc - 10 + 'A');
	write(fdes,&tc,1);
}
char hextoc(char c1, char c2) {
	unsigned char uppers;
	c1 = toupper(c1);
	c2 = toupper(c2);
	if ((c1 >= '0') && (c1 <= '9'))
		uppers = c1 - '0';
	else
		uppers = c1 - 'A' + 10;
	uppers *= 16;
	if ((c2 >= '0') && (c2 <= '9'))
		uppers += c2 - '0';
	else
		uppers += c2 - 'A' + 10;
	return (uppers);
}
