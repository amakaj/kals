// Kathy Tran, Anthony Makaj
// function implementations
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <vector>
#include <stack>

#include "main.h"

using namespace std;

DirectoryListing::DirectoryListing(bool opta, bool optf, bool optd, bool optl, bool opth)
{
    a = false;
    f = false;
    d = false;
    l = false;
    h = false;
    printedAlready = false;
}

// set flags
// printedAlready will only allow -h to print once so it does not continuously
// EX: kals -h -la Music
// usage message will print and then the other flags will still work on the directory

// remove the path from the inputStr by traversing through string in reverse order
// and pushing all chars into a str before it encounters the first '/'
// we use a stack to reverse the output string before its returned
string DirectoryListing::removePath(string inputStr)
{
    stack<char> s;
    for (int i = inputStr.length() - 1; inputStr[i] != '/'; i--)
    {
        s.push(inputStr[i]);
    }

    int stackSize = s.size();
    string outputStr = "";

    for (int j = 0; j < stackSize; j++)
    {
        outputStr += s.top();
        s.pop();
    }
    return outputStr;
}

// print a line showing the syntax of the program and then a list of arguments and how they modify the function of the program
void DirectoryListing::printUsage()
{
    cout << "Flags:\n";
    cout << "-l: directory long listing format\n";
    cout << "-d: exclude directory contents\n";
    cout << "-f: recognize links/shortcuts\n";
    cout << "-a: include above directories\n";
    cout << "-h: flags guide\n";
}

// -l long directory listing
void DirectoryListing::longDetails(char *name)
{
    struct stat info;
    lstat(name, &info);
    int mode = info.st_mode;

    // list type + permissions of directory/file
    // predicate macros test the type of a file

    // layout: directory, pipe, regular file, link, special block file, character special file, and socket
    S_ISDIR(mode) ? cout << "d" : (S_ISFIFO(mode) ? cout << "f" : (S_ISREG(mode) ? cout << "-" : (S_ISLNK(mode) ? cout << "l" : (S_ISBLK(mode) ? cout << "b" : (S_ISCHR(mode) ? cout << "c" : (S_ISSOCK(mode) ? cout << "s" : cout << ""))))));

    // masks that identify the read-write-execute bits for user
    mode &S_IRUSR ? cout << "r" : cout << "-";
    mode &S_IWUSR ? cout << "w" : cout << "-";
    mode &S_IXUSR ? cout << "x" : cout << "-";

    // masks that identify the read-write-execute bits for group
    mode &S_IRGRP ? cout << "r" : cout << "-";
    mode &S_IWGRP ? cout << "w" : cout << "-";
    mode &S_IXGRP ? cout << "x" : cout << "-";

    // masks that identify the read-write-execute bits for other
    mode &S_IROTH ? cout << "r" : cout << "-";
    mode &S_IWOTH ? cout << "w" : cout << "-";
    mode &S_IXOTH ? cout << "x" : cout << "-";

    // ownership of the entry should be displayed as user name and group name
    // get user id and group id, convert to user name and group name
    struct passwd *userName = getpwuid(info.st_uid);
    struct group *groupName = getgrgid(info.st_gid);

    // timestamps for last modified
    struct tm *ptr = localtime(&info.st_mtime);
    char str[20];
    strftime(str, sizeof(str), "%b %e %R", ptr);

    // permission link user group size name (time)
    cout << " " << info.st_nlink << " ";

    if (userName != NULL)
        cout << userName->pw_name << " ";
    if (groupName != NULL)
        cout << groupName->gr_name << " ";
    if (S_ISLNK(mode)) // if link
    {
        cout << info.st_size << " " << str << " " << name;
    }
    else // if directory or file
    {
        string nameStr = name;
        if (nameStr != ".")
        {
            cout << info.st_size << " " << str << " " << removePath(nameStr) << "\n";
        }
        else
        {
            cout << info.st_size << " " << str << " " << nameStr << "\n";
        }
    }
}

