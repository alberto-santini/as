//
// Created by alberto on 08/05/18.
//

#ifndef AS_REPEAT_H
#define AS_REPEAT_H

namespace as {
    /** @brief Executes a function a set number of times.
     *
     *  @tparam Function    The function type.
     *  @param times        The number of times
     *  @param function     The function.
     */
    template<class Function>
    void repeat(std::size_t times, const Function& function) {
        while(times--) function();
    }
}

#endif //AS_REPEAT_H
