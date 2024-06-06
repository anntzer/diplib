/*
 * (c)2018-2022, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 * Based on original DIPimage code: (c)1999-2014, Delft University of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diplib/deconvolution.h"

#include <tuple>

#include "diplib.h"
#include "diplib/math.h"
#include "diplib/statistics.h"
#include "diplib/transform.h"

#include "common_deconv_utility.h"

namespace dip {

namespace {

std::tuple< bool, bool > ParseWienerOptions( StringSet const& options ) {
   bool isOtf = false;
   bool pad = false;
   for( auto const& opt : options ) {
      if( opt == S::OTF ) {
         isOtf = true;
      } else if( opt == S::PAD ) {
         pad = true;
      } else {
         DIP_THROW_INVALID_FLAG( opt );
      }
   }
   return { isOtf, pad };
}

} // namespace

void WienerDeconvolution(
      Image const& in,
      Image const& psf,
      Image const& signalPower,
      Image const& noisePower,
      Image& out,
      StringSet const& options
) {
   DIP_THROW_IF( !in.IsForged() || !psf.IsForged() || !noisePower.IsForged(), E::IMAGE_NOT_FORGED );
   DIP_THROW_IF( !in.IsScalar() || !psf.IsScalar() || !noisePower.IsScalar(), E::IMAGE_NOT_SCALAR );
   DIP_THROW_IF( !in.DataType().IsReal() || !noisePower.DataType().IsReal(), E::DATA_TYPE_NOT_SUPPORTED );
   bool isOtf{}, pad{};
   DIP_STACK_TRACE_THIS( std::tie( isOtf, pad ) = ParseWienerOptions( options ));

   // Fourier transforms etc.
   Image G, H;
   DIP_STACK_TRACE_THIS( FourierTransformImageAndKernel( in, psf, G, H, isOtf, pad ));
   Image S;
   if( signalPower.IsForged() ) {
      DIP_THROW_IF( !signalPower.IsScalar(), E::IMAGE_NOT_SCALAR );
      DIP_THROW_IF( !signalPower.DataType().IsReal(), E::DATA_TYPE_NOT_SUPPORTED );
      S = signalPower.Pad( G.Sizes() );
   } else {
      S = SquareModulus( G );
   }
   Image N;
   if( noisePower.NumberOfPixels() > 1 ) {
      N = noisePower.Pad( G.Sizes() );
   } else {
      N = noisePower.QuickCopy();
   }

   // Compute the Wiener filter in the frequency domain
   DIP_START_STACK_TRACE
      MultiplyConjugate( G, H, G, G.DataType() );
      MultiplySampleWise( G, S, G, G.DataType() );  // Throws if `signalPower` is the wrong size
      Image divisor = SquareModulus( H );
      MultiplySampleWise( divisor, S, divisor, divisor.DataType() );
      divisor += N;
      G /= divisor; // Not using dip::SafeDivide() on purpose: zeros indicate a true problem here
   DIP_END_STACK_TRACE

   // Inverse Fourier transform
   if( pad ) {
      Image tmp;
      DIP_STACK_TRACE_THIS( FourierTransform( G, tmp, { S::INVERSE, S::REAL } ));
      out = tmp.Cropped( in.Sizes() );
   } else {
      DIP_STACK_TRACE_THIS( FourierTransform( G, out, { S::INVERSE, S::REAL } ));
   }
}

void WienerDeconvolution(
      Image const& in,
      Image const& psf,
      Image& out,
      dfloat regularization,
      StringSet const& options
) {
   DIP_THROW_IF( !in.IsForged() || !psf.IsForged(), E::IMAGE_NOT_FORGED );
   DIP_THROW_IF( !in.IsScalar() || !psf.IsScalar(), E::IMAGE_NOT_SCALAR );
   DIP_THROW_IF( !in.DataType().IsReal(), E::DATA_TYPE_NOT_SUPPORTED );
   DIP_THROW_IF( regularization <= 0, E::PARAMETER_OUT_OF_RANGE );
   bool isOtf{}, pad{};
   DIP_STACK_TRACE_THIS( std::tie( isOtf, pad ) = ParseWienerOptions( options ));

   // Fourier transforms etc.
   Image G, H;
   DIP_STACK_TRACE_THIS( FourierTransformImageAndKernel( in, psf, G, H, isOtf, pad ));

   // Compute the Wiener filter in the frequency domain
   DIP_START_STACK_TRACE
      MultiplyConjugate( G, H, G, G.DataType() );
      Image divisor = SquareModulus( H );
      dfloat K = regularization * Maximum( divisor ).As< dfloat >();
      divisor += K;
      G /= divisor; // Not using dip::SafeDivide() on purpose: zeros indicate a true problem here
   DIP_END_STACK_TRACE

   // Inverse Fourier transform
   if( pad ) {
      Image tmp;
      DIP_STACK_TRACE_THIS( FourierTransform( G, tmp, { S::INVERSE, S::REAL } ));
      out = tmp.Cropped( in.Sizes() );
   } else {
      DIP_STACK_TRACE_THIS( FourierTransform( G, out, { S::INVERSE, S::REAL } ));
   }
}

} // namespace dip
