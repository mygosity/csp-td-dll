#ifndef MGCP_LIB_HELPERS_H
#define MGCP_LIB_HELPERS_H

#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#define stdlog(x) std::cout << x << '\n';
#define m_assert(expr, msg) assert((msg, expr))

namespace mgcp {

inline bool IsStringInside(const std::string& src, const std::string& insider, const int32_t srcIndex) {
  if (insider.size() > src.size() - srcIndex) {
    return false;
  }
  for (int32_t i = 0; i < insider.size(); ++i) {
    if (insider[i] != src[srcIndex + i]) {
      return false;
    }
  }
  return true;
}

inline int64_t ExtractNumberFromString(const std::string& str) {
  // stdlog("ExtractNumberFromString:: 0: " << (int)'0' << " 9: " << (int)'9');
  for (size_t i = 0; i < str.size(); ++i) {
    int c = str[i];
    if (c >= '0' && c <= '9') {
      return (int64_t)std::atoi(str.substr(i).c_str());
    }
  }
  return -1;
}

inline std::string PadString(const uint32_t currentNumber, const size_t padLen, char c) {
  std::string numberString = std::to_string(currentNumber);
  if (numberString.size() >= padLen)
    return numberString;

  std::string answer = "";
  for (uint32_t i = 0; i < padLen - numberString.size(); ++i) answer += c;

  return answer + numberString;
}

inline std::string GetFormattedTime(const uint64_t timeMs) {
  const uint64_t timeSeconds = (uint64_t)std::floor(timeMs / 1000);
  std::string formattedString = "";

  if (timeSeconds >= 3600) {
    const std::string hours = PadString((uint32_t)std::floor(timeSeconds / 3600), 2, '0');
    formattedString.append(hours).append(":");
  }

  if (timeSeconds >= 60) {
    const std::string minutes = PadString((uint32_t)std::floor((timeSeconds / 60) % 60), 2, '0');
    formattedString.append(minutes).append(":");
  }

  const std::string seconds = PadString((uint32_t)std::floor(timeSeconds % 60), 2, '0');
  formattedString.append(seconds);
  return formattedString;
}

inline int64_t IndexOfWithKmp(std::string haystack, std::string needle) {
  // knuth morris pratt algorithm for faster string searching
  std::vector<size_t> lps(needle.size());
  for (size_t i = 1, last = 0; i < lps.size(); ++i) {
    while (last > 0 && needle[last] != needle[i]) {
      last = lps[last - 1];
    }
    lps[i] = needle[last] == needle[i] ? ++last : 0;
  }
  for (size_t i = 0, j = 0; i < haystack.size();) {
    if (haystack[i] == needle[j]) {
      ++j;
      ++i;
      if (j == needle.size()) {
        return i - needle.size();
      }
    } else {
      if (j > 0) {
        j = lps[j - 1];
        continue;
      }
      ++i;
    }
  }
  return -1;
}

inline std::vector<std::string> SplitStringWithKmp(std::string haystack, std::string needle) {
  // knuth morris pratt algorithm for faster string searching
  std::vector<size_t> lps(needle.size());
  for (size_t i = 1, last = 0; i < lps.size(); ++i) {
    while (last > 0 && needle[last] != needle[i]) {
      last = lps[last - 1];
    }
    lps[i] = needle[last] == needle[i] ? ++last : 0;
  }

  std::vector<std::string> finalResult;
  size_t lastPosition = 0;

  for (size_t i = 0, j = 0; i < haystack.size();) {
    if (haystack[i] == needle[j]) {
      ++j;
      ++i;
      if (j == needle.size()) {
        std::string current = haystack.substr(lastPosition, i - lastPosition - needle.size());
        // std::cout << "SplitStringWithKmp:: current: " << current << " i: " << i << " j: " << j << " lastPosition: " <<
        // lastPosition
        //           << "\n";
        finalResult.push_back(current);
        j = 0;
        lastPosition = i;
      }
    } else {
      if (j > 0) {
        j = lps[j - 1];
        continue;
      }
      ++i;
    }
  }
  std::string lastString = haystack.substr(lastPosition);
  finalResult.push_back(lastString);
  // std::cout << "SplitStringWithKmp:: lastString: " << lastString << "\n";
  return finalResult;
}

inline std::vector<std::string> SplitString(const std::string& target, const std::string& delimiter) {
  std::vector<std::string> answer;
  int32_t currentIndex = 0, i = 0, currentLength = 0;
  for (; i < target.size(); ++i) {
    if (IsStringInside(target, delimiter, i)) {
      answer.push_back(target.substr(currentIndex, currentLength));
      currentIndex = i + (int32_t)delimiter.size();
      currentLength = 0;
    } else {
      ++currentLength;
    }
  }
  answer.push_back(target.substr(currentIndex, i));
  return answer;
}

inline std::vector<std::string> SplitString(const std::string& target, char delimiter) {
  std::vector<std::string> answer;
  int32_t currentIndex = 0, i = 0, currentLength = 0;
  for (; i < target.size(); ++i) {
    if (delimiter == target[i]) {
      answer.push_back(target.substr(currentIndex, currentLength));
      currentIndex = i + 1;
      currentLength = 0;
    } else {
      ++currentLength;
    }
  }
  answer.push_back(target.substr(currentIndex, i));
  return answer;
}

inline std::vector<std::string> SplitString(const std::string& target,
                                            bool (*predicate)(char a, int32_t index, const std::string& source)) {
  std::vector<std::string> answer;
  int32_t currentIndex = 0, i = 0, currentLength = 0;
  for (; i < target.size(); ++i) {
    if (predicate(target[i], i, target)) {
      answer.push_back(target.substr(currentIndex, currentLength));
      currentIndex = i + 1;
      currentLength = 0;
    } else {
      ++currentLength;
    }
  }
  answer.push_back(target.substr(currentIndex, i));
  return answer;
}

inline std::string JoinStringVector(const std::vector<std::string>& vec, const std::string& delimiter = "") {
  std::string s;
  for (size_t i = 0; i < vec.size(); ++i) {
    s.append(vec[i]);
    if (i < vec.size() - 1) {
      s.append(delimiter);
    }
  }
  return s;
}

inline std::string StringifyVectorContents(const std::vector<std::string>& vec, const std::string optionalPrepend = "") {
  std::string s = optionalPrepend + " { ";
  for (int32_t i = 0; i < vec.size(); ++i) {
    s.append(vec[i]);
    if (i < vec.size() - 1) {
      s.append(", ");
    }
  }
  s.append(" }");
  return s;
}

template <typename T>
inline void PrintMapKeys(const std::unordered_map<std::string, T>& map, const std::string name = "") {
  std::cout << "PrintMap::" << name << '\n';
  for (auto itr = map.begin(); itr != map.end(); itr++) {
    auto key = itr->first;
    auto value = itr->second;
    std::cout << "\tkey: " << key << " value: " << value << '\n';
  }
  std::cout << "PrintMap:: finished" << name << '\n';
}

template <typename T>
inline void PrintMapKeys(const std::map<std::string, T>& map, const std::string name = "") {
  std::cout << "PrintMap::" << name << '\n';
  for (auto itr = map.begin(); itr != map.end(); itr++) {
    auto key = itr->first;
    auto value = itr->second;
    std::cout << "\tkey: " << key << " value: " << value << '\n';
  }
  std::cout << "PrintMap:: finished" << name << '\n';
}

inline void PrintVector(const std::vector<std::vector<int32_t>>& vec, const std::string optionalPrepend = "") {
  std::string s = optionalPrepend + "{\n";
  for (int32_t i = 0; i < vec.size(); ++i) {
    s.append("\t{ ");
    for (int32_t j = 0; j < vec[i].size(); ++j) {
      s.append(std::to_string(vec[i][j]));
      if (j < vec[i].size() - 1) {
        s.append(", ");
      }
    }
    s.append("}\n");
  }
  std::cout << s << "}" << '\n';
}

inline void PrintVector(const std::vector<int32_t>& vec, const std::string optionalPrepend = "") {
  std::string s = optionalPrepend + "{ ";
  for (int32_t i = 0; i < vec.size(); ++i) {
    s.append(std::to_string(vec[i]));
    if (i < vec.size() - 1) {
      s.append(", ");
    }
  }
  std::cout << s << " }" << '\n';
}

inline void PrintVector(const std::vector<std::string>& vec, const std::string optionalPrepend = "") {
  std::string s = optionalPrepend + "{ ";
  for (int32_t i = 0; i < vec.size(); ++i) {
    s.append(vec[i]);
    if (i < vec.size() - 1) {
      s.append(", ");
    }
  }
  std::cout << s << " }" << '\n';
}

inline void PrintVector(const std::vector<char>& vec, const std::string optionalPrepend = "") {
  std::string s = optionalPrepend + "{ ";
  for (int32_t i = 0; i < vec.size(); ++i) {
    s.push_back(vec[i]);
    if (i < vec.size() - 1) {
      s.append(", ");
    }
  }
  std::cout << s << " }" << '\n';
}

}  // namespace mgcp

#endif