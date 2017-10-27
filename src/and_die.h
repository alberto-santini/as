//
// Created by alberto on 15/10/17.
//

#ifndef AS_AND_DIE_H
#define AS_AND_DIE_H

#include <ostream>
#include <cstdlib>

namespace as {
    /** @struct and_die
     *  @brief  A struct used to signal that the programme should be terminated immediately (die).
     */
    struct and_die {};

    /** @brief  Concatenates a \ref and_die structure to an output stream, to immediately
     *          terminate a programme.
     *
     *  When concatenating an object of type and_die to an ostream, the programme is going
     *  to terminate with exit code EXIT FAILURE. This can be used in the following way:
     *
     *  using namespace as;
     *  std::cerr << console::error << "A terrible error has occurred!" << and_die();
     *
     *  It adds a new line before terminating the programme.
     *
     *  @param out The output stream currently used.
     *  @return    In theory the same output stream, but in practice the programme dies and nothing is returned.
     */
    inline std::ostream& operator<<(std::ostream& out, const and_die&) {
        out << std::endl;
        std::exit(EXIT_FAILURE);
        return out; // You are not gong to see this!
    }
}

#endif //AS_AND_DIE_H
