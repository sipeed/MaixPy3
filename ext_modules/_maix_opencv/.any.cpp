#include "any.h"

//=============================================================================
// class bad_any_cast
//=============================================================================
const char* bad_any_cast::what() const noexcept
{
    return "bad any_cast";
}

//=============================================================================
// class Any
//=============================================================================

constexpr any::any() noexcept
    : m_manager(nullptr)
{ }

any::any(const any& other)
{
    if (!other.has_value()) {
        m_manager = nullptr;
    } else {
        ManageArg marg;
        marg._any = this;
        other.m_manager(MOClone, &other, &marg);
    }
}

any::any(any&& other) noexcept
{
    if (!other.has_value()) {
        m_manager = nullptr;
    } else {
        ManageArg marg;
        marg._any = this;
        other.m_manager(MOXfer, &other, &marg);
    }
}

any::~any()
{
    reset();
}

any& any::operator=(const any& rhs)
{
    *this = any(rhs);
    return *this;
}

any& any::operator=(any&& rhs) noexcept
{
    if (!rhs.has_value()) {
        reset();
    } else if (this != &rhs) {
        reset();
        ManageArg marg;
        marg._any = this;
        rhs.m_manager(MOXfer, &rhs, &marg);
    }
    return *this;
}

void any::reset() noexcept
{
    if (has_value()) {
        m_manager(MODestroy, this, nullptr);
        m_manager = nullptr;
    }
}

void any::swap(any& rhs) noexcept
{
    if (!has_value() && !rhs.has_value()) {
        return;
    }

    if (has_value() && rhs.has_value()) {
        if (this == &rhs) {
            return;
        }

        any tmp;
        ManageArg marg;
        marg._any = &tmp;
        rhs.m_manager(MOXfer, &rhs, &marg);
        marg._any = &rhs;
        m_manager(MOXfer, this, &marg);
        marg._any = this;
        tmp.m_manager(MOXfer, &tmp, &marg);
    }
    else {
        any* empty = !has_value() ? this : &rhs;
        any* full = !has_value() ? &rhs : this;
        ManageArg marg;
        marg._any = empty;
        full->m_manager(MOXfer, full, &marg);
    }
}

bool any::has_value() const noexcept
{
    return m_manager != nullptr;
}

#if __cpp_rtti
const std::type_info& any::type() const noexcept
{
    if (!has_value()) {
        return typeid(void);
    }
    ManageArg marg;
    m_manager(MOGetTypeInfo, this, &marg);
    return *marg._typeinfo;
}
#endif // __cpp_rtti
