/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HISTREAMER_PLUGIN_COMMON_ANY_H
#define HISTREAMER_PLUGIN_COMMON_ANY_H

#ifndef HST_ANY_WITH_RTTI
#ifndef HST_ANY_WITH_NO_RTTI
#define HST_ANY_WITH_NO_RTTI
#endif
#else
#ifdef HST_ANY_WITH_NO_RTTI
#undef HST_ANY_WITH_NO_RTTI
#endif
#endif

#ifndef MEDIA_NO_OHOS
#ifndef MEDIA_OHOS
#define MEDIA_OHOS
#endif
#else
#ifdef MEDIA_OHOS
#undef MEDIA_OHOS
#endif
#endif

#if defined(__clang__) || defined(__GNUC__)
#define CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define CPP_STANDARD _MSVC_LANG
#endif

#if CPP_STANDARD >= 201103L

#include <array>
#include <cstring>
#include "cpp_ext/type_cast_ext.h"
#include "securec.h"
#include <type_traits>
#include "message_parcel.h"

namespace {
#if CPP_STANDARD < 201402L
template <typename T>
using decay_t = typename std::decay<T>::type;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;

template <typename T>
using remove_cv_t = typename std::remove_cv<T>::type;

template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
#else
using std::decay_t;
using std::enable_if_t;
using std::conditional_t;
using std::remove_cv_t;
using std::remove_reference_t;
#endif

constexpr size_t STACK_STORAGE_SIZE = 2 * sizeof(void*); // NOLINT: global var

template <typename T>
struct IsTrivialStackStorable {
    static constexpr bool value =
        alignof(T) <= alignof(max_align_t) && std::is_trivially_copyable<T>::value && sizeof(T) <= STACK_STORAGE_SIZE;
};

template <typename T>
struct IsStackStorable {
    static constexpr bool value = alignof(T) <= alignof(max_align_t) && std::is_nothrow_move_constructible<T>::value &&
        sizeof(T) <= STACK_STORAGE_SIZE;
};

template <typename T>
struct IsValidCast {
    static constexpr bool value = std::is_reference<T>::value || std::is_copy_constructible<T>::value;
};
} // namespace
namespace OHOS {
namespace Media {

enum struct AnyValueType : int32_t {
    INVALID_TYPE = 1,
    BOOL,
    INT8_T,
    UINT8_T,
    INT32_T,
    UINT32_T,
    INT64_T,
    UINT64_T,
    FLOAT,
    DOUBLE,
    VECTOR_UINT8,
    VECTOR_UINT32,
    STRING,
    VECTOR_INT32,
};
/**
 * @brief BadAnyCast exception, which is thrown when error occurs in AnyCast
 *
 * @since 1.0
 * @version 1.0
 */
class BadAnyCast : public std::bad_cast {
public:
    const char* what() const noexcept override
    {
        return "bad any cast";
    }
};
class Any;
template <typename T>
inline typename std::enable_if<std::is_enum<T>::value, bool>::type MakeAnyFromParcel(Any& value, MessageParcel& parcel);
template <typename T>
inline typename std::enable_if<!std::is_enum<T>::value, bool>::type MakeAnyFromParcel(Any& value, MessageParcel& parcel)
{
    (void)value;
    (void)parcel;
    // current only support enum from parcel
    return false;
}
template <typename T>
inline typename std::enable_if<std::is_enum<T>::value, bool>::type WriteValueToParcel(const T &value,
                                                                                      MessageParcel &parcel)
{
    parcel.WriteInt32(static_cast<int32_t>(value));
    return true;
}
template <typename T>
inline typename std::enable_if<!std::is_enum<T>::value, bool>::type WriteValueToParcel(const T &value,
                                                                                       MessageParcel &parcel)
{
    (void)value;
    (void)parcel;
    // current only support write enum to parcel
    return false;
}
template <typename T>
inline typename std::enable_if<std::is_enum<T>::value, bool>::type WriteValueToParcelInt64(const T &value,
                                                                                           MessageParcel &parcel)
{
    parcel.WriteInt64(static_cast<int64_t>(value));
    return true;
}
template <typename T>
inline typename std::enable_if<!std::is_enum<T>::value, bool>::type WriteValueToParcelInt64(const T &value,
                                                                                            MessageParcel &parcel)
{
    (void)value;
    (void)parcel;
    // current only support write enum to parcel
    return false;
}
/**
 * @brief This class describes a type-safe container for arbitrary type values which are copy constructible.
 *
 * @since 1.0
 * @version 1.0
 */
class Any final {
public:
    constexpr Any() noexcept
    {
    }

