#include "skill_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <format>
#include <print>

using namespace std;
using namespace agent::skills;

SkillLoader::SkillLoader(const string& directoryPath): skillsDirectory(directoryPath) {}

bool SkillLoader::loadSkillsFromDirectory() {
    loadedSkills.clear();
    
    if (!filesystem::exists(skillsDirectory) || !filesystem::is_directory(skillsDirectory)) {
        println("Lỗi: Địa chỉ thư mục skills không tồn tại: {}", skillsDirectory);
        return false;
    }

    bool anySkillLoaded = false;
    
    for (const auto& entry : filesystem::directory_iterator(skillsDirectory)) {
        if (entry.path().extension() == ".md") {
            ifstream file(entry.path());

            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf();
                
                string skillName = entry.path().stem().string();
                loadedSkills[skillName] = buffer.str();
                anySkillLoaded = true;
                
                println("Tải thành công skill: {}", skillName);
            }
        }
    }
    
    return anySkillLoaded;
}

int SkillLoader::calculateKeywordMatchScore(const string& skillContent, const string& taskKeywords) const {
    string contentLower = skillContent;
    string keywordsLower = taskKeywords;
    
    transform(contentLower.begin(), contentLower.end(), contentLower.begin(), ::tolower);
    transform(keywordsLower.begin(), keywordsLower.end(), keywordsLower.begin(), ::tolower);
    
    int score = 0;
    stringstream ss(keywordsLower);
    string word;
    
    while (ss >> word) {
        if (contentLower.find(word) != string::npos) {
            score++;
        }
    }
    
    return score;
}

string SkillLoader::selectSkillForTask(const string& taskKeywords) const {
    string bestSkillName = "";
    int highestScore = -1;
    
    for (const auto& pair : loadedSkills) {
        const string& skillName = pair.first;
        const string& skillContent = pair.second;
        
        int score = calculateKeywordMatchScore(skillContent, taskKeywords);
        if (score > highestScore && score > 0) {
            highestScore = score;
            bestSkillName = skillName;
        }
    }
    
    return bestSkillName;
}

string SkillLoader::getSkillContent(const string& skillName) const {
    auto it = loadedSkills.find(skillName);
    if (it != loadedSkills.end()) {
        return it->second;
    }
    return "";
}

void SkillLoader::injectSkillIntoPrompt(string& systemPrompt, const string& skillContent) const {
    if (!skillContent.empty()) {
        systemPrompt += "\n\n=== RELEVANT SKILL ===\n";
        systemPrompt += skillContent;
        systemPrompt += "\n======================\n";
    }
}
