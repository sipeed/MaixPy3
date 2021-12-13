#ifndef ANY_H
#define ANY_H

#include <exception>
#include <typeinfo>
#include <type_traits>
#include <typeindex>
#include <string>

namespace detail
{
    template <bool B, class T = void>
    struct disable_if_c
    {
        typedef T type;
    };
    template <class T>
    struct disable_if_c<true, T>
    {
    };
    template <class Cond, class T = void>
    struct disable_if : public disable_if_c<Cond::value, T>
    {
    };
    template <bool, class _Ty1, class _Ty2>
    struct _If
    { // type is _Ty2 for assumed false

        typedef _Ty2 type;
    };
    template <class _Ty1, class _Ty2>
    struct _If<true, _Ty1, _Ty2>
    { // type is _Ty1 for assumed true
        typedef _Ty1 type;
    };
    template <class _Ty>
    struct add_reference
    { // add reference
        typedef typename std::remove_reference<_Ty>::type &type;
    };
    template <>
    struct add_reference<void>
    { // add reference

        typedef void type;
    };
    template <>
    struct add_reference<const void>
    { // add reference
        typedef const void type;
    };
    template <>
    struct add_reference<volatile void>
    { // add reference
        typedef volatile void type;
    };
    template <>
    struct add_reference<const volatile void>
    { // add reference
        typedef const volatile void type;
    };
    class bad_cast
    {
    public:
        explicit bad_cast(const std::string &msg)
        {
            _what = msg;
        }
        const char *what() const
        {
            return _what.data();
        };
    private:
        std::string _what;
    };
}

class Any
{
public:
    Any()
        : content(0)
    {
    }
    template <typename _Ty>
    Any(const _Ty &value)
        : content(new holder<typename std::remove_cv<typename std::decay<const _Ty>::type>::type>(value))
    {
    }
    Any(const Any &other)
        : content(other.content ? other.content->clone() : 0)
    {
    }
    //c++11
    // Move constructor
    Any(Any &&other)
        : content(other.content)
    {
        other.content = 0;
    }
    // Perfect forwarding of ValueType
    template <typename _Ty>
    Any(_Ty &&value
        ,
        typename detail::disable_if<typename std::is_same<Any &, _Ty>>::type * = 0 // disable if value has type `any&`
        ,
        typename detail::disable_if<typename std::is_const<_Ty>>::type * = 0) // disable if value has type `const ValueType&&`
        : content(new holder<typename std::decay<_Ty>::type>(static_cast<_Ty &&>(value)))
    {
    }
    ~Any()
    {
        if (content)
        {
            delete content;
        }
        content = nullptr;
    }
public:
    Any &swap(Any &rhs)
    {
        std::swap(content, rhs.content);
        return *this;
    }
    Any &operator=(const Any &rhs)
    {
        Any(rhs).swap(*this);
        return *this;
    }
    //c++11
    //move assignement
    Any &operator=(Any &&rhs)
    {
        rhs.swap(*this);
        Any().swap(rhs);
        return *this;
    }
    //perfect forwarding of valueType
    template <class valueType>
    Any &operator=(valueType &&rhs)
    {
        Any(static_cast<valueType &&>(rhs)).swap(*this);
        return *this;
    }
public: // queries
    bool empty()
    {
        return !content;
    }
    void clear()
    {
        Any().swap(*this);
    }
    const std::type_info &type() const
    {
        return content ? content->type() : typeid(void);
    }
private:
    class placeholder
    {
    public:
        virtual ~placeholder() {}
        virtual const std::type_info &type() const = 0;
        virtual placeholder *clone() const = 0;
    };
    template <typename _Ty>
    class holder : public placeholder
    {
    public:
        typedef _Ty value_type;
        holder(const _Ty &value) : held(value) {}
        holder(_Ty &&value)
            : held(static_cast<_Ty &&>(value))
        {
        }
        virtual const std::type_info &type() const
        {
            return typeid(value_type);
        }
        virtual placeholder *clone() const
        {
            return new holder(held);
        }
    public:
        _Ty held;
    private:
        // intentionally left unimplemented
        holder &operator=(const holder &);
    };
private: // representation
    template <typename _Ty>
    friend _Ty *any_cast(Any *);
    template <typename _Ty>
    friend _Ty *unsafe_any_cast(Any *);
private:
    placeholder *content;
};
inline void swap(Any &lhs, Any &rhs)
{
    lhs.swap(rhs);
}
template <typename _Ty>
_Ty *any_cast(Any *operand)
{
    return operand && operand->type() == typeid(_Ty)
               ? &static_cast<Any::holder<typename std::remove_cv<_Ty>::type> *>(operand->content)->held
               : 0;
}
template <typename _Ty>
inline const _Ty *any_cast(const Any *operand)
{
    return any_cast<_Ty>(const_cast<Any *>(operand));
}
template <typename _Ty>
_Ty any_cast(Any &operand)
{
    typedef typename std::remove_reference<_Ty>::type nonref;
    nonref *result = any_cast<nonref>(&operand);
    if (!result)
    {
        std::string szReason = "bad any_cast : can't convert ";
        szReason += operand.type().name();
        szReason += " to ";
        szReason += typeid(_Ty).name();
        throw detail::bad_cast(szReason);
    }
    // Attempt to avoid construction of a temporary object in cases when
    // `ValueType` is not a reference. Example:
    // `static_cast<std::string>(*result);`
    // which is equal to `std::string(*result);`
    typedef typename detail::_If<
        std::is_reference<_Ty>::value,
        _Ty,
        typename detail::add_reference<_Ty>::type
        >::type ref_type;
    return static_cast<ref_type>(*result);
}
template <typename _Ty>
inline _Ty any_cast(const Any &operand)
{
    typedef typename std::remove_reference<_Ty>::type nonref;
    return any_cast<const nonref &>(const_cast<Any &>(operand));
}
template <typename _Ty>
inline _Ty any_cast(Any &&operand)
{
    static_assert(
        std::is_rvalue_reference<_Ty &&>::value /*true if ValueType is rvalue or just a value*/
            || std::is_const<typename std::remove_reference<_Ty>::type>::value,
        "any_cast shall not be used for getting nonconst references to temporary objects"
    );
    return any_cast<_Ty>(operand);
}
#endif