    __attribute__((no_sanitize("cfi"))) Any(const Any &other) : functionTable_(other.functionTable_)
    {
        if (other.HasValue()) {
            functionTable_->copy(storage_, other.storage_);
        }
    }

    __attribute__((no_sanitize("cfi"))) Any(Any &&other) noexcept : functionTable_(other.functionTable_)
    {
        if (other.HasValue()) {
            functionTable_->move(storage_, other.storage_);
            other.functionTable_ = nullptr;
        }
    }

    /**
     * constructor from right reference value with type of ValueType.
     *
     * @tparam Type ValueType is not the same as Any itself. The decay type of ValueType must be copy constructible.
     * @param value content
     */
    template <typename ValueType, enable_if_t<!std::is_same<decay_t<ValueType>, Any>::value &&
        std::is_copy_constructible<decay_t<ValueType>>::value,
        bool> = true>
    Any(ValueType&& value) // NOLINT: explicit
    {
        DoEmplace<decay_t<ValueType>>(std::forward<ValueType>(value));
    }

    Any& operator=(const Any& other)
    {
        *this = Any(other);
        return *this;
    }

    Any& operator=(Any&& other) noexcept
    {
        Reset();
        MoveFrom(std::forward<Any>(other));
        return *this;
    }

    /**
     * Assigns contents to Any.
     *
     * @tparam ValueType Type ValueType is not the same as Any itself. The decay type of ValueType must be copy
     * constructible.
     * @param value content
     * @return
     */
    template <typename ValueType, enable_if_t<!std::is_same<decay_t<ValueType>, Any>::value &&
        std::is_copy_constructible<decay_t<ValueType>>::value,
        bool> = true>
    Any& operator=(ValueType&& value)
    {
        *this = Any(std::forward<ValueType>(value));
        return *this;
    }

    /**
     * Get TypeName From function info.
     * @return Name of Type T
     */
    template<typename T>
    static constexpr std::string_view GetTypeName() noexcept
    {
        const char* functionInfo = __PRETTY_FUNCTION__ ;
        return GetTypeNameFromFunctionInfo(functionInfo);
    }

    template<typename T>
    static bool IsSameTypeWith(const Any& other) noexcept
    {
#ifndef HST_ANY_WITH_NO_RTTI
        return other.SameTypeWith(typeid(T));
#else
        return other.SameTypeWith(Any::GetTypeName<T>());
#endif
    }

    ~Any()
    {
        Reset();
    }

    /**
     * Emplace one content with type of ValueType into object. The content is constructed by args.
     *
     * @tparam ValueType The decay type of ValueType must be constructible from args and copy constructible.
     * @tparam Args args type
     * @param args args
     * @return content with type of decay ValueType
     */
    template <typename ValueType, typename... Args,
        enable_if_t<std::is_constructible<decay_t<ValueType>, Args...>::value &&
        std::is_copy_constructible<decay_t<ValueType>>::value,
        bool> = true>
    decay_t<ValueType>& Emplace(Args&&... args)
    {
        Reset();
        return DoEmplace<decay_t<ValueType>>(std::forward<Args>(args)...);
    }

    /**
     * Emplace one content with type of ValueType into object. The content is constructed by il and args.
     *
     * @tparam ValueType type of conetent. The decay type of ValueType must be constructible from il and args and copy
     * constructible
     * @tparam U type of initializer list.
     * @tparam Args type of other args
     * @param il initializer list
     * @param args args
     * @return content with type of decay ValueType
     */
    template <typename ValueType, typename U, typename... Args,
        enable_if_t<std::is_constructible<decay_t<ValueType>, std::initializer_list<U>&, Args...>::value &&
        std::is_copy_constructible<decay_t<ValueType>>::value,
        bool> = true>
    decay_t<ValueType>& Emplace(std::initializer_list<U> il, Args&&... args)
    {
        Reset();
        return DoEmplace<decay_t<ValueType>>(il, std::forward<Args>(args)...);
    }

