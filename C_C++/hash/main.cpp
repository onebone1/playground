#include <iostream>

using namespace std;

int main() {
  string s = "some strings";
  cout << s << endl;
  cout << hash<string>{}(s) << endl;
  return 0;
}
