#pragma once

// SU 타입을 unordered_map 의 키로 쓰기 위한 코드
// https://stackoverflow.com/questions/8157937/how-to-specialize-stdhashkeyoperator-for-user-defined-type-in-unordered

namespace std {
    template<> struct hash<SUMaterialRef> {
        size_t operator()(const SUMaterialRef& x) const {
            return size_t(x.ptr);
        }
    };
    template<> struct equal_to<SUMaterialRef> {
        constexpr bool operator()(const SUMaterialRef &lhs, const SUMaterialRef &rhs) const
        {
            return lhs.ptr == rhs.ptr;
        }
    };

    template<> struct hash<SUGroupRef> {
        size_t operator()(const SUGroupRef& x) const {
            return size_t(x.ptr);
        }
    };
    template<> struct equal_to<SUGroupRef> {
        constexpr bool operator()(const SUGroupRef &lhs, const SUGroupRef &rhs) const
        {
            return lhs.ptr == rhs.ptr;
        }
    };

    template<> struct hash<SUTextureRef> {
        size_t operator()(const SUTextureRef& x) const {
            return size_t(x.ptr);
        }
    };
    template<> struct equal_to<SUTextureRef> {
        constexpr bool operator()(const SUTextureRef &lhs, const SUTextureRef &rhs) const
        {
            return lhs.ptr == rhs.ptr;
        }
    };

    template<> struct hash<SULayerRef> {
        size_t operator()(const SULayerRef& x) const {
            return size_t(x.ptr);
        }
    };
    template<> struct equal_to<SULayerRef> {
        constexpr bool operator()(const SULayerRef &lhs, const SULayerRef &rhs) const
        {
            return lhs.ptr == rhs.ptr;
        }
    };
}
