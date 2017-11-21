//
// Created by alberto on 21/11/17.
//

#ifndef AS_STRING_H
#define AS_STRING_H

#include <string>
#include <algorithm>

namespace as {
    /** @namespace  string
     *  @brief      Simple utility functions to manipulate strings.
     */
    namespace string {
        /** @brief      Trims a string from the left, removing all initial whitespace.
         *
         *  @param s    The string to trim.
         */
        inline void left_trim(std::string& s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
        }

        /** @brief      Trims a string from the right, removing all final whitespace.
         *
         *  @param s    The string to trim.
         */
        inline void right_trim(std::string& s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        /** @brief      Trims a string from the left and the right, removing all initial and final whitespace.
         *
         *  @param s    The string to trim.
         */
        inline void trim(std::string& s) {
            left_trim(s);
            right_trim(s);
        }
    }
}

#endif //AS_STRING_H
