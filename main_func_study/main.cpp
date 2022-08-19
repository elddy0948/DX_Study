#include <iostream>
#include <string.h>

using namespace std;

// int main(int argc, char *argv[], char *envp[])
// {
//     bool numberLines = false;

//     if ((argc == 2) && _stricmp(argv[1], "\n") == 0)
//     {
//         numberLines = true;
//     }

//     for (int i = 0; envp[i] != NULL; ++i)
//     {
//         if (numberLines)
//         {
//             cout << i << "----- : ------------";
//         }
//         cout << envp[i] << "\n";
//     }
// }

#include <iostream>

using namespace std;
int main(int argc,     // Number of strings in array argv
         char *argv[], // Array of command-line argument strings
         char *envp[]) // Array of environment variable strings
{
    int count;

    // Display each command-line argument.
    cout << "\nCommand-line arguments:\n";
    for (count = 0; count < argc; count++)
        cout << "  argv[" << count << "]   "
             << argv[count] << "\n";
}