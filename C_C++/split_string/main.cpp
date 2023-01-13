#include <sstream>
#include <iostream>

using namespace std;

int main() {
  string str = "Original string is this.";
  stringstream ss(str);
  string tmp;
  while (ss >> tmp) {
    cout << tmp << endl;
  }
  return 0;
}
