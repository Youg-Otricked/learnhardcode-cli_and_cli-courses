#include <iostream>
#include <cstdio>
#include <array>
#include <fstream>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <sstream>
#include <iterator>
#include <filesystem>
#include "json.hpp"
#include "base64.hpp"
#include <sys/wait.h>
std::string CURRENT_VERSION = "1.0.0";
void handlerCreateLesson(int numArgs, char* args[]);
void handlerCreateCourse(int numArgs, char* args[]);
void handlerRun(int numArgs, char* args[]);
void handlerDeleteLesson(int numArgs, char* args[]);
void handlerChangeCourse(int numArgs, char* args[]);
void handlerListCourses(int numArgs, char* args[]);
void handlerListLessons(int numArgs, char* args[]);
void handlerEditCourse(int numArgs, char* args[]);
void handlerStatus(int numArgs, char* args[]);
void handlerHelp(int numArgs, char* args[]);
void handlerEditLesson(int numArgs, char* args[]);
void handlerEditBrowserLesson(int numArgs, char* args[]);
void handlerDeleteCourse(int numArgs, char* args[]);
void handlerSetup(int numArgs, char* args[]);
void handlerExportCourse(int numArgs, char* args[]);
void handlerImportCourse(int numArgs, char* args[]);
void handlerImportLesson(int numArgs, char* args[]);
void handlerExportLesson(int numArgs, char* args[]);
std::string getHomePath(const std::string& subPath) {
    const char* home = std::getenv("HOME");
    if (!home) return subPath;
    return std::string(home) + "/" + subPath;
}
void handlerListBrowserLessons(int, char*[]) {
    std::string dir = getHomePath("browser_lessons");
    if (!std::filesystem::exists(dir)) {
        std::cout << "No browser_lessons directory: " << dir << "\n";
        return;
    }

    int i = 1;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;
        std::cout << i++ << ". " << entry.path().filename().string() << "\n";
    }
}
std::string encode_b64url(const std::string& s) {
    return macaron::Base64::Encode(s);
}

