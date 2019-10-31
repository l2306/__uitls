// Implementation

#include "pimpl.h"
#include <vector>

class Container::Impl
{
public:
    Impl (const size_t size)
    {
        vec.resize (size);
    }
    std::vector<int> vec;
};

Container::Container (const size_t size)
    : impl_ (new Impl (size))
{
}

/*
We need those copy constructors, otherwise, we would
share our state. For most classes, it is best to make them
noncopyable anyway.
*/
Container::Container (const Container& other)
   : impl_ (new Impl (other.impl_->vec.size ()))
{
    impl_->vec = other.impl_->vec;
}

Container& Container::operator = (const Container& other)
{
    impl_->vec = other.impl_->vec;

    return *this;
}

int& Container::operator [] (const int index)
{
    return impl_->vec [index];
}

const int& Container::operator [] (const int index) const
{
    return impl_->vec [index];
}
