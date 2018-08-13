//
// Created by alberto on 02/11/17.
//

#ifndef AS_RANDOM_H_H
#define AS_RANDOM_H_H

#include <random>
#include <functional>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include "containers.h"

namespace as {
    /** @namespace rnd
     *  @brief     This namespace contains utilities related to random numbers, their generators, and their applications.
     */
    namespace rnd {
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
         *                       Must be copiable, usable with std::begin() and std::end(),
         *                       and constructible from a pair of iterators.
         *  @tparam Prng         The pseudo-random number generator type to use.
         *  @param container     The container to sample.
         *  @param how_many      The number of samples to extract.
         *  @param prng          The pseudo-random number generator.
         *  @return              A container with \ref how_many elements from \ref container.
         */
        template<class Container, class Prng = std::mt19937>
        inline Container sample(const Container container, typename Container::size_type how_many, Prng&& prng) {
            static_assert(std::is_copy_constructible<Container>::value, "Container needs to be copy constructible");

            Container container_copy(container);
            auto length = std::distance(std::begin(container_copy), std::end(container_copy));

            assert(length >= 0);

            if(length == 0u) {
                return container_copy;
            }

            if(how_many > static_cast<typename Container::size_type>(length)) {
                how_many = length;
            }

            auto begin = std::begin(container_copy);
            auto n_samples = how_many;

            while(n_samples--) {
                auto piv = begin;
                auto dist = std::uniform_int_distribution<typename Container::size_type>(0u, length - 1u);

                std::advance(piv, dist(prng));
                std::swap(*begin, *piv);

                ++begin;
                --length;
            }

            return Container(std::begin(container_copy), std::begin(container_copy) + how_many);
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
         *                       Must be copiable, usable with std::begin() and std::end(),
         *                       and constructible from a pair of iterators.
         *  @param container     The container to sample.
         *  @param how_many      The number of samples to extract.
         *  @return              A container with \ref how_many elements from \ref container.
         */
        template<class Container>
        inline Container sample(const Container container, typename Container::size_type how_many) {
            return sample(container, how_many, get_seeded_mt());
        }

        /** @brief  Selects a position in a vector of floating-point numbers according
         *          to a roulette-wheel criterion.
         *
         *          In a roulette-wheel random choice, each position has a probability
         *          to be chosen proportional to the weight at that position.
         *
         *  @tparam FloatingPoint   A floating-point type (e.g. float, double).
         *  @tparam Prng            The type of the pseudo-random number generator.
         *  @param  weights         The non-empty vector with weights.
         *  @param  prng            The pseudo-random number generator.
         *  @return                 An index of the vector of \ref weights.
         */
        template<class FloatingPoint, class Prng = std::mt19937>
        inline typename std::vector<FloatingPoint>::size_type roulette_wheel(
                const std::vector<FloatingPoint>& weights,
                Prng&& prng
        ) {
            static_assert(std::is_floating_point<FloatingPoint>::value, "FloatingPoint needs to be a floating point type");

            // Roulette wheel selection only works if all weights are non-negative.
            assert(std::all_of(weights.begin(), weights.end(), [](auto w){return w >= 0;}));

            // There needs to be at least one element in the vector.
            assert(!weights.empty());

            const FloatingPoint sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
            std::uniform_real_distribution<FloatingPoint> dist(0, sum);
            const FloatingPoint pivot = dist(prng);

            FloatingPoint partial = 0;

            for(typename std::vector<FloatingPoint>::size_type i = 0u; i < weights.size() - 1u; ++i) {
                partial += weights[i];

                if(partial >= pivot) {
                    return i;
                }
            }

            return weights.size() - 1u;
        }

        /** @brief  Selects a position in a vector of floating-point numbers according
         *          to a roulette-wheel criterion.
         *
         *          In a roulette-wheel random choice, each position has a probability
         *          to be chosen proportional to the weight at that position.
         *          A Mersenne Twister pseudo-random number generator is built,
         *          seeded and used for the selection.
         *
         *  @tparam FloatingPoint   A floating-point type (e.g. float, double).
         *  @param  weights         The non-empty vector with weights.
         *  @return                 An index of the vector of \ref weights.
         */
        template<class FloatingPoint>
        inline typename std::vector<FloatingPoint>::size_type roulette_wheel(
                const std::vector<FloatingPoint>& weights
        ) {
            return roulette_wheel(weights, get_seeded_mt());
        }
    }
}

#endif //AS_RANDOM_H_H
