#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <emmintrin.h>
#include <smmintrin.h>

// Math constants
#define M_PI 3.14159265358979323846f
#define M_RADPI 57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))

// Vector structures
struct vec3 {
    float x, y, z;

    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    inline float operator[](int i) const {
        return ((float*)this)[i];
    }

    inline vec3& operator-=(float v) {
        x -= v; y -= v; z -= v;
        return *this;
    }

    inline vec3 operator*(float v) const {
        return vec3(x * v, y * v, z * v);
    }

    inline vec3 operator/(float v) const {
        return vec3(x / v, y / v, z / v);
    }

    inline vec3& operator+=(const vec3& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    inline vec3 operator-(const vec3& v) const {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    inline vec3 operator+(const vec3& v) const {
        return vec3(x + v.x, y + v.y, z + v.z);
    }
};

struct vec2 {
    float x, y;

    vec2() : x(0), y(0) {}
    vec2(float x, float y) : x(x), y(y) {}
};

class Vector4 {
public:
    float x, y, z, w;
};

struct Matrix3x4 {
    Vector4 vec0;
    Vector4 vec1;
    Vector4 vec2;
};

struct Matrix34 {
    Vector4 vec0;
    Vector4 vec1;
    Vector4 vec2;
};

struct Matrix4x4 {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
};

// Bone enumeration
enum BoneList : int {
    l_hip = 1,
    l_knee,
    l_foot,
    l_toe,
    l_ankle_scale,
    pelvis,
    spine1 = 17,
    spine1_scale,
    spine2,
    spine3,
    spine4,
    l_clavicle,
    l_upperarm,
    l_forearm,
    l_hand,
    l_index1,
    l_index2,
    l_index3,
    l_little1,
    l_little2,
    l_little3,
    l_middle1,
    l_middle2,
    l_middle3,
    l_prop,
    l_ring1,
    l_ring2,
    l_ring3,
    l_thumb1,
    l_thumb2,
    l_thumb3,
    IKtarget_righthand_min,
    IKtarget_righthand_max,
    l_ulna,
    neck,
    head,
    jaw,
    eyeTranform,
    l_eye,
    l_Eyelid,
    r_eye,
    r_Eyelid,
    r_clavicle,
    r_upperarm,
    r_forearm,
    r_hand,
    r_index1,
    r_index2,
    r_index3,
    r_little1,
    r_little2,
    r_little3,
    r_middle1,
    r_middle2,
    r_middle3,
    r_prop,
    r_ring1,
    r_ring2,
    r_ring3,
    r_thumb1,
    r_thumb2,
    r_thumb3,
    IKtarget_lefthand_min,
    IKtarget_lefthand_max,
    r_ulna,
    l_breast,
    r_breast
};

// Skeleton bone pairs for ESP drawing
static std::vector<std::pair<BoneList, BoneList>> SkeletonPairs = {
    // Spine
    { spine1, pelvis },
    { spine2, spine1 },
    { spine3, spine2 },
    { spine4, spine3 },
    { neck, spine4 },

    // Left leg
    { l_hip, pelvis },
    { l_knee, l_hip },
    { l_foot, l_knee },
    { l_ankle_scale, l_foot },
    { l_toe, l_foot },

    // Right leg
    { r_hip, pelvis },
    { r_knee, r_hip },
    { r_foot, r_knee },
    { r_ankle_scale, r_foot },
    { r_toe, r_foot },

    // Left arm
    { l_clavicle, spine4 },
    { l_upperarm, l_clavicle },
    { l_forearm, l_upperarm },
    { l_hand, l_forearm },

    // Left fingers
    { l_thumb1, l_hand }, { l_thumb2, l_thumb1 }, { l_thumb3, l_thumb2 },
    { l_index1, l_hand }, { l_index2, l_index1 }, { l_index3, l_index2 },
    { l_middle1, l_hand }, { l_middle2, l_middle1 }, { l_middle3, l_middle2 },
    { l_ring1, l_hand }, { l_ring2, l_ring1 }, { l_ring3, l_ring2 },
    { l_little1, l_hand }, { l_little2, l_little1 }, { l_little3, l_little2 },

    // Right arm
    { r_clavicle, spine4 },
    { r_upperarm, r_clavicle },
    { r_forearm, r_upperarm },
    { r_hand, r_forearm },

    // Right fingers
    { r_thumb1, r_hand }, { r_thumb2, r_thumb1 }, { r_thumb3, r_thumb2 },
    { r_index1, r_hand }, { r_index2, r_index1 }, { r_index3, r_index2 },
    { r_middle1, r_hand }, { r_middle2, r_middle1 }, { r_middle3, r_middle2 },
    { r_ring1, r_hand }, { r_ring2, r_ring1 }, { r_ring3, r_ring2 },
    { r_little1, r_hand }, { r_little2, r_little1 }, { r_little3, r_little2 },
};
