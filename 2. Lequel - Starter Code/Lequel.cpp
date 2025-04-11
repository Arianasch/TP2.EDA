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

    for (auto line : text)
    {
        if ((line.length() > 0) &&
            (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length() - 1);
    }

    TrigramProfile trigramList;
    std::wstring trigramKey;

    for(const auto& line : text) {  //line points to each element of text: a string

        wstring unicodeString = converter.from_bytes(line); //converts UTF - 8 string to wstring

        for(int n = 0; (n + 3) <= unicodeString.length(); n++) {    //the loop is terminated once the iterator points to
                                                                    //the penultimate place of the string.More in the ReadMe file.
                                                                    

            trigramKey = unicodeString.substr(n, 3);    //the current trigram is selected and saved
            string trigram = converter.to_bytes(trigramKey); //convert wstring to UTF-8 string
            trigramList[trigram]++; //each time the trigram is repeated, its value increases. That's how each trigram's frequency is calculated.
            trigramKey.clear();
            trigram.clear();

        }

        unicodeString.clear();
        
    }    
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

    for(const auto& pair : trigramProfile) {   //pair points to each trigram with its value(frequency)
        squareSumation += (pair.second * pair.second);   
    }

    for(auto& pair : trigramProfile) {
        pair.second /= (sqrt(squareSumation));
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

    for(auto& pair : textProfile) {  //pair points to each element of textProfile: a trigram and its key
                                     //we did not opt for pair to point to each element of languageProfile
                                     //due to efficiency.More in ReadMe file.
        auto it = languageProfile.find(pair.first); //it points to the element of languageProfile if
                                                    //there was a match with an element of trigramProfile.
        if(it != languageProfile.end()) {   //the condition means that a match was found
            simmilarity += ((it->second) * pair.second);    
        }
    }

    return simmilarity; 
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

    float maxSimmilarity = -1;
    std::string detectedLanguage;
    detectedLanguage.clear();

    for(auto& dictionaries : languages) {   //dictionaries points to each Language object
        float currentSimmilarity = getCosineSimilarity(ourTrigram, dictionaries.trigramProfile);
        if(currentSimmilarity > maxSimmilarity) {   //the maximum value is obtained by comparing just with the last one
                                                    //this is done for efficiency. More in the ReadMe file.
            maxSimmilarity = currentSimmilarity;
            detectedLanguage = dictionaries.languageCode;
        }
    }

    return detectedLanguage; 
}