std::string decode_b64url(const std::string& b64) {
    std::string tmp = "";
    macaron::Base64::Decode(b64, tmp);
    return tmp;
}
void handlerDeleteBrowserLesson(int numArgs, char* args[]) {
    if (numArgs < 1) "Usage: lhc d-browser-lesson <id|title>";
    std::string id = args[0];
    if (!id.ends_with(".json")) id += ".json";

    std::string path = getHomePath("browser_lessons/" + id);

    std::error_code ec;
    bool removed = std::filesystem::remove(path, ec);
    if (ec) throw "Failed to delete: " + path + " (" + ec.message() + ")";
    if (!removed) throw "File not found: " + path;
}
std::unordered_map<std::string, std::function<void(int, char*[])>> commands = {{"import-l", handlerImportLesson}, {"export-l", handlerExportLesson}, {"import-c", handlerImportCourse}, {"export-c", handlerExportCourse}, {"setup", handlerSetup}, {"ls-browser-lessons", handlerListBrowserLessons}, {"d-browser-lesson", handlerDeleteBrowserLesson}, {"d-course", handlerDeleteCourse}, {"e-browser-lesson", handlerEditBrowserLesson}, {"e-lesson", handlerEditLesson},{"e-course", handlerEditCourse}, {"help", handlerHelp}, {"ls-lessons", handlerListLessons}, {"status", handlerStatus}, {"ls-courses", handlerListCourses}, {"ch-course", handlerChangeCourse},{"c-course", handlerCreateCourse}, {"c-lesson", handlerCreateLesson}, {"run", handlerRun}, {"d-lesson", handlerDeleteLesson}};
std::string generateShortID(const std::string& input) {
    unsigned int hash = 2166136261u;
    for (char c : input) {
        hash ^= static_cast<unsigned char>(c);
        hash *= 16777619u;
    }
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << hash;
    return ss.str();
}
void editConfig(std::string field, std::string value) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    data[field] = value;
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
std::string readConfig(std::string field) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    return data[field];
}
void printCourses() {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);

    std::string current = data.value("current_course", "");

    for (auto& [course, lang] : data["course_langs"].items()) {
        if (course == "") {
            continue;
        }
        std::string mark = (course == current) ? "* " : "  ";
        std::cout << mark << course << " : " << lang.get<std::string>() << "\n";
    }
}
std::string getCourseLang(std::string course) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    if (!data["course_langs"].contains(course))
        throw "Course " + course + " does not exist";
    return data["course_langs"][course];
}
void editCourseLang(std::string course, std::string new_lang) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    if (!data["course_langs"].contains(course))
        throw "Course " + course + " does not exist";
    data["course_langs"][course] = new_lang;
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void addCourse(std::string course_name, std::string lang) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    data["course_langs"][course_name] = lang;
    data["course_lessons"][course_name] = nlohmann::json::array();
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void addLessonHash(std::string lesson, std::string hash) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    data["lesson_hashes"][lesson] = hash;
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void addHashPath(std::string hash, std::string path) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    data["hash_paths"][hash] = path;
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
std::string findKeyFromHash(std::string hash) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    for (auto& [key, value] : data["lesson_hashes"].items()) {
        if (value == hash) {
            return key;
        }
    }
    return "";
}
int findIndexInArray(nlohmann::json& data, const std::string& course, const std::string& targetHash) {
    auto& lessonArray = data["course_lessons"][course];
    if (lessonArray.is_array()) {
        for (int i = 0; i < lessonArray.size(); ++i) {
            if (lessonArray[i] == targetHash) {
                return i;
            }
        }
    }
    return -1;
}
void eradicateLesson(std::string hash) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    if (!data["hash_paths"].contains(hash)) throw "Hash " + hash + " is not a valid hash.";
    std::string path = data["hash_paths"][hash];
    std::filesystem::remove(path);
    std::string key = findKeyFromHash(hash);
    if (key == "" || key.empty()) {
        throw "Hash " + hash + " is not a lesson hash.";
    }
    data["lesson_hashes"].erase(key);
    data["hash_paths"].erase(hash);
    std::string current_course = readConfig("current_course");
    int index = findIndexInArray(data, current_course, key);
    if (index == -1) {
        throw "Hash " + hash + " is not a lesson hash.";
    }
    data["course_lessons"][current_course].erase(data["course_lessons"][current_course].begin() + index);
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void addCourseLesson(std::string lesson) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    data["course_lessons"][readConfig("current_course")].push_back(lesson);
    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void createFile(const std::string& name, const std::string& contents) {
    std::string path = getHomePath("cli_lessons/" + name);
    std::ofstream userJson(path);
    if (userJson.is_open()) {
        userJson << contents;
    }
}
int getCourseLessonCount() {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    return data["course_lessons"][data["current_course"]].size();
}
void printCourseLessons() {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);

    std::string course = data.value("current_course", "");
    if (course.empty()) {
        std::cout << "No current course selected.\n";
        return;
    }

    int i = 1;
    for (auto& lessonJ : data["course_lessons"][course]) {
        std::string lesson = lessonJ.get<std::string>();
        std::string hash = data["lesson_hashes"].value(lesson, "");
        std::cout << i++ << ". " << lesson;
        if (!hash.empty()) std::cout << " (" << hash << ")";
        std::cout << "\n";
    }
}
struct CmdResult {
    std::string out;
    int exitCode;
};

CmdResult execWithCode(const std::string& cmd) {
    std::array<char, 128> buffer{};
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw "popen() failed";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    int status = pclose(pipe);

    int code = -1;
    if (WIFEXITED(status)) code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status)) code = 128 + WTERMSIG(status);

    return {result, code};
}

