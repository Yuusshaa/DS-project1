#include "LevelManager.h"

#include <fstream>

namespace {
    class JsonLevelReader {
    private:
        std::string source;
        int position;

        void skipSpaces() {
            while (position < (int)source.size() &&
                   (source[position] == ' ' || source[position] == '\n' ||
                    source[position] == '\r' || source[position] == '\t')) {
                position++;
            }
        }

        bool take(char expected) {
            skipSpaces();
            if (position >= (int)source.size() || source[position] != expected) {
                return false;
            }
            position++;
            return true;
        }

        bool readString(std::string& output) {
            skipSpaces();
            if (position >= (int)source.size() || source[position] != '"') {
                return false;
            }
            position++;
            output.clear();
            while (position < (int)source.size()) {
                char ch = source[position++];
                if (ch == '"') {
                    return true;
                }
                if (ch == '\\') {
                    if (position >= (int)source.size()) return false;
                    char escaped = source[position++];
                    if (escaped == 'n') output += '\n';
                    else if (escaped == 'r') output += '\r';
                    else if (escaped == 't') output += '\t';
                    else if (escaped == '"' || escaped == '\\' || escaped == '/') {
                        output += escaped;
                    } else {
                        return false;
                    }
                } else {
                    output += ch;
                }
            }
            return false;
        }

        bool readInteger(int& output) {
            skipSpaces();
            int start = position;
            if (position < (int)source.size() && source[position] == '-') position++;
            int digitsStart = position;
            while (position < (int)source.size() && source[position] >= '0' &&
                   source[position] <= '9') {
                position++;
            }
            if (position == digitsStart) return false;
            output = 0;
            int sign = 1;
            if (source[start] == '-') sign = -1;
            for (int i = digitsStart; i < position; i++) {
                output = output * 10 + source[i] - '0';
            }
            output *= sign;
            return true;
        }

        bool readBoolean(bool& output) {
            skipSpaces();
            if (source.compare(position, 4, "true") == 0) {
                position += 4;
                output = true;
                return true;
            }
            if (source.compare(position, 5, "false") == 0) {
                position += 5;
                output = false;
                return true;
            }
            return false;
        }

        bool skipScalar() {
            skipSpaces();
            if (position >= (int)source.size()) return false;
            if (source[position] == '"') {
                std::string ignored;
                return readString(ignored);
            }
            if (source[position] == 't' || source[position] == 'f') {
                bool ignored;
                return readBoolean(ignored);
            }
            if (source.compare(position, 4, "null") == 0) {
                position += 4;
                return true;
            }
            int ignoredNumber;
            return readInteger(ignoredNumber);
        }

        bool readLevel(LevelDefinition& level) {
            if (!take('{')) return false;
            int requiredFields = 0;
            skipSpaces();
            if (take('}')) return false;

            while (true) {
                std::string key;
                if (!readString(key) || !take(':')) return false;
                if (key == "act") {
                    if (!readInteger(level.act)) return false;
                    requiredFields |= 1;
                } else if (key == "number") {
                    if (!readInteger(level.number)) return false;
                    requiredFields |= 2;
                } else if (key == "scenarioId") {
                    if (!readInteger(level.scenarioId)) return false;
                } else if (key == "targetValue") {
                    if (!readInteger(level.targetValue)) return false;
                } else if (key == "optimalSteps") {
                    if (!readInteger(level.optimalSteps)) return false;
                } else if (key == "alarmCapacity") {
                    if (!readInteger(level.alarmCapacity)) return false;
                } else if (key == "implemented") {
                    if (!readBoolean(level.implemented)) return false;
                    requiredFields |= 4;
                } else if (key == "title") {
                    if (!readString(level.title)) return false;
                    requiredFields |= 8;
                } else if (key == "concept") {
                    if (!readString(level.conceptName)) return false;
                    requiredFields |= 16;
                } else if (key == "objective") {
                    if (!readString(level.objective)) return false;
                    requiredFields |= 32;
                } else if (!skipScalar()) {
                    return false;
                }

                skipSpaces();
                if (take('}')) break;
                if (!take(',')) return false;
            }
            return requiredFields == 63 && level.act > 0 && level.number > 0;
        }

    public:
        JsonLevelReader(std::string text) {
            source = text;
            position = 0;
        }

