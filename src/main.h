// class definition header file
#include <string>

using namespace std;

class DirectoryListing
{
public:
    DirectoryListing(bool opta, bool optf, bool optd, bool optl, bool opth);
    bool a, f, d, l, h, printedAlready;
    void printUsage();                         // option h
    void longDetails(char *name);              // details of option l
    void listDirRec(char *name, string flags); // performs ls -R (recursion) by default when there are no args
    string removePath(string inputStr);        // removes path from input string
};
