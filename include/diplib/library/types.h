/*
 * DIPlib 3.0
 * This file contains definitions for the basic data types.
 *
 * (c)2014-2017, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 */


//
// NOTE!
// This file is included through diplib.h -- no need to include directly
//


#ifndef DIP_TYPES_H
#define DIP_TYPES_H

#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <cstdint>   // std::uint8_t, etc.
#include <complex>
#include <vector>
#include <string>
#include <set>

#include "diplib/library/dimension_array.h"
#include "error.h"

#ifdef DIP__ENABLE_DOCTEST
#include "doctest.h"
#endif


/// \file
/// \brief Defines the basic types used throughout the library. This file is always included through `diplib.h`.


namespace dip {


/// \addtogroup infrastructure
/// \{


//
// Integer types for image properties, pixel coordinates, loop indices, etc.
//
// NOTE: `uint` is defined somewhere in some header, so *always* refer to it
// as `dip::uint`, everywhere in the DIPlib code base!
// For consistency, we also use `dip::sint` everywhere we refer to `sint`.
//
// NOTE: It might be better to always used signed integer types everywhere.
//       uint could lead to difficult to catch errors in loops, uint ii<0 is
//       always false. I started with the uint because the standard library
//       uses it for sizes of arrays, and sizeof() is unsigned also. Maybe
//       better to cast these to sint?
using sint = std::ptrdiff_t;  ///< An integer type to be used for strides and similar measures.
using uint = std::size_t;  ///< An integer type to be used for sizes and the like.
// ptrdiff_t and size_t are signed and unsigned integers of the same length as
// pointers: 32 bits on 32-bit systems, 64 bits on 64-bit systems.


//
// Types for pixel values
//
using uint8 = std::uint8_t;      ///< Type for samples in an 8-bit unsigned integer image; also to be used as single byte for pointer arithmetic
using uint16 = std::uint16_t;    ///< Type for samples in a 16-bit unsigned integer image
using uint32 = std::uint32_t;    ///< Type for samples in a 32-bit unsigned integer image
using sint8 = std::int8_t;       ///< Type for samples in an 8-bit signed integer image
using sint16 = std::int16_t;     ///< Type for samples in a 16-bit signed integer image
using sint32 = std::int32_t;     ///< Type for samples in a 32-bit signed integer image
using sfloat = float;            ///< Type for samples in a 32-bit floating point (single-precision) image
using dfloat = double;           ///< Type for samples in a 64-bit floating point (double-precision) image
using scomplex = std::complex< sfloat >;   ///< Type for samples in a 64-bit complex-valued (single-precision) image
using dcomplex = std::complex< dfloat >;   ///< Type for samples in a 128-bit complex-valued (double-precision) image

/// \brief Type for samples in a binary image. Can store 0 or 1. Ocupies 1 byte.
class bin {
   // Binary data stored in a single byte (don't use bool for pixels, it has
   // implementation-defined size). We define this class for binary data so
   // that we can overload functions differently for bin and for uint8.
   public:

      // Overload constructors to make sure we always write 0 or 1 in the bin.
      /// The default value is 0 (false)
      constexpr bin() : v_( 0 ) {};

      /// A bool implicitly converts to bin
      constexpr bin( bool v ) : v_( v ) {};

      /// Any arithmetic type converts to bin by comparing to zero
      template< typename T >
      constexpr explicit bin( T v ) : v_( !!v ) {};

      /// A complex value converts to bin by comparing the absolute value to zero
      template< typename T >
      constexpr explicit bin( std::complex< T > v ) : v_( !!std::abs( v ) ) {};

      /// A bin implicitly converts to bool
      constexpr operator bool() const { return v_; }