std::string readMultiline(const std::string& prompt, const std::string& endMarker="<<END") {
    std::cout << prompt << " (end with " << endMarker << ")\n";
    std::string out, line;
    while (std::getline(std::cin, line)) {
        if (line == endMarker) break;
        out += line;
        out += "\n";
    }
    if (!out.empty()) out.pop_back();
    return out;
}
void handlerCreateLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc c-lesson <title>";
    std::string title = args[0];
    nlohmann::json lesson;
    std::string type;
    std::cout << "Lesson type (cli/browser): ";
    std::getline(std::cin, type);
    if (type == "cli") {
        lesson["title"] = title;
        lesson["course"] = readConfig("current_course");
        lesson["lang"] = getCourseLang(readConfig("current_course"));
        lesson["runCommands"] = nlohmann::json::array();
        while (true) {
            std::string cmd;
            std::cout << "Enter RunHarness Command (or blank to finish): ";
            std::getline(std::cin, cmd);
            if (cmd.empty()) break;

            std::string exitStr;
            std::cout << "Enter Expected Exit Code (default 0): ";
            std::getline(std::cin, exitStr);
            int exitCode = 0;
            try { if (!exitStr.empty()) exitCode = std::stoi(exitStr); }
            catch (...) { throw "Exit code must be an integer"; }
            std::string out = readMultiline("Enter Expected Stdout");
            std::cout << "(optional)" << '\n';
            std::string mstCntain = readMultiline("Enter Must contain-stdout");
            lesson["runCommands"].push_back({
                {"command", cmd},
                {"exit_code", exitCode},
                {"expected", out},
                {"must_contain", mstCntain}
            });
        }
        lesson["submitCommands"] = nlohmann::json::array();
        while (true) {
            std::string cmd;
            std::cout << "Enter Aditional On-Submit Command (or blank to finish): ";
            std::getline(std::cin, cmd);
            if (cmd.empty()) break;

            std::string exitStr;
            std::cout << "Enter Expected Exit Code (default 0): ";
            std::getline(std::cin, exitStr);
            int exitCode = 0;
            try { if (!exitStr.empty()) exitCode = std::stoi(exitStr); }
            catch (...) { throw "Exit code must be an integer"; }
            std::string out = readMultiline("Enter Expected Stdout");
            std::cout << "(optional)" << '\n';
            std::string mstCntain = readMultiline("Enter Must contain-stdout");
            lesson["submitCommands"].push_back({
                {"command", cmd},
                {"exit_code", exitCode},
                {"expected", out},
                {"must_contain", mstCntain}
            });
        }
        createFile(title + ".json", lesson.dump(4));
        std::string course = readConfig("current_course");
        std::string lang = getCourseLang(course);
        std::string combined = course + ":" + lang + ":" + title;
        std::string lessonID = generateShortID(combined);
        std::cout << "Lesson Hash: " << lessonID << std::endl;
        addCourseLesson(title);
        addLessonHash(title, lessonID);
        addHashPath(lessonID, getHomePath("cli_lessons/" + title + ".json"));
    }
    else if (type == "browser") {
        lesson["id"] = title + ".json";
        std::string tmp;
        std::cout << "Lesson title: ";
        std::getline(std::cin, tmp);
        lesson["title"] = tmp;
        lesson["description"] = readMultiline("Lesson text: ");;
        lesson["starterCode"] = readMultiline("Starter code");

        std::cout << "(optional)\n";
        lesson["expectedOutput"] = readMultiline("Expected output");
        std::cout << "(optional)\n";
        lesson["mustContain"] = readMultiline("Output must contain");

        std::cout << "Next lesson (optional): ";
        std::getline(std::cin, tmp);
        lesson["nextLesson"] = tmp;

        std::cout << "Previous lesson (optional): ";
        std::getline(std::cin, tmp);
        lesson["previous"] = tmp;
        lesson["showButtons"] = false;

        std::cout << "Correct (optional, required for MCQ) (b1/b2/b3/b4): ";
        std::getline(std::cin, tmp);
        lesson["correct"] = tmp;
        std::string b1, b2, b3, b4;

        std::cout << "Button 1 text (blank for none): ";
        std::getline(std::cin, b1);
        std::cout << "Button 2 text (blank for none): ";
        std::getline(std::cin, b2);
        std::cout << "Button 3 text (blank for none): ";
        std::getline(std::cin, b3);
        std::cout << "Button 4 text (blank for none): ";
        std::getline(std::cin, b4);

        lesson["b1t"] = b1;
        lesson["b2t"] = b2;
        lesson["b3t"] = b3;
        lesson["b4t"] = b4;

        lesson["showButtons"] = !(b1.empty() && b2.empty() && b3.empty() && b4.empty());
        if (lesson["showButtons"].get<bool>()) {
            while (true) {
                std::string correct = lesson.value("correct", "");
                if (correct == "b1" || correct == "b2" || correct == "b3" || correct == "b4") {
                    break;
                }
                std::cout << "Invalid 'correct'. Enter b1, b2, b3, or b4";
                std::getline(std::cin, correct);
                lesson["correct"] = correct;
            }
        } else {
            lesson["correct"] = "";
        }
        std::cout << "Run harness (hash or path, optional): ";
        std::getline(std::cin, tmp);
        lesson["runHarness"] = tmp;
        lesson["mode"] = "browser";

        std::cout << "Difficulty (optional, e.g. easiest): ";
        std::getline(std::cin, tmp);
        lesson["difficulty"] = tmp;
        std::cout << "XP (default 10): ";
        std::string xpStr;
        std::getline(std::cin, xpStr);
        lesson["xp"] = xpStr.empty() ? 10 : std::stoi(xpStr);
        std::filesystem::create_directories(getHomePath("browser_lessons"));
        std::string path = getHomePath("browser_lessons/" + title + ".json");
        std::ofstream out(path);
        if (!out.is_open()) throw "Failed to write: " + path;
        out << lesson.dump(4);
    }
    else {
        throw "Unknown lesson type. Use 'cli' or 'browser'.";
    }
}
void handlerDeleteLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc d-lesson <hash>";
    std::string hash = args[0];
    eradicateLesson(hash);
}
void handlerCreateCourse(int numArgs, char* args[]) {
    if (numArgs < 2) throw "Usage: lhc c-course <course_name> <course_lang>";
    std::string course_name = args[0];
    std::string lang = args[1];
    addCourse(course_name, lang);
    editConfig("current_course", course_name);
}
void handlerChangeCourse(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc ch-course <course_name>";
    std::string course_name = args[0];
    editConfig("current_course", course_name);
}
void handlerListCourses(int numArgs, char* args[]) {
    if (numArgs > 0) throw "Usage: lhc ls-courses";
    printCourses();
}
void handlerEditLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc e-lesson <hash>";
    std::string hash = args[0];
    std::ifstream cfgIn(getHomePath("user_config.json"));
    nlohmann::json cfg = nlohmann::json::parse(cfgIn);

    std::string path = cfg["hash_paths"].value(hash, "");
    if (path.empty()) throw "Unknown lesson hash: " + hash;
    std::ifstream lessonIn(path);
    if (!lessonIn.is_open()) throw "Cannot open lesson file: " + path;
    nlohmann::json lesson = nlohmann::json::parse(lessonIn);
    std::cout << "Which commands would you like to edit? (run/submit)";
    std::string type;
    std::cin >> type;
    if (!lesson.contains(type + "Commands") || !lesson[type + "Commands"].is_array())
        throw "Invalid lesson JSON: missing commands array";

    auto& arr = lesson[type + "Commands"];
    if (arr.empty()) {
        std::cout << "No commands in lesson.\n";
        return;
    }
    for (int i = 0; i < (int)arr.size(); i++) {
        std::string cmd = arr[i].value("command", "");
        std::cout << (i + 1) << ". " << cmd << "\n";
    }
    std::string s;
    std::cout << "Which command number to edit? ";
    std::getline(std::cin, s);
    int idx = std::stoi(s) - 1;
    if (idx < 0 || idx >= (int)arr.size()) throw "Index out of range";
    std::string field;
    std::cout << "Field (command/exit_code/expected/must_contain): ";
    std::getline(std::cin, field);

    if (field == "command") {
        std::string v;
        std::cout << "New command: ";
        std::getline(std::cin, v);
        arr[idx]["command"] = v;
    } else if (field == "exit_code") {
        std::string v;
        std::cout << "New exit code: ";
        std::getline(std::cin, v);
        arr[idx]["exit_code"] = std::stoi(v);
    } else if (field == "expected") {
        std::string v = readMultiline("New expected stdout");
        arr[idx]["expected"] = v;
    } else if (field == "must_contain") {
        std::string v = readMultiline("New must-contain stdout");
        arr[idx]["must_contain"] = v;
    } else {
        throw "Unknown field: " + field;
    }
    std::ofstream lessonOut(path);
    if (!lessonOut.is_open()) throw "Cannot write lesson file: " + path;
    lessonOut << lesson.dump(4);
}
void handlerEditBrowserLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc e-browser-lesson <id|title>";
    std::string id = args[0];
    if (!id.ends_with(".json")) id += ".json";

    std::string path = getHomePath("browser_lessons/" + id);

    std::ifstream in(path);
    if (!in.is_open()) throw "Cannot open lesson file: " + path;
    nlohmann::json lesson = nlohmann::json::parse(in);

    std::cout << "Editing: " << path << "\n";
    std::cout << "Fields: id, title, description, starterCode, expectedOutput, mustContain, nextLesson, previous, showButtons, correct, runHarness, mode, difficulty, xp\n";

    std::string field;
    std::cout << "Which field to edit? ";
    std::getline(std::cin, field);

    auto setString = [&](const char* key, bool multiline) {
        if (multiline) {
            lesson[key] = readMultiline(std::string("New ") + key);
        } else {
            std::string v;
            std::cout << "New " << key << ": ";
            std::getline(std::cin, v);
            lesson[key] = v;
        }
    };

    if (field == "id") setString("id", false);
    else if (field == "title") setString("title", false);
    else if (field == "description") setString("description", true);
    else if (field == "starterCode") setString("starterCode", true);
    else if (field == "expectedOutput") setString("expectedOutput", true);
    else if (field == "mustContain") setString("mustContain", true);
    else if (field == "nextLesson") setString("nextLesson", false);
    else if (field == "previous") setString("previous", false);
    else if (field == "correct") setString("correct", false);
    else if (field == "runHarness") setString("runHarness", false);
    else if (field == "mode") setString("mode", false);
    else if (field == "difficulty") setString("difficulty", false);
    else if (field == "xp") {
        std::string v;
        std::cout << "New xp (int): ";
        std::getline(std::cin, v);
        lesson["xp"] = std::stoi(v);
    }
    else if (field == "showButtons") {
        std::string v;
        std::cout << "New showButtons (true/false): ";
        std::getline(std::cin, v);
        lesson["showButtons"] = (v == "true" || v == "1" || v == "yes" || v == "y");
    }
    else {
        throw "Unknown field: " + field;
    }

    std::ofstream out(path);
    if (!out.is_open()) throw "Cannot write lesson file: " + path;
    out << lesson.dump(4);
}
void handlerListLessons(int numArgs, char* args[]) {
    printCourseLessons();
}
void handlerEditCourse(int numArgs, char* args[]) {
    if (numArgs < 3) throw "Usage: lhc e-course <course_name> lang <newlang>";
    if (args[1] == "lang") {
        editCourseLang(args[0], args[2]);
    }
}
void handlerDeleteCourse(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc d-course <course_name>";
    std::string course = args[0];

    std::ifstream in(getHomePath("user_config.json"));
    if (!in.is_open()) throw "Cannot open config file";
    nlohmann::json data = nlohmann::json::parse(in);

    if (!data.contains("course_langs") || !data["course_langs"].contains(course))
        throw "Course does not exist: " + course;
    if (data.contains("course_lessons") && data["course_lessons"].contains(course) && data["course_lessons"][course].is_array()) {
        for (auto& lessonJ : data["course_lessons"][course]) {
            std::string title = lessonJ.get<std::string>();

            std::string hash = data["lesson_hashes"].value(title, "");
            if (!hash.empty()) {
                std::string path = data["hash_paths"].value(hash, "");
                if (!path.empty()) {
                    std::error_code ec;
                    std::filesystem::remove(path, ec);
                    if (ec) throw "Failed to delete " + path + ": " + ec.message();
                }
                data["hash_paths"].erase(hash);
            }
            data["lesson_hashes"].erase(title);
        }
    }

    data["course_lessons"].erase(course);
    data["course_langs"].erase(course);

    if (data.value("current_course", "") == course) {
        data["current_course"] = "";
    }

    std::ofstream out(getHomePath("user_config.json"));
    if (!out.is_open()) throw "Cannot write config file";
    out << data.dump(4);
}
void handlerExportLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc export-l <lesson_hash> (--browser)";
    std::string hash = args[0];
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    std::filesystem::copy(data["hash_paths"][hash].get<std::string>(), getHomePath("exported_lessons/" + findKeyFromHash(hash) + ".json"));
    std::cout << "Lesson " + hash + " successfully exported to file exported_lessons" << '\n';
}
void handlerImportLesson(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc import-l <lesson_file> (--link)";
    std::string lesson = args[0];
    std::filesystem::copy(getHomePath("exported_lessons/" + lesson), getHomePath("cli_lessons/" + lesson));
    std::string title = lesson.substr(0, lesson.length() - 5);
    std::string combined = readConfig("current_course") + ":" + getCourseLang(readConfig("current_course")) + ":" + title;
    std::string hash = generateShortID(combined);
    addCourseLesson(title);
    addLessonHash(title, hash);
    addHashPath(hash, getHomePath("cli_lessons/" + lesson));
    std::cout << "Lesson " + lesson + " successfully imported" << '\n';
}
void handlerSetup(int numArgs, char* args[]) {
    system("echo 'export PATH=\"/usr/share/:$PATH\"' >> ~/.bashrc");
    if (numArgs < 1) throw "Usage: lhc setup <username>";
    std::error_code ec;
    std::filesystem::create_directories(getHomePath("exported_courses"), ec);
    std::filesystem::create_directories(getHomePath("exported_lessons"), ec);
    std::filesystem::create_directories(getHomePath("cli_lessons"), ec);
    std::filesystem::create_directories(getHomePath("browser_lessons"), ec);
    system("cp ./lhc /usr/share/lhc");
    system("chmod +x /usr/share/lhc");
    if (ec) throw "Failed to create dir: " + ec.message();
    std::string cfgPath = getHomePath("user_config.json");
    if (!std::filesystem::exists(cfgPath)) {
        std::ofstream setupFile(getHomePath("user_config.json"));
        setupFile << R"(
{
    "course_langs": {
        "": ""
    },
    "course_lessons": {
        "": []
    },
    "current_course": "",
    "hash_paths": {
        "": ""
    },
    "lesson_hashes": {
        "": ""
    },
    "completed_lessons": {
        "": {
            "": false
        }
    },
    "completed_courses": {
        "": false
    },
    "user_name": ")" + std::string(args[0]) + R"("
})";
    }
}

