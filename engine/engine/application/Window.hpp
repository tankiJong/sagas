#pragma once
#include <cstdint>
#include <string_view>
#include <concepts>

#include "engine/core/util.hpp"
#include "engine/math/primitives.hpp"

class Window: public WithHandle<void*>
{
public:
   void Init(uint2 pixelSize, std::string_view name);

   static Window& Get(uint32_t index = 0);

protected:
   uint2 mWindowSize = {};
};
