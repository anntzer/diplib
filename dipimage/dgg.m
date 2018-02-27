%DGG   Second derivative in the gradient direction
%
% SYNOPSIS:
%  image_out = dgg(image_in,sigma,method,boundary_condition,process,truncation)
%
%  IMAGE_IN is a scalar image with N dimensions.
%  IMAGE_OUT is a scalar image, corresponding to the Raleigh quotient of the
%  Hessian matrix and the gradient vector.
%
%  PROCESS determines along which dimensions to take the derivative.
%
%  See DERIVATIVE for a description of the parameters and the defaults.
%
% NOTE:
%  This function does the equivalent of the following:
%     g = gradient(in,...);
%     H = hessian(in,...);
%     dgg = ( g' * H * g ) / ( g' * g );
%
% SEE ALSO: TFRAMEHESSIAN for reguralised 2nd derivative
%
% DIPlib:
%  This function calls the DIPlib functions dip::Dgg.

% (c)2018, Cris Luengo.
% Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
% Based on original DIPimage code: (c)1999-2014, Delft University of Technology.
%
% Licensed under the Apache License, Version 2.0 (the "License");
% you may not use this file except in compliance with the License.
% You may obtain a copy of the License at
%
%    http://www.apache.org/licenses/LICENSE-2.0
%
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
% See the License for the specific language governing permissions and
% limitations under the License.

function out = dgg(varargin)
out = compute_derivatives('dgg',varargin{:});
