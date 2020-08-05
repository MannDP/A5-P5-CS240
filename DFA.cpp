#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <fstream>
#include <set>
#include <cassert>
using namespace std;

const int startState = 0;

// helper functions
string readFile(const string& sourceFile);

class DFA {
    map<pair<int, char>, int> delta;
    vector<string> prefixes;
    const string pattern;
    const string text;
    vector<int> matches;
    set<char> charsInPattern;
    const unsigned long numStates;

    void _computeDelta();
    void _computePrefixes();
    void _computeCharsInPattern();
    int _l(const string& str);

public:
    DFA(string pattern, string text);
    void findMatches();
    void printDelta(const string& outputFile);
    void printMatches(const string& outputFile);
    void printResults();
};

DFA::DFA(string pattern, string text): pattern{std::move(pattern)}, text{std::move(text)}, numStates{pattern.size() + 1} {
    _computeDelta();
}

void DFA::_computeCharsInPattern() {
    for (const auto & c : pattern) {
        charsInPattern.insert(c);
    }
}

void DFA::_computeDelta() {
    _computePrefixes();
    _computeCharsInPattern();

    // compute delta transition table
    for (int i = 0; i < numStates; i++) {
        for (const auto & c : charsInPattern) {
            pair<int, char> key{i, c};
            int transition = _l(prefixes.at(i) + c);
            delta.insert({key, transition});
        }
    }
}

void DFA::findMatches() {
    matches.clear();
    int state = 0;
    const int acceptingState = pattern.size();
    for (const auto & character : text) {
        if (auto it = delta.find({state, character}); it != delta.end()) {
            state = it->second;
            if (state == acceptingState) {
                matches.emplace_back(state);
            }
        }
    }
}

void DFA::printDelta(const string& outputFile) {
    ofstream out{outputFile};
    for( auto const& [key, val] : delta) {
        cout << key.first << " " << key.second << " " << val << endl;
    }
}

void DFA::printMatches(const std::string& outputFile) {
    ofstream out{outputFile};
    for (const auto& match : matches) {
        out << match << endl;
    }
}

void DFA::_computePrefixes() {
    assert(prefixes.empty());
    const int patternSize = pattern.size();
    for (int i = 0; i < (patternSize + 1); i++) {
        string prefix = pattern.substr(0, i);
        prefixes.emplace_back(prefix);
    }
}

int DFA::_l(const string &str) {
    // return the length of the longest prefix in P that is a suffix of str
    int longestPrefixLength = 0;
    const int numPrefixes = prefixes.size();

    // check for each non-empty prefix
    for (int i = 1; i < numPrefixes; i++) {
        string prefix = prefixes[i];
        int prefixSize = prefix.size();
        int offsetInStr = str.size() - prefixSize;

        if (offsetInStr < 0) {
            break;
        }

        bool isSuffix = true;
        for (int j = 0; j < prefixSize; j++) {
            if (prefix.at(j) != str.at(offsetInStr + j)) {
               isSuffix = false;
               break;
            }
        }

        if (isSuffix) {
            assert(longestPrefixLength < prefix.size());
            longestPrefixLength = prefix.size();
        }
    }

    return longestPrefixLength;
}

void DFA::printResults() {
    printDelta("a5delta.txt:");
    printMatches("a5matches.txt.");
}


int main() {
    const string patternFile = "a5pattern.txt";
    const string textFile = "a5text.txt.";
    DFA dfa{readFile(patternFile), readFile(textFile)};
    dfa.printResults();
}

string readFile(const string& sourceFile) {
    string result;
    ifstream istream{sourceFile};
    assert(istream);
    string tmp;
    while(istream >> tmp) {
        result.append(tmp);
    }
    return result;
}