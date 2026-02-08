#pragma once
#include "common.h"
#include "mem.h"

// IL2CPP handle decryption
uint64_t il2cpp_get_handle(Kmem& mem, uint64_t game_assembly, uint64_t handle);

// Entity decryption functions
std::uintptr_t client_entities(Kmem& mem, std::uintptr_t a1, uint64_t game_assembly);
std::uintptr_t get_entity_list(Kmem& mem, std::uintptr_t a1, uint64_t game_assembly);

// Transform functions
vec3 GetTransformPosition(Kmem& mem, uintptr_t transform);
vec3 GetBonePosition(Kmem& mem, uintptr_t player, int boneIndex);

// World to screen conversion
vec2 WorldToScreen(const vec3& world_pos, const Matrix4x4& view_matrix, int screenWidth, int screenHeight);

// Matrix helper
Matrix4x4 Transpose(const Matrix4x4& mat);
