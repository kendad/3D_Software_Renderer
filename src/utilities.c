#include "utilities.h"

float min(float a, float b) { return (a < b) ? a : b; }
float max(float a, float b) { return (a > b) ? a : b; }
float lerp(float a, float b, float t) { return a + (t * (b - a)); }
