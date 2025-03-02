/**
 *        @file: fixer.cc
 *      @author: Kyle Carey
 *        @date: November 23, 2024
 *       @brief: Takes the generated iCal for class schedules that are in UTC format and formats them in 
 *               EST/EDT format to account for daylight savings time
 */

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;

/// Event structure that holds the data for an event
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

/**
 * Function:    readEvents
 *              this function reads all the events of the input file into a Event structure and then adds it to a vector
 *
 * @param in - input stream of the input file
 * @param events - vector of Event structures
 * @return - void
 */
void readEvents(ifstream &in, vector<Event> &events);

/**
 * Function:    convertToEST
 *              function takes a vector of Event structures and converts the time attributes to EST time
 *
 * @param events - vector of Event structures
 * @return - void
 */
void convertToEST(vector<Event> &events);

/**
 * Function:    outputNewCal
 *              function outputs the new calendar to the specified output file in iCal format
 *
 * @param out - output stream
 * @param events - vector of Event structures
 * @param header - string of the header of the file
 * @return - void
 */
void outputNewCal(ofstream &out, const vector<Event> &events, const string &header);

/// String of timezone configuration needed for EST/EDT
const string TZ = "BEGIN:VTIMEZONE\r\nTZID:America/New_York\r\nX-LIC-LOCATION:America/New_York\r\nBEGIN:DAYLIGHT\r\nTZOFFSETFROM:-0500\r\nTZOFFSETTO:-0400\r\nTZNAME:EDT\r\nDTSTART:19700308T020000\r\nRRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=2SU\r\nEND:DAYLIGHT\r\nBEGIN:STANDARD\r\nTZOFFSETFROM:-0400\r\nTZOFFSETTO:-0500\r\nTZNAME:EST\r\nDTSTART:19701101T020000\r\nRRULE:FREQ=YEARLY;BYMONTH=11;BYDAY=1SU\r\nEND:STANDARD\r\nEND:VTIMEZONE\r\n";

int main(int argc, char const *argv[]) {
    /// Declare vars
    string inputFilename, outputFilename, outputFilenameExtension;

    /// Check for proper usage and set inputFilename to calendar filename
    if (argc == 3) {
        inputFilename = argv[1];
        outputFilename = argv[2];
    }
    else if (argc == 2) {
        inputFilename = argv[1];
        cout << "Enter the output file: ";
        cin >> outputFilename;
    }
    else {
        cout << "Usage: " << argv[0] << " inputFile outputFile" << endl;
        exit(0);
    }

    outputFilenameExtension = outputFilename.substr(outputFilename.length() - 4, 4); // gets the last 4 chars of the output filename

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
        /// Goes until line read is beginning of an event. This 
        if (temp.compare("BEGIN:VEVENT") == 0) {
            break;
        }
        header = header + temp + "\r\n"; // adds to header string with proper endings between each line
    }
    inputCal.ignore(100, '\n');

    /// Record each event, convert each event to EST, then output header, timezone info, and events to output file
    vector<Event> allEvents;
    readEvents(inputCal, allEvents);
    convertToEST(allEvents);
    outputNewCal(outputCal, allEvents, header);

    /// Close files
    inputCal.close();
    outputCal.close();

    cout << "Process ran successfully: new calendar in EST/EDT is \"" << outputFilename << "\"." << endl;
    return 0;
}  /// main

void readEvents(ifstream &in, vector<Event> &events) {
    /// Declare vars to read into
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


void convertToEST(vector<Event> &events) {
    /// Creates new datetime start for each event (when to start event for EST) (for starting time)
    for (size_t i = 0; i < events.size(); i++) {
        /// Create substring of current time in UTC
        string time = "DTSTART;TZID=America/New_York:";
        string sub = events[i].dtStart.substr(8, 15);
        string hourString = sub.substr(9, 2); // extract hours in UTC from substring of datetime information
        int hour = stoi(hourString);
        
        /// Convert UTC hours to EST equivalent
        int month = stoi(events.at(i).dtStamp.substr(12, 2));
        if (month > 0 && month < 6) {
            hour = hour - 5;
        }
        else {
            hour = hour - 4;
        }

        if (hour < 0) {
            hour = hour + 24;
        }

        /// Convert back to string and insert leading zero if necessary
        hourString = to_string(hour);

        if (hourString.length() == 1) {
            hourString.insert(hourString.begin(), '0');
        }

        /// Insert new hour back into substring of datetime and add it to new DTSTART (time) then replace Event's dtStart attribute with new time
        sub.replace(9, 2, hourString);
        time += sub;
        events[i].dtStart = time;   
    }

    /// Creates new datetime start for each event (when to start event for EST) (for ending time)
    for (size_t i = 0; i < events.size(); i++) {
        /// Create substring of current time in UTC
        int index = events[i].rule.find("UNTIL=");
        string sub = events[i].rule.substr(index + 6, 15);
        string hourString = sub.substr(9, 2);
        int hour = stoi(hourString);

        /// Convert UTC hours to EST equivalent
        hour = hour - 4;
        if (hour < 0) {
            hour = hour + 24;
        }

        /// Convert back to string and insert leading zero if necessary
        hourString = to_string(hour);

        if (hourString.length() == 1) {
            hourString.insert(hourString.begin(), '0');
        }

        /// Replace ending time with new hour in EST 
        sub.replace(9, 2, hourString);
        events[i].rule.replace(index + 6, 15, sub);
    }
}

void outputNewCal(ofstream &out, const vector<Event> &events, const string &header) {
    /// Output header and timezone configuration
    out << header;
    out << TZ;

    /// Output each event
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