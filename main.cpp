#include <string>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <sstream>
#include <algorithm>
#include <iostream>
using namespace std;

#include "parser.h"

class Rule {
public:
  string leftSide;
  string rightSide;
  Rule(void) : leftSide(""), rightSide("") {}
  ~Rule(void) {}
  void input(string left, string right) {
    leftSide = left;
    rightSide = right;
  }
  void output() {
    cout << leftSide << " > " << rightSide << endl;
  }
  bool isEqual(Rule anotherRule) {
    if (this->leftSide == anotherRule.leftSide && this->rightSide == anotherRule.rightSide)
      return true;
    else
      return false;
  }
};

bool checkRuleExistInList(Rule r, vector<Rule> listRule) {
  for(auto & element : listRule) {
    if (r.isEqual(element))
      return true;
  }
  return false;
}

Rule convertStringToRule(string str) {
  // String: AB>E
  Rule r;
  string leftSide = str.substr(0, str.find(">"));
  string rightSide = str.substr(str.find(">") + 1);
  r.input(leftSide, rightSide);
  return r;
}

void convertStringToRule(string str, vector<Rule> &listRule) {
  // String: AB>E
  Rule r;

  size_t indexOrSymbol = str.find("|");
  if (indexOrSymbol != string::npos) { // symbol OR exists in string

    string leftSide = str.substr(0, str.find(">"));   // get the left side of string
    string rightSide = str.substr(str.find(">") + 1); // get the right side of string

    std::istringstream split(leftSide);              // split the string by character '|'
    std::vector<std::string> tokens;
    for (std::string each; std::getline(split, each, '|'); tokens.push_back(each));


    for(auto & element : tokens) {
      Rule newObject;
      newObject.input(element, rightSide);
      if (!checkRuleExistInList(newObject, listRule))
        listRule.push_back(newObject);
    }
  }
  else {
    string leftSide = str.substr(0, str.find(">"));
    string rightSide = str.substr(str.find(">") + 1);
    r.input(leftSide, rightSide);
    if (!checkRuleExistInList(r, listRule))
      listRule.push_back(r);
  }

}

void convertFileStringToObject(string &fact, string &hypos, string List[], int n, vector<Rule> &listRule) {
  string::iterator end_pos = remove(fact.begin(), fact.end(), ' ');
  fact.erase(end_pos, fact.end());
  end_pos = remove(hypos.begin(), hypos.end(), ' ');
  hypos.erase(end_pos, hypos.end());

 for(int i = 0; i < n-1; i++) {
   end_pos = remove(List[i].begin(), List[i].end(), ' ');
   List[i].erase(end_pos, List[i].end());          // remove all of space exsist in string rule

   end_pos = remove(List[i].begin(), List[i].end(), '&');
   List[i].erase(end_pos, List[i].end());         // remove all of & exsist in string rule

   convertStringToRule(List[i], listRule);

 }
}

void loadFileString(string path, string &fact, string &hypos, vector<string> &hyposNumber, string listRule[], vector<string> &listFormula, int &m) {
	ifstream f(path);
  m = 0;
	if(f.is_open()) {
    getline(f, fact);
    getline(f, hypos);
    string numberString;
    getline(f, numberString);
    int number;
    number = atoi(numberString.c_str());    
    for(int i = 0; i < number; i++) {
      getline(f, numberString);
      hyposNumber.push_back(numberString);
    }

		while (!f.eof()) {
			getline(f, listRule[m]);
			getline(f, numberString);
      listFormula.push_back(numberString);
			m++;
		}
		f.close();
	}
	else cout << "Can not open the file !";
}

bool checkElementInFact(string element, string fact) {
  for(int i = 0; i < element.length(); i++) {
    if (fact.find(element[i]) == string::npos) {
      return false;
    }
  }
  return true;
}

