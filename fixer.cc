/**
 *        @file: fixer.cc
 *      @author: Kyle Carey
 *        @date: November 13, 2024
 *       @brief: Add Description
 */

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;

struct Event {
    string id;
    string summary;
    string dtStamp;
    string dtStart;
    string description;
    string location;
    string rule;
    string duration;
};



int main(int argc, char const *argv[]) {
    /// Check for proper usage and set inputFilename to calendar filename
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " inputFile outputFile" << endl;
        exit(0);
    }

    string inputFilename = argv[1];
    string outputFilename = argv[2];
    string outputFilenameExtension = outputFilename.substr(outputFilename.length() - 4, 4); // gets the last 4 chars of the output filename

    /// Ensure output file will be in .ics format
    if (outputFilenameExtension != ".ics") {
        cout << "Error: output file must end in \".ics\"." << endl;
        exit(0);
    }

    /// Open files and check for failure
    ifstream inputCal;
    inputCal.open(inputFilename);
    if (inputCal.fail()) {
        cout << "Error: \"" << inputFilename << "\" does not exist." << endl;
        exit(0);
    }
    
    ofstream outputCal;
    outputCal.open(outputFilename);
    if (outputCal.fail()) {
        cout << "Error: \"" << outputFilename << "\" is an invalid file/filetype." << endl;
        inputCal.close();
        exit(0);
    }

    /// Record header of calendar
    string header = "";
    string temp;

    while (inputCal >> temp) {
        if (temp.compare("BEGIN:VEVENT") == 0) {
            break;
        }
        header = header + temp + "\n";
    }

    ///


    /// Close files
    inputCal.close();
    outputCal.close();
    return 0;
}  /// main