        bool parse(DynArray<LevelDefinition>& output) {
            if (!take('{')) return false;
            bool foundLevels = false;
            skipSpaces();
            if (take('}')) return false;

            while (true) {
                std::string key;
                if (!readString(key) || !take(':')) return false;
                if (key == "levels") {
                    if (foundLevels || !take('[')) return false;
                    foundLevels = true;
                    skipSpaces();
                    if (!take(']')) {
                        while (true) {
                            LevelDefinition level;
                            if (!readLevel(level)) return false;
                            output.push(level);
                            skipSpaces();
                            if (take(']')) break;
                            if (!take(',')) return false;
                        }
                    }
                } else if (!skipScalar()) {
                    return false;
                }

                skipSpaces();
                if (take('}')) break;
                if (!take(',')) return false;
            }
            skipSpaces();
            return foundLevels && output.size() > 0 && position == (int)source.size();
        }
    };
}

std::string LevelDefinition::code() {
    return std::to_string(act) + "." + std::to_string(number);
}

bool LevelManager::loadFromJson(std::string filePath) {
    std::ifstream input(filePath.c_str());
    if (!input.is_open()) return false;

    std::string json;
    char ch;
    while (input.get(ch)) json += ch;

    DynArray<LevelDefinition> loaded;
    JsonLevelReader reader(json);
    if (!reader.parse(loaded)) return false;

    levels.clear();
    earnedStars.clear();
    bestScores.clear();
    for (int i = 0; i < loaded.size(); i++) {
        levels.push(loaded[i]);
        earnedStars.push(0);
        bestScores.push(0);
    }
    return true;
}

bool LevelManager::loadProgress(std::string filePath) {
    std::ifstream input(filePath.c_str());
    if (!input.is_open()) return false;

    std::string signature;
    int version = 0;
    int entryCount = 0;
    if (!(input >> signature >> version >> entryCount) ||
        signature != "VAULT_HEIST_PROGRESS" || version != 1 ||
        entryCount < 0 || entryCount > 10000) {
        return false;
    }

    DynArray<int> loadedStars;
    DynArray<int> loadedScores;
    for (int i = 0; i < levels.size(); i++) {
        loadedStars.push(0);
        loadedScores.push(0);
    }

    for (int i = 0; i < entryCount; i++) {
        int act = 0;
        int number = 0;
        int stars = 0;
        int score = 0;
        if (!(input >> act >> number >> stars >> score) ||
            act <= 0 || number <= 0 || stars < 0 || stars > 3 || score < 0) {
            return false;
        }
        int index = findLevel(act, number);
        if (index >= 0) {
            loadedStars[index] = stars;
            loadedScores[index] = score;
        }
    }

    earnedStars.clear();
    bestScores.clear();
    for (int i = 0; i < levels.size(); i++) {
        earnedStars.push(loadedStars[i]);
        bestScores.push(loadedScores[i]);
    }
    return true;
}

bool LevelManager::saveProgress(std::string filePath) {
    std::ofstream output(filePath.c_str());
    if (!output.is_open()) return false;

    output << "VAULT_HEIST_PROGRESS 1\n";
    output << levels.size() << "\n";
    for (int i = 0; i < levels.size(); i++) {
        output << levels[i].act << " " << levels[i].number << " "
               << earnedStars[i] << " " << bestScores[i] << "\n";
    }
    return output.good();
}

int LevelManager::count() {
    return levels.size();
}

LevelDefinition& LevelManager::getLevel(int index) {
    return levels[index];
}

int LevelManager::findLevel(int act, int number) {
    for (int i = 0; i < levels.size(); i++) {
        if (levels[i].act == act && levels[i].number == number) return i;
    }
    return -1;
}

bool LevelManager::isUnlocked(int index) {
    if (index < 0 || index >= levels.size() || !levels[index].implemented) {
        return false;
    }
    if (index == 0) return true;
    return earnedStars[index - 1] > 0;
}

bool LevelManager::isCompleted(int index) {
    return index >= 0 && index < earnedStars.size() && earnedStars[index] > 0;
}

int LevelManager::starsFor(int index) {
    if (index < 0 || index >= earnedStars.size()) return 0;
    return earnedStars[index];
}

int LevelManager::bestScoreFor(int index) {
    if (index < 0 || index >= bestScores.size()) return 0;
    return bestScores[index];
}

void LevelManager::recordResult(int index, int stars, int score) {
    if (index < 0 || index >= earnedStars.size()) return;
    if (stars > earnedStars[index]) earnedStars[index] = stars;
    if (score > bestScores[index]) bestScores[index] = score;
}
