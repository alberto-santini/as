//
// Created by Alberto Santini on 15/10/17.
//

#ifndef AS_CONSOLE_H
#define AS_CONSOLE_H

#include <string>
#include <ostream>
#include <type_traits>
#include <sstream>

namespace as {
    /**
     * This namespace contains miscellaneous console utilities.
     */
    namespace console {
        /**
         * Console colour codes.
         */
        enum class Colour {
            Red = 31,
            Green = 32,
            Yellow = 33,
            Blue = 34,
            Magenta = 35,
            Default = 39
        };

        /**
         * Gives the numerical code corresponding to the colour.
         *
         * @param c The colour.
         * @return  The corresponding numerical code.
         */
        constexpr typename std::underlying_type<Colour>::type colour_code(const Colour& c) {
            return static_cast<typename std::underlying_type<Colour>::type>(c);
        }

        /**
         * Gives a string with the escape codes relative to the colour.
         *
         * @param c The colour.
         * @return  The corresponding string with escape codes.
         */
        inline std::string escape(Colour c) {
            return "\033[" + std::to_string(colour_code(c)) + "m";
        }

        /**
         * Prints a colour code. From this moment on, the console output will be in that colour,
         * until another colour is used (including, e.g., Colour::Default, which resets it).
         *
         * @param out   The console output iterator.
         * @param c     The colour.
         * @return      The console output iterator (for chaining).
         */
        inline std::ostream& operator<<(std::ostream& out, const Colour& c) {
            return out << escape(c);
        }

        /**
         * Colours some content and then resets the colour to default.
         *
         * @tparam T    Content type. It must be possible to input it into a stringstream via operator<<.
         * @param c     The colour.
         * @param what  The content to colour.
         * @return      A string with the proper escape characters to colourise the content.
         */
        template<class T>
        inline std::string colourise(Colour c, const T& what) {
            std::stringstream ss;
            ss << escape(c) << what << escape(Colour::Default);
            return ss.str();
        }

        /**
         * String that can be used to start a notice message.
         */
        static const std::string notice = colourise(Colour::Green, "[o] ");

        /**
         * String that can be used to start a warning message.
         */
        static const std::string warning = colourise(Colour::Yellow, "[*] ");

        /**
         * String that can be used to start an error message.
         */
        static const std::string error = colourise(Colour::Red, "[!] ");
    }
}

#endif //AS_CONSOLE_H
