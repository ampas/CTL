// NukeCtl.C

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

#include "NukeTransform.h"
#include "NukeInputParameters.h"

#include "NukeCtlUtils.h"
#include "NukeCtlTransform.h"

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
public:
	
  NukeCtlIop(Node* node) : PixelIop(node),
    modulePathEnabled(false), modulePath(""), ctlPath(""),
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

void NukeCtlIop::knobs(Knob_Callback f) {
  
  modulePathEnabledKnob = Bool_knob(f, &modulePathEnabled, "enable_module_path", "Use Module Path");
  modulePathKnob        = File_knob(f, &modulePath,        "module_path",        "Module Path");
  modulePathKnob->enable(modulePathEnabled);
	readKnob              = File_knob(f, &ctlPath,           "ctl_path",           "CTL file Path");
  SetFlags(f, Knob::EARLY_STORE);
  if (f.makeKnobs() && transform == NULL)
  {
    ;
  }
}

void NukeCtlIop::load_transform(const char* const modulePath, const char* const ctlPath)
{
  try
  {
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
    load_transform(modulePath, ctlPath);
		return 1;
	}
  
	// If we read in a file, get the input parameters, display the ctl file to the text knob,
	// and extract the user parameters from the input parameters.
	if (k->is("ctl_path")) {
    load_transform(modulePath, ctlPath);
		return 1;
	}
	return Iop::knob_changed(k);
}

static Iop* build(Node* node) {
  return (new NukeWrapper(new NukeCtlIop(node)));
}

const Iop::Description NukeCtlIop::d(CLASS, "Color/NukeCtl", build);

