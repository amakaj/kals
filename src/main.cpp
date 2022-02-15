// Kathy Tran, Anthony Makaj
// KALS main function
#include <iostream>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <vector>

#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> args, dirName, flag;
    char *commands[argc];
    string dirStr, flagStr;
    bool opta = false, optf = false, optd = false, optl = false, opth = false;
    int option, nonOpt, index;

    // getopt reorders the arguments, so keep copy of original order of commands
    for (int i = 0; i <= argc; i++)
    {
        commands[i] = argv[i];
    }

    while ((option = getopt(argc, argv, "afdlh")) != -1)
    {
        switch (option)
        {
        case 'a':
            opta = true;
            break;
        case 'f':
            optf = true;
            break;
        case 'd':
            optd = true;
            break;
        case 'l':
            optl = true;
            break;
        case 'h':
            opth = true;
            break;
        case '?':
            // unknown option
            return -1;
            break;
        default:
            break;
        }
    }

    // nonOpt keeps track of how many non options there are
    // this will be number of directories
    nonOpt = 0;
    // start at 1 because argv[0]/commands[0] is program
    // we check if its just the command by itself down below
    for (int i = 1; i < argc; i++)
    {
        // if the first char of argument isn't - it is not a flag
        if (commands[i][0] != '-')
        {
            nonOpt++;
            // add directory names to string
            dirStr = dirStr + commands[i] + " ";

            // if flag string isn't empty, push it to the vector
            if (flagStr != "")
            {
                flag.push_back(flagStr);
                flagStr = "";
            }

            // last argument; if name isn't empty, push it to the vector
            if (i + 1 == argc)
            {
                if (dirStr != "")
                {
                    dirName.push_back(dirStr);
                    dirStr = "";
                }
            }
        }
        else
        {
            // if first char of arg is '-' it is a flag
            // when we encounter a new '-' we only apply the previous flag to the directory before this arg
            // this handles multiple arglists
            // add flag to flag string
            flagStr += commands[i];

            // if name string isn't empty, push it to the vector
            if (dirStr != "")
            {
                dirName.push_back(dirStr);
                dirStr = "";
            }

            // last argument; if flag string isn't empty, push it to the vector
            if (i + 1 == argc)
            {
                if (flagStr != "")
                {
                    flag.push_back(flagStr);
                    flagStr = "";
                }
                // typically if the flag is the last arg it means no directory was provided
                // so we assume current working directory '.' for that last flag
                if (dirStr == "")
                {
                    dirStr = ".";
                    dirName.push_back(dirStr);
                    nonOpt++;
                    dirStr = "";
                }
            }
        }

        // push all arguments to a vector
        // this is to see if only the program command was called
        args.push_back(commands[i]);
    }

    // if there are no arguments and no directories default to current working directory (pwd)
    // in the instance that only the program command was called, assume we are using current working directory '.'
    if (nonOpt == 0 && args.empty() && dirName.empty())
    {
        dirName.push_back(".");
    }

    vector<string> dirArgs, optArgs;

    // parse the vectors to assign specific flags with the directories they are supposed to be used on
    // the vectors holding flags and directory names are the same size
    for (int i = 0; i < dirName.size(); i++)
    {
        // the string could have 2 or 3 directories separated by whitespaces but they have the SAME flags applied to them
        // EX: kals -la Music -d dir1/lab1
        // fr will hold "Music " and the corresponding string in the vector flag at the same index as dirName will be "-la"
        string fr = dirName[i];
        // temporary string
        string tmp = "";
        // loop will go through each character to find a white space to split directories
        // directory name pushed to vector and its corresponding flag also pushed to a diff vector; the index match up
        for (int j = 0; j < fr.length(); j++)
        {
            if (fr[j] == '.')
            {
                tmp = tmp + fr[j];
                dirArgs.push_back(tmp);
                if (!flag.empty())
                {
                    optArgs.push_back(flag[i]);
                }
                else
                {
                    // flag will not exist; placeholder
                    optArgs.push_back("x");
                }
                tmp = "";
            }
            else if (fr[j] != ' ')
            {
                tmp += fr[j];
            }
            else
            {
                dirArgs.push_back(tmp);
                if (!flag.empty())
                {
                    optArgs.push_back(flag[i]);
                }
                else
                {
                    // flag will not exist; placeholder
                    // in this case, directory was only inputted but no flags
                    optArgs.push_back("x");
                }
                tmp = "";
            }
        }
    }

    // create DirectoryListing object and initialize the options
    DirectoryListing ls = DirectoryListing(opta, optf, optd, optl, opth);

    // index for flags and directory names
    index = 0;

    // if only the command was called assume we are using current working directory
    if (nonOpt == 0)
    {
        // will hold "."
        string cwd = dirArgs[index];
        // placeholder flag, all are false so it should just do ls -R by default
        string flg = optArgs[index];
        // convert string to char*
        vector<char> tmp(cwd.begin(), cwd.end());
        tmp.push_back('\0');
        char *path = &tmp[0];
        // call recursive function with path/directory name and flags
        ls.listDirRec(path, flg);
    }
    else
    {
        // go through the directories and apply their flags
        // nonOpt keeps track of how many directories are left that need to be traversed
        while (nonOpt > 0 && index < dirArgs.size())
        {
            // holds the directory name
            string pathStr = dirArgs[index];
            // holds the flag that applies to that directory
            string flagArgs = optArgs[index];
            // convert string to char*
            vector<char> tmp(pathStr.begin(), pathStr.end());
            tmp.push_back('\0');
            char *path = &tmp[0];
            // call recursive function
            ls.listDirRec(path, flagArgs);
            // decrement nonOpt and increment index after each successful traversal
            nonOpt--;
            index++;
        }
    }
    return 0;
}
