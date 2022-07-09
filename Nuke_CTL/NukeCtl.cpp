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

const char* const HELP =
"<p>Applies Ctl transforms onto an image.</p>";

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"

#include "NukeCtlUtils.h"
#include "NukeCtlTransform.h"

#include "Iex.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

using namespace DD::Image;
using namespace Iex;
using namespace std;
using namespace NukeCtl;

static const char* const CLASS = "NukeCtl";

class NukeCtlIop : public PixelIop {
  
private:
  DD::Image::Knob* modulePathEnabledKnob;
	DD::Image::Knob* modulePathKnob;
	DD::Image::Knob* readKnob;
  bool modulePathEnabled;
	const char *modulePath;
	const char *ctlPath;
  NukeCtl::Transform* transform;
  int reloadCount;
public:
	
  NukeCtlIop(Node* node) : PixelIop(node),
    modulePathEnabled(false), modulePath(""), ctlPath(""),
    transform(NULL), reloadCount(0) {
  }
  
  static const Iop::Description d;
  void in_channels(int input_number, ChannelSet& channels) const {
    channels = Mask_RGBA;
  }
  
  void append(Hash& h) {
    h.append(__DATE__);
    h.append(__TIME__);
  }
  
  void expand_ctl_path(std::string& file) {
    if (script_expand(knob("ctl_path")->get_text(&outputContext())) && script_result())
      file = script_result();
    else
      file = "";
    script_unlock();
  }

  void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row &);
  void knobs(Knob_Callback);
  void load_transform(const char* const modulePath, const char* const ctlPath);
  int knob_changed(Knob*);
  const char* Class() const { return CLASS; }
  const char* node_help() const { return HELP; }
  void _validate(bool);
};

void NukeCtlIop::_validate(bool for_real) {
  set_out_channels(Mask_RGBA);
  PixelIop::_validate(for_real);
}

// Called on each scanline
void NukeCtlIop::pixel_engine(const Row& in, int y, int x, int r, ChannelMask channels, Row& out) {
  if (transform != NULL)
  {
    try {
      transform->execute(in, x, r, out);
    }
    catch (const BaseExc& e) {
      error("Could not execute CTL argument transform: %s", e.what());
    }
    catch (const exception& e) {
      error("Could not execute CTL argument transform: %s", e.what());
    }
    catch (...) // Something wicked this way comes
    {
      error("Could not execute CTL argument transform");
    }
  }
  else
  {
    out.copy(in, channels, x, r);
  }
}

void NukeCtlIop::load_transform(const char* const modulePath, const char* const ctlPath)
{
  try
  {
    if (transform != NULL)
    {
      delete transform;
      transform = NULL;
    }
    transform = new NukeCtl::Transform(modulePath, ctlPath);
  }
  catch (const Iex::BaseExc& e)
  {
    error((string("Error instantiating CTL transform: ") + e.what()).c_str());
  }
  catch (const exception& e)
  {
    error((string("Error instantiating CTL transform: ") + e.what()).c_str());
  }
  catch (...)
  {
    error("Error instantiating CTL transform");
  }
}

void NukeCtlIop::knobs(Knob_Callback f) {
  Newline(f, "Module Path");
  modulePathEnabledKnob = Bool_knob(f, &modulePathEnabled, "enable_module_path", "");
  modulePathKnob        = File_knob(f, &modulePath,        "module_path",        "");
  ClearFlags(f, Knob::STARTLINE);
	readKnob              = File_knob(f, &ctlPath,           "ctl_path",           "CTL file Path");
  SetFlags(f, Knob::EARLY_STORE);
  SetFlags(f, Knob::KNOB_CHANGED_ALWAYS);
  Script_knob(f, "knob reload_count [expr [value reload_count] + 1]", "reload");
  Int_knob(f, &reloadCount, "reload_count", INVISIBLE);
  SetFlags(f, Knob::DO_NOT_WRITE);
  Divider(f);
}

// Knob state changed
int NukeCtlIop::knob_changed(Knob *k) {
  
  if (k == &Knob::showPanel) {
    knob("module_path")->enable(modulePathEnabled);
    return 1;
  }

  // if the box is checked, enable or disable the set module knob
  if (k->is("enable_module_path")) {
    knob("module_path")->enable(modulePathEnabled);
    return 1;
  }
  
  // if the module path is changed, make sure it is valid
	if (k->is("module_path")) {
      std::string file;
      expand_ctl_path(file);
      if (!file.empty())
        load_transform(modulePath, file.c_str());
      return 1;
	}

	if (k->is("ctl_path") || k->is("reload")) {
      std::string file;
      expand_ctl_path(file);
      if (!file.empty())
        load_transform(modulePath, file.c_str());
      return 1;
	}
	return Iop::knob_changed(k);
}

static Iop* build(Node* node) {
  return (new NukeWrapper(new NukeCtlIop(node)));
}

const Iop::Description NukeCtlIop::d(CLASS, "Color/NukeCtl", build);

