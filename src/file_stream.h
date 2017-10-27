//
// Created by alberto on 17/10/17.
//

#ifndef AS_FILE_STREAM_H_H
#define AS_FILE_STREAM_H_H

#include <cstddef>
#include <fstream>
#include <limits>

namespace as {
    /** @brief  Skips a certain number of lines from an input file stream.
     *
     *  @param stream    The file stream.
     *  @param how_many  Number of lines to skip.
     */
    inline void skip_lines(std::ifstream& stream, std::size_t how_many = 1u) {
        for(auto i = 0u; i < how_many; ++i) {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    };
}

#endif //AS_FILE_STREAM_H_H