   private:
      uint8 v_;
};

// if 8 bits is not a byte...
static_assert( sizeof( dip::uint8 ) == 1, "8 bits is not a byte in your system!" );
// Seriously, though. We rely on this property, and there is no guarantee
// that a system actually has 8 bits in a byte. Maybe we should use char
// (which is guaranteed to be size 1) for generic pointer arithmetic?

static_assert( sizeof( dip::bin ) == 1, "The binary type is not a single byte!" );


//
// Array types
//

using IntegerArray = DimensionArray< dip::sint >;   ///< An array to hold strides, filter sizes, etc.
using UnsignedArray = DimensionArray< dip::uint >;  ///< An array to hold dimensions, dimension lists, etc.
using FloatArray = DimensionArray< dip::dfloat >;   ///< An array to hold filter parameters.
using BooleanArray = DimensionArray< bool >;        ///< An array used as a dimension selector.

using CoordinateArray = std::vector< UnsignedArray >; ///< An array of pixel coordinates.


/// \brief Check the length of an array, and extend it if necessary and possible.
///
/// This function is used where a function's
/// input parameter is an array that is supposed to match the image dimensionality `nDims`. The user can give an
/// array of that length, or an array with a single value, which will be used for all dimensions, or an empty array,
/// in which case the default value `defaultValue` will be used for all dimensions.
template< typename T >
inline void ArrayUseParameter( DimensionArray< T >& array, dip::uint nDims, T defaultValue = 0 ) {
   if( array.empty() ) {
      array.resize( nDims, defaultValue );
   } else if( array.size() == 1 ) {
      array.resize( nDims, array[ 0 ] );
   } else if( array.size() != nDims ) {
      DIP_THROW( E::ARRAY_PARAMETER_WRONG_LENGTH );
   }
}


//
// Strings, used for parameters and other things
//

using String = std::string;                 ///< A string, used to specify an option
using StringArray = std::vector< String >;  ///< An array of strings, used to specify an option per dimension
using StringSet = std::set< String >;       ///< A collection of strings, used to specify multiple independent options

//
// Ranges, used for indexing
//

/// \brief Used in indexing to indicate a regular subset of pixels along one
/// image dimension.
///
/// `%dip::Range{ start, stop }` generates a range of pixels where
/// `start` and `stop` are the first and last indices in the range. That is,
/// `stop` is included in the range. `%dip::Range{ start }` generates a range
/// for a single pixel. For example, `%dip::Range{ 0 }` is the first pixel,
/// and is equivalent to `%dip::Range{ 0, 0 }`. `%dip::Range{ 0, N-1 }` is
/// a range of the first N pixels.
///
/// `%dip::Range{ start, stop, step }` generates a range of pixels where
/// `step` is the number of pixels between
/// subsequent indices. The pixels indexed are the ones generated by the
/// following loop:
///
/// ```cpp
///     offset = start;
///     do {
///        // use this offset
///        offset += step;
///     } while( offset <= stop );
/// ```
///
/// That is, it is possible that the range does not include `stop`, if the
/// `step` would make the range step over `stop`.
///
/// Negative `start` and `stop` values indicate offset from the end (-1 is the
/// last pixel, -2 the second to last, etc.): `%dip::Range{ 5, -6 }` indicates
/// a range that skips the first and last five pixels. `%dip::Range{ -1, -1, 1 }`
/// (or simply `%dip::Range{ -1 }` indicates the last pixel only.
///
/// `%dip::Range{ 0, -1 }` is equivalent to `%dip::Range{}`, and indicates all
/// pixels.
///
/// The `dip::Range::Fix` method converts the negative `start` and `stop` values
/// to actual offsets:
///
/// ```cpp
///     dip::Range r{ 5, -6 };
///     r.fix( 50 );
///     // now r.stop == 50 - 6
/// ```
///
/// If `stop` comes before `start`, then the range generates pixel indicates in
/// the reverse order. That is, negative steps are taken to go from `start` to `stop`.
/// `step` is always a positive integer, the direction of the steps is given
/// soley by the ordering of `start` and `stop`.
struct Range {
   dip::sint start;    ///< First index included in range
   dip::sint stop;     ///< Last index included in range
   dip::uint step;     ///< Step size when going from start to stop

   /// Create a range that indicates all pixels
   Range() : start{ 0 }, stop{ -1 }, step{ 1 } {}

   /// Create a range that indicates a single pixel
   Range( dip::sint i ) : start{ i }, stop{ i }, step{ 1 } {}

   /// \brief Create a range using two or three values; it indicates all pixels between `i` and `j`, both inclusive.
   /// The step size defaults to 1.
   Range( dip::sint i, dip::sint j, dip::uint s = 1 ) : start{ i }, stop{ j }, step{ s } {}

   /// \brief Modify a range so that negative values are assigned correct
   /// values according to the given size. Throws if the range falls
   /// out of bounds.
   void Fix( dip::uint size ) {
      // Check step is non-zero
      DIP_THROW_IF( step == 0, E::PARAMETER_OUT_OF_RANGE );
      // Compute indices from end
      dip::sint sz = static_cast< dip::sint >( size );
      if( start < 0 ) { start += sz; }
      if( stop < 0 ) { stop += sz; }
      // Check start and stop are within range
      DIP_THROW_IF( ( start < 0 ) || ( start >= sz ) || ( stop < 0 ) || ( stop >= sz ), E::INDEX_OUT_OF_RANGE );
      // Compute stop given start and step
      //stop = start + ((stop-start)/step)*step;
   }

   /// Get the number of pixels addressed by the range (must be fixed first!).
   dip::uint Size() const {
      if( start > stop ) {
         return 1 + ( start - stop ) / step;
      } else {
         return 1 + ( stop - start ) / step;
      }
   }

   /// Get the offset for the range (must be fixed first!).
   dip::uint Offset() const {
      return static_cast< dip::uint >( start );
   }

