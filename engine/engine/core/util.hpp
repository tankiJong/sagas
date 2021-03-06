#pragma once
#include <cstdint>
#include <memory>
#include <concepts>
// Adopted from Falcor

// This is a helper class which should be used in case a class derives from a base class which derives from enable_shared_from_this
// If Derived will also inherit enable_shared_from_this, it will cause multiple inheritance from enable_shared_from_this, which results in a runtime errors because we have 2 copies of the WeakPtr inside shared_ptr
template<typename Base, typename Derived>
class inherit_shared_from_this {
public:
  std::shared_ptr<Derived> shared_from_this() {
    Base* pBase = static_cast<Derived*>(this);
    std::shared_ptr<Base> pShared = pBase->shared_from_this();
    return std::static_pointer_cast<Derived>(pShared);
  }

  typename std::shared_ptr<const Derived> shared_from_this() const {
    const Base* pBase = static_cast<const Derived*>(this);
    std::shared_ptr<const Base> pShared = pBase->shared_from_this();
    return std::static_pointer_cast<const Derived>(pShared);
  }
};

struct NonCopyable
{
   NonCopyable() = default;
   NonCopyable( const NonCopyable& ) = delete;
   auto operator=( const NonCopyable& ) = delete;
};

template<typename H>
concept Handle = requires(H h)
{
   std::is_trivially_copyable<H>::value;
};

template< Handle HandleT >
class WithHandle {
public:
   WithHandle(const WithHandle& other) = default;

   WithHandle(WithHandle&& other) noexcept
      : mHandle(std::move(other.mHandle)) {}

   WithHandle& operator=(const WithHandle& other) = default;

   WithHandle& operator=(WithHandle&& other) noexcept
   {
      if (this == &other)
         return *this;
      std::swap(mHandle, other.mHandle);
      return *this;
   }

   WithHandle() = default;
   WithHandle(HandleT handle) : mHandle(handle) {}

   const HandleT& Handle() const { return mHandle; }
   HandleT& Handle() { return mHandle; }

   template<typename = decltype(HandleT()->SetName(L""))>
   void SetName(const wchar_t* name)
   {
      mHandle->SetName(name);
   }
protected:
   HandleT mHandle;
};
#define KB * 1024
#define MB KB * 1024
#define GB MB * 1024

#define SAFE_DELETE(p) if((p)) { delete (p); (p) = nullptr; } 

#define BIT_FLAG(f) (1U<<(f))

#define align_to(_alignment, _val) (((_val + _alignment - 1) / _alignment) * _alignment)

#define enum_class_operators(e_) inline e_ operator& (e_ a, e_ b){return static_cast<e_>(static_cast<int>(a)& static_cast<int>(b));}  \
    inline e_ operator| (e_ a, e_ b){return static_cast<e_>(static_cast<int>(a)| static_cast<int>(b));} \
    inline e_& operator|= (e_& a, e_ b){a = a | b; return a;};  \
    inline e_& operator&= (e_& a, e_ b) { a = a & b; return a; };   \
    inline e_  operator~ (e_ a) { return static_cast<e_>(~static_cast<int>(a));}   \
    inline bool is_any_set(e_ val, e_ flag) { return (val & flag) != (e_)0;} \
    inline bool is_all_set(e_ val, e_ flag) { return (val & flag) == flag;}   

#define UNUSED(X) (void*)&X;

#define ___APPEND_IMPL(a, b) a##b
#define APPEND(a, b) ___APPEND_IMPL(a, b)

namespace detail
{
   struct __Static_Block {
      template<typename T>
      __Static_Block(T&& fn)
      {
         fn();
      }
   };
}

#define STATIC_BLOCK static detail::__Static_Block APPEND(__STATIC__, __LINE__) = [&]() -> void