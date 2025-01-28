#include "vector_math.h"
#include <math.h>

// Computes the magnitude (length) of a vector
float MagnitudeVector2(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

// Normalizes a vector (scales it to a unit length)
Vector2 NormalizeVector2(Vector2 v) {
    float magnitude = MagnitudeVector2(v);
    if (magnitude == 0) return (Vector2){0, 0}; // Handle zero vector to avoid division by zero
    return (Vector2){v.x / magnitude, v.y / magnitude};
}

// Scales a vector by a scalar value
Vector2 ScaleVector2(Vector2 v, float scale) {
    return (Vector2){v.x * scale, v.y * scale};
}

// Returns a zero vector
Vector2 ZeroVector2() {
    return (Vector2){0, 0};
}

Vector2 ReflectVector2(Vector2 v, Vector2 normal) {
    float dotProduct = (v.x * normal.x) + (v.y * normal.y); // Calculate V · N
    return ScaleVector2(
        (Vector2){v.x - 2.0f * dotProduct * normal.x, v.y - 2.0f * dotProduct * normal.y},
        1.0f // Maintain consistent scaling structure
    );
}