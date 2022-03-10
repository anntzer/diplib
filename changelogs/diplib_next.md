---
layout: post
title: "Changes DIPlib 3.x.x"
---

## Changes to *DIPlib*

### New functionality

- Added `dip::MakeRegionsConvex2D()`, which works for both labelled and binary images.

- Added `Perimeter()` (as an alias for `Length()`), `Reverse()`, `Rotate()`, `Scale()`, and `Translate()`
  as member functions to `dip::Polygon`.

- `dip::MomentAccumulator` has a new method `PlainSecondOrder()`, which returns the plain old second
  order central moments, compared to the moment of inertia tensor returned by `SecondOrder()`.

- Added `dip::Image::Mask()`, to apply a mask to an image.

- Added `reverse()` to `dip::DimensionArray<>` and `Reverse()` to `dip::PixelSize`.

- Added `dip::Image::ReverseDimensions()`, which permutes dimensions to reverse their order.

### Changed functionality

- The deterministic initialization for `dip::GaussianMixtureModel()` is more robust, making the
  initial Gaussians overlap instead of setting their sigma to 1.

### Bug fixes

- `dip::DrawPolygon2D()`, when drawing filled polygons, would skip the bottom row in the polygon. The
  algorithm is a bit more sophisticated now to properly handle these bottom rows. This also takes care
  of some rounding errors that could be seen for polygons with very short edges.

- `dip::ResampleAt` with a `map` input argument and using `"cubic"` interpolation could overflow,
  yielding unsightly artifacts. See [issue #107](https://github.com/DIPlib/diplib/issues/107).

- Better error messages for some forms of `dip::Image::View::At()`

- `dip::ImageDisplay()` didn't pay attention to the `dim1` and `dim2` parameters if the image was 2D.

- `dip::DefineROI()` was incorrect if the input and output images were the same object.

- `dip::GaussianMixtureModel()` could produce NaN for amplitude, those components now have a zero amplitude.




## Changes to *DIPimage*

### New functionality

### Changed functionality

(See also changes to *DIPlib*.)

### Bug fixes

(See also bugfixes to *DIPlib*.)




## Changes to *PyDIP*

### New functionality

- Added `dip.MakeRegionsConvex2D()`.

- Added `Perimeter()` (as an alias for `Length()`), `Reverse()`, `Rotate()`, `Scale()`, and `Translate()`
  as methods to `dip.Polygon`.

- `dip.Polygon` and `dip.ChainCode` now both have `__len__`, `__getitem__` and `__iter__` methods,
  meaning that they can be treated like lists or other iterables. Previously, the values of `dip.Polygon`
  had to be extracted by conversion to a NumPy array, and the values of `dip.ChainCode` by copying to
  a list when accessing its `codes` property.

- The structure returned by `dip.Moments()` has a new component `plainSecondOrder`, which contains
  the plain old second order central moments, compared to the moment of inertia tensor contained
  in `secondOrder`.

- Added `Mask()` and `ReverseDimensions()` as methods to `dip.Image`.

- Added `dip.ReverseDimensions()`, which reverses the indexing order for images for the remainder of the
  session. Has repercussion on how the `dip.Image` buffer is exposed, buffer protocol objects are
  converted to a `dip.Image`, files are read and written, and how *DIPviewer* displays images.
  It is intended to make indexing into a `dip.Image` match the indexing into the corresponding
  *NumPy* array, which should make it easier to mix calls to *DIPlib* and *scikit-image* in the same
  program. Note that this also causes positive angles to be counter-clockwise instead of clockwise.

- Added the `@` and `@=` operators for `dip.Image` objects. These apply matrix multiplication if both
  operands are non-scalar images. That is, the vector or matrix at each pixel is multiplied by the
  vector or matrix at the corresponding pixel in the other operand. The two image's tensor dimensions
  must be compatible. If one of the operands is a scalar image, the normal element-wise multiplication
  is applied. One of the operands can be a single pixel (a list of numbers).

### Changed functionality

- Operators overloaded for `dip.Image` objects can use lists of numbers as a second argument, which
  is interpreted as a 0D tensor image (column vector). This makes `img / img[0]` possible.

- When PyDIPjavaio fails to load, the error is no longer displayed immediately. Instead, it is
  shown when `dip.ImageRead()` fails. The error message is also a bit more helpful.
  See [issue #106](https://github.com/DIPlib/diplib/issues/106).

- The `__repr__` string for many classes has changed to be more consistent and informative.

- The `*` and `*=` operators have changed meaning, they now always apply element-wise multiplication,
    their previous behavior is now obtained with the new `@` and `@=` operators.
    **NOTE! This breaks backwards compatibility.** To keep old code working that depends on image matrix
    multiplications, you can do
    ```python
    import diplib as dip
    dip.Image.__mul__ = dip.Image.__matmul__
    dip.Image.__rmul__ = dip.Image.__rmatmul__
    dip.Image.__imul__ = dip.Image.__imatmul__
    ```
    But we recommend instead that you update the code to use the right operators.

(See also changes to *DIPlib*.)

### Bug fixes

- `__len__()` now properly returns 0 for an empty (raw) image. This makes `if img` fail for a raw image.

- Fixed a few issues with indexing into image, allowing using a list of coordinates, and allowing assigning
  a scalar value into a multi-channel image.

- `dip.SubpixelMaxima()` and `dip.SubpixelMinima()`, when a mask image was given, didn't work correctly.

- The `**=` operator did the computations correctly, but then assigned `None` to the variable instead of the
  result of the operation. The `**` operator didn't work with a single pixel (a number or a list) on the
  left-hand side.

(See also bugfixes to *DIPlib*.)




## Changes to *DIPviewer*

### New functionality

- Added option to change axis labels.

### Changed functionality

### Bug fixes

- Automatically close all PyDIPviewer windows at program exit to avoid segfault.


## Changes to *DIPjavaio*

### New functionality

### Changed functionality

### Bug fixes