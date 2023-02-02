/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#if 0
#error THIS HEADER IS A GENERATOR. DO NOT INCLUDE.
#endif

//***************************************************************************
// THIS FILE HAS BEEN AUTO GENERATED. DO NOT EDIT THIS FILE.
//***************************************************************************

//***************************************************************************
// To generate to header file, run this at the command line.
// Note: You will need Python and COG installed.
//
// python -m cogapp -d -e -ovariant_pool.h -DNTypes=<n> variant_pool_generator.h
// Where <n> is the number of types to support.
//
// e.g.
// To generate handlers for up to 16 types...
// python -m cogapp -d -e -ovariant_pool.h -DNTypes=16 variant_pool_generator.h
//
// See generate.bat
//***************************************************************************

#ifndef ETL_VARIANT_POOL_INCLUDED
#define ETL_VARIANT_POOL_INCLUDED

#include "platform.h"
#include "pool.h"
#include "type_traits.h"
#include "static_assert.h"
#include "largest.h"

#include <stdint.h>

namespace etl
{
  //***************************************************************************
  template <const size_t MAX_SIZE_,
            typename T1,
            typename T2 = void,
            typename T3 = void,
            typename T4 = void,
            typename T5 = void,
            typename T6 = void,
            typename T7 = void,
            typename T8 = void,
            typename T9 = void,
            typename T10 = void,
            typename T11 = void,
            typename T12 = void,
            typename T13 = void,
            typename T14 = void,
            typename T15 = void,
            typename T16 = void>
  class variant_pool
    : public etl::generic_pool<etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::size,
                               etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::alignment,
                               MAX_SIZE_>
  {
  public:

    typedef etl::generic_pool<etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::size,
                              etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::alignment,
                              MAX_SIZE_> base_t;

    static const size_t MAX_SIZE = MAX_SIZE_;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    variant_pool()
    {
    }

#if ETL_CPP11_NOT_SUPPORTED || ETL_USING_STLPORT
    //*************************************************************************
    /// Creates the object. Default constructor.
    //*************************************************************************
    template <typename T>
    T* create()
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>();
    }

    //*************************************************************************
    /// Creates the object. One parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1>
    T* create(const TP1& p1)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1);
    }

    //*************************************************************************
    /// Creates the object. Two parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2>
    T* create(const TP1& p1, const TP2& p2)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2);
    }

    //*************************************************************************
    /// Creates the object. Three parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2, typename TP3>
    T* create(const TP1& p1, const TP2& p2, const TP3& p3)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2, p3);
    }

    //*************************************************************************
    /// Creates the object. Four parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2, typename TP3, typename TP4>
    T* create(const TP1& p1, const TP2& p2, const TP3& p3, const TP4& p4)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2, p3, p4);
    }
#else
    //*************************************************************************
    /// Creates the object from a type. Variadic parameter constructor.
    //*************************************************************************
    template <typename T, typename... Args>
    T* create(Args&&... args)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(args...);
    }