std::unordered_map<std::string, std::string> usage = {
    {"help", "lhc help"},
    {"setup", "lhc setup <username>"},
    {"c-course", "lhc c-course <course_name> <lang>"},
    {"ch-course", "lhc ch-course <course_name>"},
    {"ls-courses", "lhc ls-courses"},
    {"d-course", "lhc d-course <course_name>"},
    {"e-course", "lhc e-course <course_name> lang <newlang>"},
    {"c-lesson", "lhc c-lesson <lesson_title>"},
    {"ls-lessons", "lhc ls-lessons"},
    {"d-lesson", "lhc d-lesson <hash>"},
    {"e-lesson", "lhc e-lesson <hash>"},
    {"run", "lhc run <hash> (-s)"},
    {"status", "lhc status"},
    {"export-c", "lhc export-c <course_name> <description> <tags> <version> <difficulty> <prereqs> (--browser)"},
    {"import-c", "lhc import-c <course_file> (--link)"},
    {"export-l", "lhc export-l <hash> (--browser)"},
    {"import-l", "lhc import-l <lesson_file> (--link)"},
    {"ls-browser-lessons", "lhc ls-browser-lessons"},
    {"d-browser-lesson", "lhc d-browser-lesson <id>"},
    {"e-browser-lesson", "lhc e-browser-lesson <id>"},
    {"upgrade", "lhc upgrade"},
};
std::string getCourseFromLesson(const std::string& lessonTitle) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);

    for (auto& [course, lessons] : data["course_lessons"].items()) {
        for (auto& l : lessons) {
            if (l.get<std::string>() == lessonTitle) {
                return course;
            }
        }
    }
    throw "No course found for lesson: " + lessonTitle;
}
void handlerHelp(int, char*[]) {
    std::cout
        << "Prefixes:\n"
        << "  ls: list\n"
        << "  c: create\n"
        << "  ch: change\n"
        << "  d: delete\n"
        << "  e: edit\n\n"
        << "Commands:\n";

    for (const auto& [name, _] : commands) {
        auto it = usage.find(name);
        std::cout << "  " << (it == usage.end() ? name : it->second) << "\n";
    }
}
void handlerStatus(int numArgs, char* args[]) {
    std::cout << "Course: " + readConfig("current_course") << '\n' << "Language: " << getCourseLang(readConfig("current_course")) << '\n' << "Lesson Count: " << getCourseLessonCount() << '\n';
}
void markLessonComplete(const std::string& course, const std::string& lessonTitle) {
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);

    data["completed_lessons"][course][lessonTitle] = true;
    int completed = data["completed_lessons"][course].size();
    int total = data["course_lessons"][course].size();
    std::cout << "Lesson " + lessonTitle << " complete!";
    if (completed >= total) {
        data["completed_courses"][course] = true;
        std::cout << "Course " << course << " complete!\n";
    }

    std::ofstream file_out(getHomePath("user_config.json"));
    file_out << data.dump(4);
}
void handlerRun(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc run <HASH> (-s)";
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    std::string hash = args[0];
    std::string path = data["hash_paths"].value(hash, "");
    if (path.empty()) throw "Unknown hash: " + hash;
    std::ifstream in(path);
    nlohmann::json lesson = nlohmann::json::parse(in);
    bool isSubmit = false;
    if (numArgs > 1) {
        if (strstr(args[1], "-s") != nullptr) {
            isSubmit = true;
        } else {
            throw "Usage: lhc run <HASH> (-s)";
        }  
    }
    std::string lessonTitle = findKeyFromHash(hash);
    std::string course = getCourseFromLesson(lessonTitle);
    std::string courseLang = getCourseLang(course);
    bool hasPassed = true;
    int i = 0;
    for (auto commandJson : lesson["runCommands"]) {
        i++;
        hasPassed = false;
        std::cout << commandJson["command"] << '\n';
        const CmdResult commandOutput = execWithCode(commandJson["command"]);
        std::string out = commandOutput.out;
        if (!out.empty() && out.back() == '\n') out.pop_back();
        if (commandJson["must_contain"] != "") {
            hasPassed = out.find(commandJson["must_contain"].get<std::string>()) != std::string::npos && commandOutput.exitCode == commandJson["exit_code"].get<int>();
        } else {
            hasPassed = out == commandJson["expected"].get<std::string>() && commandOutput.exitCode == commandJson["exit_code"].get<int>();
        }
        if (isSubmit || i < 3) {
            std::cout << (hasPassed ? "Success" : "Fail") << '\n';
            std::cout << "Exit Code: " << commandOutput.exitCode << '\n' << "Command Output: " << commandOutput.out << '\n';
        }
        if (!hasPassed) {
            std::cout << "Fail";
            if (isSubmit) {
                std::string url = "https://youg-otricked.github.io/api/?type=cli&course=" + courseLang + "&lesson=" + lessonTitle + "&success=false";

                #ifdef __linux__
                    system(("xdg-open '" + url + "'").c_str());
                #elif __APPLE__
                    system(("open '" + url + "'").c_str());
                #endif
                break;
            }
        }
    }
    if (!hasPassed || !isSubmit) return;
    for (auto commandJson : lesson["submitCommands"]) {
        hasPassed = false;
        std::cout << commandJson["command"] << '\n';
        const CmdResult commandOutput = execWithCode(commandJson["command"]);
        std::string out = commandOutput.out;
        if (!out.empty() && out.back() == '\n') out.pop_back();
        if (commandJson["must_contain"] != "") {
            hasPassed = out.find(commandJson["must_contain"].get<std::string>()) != std::string::npos && commandOutput.exitCode == commandJson["exit_code"].get<int>();
        } else {
            hasPassed = out == commandJson["expected"].get<std::string>() && commandOutput.exitCode == commandJson["exit_code"].get<int>();
        }
        std::cout << (hasPassed ? "Success" : "Fail") << '\n';
        std::cout << "Exit Code: " << commandOutput.exitCode << '\n' << "Command Output: " << commandOutput.out << '\n';
        if (!hasPassed) {
            std::string url = "https://youg-otricked.github.io/api/?type=cli&course=" + courseLang + "&lesson=" + lessonTitle + "&success=false";

            #ifdef __linux__
                system(("xdg-open '" + url + "'").c_str());
            #elif __APPLE__
                system(("open '" + url + "'").c_str());
            #endif
            break;
        }
    }
    if (!hasPassed) return;
    std::string url = "https://youg-otricked.github.io/api/?type=cli&course=" + courseLang + "&lesson=" + lessonTitle + "&success=true";

    #ifdef __linux__
        system(("xdg-open '" + url + "'").c_str());
    #elif __APPLE__
        system(("open '" + url + "'").c_str());
    #endif
    markLessonComplete(course, lessonTitle);
}
std::vector<std::string> parseArrayArg(const std::string& arg) {
    std::vector<std::string> result;
    std::string stripped = arg;
    if (stripped.front() == '[') stripped.erase(0, 1);
    if (stripped.back() == ']') stripped.pop_back();
    
    if (stripped.empty()) return result;
    
    std::stringstream ss(stripped);
    std::string item;
    while (std::getline(ss, item, ',')) {
        while (!item.empty() && item.front() == ' ') item.erase(0, 1);
        while (!item.empty() && item.back() == ' ') item.pop_back();
        result.push_back(item);
    }
    return result;
}
void importCourseFromString(std::string course_name) {
    std::ifstream course_in(getHomePath("exported_courses/" + course_name));
    nlohmann::json course = nlohmann::json::parse(course_in);
    std::string name = course["course_name"].get<std::string>();
    std::string lang = course["course_lang"].get<std::string>();
    addCourse(name, lang);
    editConfig("current_course", name);
    for (auto& lesson : course["lessons"]) {
        std::string title = lesson["title"].get<std::string>();
        createFile(title + ".json", lesson.dump(4));
        std::string combined = name + ":" + lang + ":" + title;
        std::string hash = generateShortID(combined);
        
        addCourseLesson(title);
        addLessonHash(title, hash);
        addHashPath(hash, getHomePath("cli_lessons/" + title + ".json"));
        
        std::cout << "Imported: " << title << " (" << hash << ")\n";
    }
    std::cout << "Successfully imported course " << course_name << '\n';
}
void handlerUpgrade(int numArgs, char* args[]) {
    std::string remoteVersion = execWithCode(
        "curl -s https://raw.githubusercontent.com/Youg-Otricked/learnhardcode-cli_and_cli-courses/main/version.txt"
    ).out;
    if (!remoteVersion.empty() && remoteVersion.back() == '\n') 
        remoteVersion.pop_back();
    
    if (remoteVersion != CURRENT_VERSION) {
        std::cout << "Updating CLI: " << CURRENT_VERSION << " -> " << remoteVersion << "\n";
        system("curl -sL https://github.com/Youg-Otricked/learnhardcode-cli_and_cli-courses/releases/latest/download/lhc -o /tmp/lhc");
        system("chmod +x /tmp/lhc");
        system("mv /tmp/lhc /usr/share/lhc");
    } else {
        std::cout << "CLI is up to date.\n";
    }
    std::string courseList = execWithCode(
        "curl -s https://raw.githubusercontent.com/Youg-Otricked/learnhardcode-cli_and_cli-courses/main/courses.json"
    ).out;
    
    nlohmann::json courses = nlohmann::json::parse(courseList);
    for (auto& c : courses["courses"]) {
        std::string name = c["name"].get<std::string>();
        std::string remoteVersion = c["version"].get<std::string>();
        std::string localPath = getHomePath("exported_courses/" + name + ".json");
        if (std::filesystem::exists(localPath)) {
            std::ifstream in(localPath);
            nlohmann::json local = nlohmann::json::parse(in);
            std::string localVersion = local["version"].get<std::string>();
            
            if (localVersion >= remoteVersion) {
                std::cout << name << " is up to date (" << localVersion << ")\n";
                continue;
            }
            std::cout << name << ": " << localVersion << " -> " << remoteVersion << "\n";
        } else {
            std::cout << "New course: " << name << " (" << remoteVersion << ")\n";
        }
        std::string courseData = execWithCode(
            "curl -s " + c["url"].get<std::string>()
        ).out;
        importCourseFromString(courseData);
    }
}
void handlerImportCourse(int numArgs, char* args[]) {
    if (numArgs < 1) throw "Usage: lhc import-c <course_save_file_name> (--link)";
    std::string course_name = args[0];
    std::ifstream course_in(getHomePath("exported_courses/" + course_name));
    nlohmann::json course = nlohmann::json::parse(course_in);
    std::string name = course["course_name"].get<std::string>();
    std::string lang = course["course_lang"].get<std::string>();
    addCourse(name, lang);
    editConfig("current_course", name);
    for (auto& lesson : course["lessons"]) {
        std::string title = lesson["title"].get<std::string>();
        createFile(title + ".json", lesson.dump(4));
        std::string combined = name + ":" + lang + ":" + title;
        std::string hash = generateShortID(combined);
        
        addCourseLesson(title);
        addLessonHash(title, hash);
        addHashPath(hash, getHomePath("cli_lessons/" + title + ".json"));
        
        std::cout << "Imported: " << title << " (" << hash << ")\n";
    }
    std::cout << "Successfully imported course " << course_name << '\n';
}
void handlerExportCourse(int numArgs, char* args[]) {
    if (numArgs < 6) throw "Usage: lhc export-c <course_name> <course_description> <tags([tag1, tag2...]> <version> <dificulty> <prereqs(array of strings> (--browser)";
    std::string course = args[0];
    std::ifstream file_in(getHomePath("user_config.json"));
    nlohmann::json data = nlohmann::json::parse(file_in);
    if (!data["course_langs"].contains(course)) throw "Course " + course + " doesn't exist.";
    std::ofstream out(getHomePath("exported_courses/" + course + ".json"));
    nlohmann::json j;
    j["course_name"] = course;
    j["course_lang"] = data["course_langs"][course];
    j["author"] = readConfig("user_name");
    j["tags"] = parseArrayArg(args[2]);
    j["prerequisites"] = parseArrayArg(args[5]);
    j["version"] = args[3];
    j["dificulty"] = args[4];
    j["description"] = args[1];
    j["lesson_hashes"] = nlohmann::json::array();
    j["lessons"] = nlohmann::json::array();
    for (auto& lessonTitle : data["course_lessons"][course]) {
        std::string title = lessonTitle.get<std::string>();
        std::string hash = data["lesson_hashes"].value(title, "");
        std::string path = data["hash_paths"].value(hash, "");
        
        j["lesson_hashes"].push_back(hash);
        std::ifstream lesson_in(path);
        nlohmann::json course_lesson = nlohmann::json::parse(lesson_in);
        j["lessons"].push_back(course_lesson);
    }
    out << j.dump(4);
    out.close();
    std::cout << "Course " + course + " successfully exported to file exported_courses/" + course + ".json";
}
int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "Usage: `lhc <args>" << '\n' << "Try `lhc help`" << '\n';
        return 1;
    }
    std::string command_name = argv[1];
    auto it = commands.find(command_name);
    if (it == commands.end()) {
        std::cerr << "Command " + command_name + " Doesn't exist." << '\n' << "Try `lhc help`" << '\n';
        return 1;
    }
    try {
        it->second(argc - 2, argv + 2);
    } catch (const char* e) {
        std::cerr << "Error: " << e << std::endl;
        return 1;
    } catch (std::string e) {
        std::cerr << "Error: " << e << std::endl;
        return 1;
    }
    return 0;
}