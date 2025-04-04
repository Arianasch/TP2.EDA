/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 */

#include <cmath>
#include <codecvt>
#include <locale>
#include <iostream>

#include "Lequel.h"

using namespace std;

/**
 * @brief Builds a trigram profile from a given text.
 *
 * @param text Vector of lines (Text)
 * @return TrigramProfile The trigram profile
 */
TrigramProfile buildTrigramProfile(const Text &text)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // Your code goes here...
    for (auto line : text)
    {
        if ((line.length() > 0) &&
            (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length() - 1);
    }

    TrigramProfile trigramList;
    std::wstring trigramKey;

    for(const auto& line : text) {

        std::string currentLine = line;
        wstring unicodeString = converter.from_bytes(currentLine);

        for(int n = 0; (n + 3) <= unicodeString.length(); n++) {

            for(int j = n; j < (n + 3); j++) {
                trigramKey += unicodeString[j];
            }

        string trigram = converter.to_bytes(trigramKey); 
        trigramList[trigram]++;
        trigramKey.clear();

        }
        
    }    


    // Tip: converts UTF-8 string to wstring
    //wstring unicodeString = converter.from_bytes(textLine);

    // Tip: convert wstring to UTF-8 string
    //string trigram = converter.to_bytes(unicodeTrigram);

    //return TrigramProfile(trigramList); // Fill-in result here
    return trigramList;
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    float squareSumation = 0;
    
    for(const auto& pair : trigramProfile) {
        squareSumation += (pair.second * pair.second);
    }

    for(auto& pair : trigramProfile) {
        pair.second /= squareSumation;
    }

    return;
}

/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile (the profile WE created)
 * @param languageProfile The language trigram profile (the profile we're comparing against)
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    float simmilarity = 0;

    for(auto& pair : textProfile) {
        auto it = languageProfile.find(pair.first);
        if(it != languageProfile.end()) {
            simmilarity += ((it->second) * pair.second);
        }
    }

    return simmilarity; // Fill-in result here
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{
    TrigramProfile ourTrigram = buildTrigramProfile(text);
    normalizeTrigramProfile(ourTrigram);

    float maxSimmilarity = 0;
    std::string detectedLanguage;
    detectedLanguage.clear();

    for(auto& dictionaries : languages) {
        float currentSimmilarity = getCosineSimilarity(ourTrigram, dictionaries.trigramProfile);
        if(currentSimmilarity > maxSimmilarity) {
            maxSimmilarity = currentSimmilarity;
            detectedLanguage = dictionaries.languageCode;
        }
    }

    return detectedLanguage; // Fill-in result here
}
