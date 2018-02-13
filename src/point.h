//
// Created by alberto on 21/11/17.
//

#ifndef AS_POINT_H
#define AS_POINT_H

#include <cmath>

namespace as {
    /** @struct TwoDimPoint
     *  @brief  Simple named pair of floats to represent a 2D-point.
     */
    struct TwoDimPoint {
        /** @brief  The x coordinate.
         */
        float x;

        /** @brief The y coordinate.
         */
        float y;
    };

    /** @brief  Returns the Euclidan distance between two 2-dimensional points.
     *
     * @param point1    First point.
     * @param point2    Second point.
     * @return          The euclidean distance between the two points.
     */
    inline float euclidean_distance(const TwoDimPoint& point1, const TwoDimPoint& point2) {
        return std::sqrt(std::pow(point1.x - point2.x, 2.0f) + std::pow(point1.y - point2.y, 2.0f));
    }
}

#endif //AS_POINT_H