void handleForwardChaining(string fact, string hypos, vector<Rule> listRule, vector<string> hyposNumber, vector<string> listFormula, vector<string> &resultFormula) {
  Rule hyposRule = convertStringToRule(hypos);

  fact = hyposRule.leftSide;
  string goal = hyposRule.rightSide;

  vector<Rule> sequence;
   
  while (!checkElementInFact(goal, fact)) {
    vector <int> index;
    int flag = 0;
    for (int i = 0; i < listRule.size(); i++) {
      if (checkElementInFact(listRule[i].leftSide, fact)) {

        // checking elements of rightSide exists in fact or not. If exsist, we'll ignore it, Otherwise, we willl push it to fact.
        string rightSide = listRule[i].rightSide;
        bool store = false;
        for (int k = 0; k < rightSide.length(); k++) {
          if (fact.find(rightSide[k]) == string::npos) {
            fact += rightSide[k];
            store = true;
          }
        }
        if (store == true) {          
          sequence.push_back(listRule[i]);        
          resultFormula.push_back(listFormula[i]);
        }

        index.push_back(i);
        flag = 1;
        if(checkElementInFact(goal, fact) == true) {          
          cout << "Fact " << fact << endl;
          for(auto & element : sequence) {
            element.output();
          }
          return;
        }
      }
    }  
    if (flag == 1) {
      int distance = 0;
      for(auto & element : index) {
        listRule.erase(listRule.begin() + element - distance); 
        listFormula.erase(listFormula.begin() + element - distance); 
        distance++;
      }
    }
    else {
      cout << "Not found" << endl;
      return;
    }
  }

  cout << "Fact " << fact << endl;
  for(auto & element : sequence) {
    element.output();
  }
}

void checkListInFact(string &fact, string leftSide) {
  // check each element of leftSide already exsist in Fact or not. If not then adding into fact
  for(int i = 0; i < leftSide.length(); i++) {
    if (fact.find(leftSide[i]) == string::npos) { // character in list not exist in fact before
      fact += leftSide[i];
    }
  }
}

class SetFact {
public:
  string factNow;
  string parentString;
  vector<Rule> listParentRule;
  SetFact *parent;

  SetFact() : factNow(""), parent(NULL), parentString(""), listParentRule() {}
  SetFact(string stringNow) : factNow(stringNow), parent(NULL), parentString("") {}
  SetFact(string stringNow, SetFact *pare) : factNow(stringNow), parent(pare) {}
  void output() {
    cout << "Fact now: " << factNow << endl;
    cout << "Parent String: " << parentString << endl;
    for(auto & element : listParentRule) {
      element.output();
    }    
  }
};

bool checkRemoveElementInFact(string element, string &fact, vector<int> &indexRemove) {

  for(int i = 0; i < element.length(); i++) {
    auto indexFound = fact.find(element[i]);
    if ( indexFound == string::npos) {
      return false;
    }
    else {
      indexRemove.push_back(indexFound);
    }
  }

  return true;
}

string concatenateTwoString(string A, string B) {
  // concatenate string B into string A, condition is check the character of string B does not exists in string A
  string result = A;
  for(int i = 0; i < B.size(); i++) {
    if (A.find(B[i]) == string::npos)
      result += B[i];
  }
  return result;
}

vector<SetFact> generateSetOfFact(SetFact f, vector<Rule> listRule) {
  vector<SetFact> result;
  string factNow = f.factNow;
  vector<int> indexRemove;

  for (int i = 0; i < listRule.size(); i++) {
    if (checkRemoveElementInFact(listRule[i].rightSide, f.factNow, indexRemove)) {
      string removedStr = f.factNow;

      for(auto & element : indexRemove) {  
        // cout << "what " << endl;
        // cout << removedStr << endl;
        if (removedStr.length() < 1)
          removedStr = "";
        else
          removedStr.erase(removedStr.begin() + element);
      }

      string combine = concatenateTwoString(removedStr, listRule[i].leftSide);

      SetFact* X = new SetFact(combine, &f);

      X->parentString = f.factNow + " " + f.parentString;
      X->listParentRule = f.listParentRule;
      X->listParentRule.push_back(listRule[i]);

      result.push_back(*X);
      indexRemove.clear();
    }    
  }
  return result;
}

bool isEqualValueString(string A, string B) {  // compare string A == B, regardless of the position of two string's character.
  for(int i = 0; i < A.length(); i++) {
    if (B.find(A[i]) == string::npos) {
      return false;
    }
  }
  return true;
}

void handleBackwardChaining(string fact, string hypos, vector<Rule> listRule) {
  Rule hyposRule = convertStringToRule(hypos);
  string goal = hyposRule.leftSide;

  SetFact realFact;
  vector<SetFact> listRealFact;

  for(int i = 0; i < hyposRule.rightSide.length(); i++) {

    fact = hyposRule.rightSide[i];
    vector<SetFact> sequence;
    int flag = 0;
    SetFact* newFact = new SetFact(fact);
    vector<SetFact> generatedSet = generateSetOfFact(*newFact, listRule);
    sequence.insert(sequence.end(), generatedSet.begin(), generatedSet.end());

    // for(auto & element : sequence) {
    //   element.output();
    // }    

    while (sequence.size() != 0 && flag == 0) {

      SetFact hyposObj;
      hyposObj = sequence.front();
      realFact = hyposObj;
      sequence.erase(sequence.begin());
      if (isEqualValueString(hyposObj.factNow, goal)) {
        flag = 1;        
      }
      else {
        generatedSet = generateSetOfFact(hyposObj, listRule);
        sequence.insert(sequence.end(), generatedSet.begin(), generatedSet.end());        
      }

    }    
    listRealFact.push_back(realFact);

  }

  for(auto & element : listRealFact) {
    element.output();
  }    

}

