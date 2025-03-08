#include "Vec3.h"
#include "iostream"

extern Vec3 addVec3(const Vec3&, const Vec3&);
extern float dotVec3(const Vec3&, const Vec3&);

int main()  {
    Vec3 v1 {1.0f, 2.0f, 3.0f};
    Vec3 v2 {4.0f, 5.0f, 6.0f};

    Vec3 resultAdd = addVec3(v1, v2);
    std::cout << "Add: (" << resultAdd.x << ", "
    << resultAdd.y << ", "
    << resultAdd.z << ")\n";

    float dot = dotVec3(v1, v2);
    std::cout << "Dot: " << dot << "\n";
    return 0;
}