//
// Created by Anhelina Modenko on 07.10.2023.
//

#ifndef MARCHING_CUBES_LINEARALGEBRA_H
#define MARCHING_CUBES_LINEARALGEBRA_H

#include <glm/glm.hpp>


enum class PlaneIntersection : char {
    CASE0 = 0x00, // line lies in plane
    CASE1 = 0x01, // no planar intersection
    CASE2 = 0x02,  // planar intersection between points 1 & 2
    CASE3 = 0x03 // planar intersection outside of 1 & 2
};

class LinearAlgebra {
public:
    template <int T>
    static float dot(glm::vec<T, float, glm::packed_highp> vec) {
        float result = 0.0f;

        for (int i = 0; i < T; i++)
            result += vec[i] * vec[i];

        return result;
    }

    template <int C, int R>
    static void rref(glm::mat<C, R, float>& m) {
        unsigned int currentRow = 0;
        for (unsigned int c = 0; c < C; c++) {
            unsigned int r = currentRow;
            if (r >= R)
                break;

            for (; r < R; r++)
                if (m[c][r] != 0.0f)
                    break;

            if (r == R)
                continue;

            if (r != currentRow) {
                for (unsigned int i = 0; i < C; i++) {
                    float tmp = m[i][currentRow];
                    m[i][currentRow] = m[i][r];
                    m[i][r] = tmp;
                }
            }

            if (m[c][currentRow] != 0.0f) {
                float k = 1 / m[c][currentRow];
                m[c][currentRow] = 1.0f;
                for (unsigned int col = c + 1; col < C; col++)
                    m[col][currentRow] *= k;
            }

            for (r = 0; r < R; r++) {
                if (r == currentRow)
                    continue;
                float k = -m[c][r];
                for (unsigned int i = 0; i < C; i++)
                    m[i][r] += k * m[i][currentRow];
            }
            currentRow++;
        }
    }

    static PlaneIntersection lineIntersectPlane(glm::vec3 P1, glm::vec3 norm, glm::vec3 U1, glm::vec3 side, float& t) {
        glm::vec3 U1P1 = P1 - U1;

        float tnum = glm::dot(norm, U1P1);
        float tden = glm::dot(norm, side);

        if (tden == 0.0f)
            return tnum == 0.0f ? PlaneIntersection::CASE0 : PlaneIntersection::CASE1;
        else {
            t = tnum / tden;
            return t >= 0.0f && t <= 1.0f ? PlaneIntersection::CASE2 : PlaneIntersection::CASE3;
        }
    }

    static glm::vec3 mat4vec3mult(glm::mat4& m, glm::vec3& v) {
        glm::vec3 result;
        for (int i = 0; i < 3; i++)
            result[i] = v[0] * m[0][i] + v[1] * m[1][i] + v[2] * m[2][i] + m[3][i];
        return result;
    }

    static glm::vec3 linCombSolution(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 point) {
        glm::mat4x3 m(A, B, C, point);
        rref<4, 3>(m);
        return m[3];
    }

    static bool faceContainsPointRange(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point, float radius) {
        glm::vec3 c = linCombSolution(A, B, N, point);
        return c[0] >= -radius && c[1] >= -radius && c[0] + c[1] <= 1.0f + radius;
    }

    static bool faceContainsPoint(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point) {
        return faceContainsPointRange(A, B, N, point, 0.0f);
    }
};





#endif //MARCHING_CUBES_LINEARALGEBRA_H