    /**
     * Destroy the inner content if exists.
     */
    void __attribute__((no_sanitize("cfi"))) Reset() noexcept
    {
        if (HasValue()) {
            functionTable_->destroy(storage_);
            storage_.trivialStack_.fill(0);
        }
        functionTable_ = nullptr;
    }

    /**
     * swap contents of two any objects
     *
     * @param other object to swap with
     */
    void Swap(Any& other) noexcept
    {
        Any tmp(std::move(*this));
        *this = std::move(other);
        other = std::move(tmp);
    }

    /**
     * Checks whether the object has one content.
     *
     * @return true if object has one content, otherwise false.
     */
    bool HasValue() const noexcept
    {
        return IsFunctionTableValid();
    }

#ifndef HST_ANY_WITH_NO_RTTI
    /**
     * Get tye type_info of object
     *
     * @return type info of object
     */
    const std::type_info& Type() const noexcept
    {
        if (!HasValue()) {
            return typeid(void);
        }
        return functionTable_->type();
    }
#else
    std::string_view __attribute__((no_sanitize("cfi"))) TypeName() const noexcept
    {
        if (!HasValue()) {
            return "empty"; // no value
        }
        return functionTable_->type_name();
    }
    bool __attribute__((no_sanitize("cfi"))) ToParcel(MessageParcel &parcel) const noexcept
    {
        if (!HasValue()) {
            return false; // no value
        }
        return functionTable_->toParcel(this, parcel);
    }
    bool __attribute__((no_sanitize("cfi"))) FromParcel(MessageParcel &parcel) const noexcept
    {
        return functionTable_->fromParcel(const_cast<Any *>(this), parcel);
    }
#endif

#ifndef HST_ANY_WITH_NO_RTTI
    bool SameTypeWith(const std::type_info& otherInfo) const noexcept
    {
        if (functionTable_ == nullptr) {
            return false;
        }
        return IsSameType(functionTable_->type(), otherInfo);
    }
#else
    bool __attribute__((no_sanitize("cfi"))) SameTypeWith(std::string_view otherTypeName) const noexcept
    {
        if (functionTable_ == nullptr) {
            return false;
        }
        return IsSameType(functionTable_->type_name(), otherTypeName);
    }
#endif

    bool __attribute__((no_sanitize("cfi"))) SameTypeWith(const Any &other) const noexcept
    {
#ifndef HST_ANY_WITH_NO_RTTI
        return IsSameType(functionTable_->type(), other.Type());
#else
        return IsSameType(functionTable_->type_name(), other.TypeName());
#endif
    }

private:
    template <typename T>
    friend const T* AnyCast(const Any* operand) noexcept;
    template <typename T>
    friend T* AnyCast(Any* operand) noexcept;
    template <typename T>
    friend bool AnyCast(const Any* operand, T& value) noexcept;

    union Storage {
        using Stack = std::aligned_storage<STACK_STORAGE_SIZE, std::alignment_of<void*>::value>::type;
        using Heap = void*;

        std::array<uint8_t, STACK_STORAGE_SIZE> trivialStack_;
        Stack nonTrivialStack_;
        Heap heap_;
    };

    struct FunctionTable {
#ifndef HST_ANY_WITH_NO_RTTI
        const std::type_info& (*type)() noexcept;
#else
        std::string_view (*type_name)() noexcept;
        bool (*toParcel)(const Any *operand, MessageParcel& parcel) noexcept;
        bool (*fromParcel)(Any *operand, MessageParcel& parcel) noexcept;
#endif
        void (*destroy)(Storage&) noexcept;
        void (*copy)(Storage&, const Storage&) noexcept;
        void (*move)(Storage&, Storage&) noexcept;
        void* (*getPtr)(Storage&) noexcept;
        const void* (*getConstPtr)(const Storage&) noexcept;
    };
    static bool BaseTypesToParcel(const Any *operand, MessageParcel& parcel) noexcept;
    // returnValue : 0 -- success; 1 -- retry enum; 2 -- failed no retry
    static int BaseTypesFromParcel(Any *operand, MessageParcel& parcel) noexcept;

    static std::string_view GetTypeNameFromFunctionInfo(const char* functionInfo) noexcept;

