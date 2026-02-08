#include "game_functions.h"

uint64_t il2cpp_get_handle(Kmem& mem, uint64_t game_assembly, uint64_t handle) {
    uint64_t rdi_1 = (uint64_t)((int32_t)handle >> 3);
    uint64_t rcx_1 = (uint64_t)(((int32_t)handle & 7) - 1);
    
    uintptr_t ObjectArray = mem.read<uintptr_t>((rcx_1 * 0x28) + (game_assembly + 0xd4c3cf0 + 0x8)) + (rdi_1 << 3);
    
    if (mem.read<uint8_t>((rcx_1 * 0x28) + (game_assembly + 0xd4c3cf0 + 0x14)) > 1) {
        return mem.read<uintptr_t>(ObjectArray);
    } else {
        uint32_t eax = mem.read<uint32_t>(ObjectArray);
        eax = ~eax;
        return eax;
    }
}

std::uintptr_t client_entities(Kmem& mem, std::uintptr_t a1, uint64_t game_assembly) {
    std::uint32_t r8d = 0, eax = 0, ecx = 0;
    std::uintptr_t rax = mem.read<std::uintptr_t>(a1 + 0x18);
    std::uintptr_t* rdx = &rax;
    
    r8d = 0x02;
    do {
        ecx = *(std::uint32_t*)(rdx);
        eax = *(std::uint32_t*)(rdx);
        rdx = (std::uintptr_t*)((char*)rdx + 0x04);
        eax <<= 0x0F;
        ecx >>= 0x11;
        ecx |= eax;
        ecx += 0xE5766956;
        eax = ecx;
        ecx <<= 0x06;
        eax >>= 0x1A;
        eax |= ecx;
        *((std::uint32_t*)rdx - 1) = eax;
        r8d -= 0x01;
    } while (r8d);

    return il2cpp_get_handle(mem, game_assembly, rax);
}

std::uintptr_t get_entity_list(Kmem& mem, std::uintptr_t a1, uint64_t game_assembly) {
    std::uint32_t r8d = 0, eax = 0, ecx = 0;
    std::uintptr_t rax = mem.read<std::uintptr_t>(a1 + 0x18);
    std::uintptr_t* rdx = &rax;
    
    r8d = 0x02;
    do {
        ecx = *(std::uint32_t*)(rdx);
        rdx = (std::uintptr_t*)((char*)rdx + 0x04);
        ecx ^= 0x5923674C;
        eax = ecx;
        ecx <<= 0x06;
        eax >>= 0x1A;
        eax |= ecx;
        eax += 0xA293B27A;
        ecx = eax;
        eax <<= 0x18;
        ecx >>= 0x08;
        ecx |= eax;
        *((std::uint32_t*)rdx - 1) = ecx;
        r8d -= 0x01;
    } while (r8d);

    return il2cpp_get_handle(mem, game_assembly, rax);
}

vec3 GetTransformPosition(Kmem& mem, uintptr_t transform) {
    if (!transform) return vec3();
    
    int index = mem.read<int>(transform + 0x40);
    uintptr_t pTransformData = mem.read<uintptr_t>(transform + 0x38);
    if (!pTransformData) return vec3();
    
    uintptr_t matrixBase = mem.read<uintptr_t>(pTransformData + 0x18);
    uintptr_t indexBase = mem.read<uintptr_t>(pTransformData + 0x20);
    if (!matrixBase || !indexBase) return vec3();
    
    __m128 result = mem.read<__m128>(matrixBase + 0x30 * index);
    int transformIndex = mem.read<int>(indexBase + 0x4 * index);
    
    while (transformIndex >= 0) {
        struct Matrix34Local {
            BYTE vec0[16];
            BYTE vec1[16];
            BYTE vec2[16];
        };
        
        Matrix34Local matrix34 = mem.read<Matrix34Local>(matrixBase + 0x30 * transformIndex);
        
        const __m128 mulVec0 = { -2.f, 2.f, -2.f, 0.f };
        const __m128 mulVec1 = { 2.f, -2.f, -2.f, 0.f };
        const __m128 mulVec2 = { -2.f, -2.f, 2.f, 0.f };
        
        __m128 vec1 = *reinterpret_cast<const __m128*>(&matrix34.vec1);
        __m128 vec2 = *reinterpret_cast<const __m128*>(&matrix34.vec2);
        __m128 vec0 = *reinterpret_cast<const __m128*>(&matrix34.vec0);
        
        __m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x00));
        __m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x55));
        __m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x8E));
        __m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0xDB));
        __m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0xAA));
        __m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x71));
        
        __m128 tmp7 = _mm_mul_ps(vec2, result);
        
        result = _mm_add_ps(
            _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
                                   _mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
                                   _mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
                                   _mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
                    tmp7)),
            vec0);
        
        transformIndex = mem.read<int>(indexBase + 0x4 * transformIndex);
    }
    
    return vec3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

vec3 GetBonePosition(Kmem& mem, uintptr_t player, int boneIndex) {
    uintptr_t playerModel = mem.read<uintptr_t>(player + 0xd0);
    if (!playerModel) return vec3();
    
    uintptr_t boneTransforms = mem.read<uintptr_t>(playerModel + 0x50);
    if (!boneTransforms) return vec3();
    
    uintptr_t entityBone = mem.read<uintptr_t>(boneTransforms + 0x20 + boneIndex * 0x8);
    if (!entityBone) return vec3();
    
    uintptr_t transform = mem.read<uintptr_t>(entityBone + 0x10);
    if (!transform) return vec3();
    
    return GetTransformPosition(mem, transform);
}

vec2 WorldToScreen(const vec3& world_pos, const Matrix4x4& view_matrix, int screenWidth, int screenHeight) {
    // W calculation: dot column 4 with position
    float w = view_matrix._14 * world_pos.x +
              view_matrix._24 * world_pos.y +
              view_matrix._34 * world_pos.z +
              view_matrix._44;

    if (w < 0.097f)
        return vec2(-999, -999);

    // X calculation: dot column 1 with position
    float x = view_matrix._11 * world_pos.x +
              view_matrix._21 * world_pos.y +
              view_matrix._31 * world_pos.z +
              view_matrix._41;

    // Y calculation: dot column 2 with position  
    float y = view_matrix._12 * world_pos.x +
              view_matrix._22 * world_pos.y +
              view_matrix._32 * world_pos.z +
              view_matrix._42;

    vec2 screen;
    screen.x = (screenWidth * 0.5f) * (1.0f + x / w);
    screen.y = (screenHeight * 0.5f) * (1.0f - y / w);

    return screen;
}

Matrix4x4 Transpose(const Matrix4x4& mat) {
    Matrix4x4 t;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t.m[i][j] = mat.m[j][i];
    return t;
}