   /// Get the signed step size for the range (must be fixed first!).
   dip::sint Step() const {
      if( start > stop ) {
         return -step;
      } else {
         return step;
      }
   }

};

using RangeArray = DimensionArray< Range >;  ///< An array of ranges


//
// The following is support for defining an options type, where the user can
// specify multiple options to pass on to a function or class. The class should
// not be used directly, only through the macros defined below it.
//

template< typename E >
class dip__Options {
      using value_type = unsigned long;
      value_type values;
      constexpr dip__Options( value_type v, int ) : values{ v } {} // used by operator+
   public:
      constexpr dip__Options() : values( 0 ) {}
      constexpr dip__Options( dip::uint n ) : values{ 1UL << n } {}
      constexpr bool operator==( dip__Options const& other ) const {
         return ( values & other.values ) == other.values;
      }
      constexpr bool operator!=( dip__Options const& other ) const {
         return !operator==( other );
      }
      constexpr dip__Options operator+( dip__Options const& other ) const {
         return { values | other.values, 0 };
      }
      dip__Options& operator+=( dip__Options const& other ) {
         values |= other.values;
         return *this;
      }
      dip__Options& operator-=( dip__Options const& other ) {
         values &= ~other.values;
         return *this;
      }
};

/// \brief Declare a type used to pass options to a function or class.
///
/// This macro is used as follows:
///
/// ```cpp
///     DIP_DECLARE_OPTIONS( MyOptions );
///     DIP_DEFINE_OPTION( MyOptions, Option_clean, 0 );
///     DIP_DEFINE_OPTION( MyOptions, Option_fresh, 1 );
///     DIP_DEFINE_OPTION( MyOptions, Option_shine, 2 );
/// ```
///
/// `MyOptions` will be a type that has three non-exclusive flags. Each of the
/// three DIP_DEFINE_OPTION commands defines a `constexpr` variable for the
/// given flag. These values can be combined using the `+` operator.
/// A variable of type `MyOptions` can be tested using the `==` and `!=`
/// operators, which return a `bool`:
///
/// ```cpp
///     MyOptions opts {};                      // No options are set
///     opts = Option_fresh;                    // Set only one option
///     opts = Option_clean + Option_shine;     // Set only these two options
///     if( opts == Option_clean ) {...}        // Test to see if `Option_clean` is set
/// ```
///
/// It is possible to declare additional values as a combination of existing
/// values:
///
/// ```cpp
///     DIP_DEFINE_OPTION( MyOptions, Option_freshNclean, Option_fresh + Option_clean );
/// ```
///
/// For class member values, add `static` in front of `DIP_DEFINE_OPTION`.
#define DIP_DECLARE_OPTIONS( name ) class name##__Tag; using name = dip::dip__Options< name##__Tag >

/// \brief Use in conjunction with `DIP_DECLARE_OPTIONS`. `index` should be no higher than 31.
#define DIP_DEFINE_OPTION( name, option, index ) constexpr name option { index }


//
// The following are some types for often-used parameters
//

/// \brief Enumerated options are defined in the namespace `dip::Option`, unless they
/// are specific to some other sub-namespace.
namespace Option {

/// \brief Some functions that check for a condition optionally throw an exception
/// if that condition is not met.
enum class ThrowException {
   DONT_THROW, ///< Do not throw and exception, return false if the condition is not met.
   DO_THROW    ///< Throw an exception if the condition is not met.
};

/// \brief The function `dip::Image::CheckIsMask` takes this option to control how sizes are compared.
enum class AllowSingletonExpansion {
   DONT_ALLOW, ///< Do not allow singleton expansion.
   DO_ALLOW    ///< Allow singleton expansion.
};

/// \brief The function `dip::Image::ReForge` takes this option to control how to handle protected images.
enum class AcceptDataTypeChange {
   DONT_ALLOW, ///< Do not allow data type change, the output image is always of the requested type.
   DO_ALLOW    ///< Allow data type change, if the output image is protected, it will be used as is.
};

/// \class dip::Option::CmpProps
/// \brief Determines which properties to compare.
///
/// Valid values are:
///
/// %CmpProps constant      | Definition
/// ----------------------- | ----------
/// CmpProps_DataType       | Compares data type
/// CmpProps_Dimensionality | Compares number of dimensions
/// CmpProps_Sizes          | Compares image size
/// CmpProps_Strides        | Compares image strides
/// CmpProps_TensorShape    | Compares tensor size and shape
/// CmpProps_TensorElements | Compares number of tensor elements
/// CmpProps_TensorStride   | Compares tensor stride
/// CmpProps_ColorSpace     | Compares color space
/// CmpProps_PixelSize      | Compares pixel size
/// CmpProps_Samples        | CmpProps_DataType + CmpProps_Sizes + CmpProps_TensorElements
/// CmpProps_Shape          | CmpProps_DataType + CmpProps_Sizes + CmpProps_TensorShape
/// CmpProps_Full           | CmpProps_Shape + CmpProps_Strides + CmpProps_TensorStride
/// CmpProps_All            | CmpProps_Shape + CmpProps_ColorSpace + CmpProps_PixelSize
///
/// Note that you can add these constants together, for example `dip::Option::CmpProps_Sizes + dip::Option::CmpProps_Strides`.
DIP_DECLARE_OPTIONS( CmpProps );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_DataType, 0 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Dimensionality, 1 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Sizes, 2 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Strides, 3 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_TensorShape, 4 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_TensorElements, 5 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_TensorStride, 6 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_ColorSpace, 7 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_PixelSize, 8 );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Samples,
                          CmpProps_DataType + CmpProps_Sizes + CmpProps_TensorElements );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Shape,
                          CmpProps_DataType + CmpProps_Sizes + CmpProps_TensorShape );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_Full,
                          CmpProps_Shape + CmpProps_Strides + CmpProps_TensorStride );