    template <typename T>
    struct TrivialStackFunctionTable {
#ifndef HST_ANY_WITH_NO_RTTI
        static const std::type_info& Type() noexcept
        {
            return typeid(T);
        }
#else
        static std::string_view TypeName() noexcept
        {
            return GetTypeName<T>();
        }
        // Only support parcel enum value to int32_t except base types
        static bool ToParcel(const Any *operand, MessageParcel& parcel) noexcept
        {
            if (BaseTypesToParcel(operand, parcel)) {
                return true;
            }
            if (sizeof(T) > sizeof(int64_t)) { // Only support enum
                return false;
            }
            if (sizeof(T) == sizeof(int64_t)) { //  support Int64 enum
                T value;
                if (AnyCast(operand, value)) {
                    WriteValueToParcelInt64(value, parcel);
                    return true;
                }
                return false;
            }
            T value;
            if (AnyCast(operand, value)) {
                WriteValueToParcel(value, parcel);
                return true;
            }
            return false;
        }

        static bool FromParcel(Any *operand, MessageParcel& parcel) noexcept
        {
            int ret = BaseTypesFromParcel(operand, parcel);
            if (ret == 0) {
                return true;
            }
            MakeAnyFromParcel<T>(*operand, parcel);
            return true;
        }
#endif

        static void Destroy(Storage& storage) noexcept
        {
            reinterpret_cast<T*>(storage.trivialStack_.data())->~T();
        }

        static void Copy(Storage& dest, const Storage& source) noexcept
        {
            // memcpy_s will always success in this function
            (void)memcpy_s(GetPtr(dest), sizeof(Storage), GetConstPtr(source), sizeof(Storage));
        }

        static void Move(Storage& dest, Storage& source) noexcept
        {
            Copy(dest, source);
            source.trivialStack_.fill(0);
        }

        static const void* GetConstPtr(const Storage& storage) noexcept
        {
            return reinterpret_cast<const void *>(storage.trivialStack_.data());
        }

        static void* GetPtr(Storage& storage) noexcept
        {
            return reinterpret_cast<void *>(storage.trivialStack_.data());
        }
    };

    template <typename T>
    struct StackFunctionTable {
#ifndef HST_ANY_WITH_NO_RTTI
        static const std::type_info& Type() noexcept
        {
            return typeid(T);
        }
#else
        static std::string_view TypeName() noexcept
        {
            return GetTypeName<T>();
        }
        // Only support parcel enum value to int32_t except base types
        static bool ToParcel(const Any *operand, MessageParcel& parcel) noexcept
        {
            if (BaseTypesToParcel(operand, parcel)) {
                return true;
            }
            if (sizeof(T) > sizeof(int64_t)) { // Only support enum
                return false;
            }
            if (sizeof(T) == sizeof(int64_t)) { //  support Int64 enum
                T value;
                if (AnyCast(operand, value)) {
                    WriteValueToParcelInt64(value, parcel);
                    return true;
                }
                return false;
            }
            T value;
            if (AnyCast(operand, value)) {
                WriteValueToParcel(value, parcel);
                return true;
            }
            return false;
        }

        static bool FromParcel(Any *operand, MessageParcel& parcel) noexcept
        {
            int ret = BaseTypesFromParcel(operand, parcel);
            if (ret == 0) {
                return true;
            }
            MakeAnyFromParcel<T>(*operand, parcel);
            return true;
        }
#endif

        static void Destroy(Storage& storage) noexcept
        {
            reinterpret_cast<T*>(GetPtr(storage))->~T(); // NOLINT: cast
        }

        static void Copy(Storage& dest, const Storage& source) noexcept
        {
            // NOLINTNEXTLINE: reinterpret_cast
            new (reinterpret_cast<T*>(GetPtr(dest))) T(*reinterpret_cast<const T*>(GetConstPtr(source)));
        }

        static void Move(Storage& dest, Storage& source) noexcept
        {
            // NOLINTNEXTLINE: reinterpret_cast
            new (reinterpret_cast<T*>(GetPtr(dest))) T(std::move(*reinterpret_cast<T*>(GetPtr(source))));
            Destroy(source);
        }

        static const void* GetConstPtr(const Storage& storage) noexcept
        {
            return reinterpret_cast<const void*>(&storage.nonTrivialStack_);
        }

        static void* GetPtr(Storage& storage) noexcept
        {
            return reinterpret_cast<void*>(&storage.nonTrivialStack_);
        }
    };

