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

void readEvents(ifstream &in, vector<Event> &events);
void incrementHours(vector<Event> &events);
void outputNewCal(ofstream &out, const vector<Event> &events, const string &header);



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
        header = header + temp + "\r\n";
    }
    inputCal.ignore(100, '\n');

    /// Record each event
    vector<Event> allEvents;
    readEvents(inputCal, allEvents);
    incrementHours(allEvents);
    outputNewCal(outputCal, allEvents, header);

    /// Close files
    inputCal.close();
    outputCal.close();
    return 0;
}  /// main

void readEvents(ifstream &in, vector<Event> &events) {
    string id, summary, stamp, start, des, loc, rule, dur, temp;
    while (getline(in, id)) {
        /// Read each line
        getline(in, summary);
        getline(in, stamp);
        getline(in, start);
        getline(in, des);
        getline(in, loc);
        getline(in, rule);
        getline(in, dur);

        /// Remove the carriage return character from each string
        id = id.substr(0, id.length() - 1);
        summary = summary.substr(0, summary.length() - 1);
        stamp = stamp.substr(0, stamp.length() - 1);
        start = start.substr(0, start.length() - 1);
        des = des.substr(0, des.length() - 1);
        loc = loc.substr(0, loc.length() - 1);
        rule = rule.substr(0, rule.length() - 1);
        dur = dur.substr(0, dur.length() - 1);

        
        /// Read end/begin of events into temp vars and then place pointer at beginning of new line
        in >> temp >> temp;
        in.ignore(2, '\n');
        
        /// Create an Event object and push it to vector of Event objects
        Event e = {id, summary, stamp, start, des, loc, rule, dur};
        events.push_back(e);
    }
}

void incrementHours(vector<Event> &events) {
    for (size_t i = 0; i < events.size(); i++) {
        /// Extract hours part of dtStart and increment it by 1 to represent daylight savings
        string sub = events[i].dtStart.substr(17, 2);
        int hour = stoi(sub);
        hour++;
        if (hour == 25) {
            hour = 0;
        }

        /// Place incremented hour back into dtStart
        sub = to_string(hour);
        events[i].dtStart.replace(17, 2, sub);
    }
}

void outputNewCal(ofstream &out, const vector<Event> &events, const string &header) {
    out << header;
    for (size_t i = 0; i < events.size(); i++) {
        out << "BEGIN:VEVENT\r\n";
        out << events[i].id << "\r\n";
        out << events[i].summary << "\r\n";
        out << events[i].dtStamp << "\r\n";
        out << events[i].dtStart << "\r\n";
        out << events[i].description << "\r\n";
        out << events[i].location << "\r\n";
        out << events[i].rule << "\r\n";
        out << events[i].duration << "\r\n";
        out << "END:VEVENT\r\n";
    }
    out << "END:VCALENDAR\r\n";
}