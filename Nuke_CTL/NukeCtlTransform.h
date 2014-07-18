///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Academy of Motion Picture Arts and Sciences
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
//
// A worldwide, royalty-free, non-exclusive right to copy, modify, create
// derivatives, and use, in source and binary forms, is hereby granted,
// subject to acceptance of this license. Performance of any of the
// aforementioned acts indicates acceptance to be bound by the following
// terms and conditions:
//
//  * Copies of source code, in whole or in part, must retain the
//    above copyright notice, this list of conditions and the
//    Disclaimer of Warranty.
//
//  * Use in binary form must retain the above copyright notice,
//    this list of conditions and the Disclaimer of Warranty in the
//    documentation and/or other materials provided with the distribution.
//
//  * Nothing in this license shall be deemed to grant any rights to
//    trademarks, copyrights, patents, trade secrets or any other
//    intellectual property of A.M.P.A.S. or any contributors, except
//    as expressly stated herein.
//
//  * Neither the name "A.M.P.A.S." nor the name of any other
//    contributors to this software may be used to endorse or promote
//    products derivative of or based on this software without express
//    prior written permission of A.M.P.A.S. or the contributors, as
//    appropriate.
//
// This license shall be construed pursuant to the laws of the State of
// California, and any disputes related thereto shall be subject to the
// jurisdiction of the courts therein.
//
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
// EVENT SHALL A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS, BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, RESITUTIONARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// WITHOUT LIMITING THE GENERALITY OF THE FOREGOING, THE ACADEMY
// SPECIFICALLY DISCLAIMS ANY REPRESENTATIONS OR WARRANTIES WHATSOEVER
// RELATED TO PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS IN THE ACADEMY
// COLOR ENCODING SYSTEM, OR APPLICATIONS THEREOF, HELD BY PARTIES OTHER
// THAN A.M.P.A.S., WHETHER DISCLOSED OR UNDISCLOSED.
///////////////////////////////////////////////////////////////////////////


#ifndef __IIF__NukeCtlTransform__
#define __IIF__NukeCtlTransform__

#include "DDImage/Channel.h"
#include "DDImage/Row.h"

#include <CtlRcPtr.h>
#include <CtlSimdInterpreter.h>
#include <CtlFunctionCall.h>

#include <vector>
#include <string>

namespace NukeCtl
{
  class TransformFriend;
  
  class RcSimdInterpreter : public Ctl::SimdInterpreter, public Ctl::RcObject
  {
  };
  
  typedef Ctl::RcPtr<RcSimdInterpreter> SimdInterpreterPtr;
  
  class Transform {
    friend class TransformFriend;
  public:
    Transform(const std::string &modulePath,
              const std::string &transformPath);

    void
    execute(const DD::Image::Row &in, int l, int r, DD::Image::Row &out);
  private:
    static
    const std::vector<std::string>
    parseModulePath(const std::string &modulePath);
    
    static
    void
    verifyModuleName(const std::string &moduleName);
    
    static
    bool
    matchesCTLCannotFindFunctionExceptionText(const std::exception& e, const std::string &functionName);
    
    static
    bool
    matchesCTLCannotFindModuleExceptionText(const std::exception& e);
    
    static
    const std::string
    missingModuleFromException(const std::exception& e);
    
    const std::string
    topLevelFunctionNameInTransform();
    
    void
    loadArgMap();
    
    std::vector<std::string>  modulePathComponents_;
    
    // Keeping this in the heap makes it possible to share it between the Transform
    // and TransformFriend (which gets used in unit testing).
     SimdInterpreterPtr        interpreter_;
    
    // On the other hand, you do NOT want to try anything comparable for sharing
    // functional calls with Ctl::FunctionCallPtr. That sort of thing will work in
    // unit testing, which is single-threaded, but Nuke has multiple threads sharing
    // from a single Nuke Op.
    
    // transformPath_ is not used at runtime but is handy for forensics
    std::string               transformPath_;
    std::string               functionName_;

    // Make this private, and don't implement them - that way we know that the compiler
    // isn't doing anything odd.
    Transform(const Transform &transform);
    
    // Make this private, and don't implement them - that way we know that the compiler
    // isn't doing anything odd.
    Transform&
    operator=(const Transform &rhs);
    
  };
}

#endif /* defined(__IIF__NukeCtlTransform__) */