static DIP_DEFINE_OPTION( CmpProps, CmpProps_All,
                          CmpProps_Shape + CmpProps_ColorSpace + CmpProps_PixelSize );


} // namespace Option

/// \}

} // namespace dip


#ifdef DIP__ENABLE_DOCTEST

DOCTEST_TEST_CASE("[DIPlib] testing the dip::bin class") {
   dip::bin A = false;
   dip::bin B = true;
   DOCTEST_CHECK( A < B );
   DOCTEST_CHECK( B > A );
   DOCTEST_CHECK( A >= A );
   DOCTEST_CHECK( A <= B );
   DOCTEST_CHECK( A == A );
   DOCTEST_CHECK( A == false );
   DOCTEST_CHECK( A == 0 );
   DOCTEST_CHECK( A != B );
   DOCTEST_CHECK( A != true );
   DOCTEST_CHECK( A != 100 );
}

DOCTEST_TEST_CASE("[DIPlib] testing the dip::dip__Options class") {
   DIP_DECLARE_OPTIONS( MyOptions );
   DIP_DEFINE_OPTION( MyOptions, Option_clean, 0 );
   DIP_DEFINE_OPTION( MyOptions, Option_fresh, 1 );
   DIP_DEFINE_OPTION( MyOptions, Option_shine, 2 );
   DIP_DEFINE_OPTION( MyOptions, Option_flower, 3 );
   DIP_DEFINE_OPTION( MyOptions, Option_burn, 4 );
   DIP_DEFINE_OPTION( MyOptions, Option_freshNclean, Option_fresh + Option_clean );
   MyOptions opts {};
   DOCTEST_CHECK( opts != Option_clean );
   opts = Option_fresh;
   DOCTEST_CHECK( opts != Option_clean );
   DOCTEST_CHECK( opts == Option_fresh );
   DOCTEST_CHECK( opts != Option_fresh + Option_burn );
   opts = Option_clean + Option_burn;
   DOCTEST_CHECK( opts == Option_clean );
   DOCTEST_CHECK( opts == Option_burn );
   DOCTEST_CHECK( opts == Option_burn + Option_clean );
   DOCTEST_CHECK( opts != Option_shine );
   DOCTEST_CHECK( opts != Option_fresh );
   DOCTEST_CHECK( opts != Option_fresh + Option_burn );
   opts += Option_shine;
   DOCTEST_CHECK( opts == Option_clean );
   DOCTEST_CHECK( opts == Option_burn );
   DOCTEST_CHECK( opts == Option_shine );
   DOCTEST_CHECK( opts != Option_fresh );
   opts = Option_freshNclean;
   DOCTEST_CHECK( opts == Option_clean );
   DOCTEST_CHECK( opts == Option_fresh );
   DOCTEST_CHECK( opts != Option_shine );
   opts -= Option_clean ;
   DOCTEST_CHECK( opts != Option_clean );
   DOCTEST_CHECK( opts == Option_fresh );
   DOCTEST_CHECK( opts != Option_shine );

   DIP_DECLARE_OPTIONS( HisOptions );
   DIP_DEFINE_OPTION( HisOptions, Option_ugly, 0 );
   DIP_DEFINE_OPTION( HisOptions, Option_cheap, 1 );
   DIP_DEFINE_OPTION( HisOptions, Option_fast, 1 );  // repeated value
   DOCTEST_CHECK( Option_cheap == Option_fast );

   // DOCTEST_CHECK( Option_cheap == Option_shine ); // compiler error: assignment different types
   // HisOptions b = Option_fast + Option_flower;    // compiler error: addition different types
}

#endif // DIP__ENABLE_DOCTEST


#endif // DIP_TYPES_H
