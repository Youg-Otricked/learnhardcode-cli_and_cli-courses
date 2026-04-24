#! /bin/bash
if [ $# -lt 6 ]; then
    echo "Please provide Course name desciprion tags version difficulty and prerquists. Just use the cli normally, this is just to make me one run command instead of 2 when adding a course to the site. unless you are me in that case you need caffine"
    exit 1
fi;
CourseToExport=$1
CourseDesc=$2
Tags=$3
Ver=$4
Diff=$5
Prereqs=$6
DELIM=""
if [ $# -gt 6 ]; then
    DELIM=$7
fi;
lhc export-c "$CourseToExport" "$CourseDesc" "$Tags" "$Ver" "$Diff" "$Prereqs"
NewName="${CourseToExport// /$DELIM}"
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
mv "$HOME/exported_courses/$CourseToExport.json" "$SCRIPT_DIR/courses/$NewName.json"