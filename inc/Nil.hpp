#ifndef NIL_HPP
# define NIL_HPP

struct nullptr_t
{
private:
    void operator&() const ;
public:
    template<typename T>
    operator T*() const
    {
        return 0;
    }

    template<typename T, typename U>
    operator U T::*() const
    {
        return 0;
    }
};

const static nullptr_t    nil = {};

#endif