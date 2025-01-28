#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include "raylib.h"

// Custom Vector Math Functions
float MagnitudeVector2(Vector2 v);
Vector2 NormalizeVector2(Vector2 v);
Vector2 ScaleVector2(Vector2 v, float scale);
Vector2 ZeroVector2();
Vector2 ReflectVector2(Vector2 v, Vector2 normal);


#endif