void DirectoryListing::listDirRec(char *name, string flags)
{
    DIR *directory;
    vector<string> dirName;
    struct dirent *de;
    string curr = ".";
    string dn;
    string printDir = name;

    // set flags
    // int cnt = 0;
    string opt = flags;
    for (int i = 1; i < flags.length(); i++)
    {
        if (opt[i] == 'a')
            a = true;
        if (opt[i] == 'f')
            f = true;
        if (opt[i] == 'd')
            d = true;
        if (opt[i] == 'l')
            l = true;
        if (opt[i] == 'h')
            h = true;
    }

    // h is true, print out usage message
    if (h && !a && !d && !f && !l)
    {
        if (!printedAlready)
        {
            printUsage();
            printedAlready = true;
        }
        h = false;
        return;
    }

    // h is true but may be paired with some command
    if (h)
    {
        if (!printedAlready)
        {
            printUsage();
            printedAlready = true;
        }
        h = false;
    }

    // d is true, print out directory
    if (d)
    {
        // paired with -l long directory listing
        if (d && f)
        {
            cout << "Cannot list directory and follow symlink."
                 << "\n";
            f = false;
        }
        else if (d && l)
        {
            // if we are in current working directory, print out '.'
            // if l is true, print out the long details of the directory
            if (name == NULL)
            {
                string pwd = ".";
                vector<char> tmp(pwd.begin(), pwd.end());
                tmp.push_back('\0');
                char *dir = &tmp[0];
                // print long details of directory name
                longDetails(dir);
            }
            else
            {
                longDetails(name);
            }
            l = false;
        }
        else
        {
            // print directory name without long details
            name == NULL ? cout << "."
                                << "\n"
                         : cout << name << " ";
        }
        d = false;
        return;
    }

    // open directory
    if (name == NULL)
    {
        vector<char> p(curr.begin(), curr.end());
        p.push_back('\0');
        name = &p[0];

        // current working directory
        directory = opendir(".");
        dn = name;
    }
    else
    {
        // open directory with given name
        directory = opendir(name);
        dn = name;
    }

    // handle bad/invalid directory
    if (directory == NULL)
    {
        cout << "No such file or directory called " << name << "\n";
        return;
    }

    // print out directory name unless it is cwd
    cout << printDir << ":"
         << "\n";

    while ((de = readdir(directory)) != NULL)
    {
        // entry is a directory
        if (de->d_type == DT_DIR)
        {
            // first character of entry name
            // will let us know if '.' is present
            char n = de->d_name[0];
            // DO NOT TRAVERSE '.' or '..' when they come up in the list to avoid infinite recursion
            if (!a && n == '.')
            {
                continue;
            }
            else if (a && n == '.')
            {
                if (l)
                {
                    string pathdir = dn + "/" + de->d_name;
                    vector<char> tmp(pathdir.begin(), pathdir.end());
                    tmp.push_back('\0');
                    char *pn = &tmp[0];
                    longDetails(pn);
                }
                else
                {
                    cout << de->d_name << " ";
                }
            }
            else
            {
                // form path and print contents
                string next = dn + "/" + de->d_name;
                if (l)
                {
                    string pathdir = dn + "/" + de->d_name;
                    vector<char> tmp(pathdir.begin(), pathdir.end());
                    tmp.push_back('\0');
                    char *pn = &tmp[0];
                    longDetails(pn);
                }
                else
                {
                    cout << de->d_name << " ";
                }
                // add path to vector of directory names
                dirName.push_back(next);
            }
        }
        else if (de->d_type == DT_REG)
        {
            char n = de->d_name[0];
            // entry is file
            if (!a && n == '.')
            {
                continue;
            }
            else if (a && n == '.')
            {
                // if a is true and this is a dot file, print out its name and if l is also true, print out its long details
                if (l)
                {
                    string pathdir = dn + "/" + de->d_name;
                    vector<char> tmp(pathdir.begin(), pathdir.end());
                    tmp.push_back('\0');
                    char *pn = &tmp[0];
                    longDetails(pn);
                }
                else
                {
                    cout << de->d_name << " ";
                }
            }
            else
            {
                // just print out file name and if l is true, print out its long details
                if (l)
                {
                    string pathdir = dn + "/" + de->d_name;
                    vector<char> tmp(pathdir.begin(), pathdir.end());
                    tmp.push_back('\0');
                    char *pn = &tmp[0];
                    longDetails(pn);
                }
                else
                {
                    cout << de->d_name << " ";
                }
            }
        }
        else if (de->d_type == DT_LNK)
        {
            // entry is symbolic link
            char n = de->d_name[0];
            // entry is file
            if (!a && n == '.')
            {
                continue;
            }
            else if (f)
            {
                if (l)
                {
                    longDetails(de->d_name);
                }
                else
                {
                    cout << de->d_name;
                }
                cout << " -> " << realpath(de->d_name, NULL) << "\n";
            }
        }
    }

    cout << "\n\n";

    // using similar idea to the approach adrianeddy (link above function header)
    // subdirectory string pushed to vector and traversed after base has been traversed
    while (!dirName.empty())
    {
        string dn = dirName.front();
        vector<char> chars(dn.begin(), dn.end());
        chars.push_back('\0');
        char *p = &chars[0];

        // recursion of directories
        listDirRec(p, opt);

        // remove directory name after traversal
        dirName.erase(dirName.begin());
    }

    // close directory
    closedir(directory);

    // reset flags
    a = false;
    f = false;
    d = false;
    l = false;
    h = false;
}
