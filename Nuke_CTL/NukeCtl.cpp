// NukeCtl.C

const char* const HELP =
"<p>Applies Ctl transforms onto an image.</p>";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"

#include "NukeTransform.h"
#include "NukeInputParameters.h"

#include "NukeCtlUtils.h"
#include "NukeCtlTransform.h"

using namespace DD::Image;
using namespace Iex;
using namespace std;
using namespace NukeCtl;

static const char* const CLASS = "NukeCtl";

class NukeCtlIop : public PixelIop {
  
private:
  DD::Image::Knob* modSetKnob;
	DD::Image::Knob* moduleKnob;
	DD::Image::Knob* readKnob;
  NukeCtl::Transform* transform;
  bool moduleSet;
	const char *modulePath;
	const char *inFilename;
public:
	
  NukeCtlIop(Node* node) : PixelIop(node),
    moduleSet(false), modulePath(""), inFilename(""),
    transform(NULL) {
  }
  
  static const Iop::Description d;
  void in_channels(int input_number, ChannelSet& channels) const {
    //channels = Mask_RGBA;
  }
  
  void append(Hash& h) {
    h.append(__DATE__);
    h.append(__TIME__);
  }
  
  void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row &);
  void knobs(Knob_Callback);
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

void NukeCtlIop::knobs(Knob_Callback f) {
  
  modSetKnob = Bool_knob(f, &moduleSet, "set_module_path", "Set Module Path");
  moduleKnob = File_knob(f, &modulePath, "module_path", "Module Path");
	readKnob   = File_knob(f, &inFilename, "read_ctl_file", "Read CTL File");
  moduleKnob->enable(moduleSet);
}

// Knob state changed
int NukeCtlIop::knob_changed(Knob *k) {
  if (k == &Knob::showPanel) {
    knob("module_path")->enable(moduleSet);
    return 1;
  }

  // if the box is checked, enable or disable the set module knob
  if (k->is("set_module_path")) {
    knob("module_path")->enable(moduleSet);
    return 1;
  }
  
  // if the module path is changed, make sure it is valid
	if (k->is("module_path")) {
    // TODO: Add something to check module path here
		return 1;
	}
  
	// If we read in a file, get the input parameters, display the ctl file to the text knob,
	// and extract the user parameters from the input parameters.
	if (k->is("read_ctl_file")) {
    try
    {
      transform = new NukeCtl::Transform(modulePath, inFilename);
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
		return 1;
	}
	return Iop::knob_changed(k);
}

static Iop* build(Node* node) {
  return (new NukeWrapper(new NukeCtlIop(node)));
}

const Iop::Description NukeCtlIop::d(CLASS, "Color/NukeCtl", build);

