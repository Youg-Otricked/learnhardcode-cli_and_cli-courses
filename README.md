### This is the CLI for my website, LearnHardCode.
This CLI allows you to do many things, but the most important being run local development lessons for [my site, learnhardcode](https://youg-otricked.github.io/learnhardcode).

## Geting started:
After installing the CLI, `cd`' into the directory, and run
```bash
./lhc setup <chosen username>
# This A: adds the cli to your PATH and B: creates user_config.json, exported-courses/lessons/browser-lessons, and browser-lessons/lessons/courses directory
lhc upgrade
# This installs all of the courses listed in offical courses.json on the github, and unpacks them
```

## Running lessons
You run a lesson with `lhc run <HASH>`. HASH is the lesson hash. This runs the lesson commands, and to submit add the `-s` command and it also runs the bonus submit commands.




### Bonus commands
From here on out, i will cover the special commands.


#### Courses

Create Course
`lhc c-course <course_name> <course_language>`
This creates a course and sets current course to that course.

Change Course
`lhc ch-course <course_name>`
This changes the current course to course_name

List Courses
`lhc ls-courses`
This lists all existing courses you have installed along with their languages

Delete Course
`lhc d-course <course_name>`
This deletes a course and all lessons tied to it.

Edit Course
`lhc e-course <course_name> lang <new_lang>`
This changes the language of course course_name to new_lang


#### Lessons

Create Lesson
`lhc c-lesson <lesson_name>`
This will create the file at ~/created-lessons/lesson_name.json, and after you go through the createlessonwizard, it will give you the lesson hash (for use with RUN). This lesson will be added to current_courses lessons

List Lessons
`lhc ls-lessons`
Will print all lessons in the current course

Edit Lesson
`lhc e-lesson <lesson_hash>`
Will prompt which field to edit, and will allow you to edit that field

Delete Lesson
`lhc d-lesson <lesson_hash>`
Will eradicate the existance of lesson_hash, deleting its lesson, it from your user_config.json, its file, and all occurences of it in courses.

#### Browser Lessons

Create Browser Lesson
Just use create lesson, and choose the browswer option at the start instead of cli

List Browser Lessons
`lhc ls-browser-lessons`
Lists all browser lessons

Edit Browser Lesson
`lhc e-browser-lesson <lesson_name>`
Same as e-lesson, but for browser lessons

Delete Browser Lesson
`lhc d-browser-lesson <lesson_name>`
Same as d-lesson, but for browser lessons

#### Misc

Setup
`lhc setup <username>`
Creates neccesary files and directorys, adds lhc to PATH

Help
`lhc help`
Lists all command usages

Status
`lhc status`
Prints current courses name, language, and lesson count.

Upgrade
`lhc upgrade`
Updates all currently installed courses and the binary

#### Import/Export

Export Lesson (--browser)
`lhc export-l <hash>`
Copys a lesson file to your exported-lessons directory.

Export Course
`lhc export-c <course_name> <description> <tags> <version> <difficulty> <prereqs>`
Args are: course_name: string. description: string. tags: string array of string ("[tag1, tag2...]"). Version: Number in format Major.Minor.Patch (EG 1.0.0). Dificulty: String, typicaly: easiest, beginner, basic, intermidiate, hard, crazy, INSANE. Prereqs: string array of strings in same format as tags.
Creates a course json (format:
```json
{
    "author": YourUsername,
    "course_lang": course_nameslanguage,
    "course_name": course_name,
    "description": description,
    "difficulty": difficulty,
    "lesson_hashes": [
        course_nameslessonhashes
    ],
    "lessons": [
        course_nameslessonjsons
    ],
    "prerequisites": [prereqs],
    "tags": [
        tags
    ],
    "version": version
}
```
and copys it to your exported-courses directory

Export Browser Course
`lhc export-bc <course_name> <lesson1name> <lesson2name>.....`
Creates a course json, simmilar to above, and either copys it to yout exported-courses dir.
Import lesson
`lhc import-l <lesson-files-name-in-your-exported-lessons-directory> (--link)`
If not using link, puts it in your user_config.json and copys it.
If using link, then instead of file name, provided a link that can be curled that only contains the lesson (use a github.rawusercontent link if you can), and it will do as above + copy the file from the link

Import Course 
`lhc import-c <course-files-name-in-your-exported-courswes-directory> (--link)`
Same as above, but for course jsons