    template <typename T>
    struct HeapFunctionTable {
#ifndef HST_ANY_WITH_NO_RTTI
        static const std::type_info& Type() noexcept
        {
            return typeid(T);
        }
#else
        static std::string_view TypeName() noexcept
        {
            return GetTypeName<T>();
        }
        // Only support parcel enum value to int32_t except base types
        static bool ToParcel(const Any *operand, MessageParcel& parcel) noexcept
        {
            if (BaseTypesToParcel(operand, parcel)) {
                return true;
            }
            if (sizeof(T) > sizeof(int64_t)) { // Only support enum
                return false;
            }
            if (sizeof(T) == sizeof(int64_t)) { //  support Int64 enum
                T value;
                if (AnyCast(operand, value)) {
                    WriteValueToParcelInt64(value, parcel);
                    return true;
                }
                return false;
            }
            T value;
            if (AnyCast(operand, value)) {
                WriteValueToParcel(value, parcel);
                return true;
            }
            return false;
        }

        static bool FromParcel(Any *operand, MessageParcel& parcel) noexcept
        {
            int ret = BaseTypesFromParcel(operand, parcel);
            if (ret == 0) {
                return true;
            }
            MakeAnyFromParcel<T>(*operand, parcel);
            return true;
        }
#endif

        static void Destroy(Storage& storage) noexcept
        {
            delete reinterpret_cast<T*>(storage.heap_); // NOLINT: cast
            storage.heap_ = nullptr;
        }
        static void Copy(Storage& dest, const Storage& source) noexcept
        {
            dest.heap_ = new T(*reinterpret_cast<T*>(source.heap_)); // NOLINT: cast
        }
        static void Move(Storage& dest, Storage& source) noexcept
        {
            dest.heap_ = source.heap_;
            source.heap_ = nullptr;
        }
        static const void* GetConstPtr(const Storage& storage) noexcept
        {
            return storage.heap_;
        }
        static void* GetPtr(Storage& storage) noexcept
        {
            return storage.heap_;
        }
    };

    template <typename ValueType>
    static FunctionTable* GetFunctionTable()
    {
        using DecayedValueType = decay_t<ValueType>;
        using DetailFunctionTable =
            conditional_t<IsTrivialStackStorable<DecayedValueType>::value,
            TrivialStackFunctionTable<DecayedValueType>,
            conditional_t<IsStackStorable<DecayedValueType>::value,
            StackFunctionTable<DecayedValueType>, HeapFunctionTable<DecayedValueType>>>;
        static FunctionTable table = {
#ifndef HST_ANY_WITH_NO_RTTI
            .type = DetailFunctionTable::Type,
#else
            .type_name = DetailFunctionTable::TypeName,
            .toParcel = DetailFunctionTable::ToParcel,
            .fromParcel = DetailFunctionTable::FromParcel,
#endif
            .destroy = DetailFunctionTable::Destroy,
            .copy = DetailFunctionTable::Copy,
            .move = DetailFunctionTable::Move,
            .getPtr = DetailFunctionTable::GetPtr,
            .getConstPtr = DetailFunctionTable::GetConstPtr,
        };
        return &table;
    }

    bool IsFunctionTableValid() const noexcept
    {
        return functionTable_ != nullptr;
    }

    template <typename DecayedValueType, typename... Args>
    DecayedValueType &__attribute__((no_sanitize("cfi"))) DoEmplace(Args &&...args)
    {
        functionTable_ = GetFunctionTable<DecayedValueType>();
        DecayedValueType* ptr = nullptr;
        if (IsTrivialStackStorable<DecayedValueType>::value || IsStackStorable<DecayedValueType>::value) {
            ptr = reinterpret_cast<DecayedValueType*>(functionTable_->getPtr(storage_));
            new (ptr) DecayedValueType(std::forward<Args>(args)...);
        } else {
            storage_.heap_ = new DecayedValueType(std::forward<Args>(args)...);
            ptr = reinterpret_cast<DecayedValueType*>(storage_.heap_);
        }
        return *ptr;
    }

    void __attribute__((no_sanitize("cfi"))) MoveFrom(Any &&other) noexcept
    {
        if (other.HasValue()) {
            functionTable_ = other.functionTable_;
            functionTable_->move(storage_, other.storage_);
            other.Reset();
        }
    }

