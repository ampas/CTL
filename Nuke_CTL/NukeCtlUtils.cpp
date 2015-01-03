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

#include "NukeCtlUtils.h"

#include <sstream>
using namespace std;

#include "Iex.h"
using namespace Iex;

namespace NukeCtl {

  // These two are from http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  vector<string>&
  split(const string &s, char delim, vector<string> &elems)
  {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
      elems.push_back(item);
    }
    // special-case (not in the StackOverflow version)
    // For some reason I can't find this C++11 predicate from <algorithms>
    //   if (all_of(elems.begin(), elems.end(), [](string s){ return s.size() == 0; }))
    // Probably all for the best; I can't assume someone else compiling this would have C++11.
    bool all_empty = true;
    for (vector<string>::const_iterator i = elems.begin(); i < elems.end(); ++i)
    {
      if (i->size() != 0)
      {
        all_empty = false;
        break;
      }
    }
    if (all_empty)
    {
      elems.clear();
    }
    return elems;
  }
  
  vector<string>
  split(const string &s, char delim)
  {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
  }
  
  const string
  trimLeft(const string &s, const string &debris)
  {
    // Beats me why passing an empty string as the first arg causes an exception, but the comparable trimRight case does not
    return s.size() == 0 ? s : s.substr(s.find_first_not_of(debris), string::npos);
  }
  
  const string
  trimRight(const string &s, const string &debris)
  {
    return s.substr(0, s.find_last_not_of(debris) + 1);
  }
  
  string
  filenameFromPathname(const string &path)
  {
    string strippedPath(trimRight(trimLeft(path, "\t "), "\t "));
    // special-case the root
    if (strippedPath == "/")
    {
      return strippedPath;
    }
    // directories are components too
    strippedPath = trimRight(strippedPath, "/");
    vector<string> pathComponents(split(path, '/'));
    if (pathComponents.size() == 0)
    {
      THROW(ArgExc, "Expected at least one component of path '" << path << "', but saw none");
    }
    return pathComponents[pathComponents.size() -1];
  }
  
  string
  modulenameFromFilename(const string &filename)
  {
    // blank filename is an error
    string strippedFilename(trimRight(trimLeft(filename, "\t "), "\t "));
    if (strippedFilename == "")
    {
      THROW(ArgExc, "Expected non-blank filename");
    }
    vector<string> filenameComponents(split(strippedFilename, '.'));
    return filenameComponents[0];
  }
  
}