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


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "NukeCtlChanArgMap.h"
#include "Iex.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

using namespace Ctl;
using namespace DD::Image;
using namespace std;
using namespace Iex;

namespace NukeCtl
{
  ChanArgMap::ChanArgMap(FunctionCallPtr fn)
  {
    argNameToChanName_["rIn"] = "r";
    argNameToChanName_["gIn"] = "g";
    argNameToChanName_["bIn"] = "b";
    argNameToChanName_["aIn"] = "a";
    argNameToChanName_["rOut"] = "r";
    argNameToChanName_["gOut"] = "g";
    argNameToChanName_["bOut"] = "b";
    argNameToChanName_["aOut"] = "a";
    chanToInArg_.clear();
    outArgToChan_.clear();
    for (size_t i = 0, n = fn->numInputArgs(); i < n; ++i)
    {
      FunctionArgPtr arg = fn->inputArg(i);
      if (! arg)
      {
        THROW(LogicExc, "CTL function claims " << n << " arguments but cannot get info for arg " << i);
      }
      if (! arg->isVarying())
      {
        THROW(LogicExc, "CTL function parameter " << arg->name() << " is not a varying parameter");
      }
      map<string, string>::const_iterator j = argNameToChanName_.find(arg->name());
      if (j == argNameToChanName_.end())
      {
        THROW(LogicExc, "CTL function parameter " << arg->name() << " has no mapping to a Nuke channel");
      }
      Channel c = findChannel(j->second.c_str());
      // TODO: Find out what findChannel returns if you ask for something it doesn't have, and throw an appropriate error.
      chanToInArg_[c] = arg;
    }
    for (size_t i = 0, n = fn->numOutputArgs(); i < n; ++i)
    {
      FunctionArgPtr arg = fn->outputArg(i);
      if (! arg)
      {
        THROW(LogicExc, "CTL function claims " << n << " output arguments but cannot get info for output arg " << i);
      }
      if (! arg->isVarying())
      {
        THROW(LogicExc, "CTL function output argument " << arg->name() << " is not a varying parameter");
      }
      map<string, string>::const_iterator j = argNameToChanName_.find(arg->name());
      if (j == argNameToChanName_.end())
      {
        THROW(LogicExc, "Could not find expected Nuke channel name for CTL "
              "function output arg name '" << arg->name() << "'");
      }
      outArgToChan_[arg] = findChannel(j->second.c_str());
    }
  }
  
  void
  ChanArgMap::copyInputRowToArgData(const DD::Image::Row &in, int x0, int x1)
  {
    for (map<Channel, FunctionArgPtr>::const_iterator i = chanToInArg_.begin(); i != chanToInArg_.end(); ++i)
    {
      Channel channel = i->first;
      FunctionArgPtr arg = i->second;
      size_t src_stride = sizeof(float);
      size_t dst_offset = 0;
      size_t count = x1 - x0;
      arg->set(in[channel] + x0, src_stride, dst_offset, count);
    }
  }
  
  void
  ChanArgMap::copyArgDataToOutputRow(int x0, int x1, DD::Image::Row &out)
  {
    for (map<FunctionArgPtr, Channel>::const_iterator i = outArgToChan_.begin(); i != outArgToChan_.end(); ++i)
    {
      FunctionArgPtr arg = i->first;
      Channel channel = i->second;
      size_t dst_stride = sizeof(float);
      size_t src_offset = 0;
      size_t count = x1 - x0;
      arg->get(out.writable(channel) + x0, dst_stride, src_offset, count);
    }
  }

}
