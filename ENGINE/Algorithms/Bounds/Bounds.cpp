//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "Bounds.h"

Bounds::Bounds(BoundTypes type)
        : type(type) {}

Bounds::Bounds(glm::vec3 center, float radius)
        : type(BoundTypes::SPHERE), center(center), ogCenter(center), radius(radius), ogRadius(radius) {}

Bounds::Bounds(glm::vec3 min, glm::vec3 max)
        : type(BoundTypes::AABB), min(min), ogMin(min), max(max), ogMax(max) {}

void Bounds::transform() {
    if (instance) {
        if (type == BoundTypes::AABB) {
            min = ogMin * instance->size + instance->pos;
            max = ogMax * instance->size + instance->pos;
        }
        else {
            center = ogCenter * instance->size + instance->pos;

            float maxDim = instance->size[0];
            for (int i = 1; i < 3; i++) {
                if (instance->size[i] > maxDim) {
                    maxDim = instance->size[i];
                }
            }

            radius = ogRadius * maxDim;
        }
    }
}

glm::vec3 Bounds::calculateCenter() {
    return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

glm::vec3 Bounds::calculateDimensions() {
    return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

bool Bounds::containsPoint(glm::vec3 pt) {
    if (type == BoundTypes::AABB) {
        return (pt.x >= min.x) && (pt.x <= max.x) &&
               (pt.y >= min.y) && (pt.y <= max.y) &&
               (pt.z >= min.z) && (pt.z <= max.z);
    }
    else {
        float distSquared = 0.0f;
        for (int i = 0; i < 3; i++) {
            distSquared += (center[i] - pt[i]) * (center[i] - pt[i]);
        }
        return distSquared <= (radius * radius);
    }
}

bool Bounds::containsRegion(Bounds br) {
    if (br.type == BoundTypes::AABB) {
        return containsPoint(br.min) && containsPoint(br.max);
    }
    else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        return glm::length(center - br.center) + br.radius < radius;
    }
    else {
        if (!containsPoint(br.center)) {
            return false;
        }
        for (int i = 0; i < 3; i++) {
            if (abs(max[i] - br.center[i]) < br.radius ||
                abs(br.center[i] - min[i]) < br.radius) {
                return false;
            }
        }
        return true;
    }
}

bool Bounds::intersectsWith(Bounds br) {
    if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
        glm::vec3 rad = calculateDimensions() / 2.0f;
        glm::vec3 radBr = br.calculateDimensions() / 2.0f;

        glm::vec3 center = calculateCenter();
        glm::vec3 centerBr = br.calculateCenter();

        glm::vec3 dist = abs(center - centerBr);

        for (int i = 0; i < 3; i++) {
            if (dist[i] > rad[i] + radBr[i]) {
                return false;
            }
        }
        return true;
    }
    else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        glm::vec3 centerDiff = center - br.center;
        float distSquared = 0.0f;
        for (int i = 0; i < 3; i++) {
            distSquared += centerDiff[i] * centerDiff[i];
        }
        float maxMagSquared = radius + br.radius;
        maxMagSquared *= maxMagSquared;
        return distSquared <= maxMagSquared;
    }
    else if (type == BoundTypes::SPHERE) {
        float distSquared = 0.0f;
        for (int i = 0; i < 3; i++) {
            float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
            distSquared += (closestPt - center[i]) * (closestPt - center[i]);
        }

        return distSquared < (radius * radius);
    }
    else {
        return br.intersectsWith(*this);
    }
}

bool Bounds::operator==(Bounds br) {
    if (type != br.type)
        return false;
    if (type == BoundTypes::AABB)
        return min == br.min && max == br.max;
    else
        return center == br.center && radius == br.radius;
}