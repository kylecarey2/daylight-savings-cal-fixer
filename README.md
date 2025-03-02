# Daylight-Savings-iCal-Fixer

The Go Ohio app (a university class scheduling application) generates iCal files for class schedules that do not correctly account for daylight saving time. This program fixes the issue by adding the proper timezone information (TZID:America/New_York) and adjusting the class time information from UTC to Eastern Time (EST/EDT).

## Compilation

Compile the code using g++

```bash
g++ fixer.cc
```
This creates an executable file called ./a.out. Alternatively you can name your executable to something more meaningful by running g++ as 
```bash
g++ fixer.cc -o ics-fixer
```
This renames the executable to ```ics-fixer```. You can call this whatever you wish.

## Usage
Running this executable is extremely easy. It takes two additional arguments after the name of the executable. The first is the path to the original file to be converted. The second is the path and file to write the new calendar to. 
```bash
./ics-fixer downloaded.ics fixed.ics
```
This creates a new file in the directory named ```fixed.ics``` with the corrected timezone information. Additionally, if you have a seperate directory you keep the calendars in you could use the paths to that directory to read/create the files
```bash
./ics-fixer cals/downloaded.ics cals/new/spring25.ics
```
It is important that such directories exist beforehand or this will not work. It will not create a new directory for you. 