    template <typename ValueType>
    ValueType* Cast() noexcept
    {
        using DecayedValueType = decay_t<ValueType>;
        if (!IsFunctionTableValid()) {
            return nullptr;
        }
#ifndef HST_ANY_WITH_NO_RTTI
        if (!SameTypeWith(typeid(DecayedValueType))) {
#else
        if (!SameTypeWith(Any::GetTypeName<DecayedValueType>())) {
#endif
            return nullptr;
        }
        return IsTrivialStackStorable<DecayedValueType>::value
            ? reinterpret_cast<DecayedValueType*>(storage_.trivialStack_.data())
            : (IsStackStorable<DecayedValueType>::value
            ? reinterpret_cast<DecayedValueType*>(&storage_.nonTrivialStack_)
            : reinterpret_cast<DecayedValueType*>(storage_.heap_));
    }
    template <typename ValueType>
    const ValueType* Cast() const noexcept
    {
        using DecayedValueType = decay_t<ValueType>;
        if (!IsFunctionTableValid()) {
            return nullptr;
        }
#ifndef HST_ANY_WITH_NO_RTTI
        if (!SameTypeWith(typeid(DecayedValueType))) {
#else
            if (!SameTypeWith(Any::GetTypeName<DecayedValueType>())) {
#endif
            return nullptr;
        }
        return IsTrivialStackStorable<DecayedValueType>::value
            ? reinterpret_cast<const DecayedValueType*>(storage_.trivialStack_.data())
            : (IsStackStorable<DecayedValueType>::value
            ? reinterpret_cast<const DecayedValueType*>(&storage_.nonTrivialStack_)
            : reinterpret_cast<const DecayedValueType*>(storage_.heap_));
    }

private:
    Storage storage_ {};
    FunctionTable* functionTable_ {nullptr};
};

/**
 * cast one Any pointer into ValueType pointer
 *
 * @tparam ValueType target value type
 * @param operand any object
 * @return nullptr if type mismatch, operand is nullptr, or valueType is function/array. Otherwise, a pointer to the
 * const value contained by operand.
 */
template <typename ValueType>
const ValueType* AnyCast(const Any* operand) noexcept
{
    static_assert(!std::is_void<ValueType>::value, "ValueType of any_cast must not be void");
    if (std::is_function<ValueType>::value || std::is_array<ValueType>::value || operand == nullptr) {
        return nullptr;
    }
    return operand->Cast<ValueType>();
}

 /**
  * cast one Any pointer into ValueType object
  *
  * @tparam ValueType target value type
  * @param operand any object
  * @param value ValueType
  * @return false if type mismatch, operand is nullptr, or valueType is function/array. Otherwise, true to the
  *  value contained by operand.
  */
template <typename ValueType>
bool AnyCast(const Any* operand, ValueType& value) noexcept
{
    static_assert(!std::is_void<ValueType>::value, "ValueType of any_cast must not be void");
    if (std::is_function<ValueType>::value || std::is_array<ValueType>::value || operand == nullptr) {
        return false;
    }
#ifndef HST_ANY_WITH_NO_RTTI
    if (!operand->SameTypeWith(typeid(ValueType))) {
#else
        if (!operand->SameTypeWith(Any::GetTypeName<ValueType>())) {
#endif
        return false;
    } else {
        auto casted_value = operand->Cast<ValueType>();
        if (casted_value != nullptr) {
            value = *casted_value;
            return true;
        }
        return false;
    }
}

/**
 * cast one Any pointer into ValueType pointer
 *
 * @tparam ValueType target value type
 * @param operand any object
 * @return nullptr if type mismatch, operand is nullptr, or valueType is function/array. Otherwise, a pointer to the
 * value contained by operand.
 */
template <typename ValueType>
ValueType* AnyCast(Any* operand) noexcept
{
    static_assert(!std::is_void<ValueType>::value, "ValueType of any_cast must not be void");
    if (std::is_function<ValueType>::value || std::is_array<ValueType>::value || operand == nullptr) {
        return nullptr;
    }
    return operand->Cast<ValueType>();
}

/**
 * cast one Any object into ValueType object
 *
 * @tparam ValueType target value type. It must match both conditions:
 * 1. ValueType must be reference or constructible
 * 2. Let U be remove_cv_t<remove_reference_t<ValueType>>, then std::is_constructible<ValueType, const U&> must be true
 * @param operand any object
 * @return throws BadAnyCast exception if type mismatch, operand is nullptr, or valueType is function/array. Otherwise,
 * one object of ValueType contained in Any.
 */
template <typename ValueType>
ValueType AnyCast(const Any& other)
{
    using U = remove_cv_t<remove_reference_t<ValueType>>;
    static_assert(IsValidCast<ValueType>::value, "template argument must be a reference or has copy constructors");
    static_assert(std::is_constructible<ValueType, const U&>::value,
                  "any_cast<ValueType>(const any&) requires ValueType constructable from const "
                  "remove_cv_t<remove_reference_t<ValueType>>&");
    auto ptr = AnyCast<U>(&other);
    return static_cast<ValueType>(*ptr);
}

/**
 * cast one Any object into ValueType object
 *
 * @tparam ValueType target value type. It must match both conditions:
 * 1. ValueType must be reference or constructible
 * 2. Let U be remove_cv_t<remove_reference_t<ValueType>>, then std::is_constructible<ValueType, U&> must be true
 * @param operand any object
 * @return throws BadAnyCast exception if type mismatch, operand is nullptr, or valueType is function/array. Otherwise,
 * one object of ValueType contained in Any.
 */
template <typename ValueType>
ValueType AnyCast(Any& other)
{
    using U = remove_cv_t<remove_reference_t<ValueType>>;
    static_assert(IsValidCast<ValueType>::value, "template argument must be a reference or has copy constructors");
    static_assert(std::is_constructible<ValueType, U&>::value,
                  "any_cast<ValueType>(const any&) requires ValueType constructable from "
                  "remove_cv_t<remove_reference_t<ValueType>>&");
    auto ptr = AnyCast<U>(&other);
    return static_cast<ValueType>(*ptr);
}

/**
 * cast one Any object into ValueType object
 *
 * @tparam ValueType target value type. It must match both conditions:
 * 1. ValueType must be reference or constructible
 * 2. Let U be remove_cv_t<remove_reference_t<ValueType>>, then std::is_constructible<ValueType, U> must be true
 * @param operand any object
 * @return throws BadAnyCast exception if type mismatch, operand is nullptr, or valueType is function/array. Otherwise,
 * one object of ValueType contained in Any.
 */
template <typename ValueType>
ValueType AnyCast(Any&& other)
{
    using U = remove_cv_t<remove_reference_t<ValueType>>;
    static_assert(IsValidCast<ValueType>::value, "template argument must be a reference or has copy constructors");
    static_assert(std::is_constructible<ValueType, U>::value,
                  "any_cast<ValueType>(const any&) requires ValueType constructable from "
                  "remove_cv_t<remove_reference_t<ValueType>>");
    auto ptr = AnyCast<U>(&other);
    return static_cast<ValueType>(std::move(*ptr));
}

/**
 * Constructs Any object, whose content is constructed by args. The content type is T.
 *
 * @tparam T type of Any's content
 * @tparam Args type of args
 * @param args args used to construct the content
 * @return Any object
 */
template <typename T, typename... Args>
Any MakeAny(Args&&... args)
{
    Any tmp;
    tmp.Emplace<T, Args...>(std::forward<Args>(args)...);
    return tmp;
}

/**
 * Constructs Any object, whose content is constructed by il and args. The content type is T.
 *
 * @tparam T type of Any's content
 * @tparam U type of initializer list
 * @tparam Args type of args
 * @param il initializer list
 * @param args args
 * @return Any object
 */
template <typename T, typename U, typename... Args>
Any MakeAny(std::initializer_list<U> il, Args&&... args)
{
    Any tmp;
    tmp.Emplace<T, U, Args...>(il, std::forward<Args>(args)...);
    return tmp;
}

template <typename T>
inline typename std::enable_if<std::is_enum<T>::value, bool>::type MakeAnyFromParcel(Any& value, MessageParcel
& parcel)
{
    if (sizeof(T) == sizeof(int64_t)) {
        value.Emplace<T>(static_cast<T>(parcel.ReadInt64()));
        return true;
    }
    value.Emplace<T>(static_cast<T>(parcel.ReadInt32()));
    return true;
}
} // namespace Media
} // namespace OHOS
#endif
namespace std {
inline void swap(OHOS::Media::Any& lhs, OHOS::Media::Any& rhs) noexcept
{
    lhs.Swap(rhs);
}
} // namespace std
#endif // HISTREAMER_PLUGIN_COMMON_ANY_H
