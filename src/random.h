//
// Created by alberto on 02/11/17.
//

#ifndef AS_RANDOM_H_H
#define AS_RANDOM_H_H

#include <random>
#include <functional>
#include <algorithm>
#include "containers.h"

namespace as {
    /** @brief  Gets a properly seeded Mersenne Twister prng.
     *
     *  @return A seeded instance of std::mt19937.
     */
    inline std::mt19937 get_seeded_mt() {
        std::array<std::mt19937::result_type,std::mt19937::state_size> random_data;
        std::random_device random_source;

        std::generate(random_data.begin(), random_data.end(), std::ref(random_source));

        std::seed_seq seeds(random_data.begin(), random_data.end());

        return std::mt19937(seeds);
    }

    /** @brief  Gets samples from a container. The samples are guaranteed
     *          to be unique. If we are requesting more samples than elements
     *          in the container, the request is ignored, and instead a
     *          random permutation of the container is returned.
     *
     *          A copy of the input container is made, so this method might
     *          be unsuitable for particularly large containers.
     *
     *  @tparam Container    The container type.
     *                       Must be copiable, implement begin() and end(),
     *                       and constructible from a pair of iterators.
     *  @tparam Prng         The pseudo-random number generator type to use.
     *  @param container     The container to sample.
     *  @param how_many      The number of samples to extract.
     *  @param prng          The pseudo-random number generator.
     *  @return              A container with \ref how_many elements from \ref container.
     */
    template<class Container, class Prng = std::mt19937>
    inline Container sample(const Container container, typename Container::size_type how_many, Prng&& prng) {
        Container container_copy(container);
        auto length = std::distance(container_copy.begin(), container_copy.end());

        if(length == 0u) {
            return container_copy;
        }

        if(how_many > length) {
            how_many = length;
        }

        auto begin = container_copy.begin();
        auto n_samples = how_many;

        while(n_samples--) {
            auto piv = begin;
            auto dist = std::uniform_int_distribution<typename Container::size_type>(0u, length - 1u);

            std::advance(piv, dist(prng));
            std::swap(*begin, *piv);

            ++begin;
            --length;
        }

        return Container(container_copy.begin(), container_copy.begin() + how_many);
    }

    /** @brief  Gets samples from a container. The samples are guaranteed
     *          to be unique. If we are requesting more samples than elements
     *          in the container, the request is ignored, and instead a
     *          random permutation of the container is returned.
     *
     *          A copy of the input container is made, so this method might
     *          be unsuitable for particularly large containers.
     *          A Mersenne Twister pseudo-random number generator is built,
     *          seeded and used to extract the samples.
     *
     *  @tparam Container    The container type.
     *                      Must be copiable, implement begin() and end(),
     *                      and constructible from a pair of iterators.
     *  @param container     The container to sample.
     *  @param how_many      The number of samples to extract.
     *  @return              A container with \ref how_many elements from \ref container.
     */
    template<class Container>
    inline Container sample(const Container container, typename Container::size_type how_many) {
        return sample(container, how_many, get_seeded_mt());
    }
}

#endif //AS_RANDOM_H_H
