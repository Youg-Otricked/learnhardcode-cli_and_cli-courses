### This is the CLI for my website, LearnHardCode.
This CLI allows you to do many things, but the most important being run local development lessons for [my site, learnhardcode](https://youg-otricked.github.io/learnhardcode).

## Geting started:
After installing the CLI, `cd`' into the directory, and run
```bash
sudo ./lhc setup
# This A: moves lhc to usr/local/bin (why it needs sudo) and B: creates user_config.json, exported-courses/lessons/browser-lessons, and browser-lessons/lessons/courses directory
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
`lhc ls-course`
This lists all existing courses you have installed

Delete Course
`lhc d-course <course_name>`
This deletes a course and all lessons tied to it.

Edit Course
`lhc e-course <course_name> lang <new_lang>`
This changes the language of course course_name to new_lang


#### Lessons

