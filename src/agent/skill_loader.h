#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace agent::skills {
    class SkillLoader {
        private:
            std::string skillsDirectory;
            std::unordered_map<std::string, std::string> loadedSkills;

            int calculateKeywordMatchScore(const std::string& skillContent, const std::string& taskKeywords) const;

        public:
            SkillLoader(const std::string& directoryPath);

            bool loadSkillsFromDirectory();
            std::string selectSkillForTask(const std::string& taskKeywords) const;
            std::string getSkillContent(const std::string& skillName) const;
            void injectSkillIntoPrompt(std::string& systemPrompt, const std::string& skillContent) const;
    };
}