#endif

    //*************************************************************************
    /// Destroys the object.
    //*************************************************************************
    template <typename T>
    void destroy(const T* const p)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value ||
                         etl::is_base_of<T, T1>::value ||
                         etl::is_base_of<T, T2>::value ||
                         etl::is_base_of<T, T3>::value ||
                         etl::is_base_of<T, T4>::value ||
                         etl::is_base_of<T, T5>::value ||
                         etl::is_base_of<T, T6>::value ||
                         etl::is_base_of<T, T7>::value ||
                         etl::is_base_of<T, T8>::value ||
                         etl::is_base_of<T, T9>::value ||
                         etl::is_base_of<T, T10>::value ||
                         etl::is_base_of<T, T11>::value ||
                         etl::is_base_of<T, T12>::value ||
                         etl::is_base_of<T, T13>::value ||
                         etl::is_base_of<T, T14>::value ||
                         etl::is_base_of<T, T15>::value ||
                         etl::is_base_of<T, T16>::value), "Invalid type");

      base_t::destroy(p);
    }

    //*************************************************************************
    /// Returns the maximum number of items in the variant_pool.
    //*************************************************************************
    size_t max_size() const
    {
      return MAX_SIZE;
    }

  private:

    variant_pool(const variant_pool&) ETL_DELETE;
    variant_pool& operator =(const variant_pool&) ETL_DELETE;
  };

  //***************************************************************************
  template <typename T1,
            typename T2 = void,
            typename T3 = void,
            typename T4 = void,
            typename T5 = void,
            typename T6 = void,
            typename T7 = void,
            typename T8 = void,
            typename T9 = void,
            typename T10 = void,
            typename T11 = void,
            typename T12 = void,
            typename T13 = void,
            typename T14 = void,
            typename T15 = void,
            typename T16 = void>
  class variant_pool_ext
    : public etl::generic_pool_ext<etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::size,
                                   etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::alignment>
  {
  public:

    typedef etl::generic_pool_ext<etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::size,
                                  etl::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::alignment> base_t;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    variant_pool_ext(typename base_t::element* buffer, size_t size)
      : base_t(buffer, size) 
    {
    }

#if ETL_CPP11_NOT_SUPPORTED || ETL_USING_STLPORT
    //*************************************************************************
    /// Creates the object. Default constructor.
    //*************************************************************************
    template <typename T>
    T* create()
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>();
    }

    //*************************************************************************
    /// Creates the object. One parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1>
    T* create(const TP1& p1)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1);
    }

    //*************************************************************************
    /// Creates the object. Two parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2>
    T* create(const TP1& p1, const TP2& p2)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2);
    }

    //*************************************************************************
    /// Creates the object. Three parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2, typename TP3>
    T* create(const TP1& p1, const TP2& p2, const TP3& p3)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2, p3);
    }

    //*************************************************************************
    /// Creates the object. Four parameter constructor.
    //*************************************************************************
    template <typename T, typename TP1, typename TP2, typename TP3, typename TP4>
    T* create(const TP1& p1, const TP2& p2, const TP3& p3, const TP4& p4)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(p1, p2, p3, p4);
    }
#else
    //*************************************************************************
    /// Creates the object from a type. Variadic parameter constructor.
    //*************************************************************************
    template <typename T, typename... Args>
    T* create(Args&&... args)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value), "Unsupported type");

      return base_t::template create<T>(args...);
    }
#endif

    //*************************************************************************
    /// Destroys the object.
    //*************************************************************************
    template <typename T>
    void destroy(const T* const p)
    {
      ETL_STATIC_ASSERT((etl::is_one_of<T, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::value ||
                         etl::is_base_of<T, T1>::value ||
                         etl::is_base_of<T, T2>::value ||
                         etl::is_base_of<T, T3>::value ||
                         etl::is_base_of<T, T4>::value ||
                         etl::is_base_of<T, T5>::value ||
                         etl::is_base_of<T, T6>::value ||
                         etl::is_base_of<T, T7>::value ||
                         etl::is_base_of<T, T8>::value ||
                         etl::is_base_of<T, T9>::value ||
                         etl::is_base_of<T, T10>::value ||
                         etl::is_base_of<T, T11>::value ||
                         etl::is_base_of<T, T12>::value ||
                         etl::is_base_of<T, T13>::value ||
                         etl::is_base_of<T, T14>::value ||
                         etl::is_base_of<T, T15>::value ||
                         etl::is_base_of<T, T16>::value), "Invalid type");

      base_t::destroy(p);
    }

    //*************************************************************************
    /// Returns the maximum number of items in the variant_pool.
    //*************************************************************************
    size_t max_size() const 
    { 
      return base_t::max_size(); 
    }

  private:

    variant_pool_ext(const variant_pool_ext&) ETL_DELETE;
    variant_pool_ext& operator =(const variant_pool_ext&) ETL_DELETE;
  };
}

#endif