string baoDong(string leftSide, int index, vector<Rule> listRule) {  // index: index of element in listRule, we'll ignore it.
  string result = leftSide;
  for(int i = 0; i < listRule.size(); i++) {
    if (i != index) {  // ignore the index element
      if (checkElementInFact(listRule[i].leftSide, result))
        result += listRule[i].rightSide;
    }
  }
  return result;
}

void handleOptimalListRule(vector<Rule> &listRule) {

  int i = 0;
  while (i < listRule.size()) {
    if (listRule[i].rightSide.empty()) {
      listRule.erase(listRule.begin() + i);
    }
    else {
      string leftSide = listRule[i].leftSide;
      for(int j = 0; j < leftSide.length(); j++) {
        size_t found = listRule[i].rightSide.find(leftSide[j]);
        if (found != string::npos) {  // if leftSide character exists in rightSide, then remove
          listRule[i].rightSide.erase(listRule[i].rightSide.begin() + found);
        }
      }
      i++;
    }
  }

  i = 0;       // step 3: remove the redunt rule
  while (i < listRule.size()) {
    string tapBaoDong;
    tapBaoDong = baoDong(listRule[i].leftSide, i, listRule);

    if (checkElementInFact(listRule[i].rightSide, tapBaoDong)) {
      listRule.erase(listRule.begin() + i);
    }
    else {
      i++;
    }
  }

  // step 4: remove the redunt fact in left side
  for(int i = 0; i < listRule.size(); i++) {
    string leftSide;
    leftSide = listRule[i].leftSide;
    if (leftSide.length() > 1) {
      int j = 0;
      while (j < leftSide.length()) {
        string temp;
        string removedCharacter;
        temp = leftSide;
        removedCharacter = temp[j];
        temp.erase(temp.begin() + j);  // remove the current character, we do not need to use BaoDong
        string tapBaoDong;
        tapBaoDong = baoDong(temp, i, listRule);
        if (tapBaoDong.find(removedCharacter) != string::npos) { // find out the character in tapBaoDong
          leftSide.erase(leftSide.begin() + j);
        }
        else {
          j++;
        }
      }
      listRule[i].leftSide = leftSide;
    }
  }

}

int main() {
  string fact, hypos, List[100];

  vector<string> hyposNumber, listFormula;
  vector<Rule> listRule;
  int n;
  Parser prs;
  loadFileString("./test.txt", fact, hypos, hyposNumber, List, listFormula, n);

  // for(int i = 0; i < n; i++) {
  //   cout << List[i] << endl;
  //   cout << listFormula[i] << endl;
  // } 

  // cout << "------------------------" << endl;

  
  convertFileStringToObject(fact, hypos, List, n, listRule);

  // cout << "Optimize listRule" << endl;
  // handleOptimalListRule(listRule);

  // for(auto & element : listRule) {
  //   element.output();
  // } 
  
  cout << "solving" << endl;

  vector<string> indexResult;
  cout << "Forward chaining" << endl;
  handleForwardChaining(fact, hypos, listRule, hyposNumber, listFormula, indexResult);

  // cout << "Backward chaining" << endl;
  // handleBackwardChaining(fact, hypos, listRule);

  char* result;
  for(auto & element : hyposNumber) { // input hyposNumber into Parser
    result = prs.parse(element.c_str());
  }

  for(auto & element : indexResult) {
    cout << element << endl;
    result = prs.parse(element.c_str());
    printf("\t%s\n", result);
  }
  getch();

  return 0;
}


// void saveFileString(string path, string str[], int n) {
// 	ofstream f(path);
// 	if(f.is_open())  {
// 		for(int i = 0; i < n; i++)
// 			f << str[i] << "\n";	
// 		cout << "Save Done" << endl;
// 	}
// 	else cout << "Can not save the file" << endl;
// 	f.close();
// }
