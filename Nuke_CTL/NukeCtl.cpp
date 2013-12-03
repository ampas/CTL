// NukeCtl.C

const char* const HELP =
"<p>Applies Ctl transforms onto an image.</p>";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"

#include "NukeTransform.h"
#include "NukeInputParameters.h"

#include "NukeCtlTransform.h"

using namespace DD::Image;
using namespace Iex;
using namespace std;

static const char* const CLASS = "NukeCtl";

class NukeCtlIop : public PixelIop {
  
private:
  std::vector<std::string> user_module_paths;
  std::vector<std::string> parseModulePath(const char*);
  void checkModulePath();
  char* readFile(const char*, std::string*);
  void saveFile(const char*, const char*);
  bool fileExists(const char*);
  void getUserParameters(const char*, std::vector<std::string>*, std::vector<std::vector<float> >*, std::vector<int>);
  
  DD::Image::Knob* modSetKnob;
	DD::Image::Knob* moduleKnob;
	DD::Image::Knob* readKnob;
	DD::Image::Knob* textKnob;
	DD::Image::Knob* paramKnob;
	DD::Image::Knob* writeKnob;
  
  NukeCtl::Transform* transform;
  bool transformArgMapValid;
  
  const char *defaultModulePath;
  bool moduleSet;
	const char *modulePath;
	const char *inFilename;
	const char *ctlText;
	const char *parameters;
	const char *outFilename;
	std::string parameterString;
	
	std::vector<std::string>         paramName;
	std::vector<std::vector<float> > paramValues;
	std::vector<int>                 paramSize;
  
  
public:
	
  NukeCtlIop(Node* node) : PixelIop(node) {
    const char *env = std::getenv ("CTL_MODULE_PATH");
    defaultModulePath = env ? env : "";
    moduleSet       = false;
    modulePath      = "";
    inFilename      = "";
    outFilename     = "";
    ctlText         = "";
    parameters      = "";
    parameterString = "";
    user_module_paths.push_back("");
    transform            = NULL;
    transformArgMapValid = false;
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
  void _open();
  void _validate(bool);
};

void NukeCtlIop::_validate(bool for_real) {
  set_out_channels(Mask_RGBA);
  PixelIop::_validate(for_real);
}

void NukeCtlIop::_open() {
    transformArgMapValid = false;
}

// Called on each scanline
// n.b. we assume (and hope this is a safe assumption) that the number of channels in the image is constant across all invocations of pixel_engine not interrupted by _open() or _close().
void NukeCtlIop::pixel_engine(const Row& in, int y, int x, int r, ChannelMask channels, Row& out) {
//  if (strlen(inFilename) > 0 && (! transformArgMapValid))
//  {
//    try {
//      if (! transformArgMapValid)
//      {
//        transform->loadArgMap(in, x, r);
//      }
//      transformArgMapValid = true;
//      try {
//        transform->execute(in, x, r, out);
//      }
//      catch (const BaseExc& e) {
//        error("Could not execute CTL argument transform: %s", e.what());
//      }
//      catch (const exception& e) {
//        error("Could not execute CTL argument transform: %s", e.what());
//      }
//      catch (...) // Something wicked this way comes
//      {
//        error("Could not execute CTL argument transform");
//      }
//    }
//    catch (const BaseExc& e) {
//      error("Could not configure CTL transform engine: %s", e.what());
//    }
//    catch (const exception& e) {
//      error("Could not configure CTL transform engine: %s", e.what());
//    }
//    catch (...) // Something wicked this way comes
//    {
//      error("Could not configure CTL transform engine: %s");
//    }
//  }
//  else
//  {
//    // just do a copy here
//  }
  NukeTransform(in, y, x, r, channels, out, inFilename, paramName, paramValues, paramSize, user_module_paths, moduleSet);
}

void NukeCtlIop::knobs(Knob_Callback f) {
  
  modSetKnob = Bool_knob(f, &moduleSet, "set_module_path", "Set Module Path");
  moduleKnob = File_knob(f, &modulePath, "module_path", "Module Path");
	readKnob   = File_knob(f, &inFilename, "read_ctl_file", "Read CTL File");
	textKnob   = Multiline_String_knob(f, &ctlText, "ctl_text", "CTL Text");
	paramKnob  = Multiline_String_knob(f, &parameters, "set_parameters", "Input Parameters");
	writeKnob  = Write_File_knob(f, &outFilename, "write_ctl_file", "Write CTL File");
  moduleKnob->enable(moduleSet);
  checkModulePath();
}

// Knob state changed
int NukeCtlIop::knob_changed(Knob *k) {
  
	char *buffer;
	
  if (k == &Knob::showPanel) {
    knob("module_path")->enable(moduleSet);
    return 1;
  }

  // if the box is checked, enable or disable the set module knob
  if (k->is("set_module_path")) {
    knob("module_path")->enable(moduleSet);
    if (moduleSet)
      checkModulePath();
    return 1;
  }
  
  // if the module path is changed, make sure it is valid
	if (k->is("module_path")) {
		checkModulePath();
		return 1;
	}
  
	// If we read in a file, get the input parameters, display the ctl file to the text knob,
	// and extract the user parameters from the input parameters.
	if (k->is("read_ctl_file")) {
		parameterString = inputParameters(inFilename, &paramName, &paramValues, &paramSize);
		buffer = readFile(inFilename, &parameterString);
		textKnob->set_text(buffer);
		paramKnob->set_text(parameterString.c_str());
		getUserParameters(parameterString.c_str(), &paramName, &paramValues, paramSize);
		// prevent memory leak
		delete(buffer);
		return 1;
	}
	
	// If the write knob is clicked, save the file.
	if (k->is("write_ctl_file")) {
		saveFile(outFilename, textKnob->get_text());
		return 1;
	}
  
	// If the parameters are changed, extract the new information so that the changes
	// can be displayed.
	if (k->is("set_parameters")) {
		getUserParameters(paramKnob->get_text(), &paramName, &paramValues, paramSize);
		return 1;
	}
	
	return Iop::knob_changed(k);
}

static Iop* build(Node* node) {
  return (new NukeWrapper(new NukeCtlIop(node)));
}

const Iop::Description NukeCtlIop::d(CLASS, "Color/NukeCtl", build);

// parse the module path into an array of strings
std::vector<std::string> NukeCtlIop::parseModulePath(const char* inPath) {
  
  std::vector<std::string> modPaths;
  size_t pos = 0;
  std::string path = std::string(inPath);
  
  while (pos < path.size())
  {
#if defined (WIN32) || defined (WIN64)
    
    size_t end = path.find (';', pos);
    
#else
    
    size_t end = path.find (':', pos);
    
#endif
    
    if (end == std::string::npos)
      end = path.size();
    
    std::string pathItem = path.substr (pos, end - pos);
    
    if(find(modPaths.begin(), modPaths.end(), pathItem )
       == modPaths.end())
      modPaths.push_back(pathItem);
    
    pos = end + 1;
  }
  
  return modPaths;
  
}

// Make sure module path is a valid directory
void NukeCtlIop::checkModulePath() {
	struct stat buf;
  int status;
  const char *temp;
  bool err = false;
  
  const char* candidatePath = moduleSet ? modulePath : defaultModulePath;
  std::vector<std::string> paths = parseModulePath(candidatePath);
  // if a path is not valid then give an error message
  for(std::vector<std::string>::iterator it = paths.begin(); it != paths.end(); it++) {
    temp = it->c_str();
    status = stat(temp, &buf);
    
    if (status == -1 || !S_ISDIR(buf.st_mode)) {
      err = true;
    }
    
  }
  
	if (err) {
    if (moduleSet)
      error("Module path is not set to a valid directory: '%s'", candidatePath);
    else
      error("CTL_MODULE_PATH is not set to a valid directory: '%s'", candidatePath);
    paths.clear();
	}
  
  user_module_paths = paths;
  
	return;
}


// Read ctl file and copy to buffer
char* NukeCtlIop::readFile(const char *filename, std::string *paramString) {
	char *buffer;
	size_t size = 0;
	std::ifstream inf;
	
	inf.open(filename);
	
	// If the file is open read it, otherwise set the buffer to a zero character array
	if (inf) {
		// obtain file size:
    inf.seekg (0, std::ios::end);
    size = inf.tellg();
    inf.seekg (0, std::ios::beg);
    buffer = new char[size + 1];
    // read file to buffer
    inf.read(buffer, size);
    buffer[size] = '\0';
    
		inf.close();
    
	}
	else {
		buffer = new char[size + 1];
		buffer[size] = '\0';
	}
	
	return buffer;
}

// write data in CTL Text
void NukeCtlIop::saveFile(const char *filename, const char *ctlText) {
	size_t size;
	int write = 1;
	std::ofstream outf;
  
	// Check if file exists so it isn't accidentally overwritten
	if (fileExists(filename)) {
    write = Op::message_f('?', "File already exists. Are you sure you would like to overwrite it?");
  }
  else {
    return;
  }
  
  if (write) {
    size = strlen(ctlText);
    outf.open(filename);
    outf.write(ctlText, size);
    outf.close();
  }
}


// check to see if file exists
bool NukeCtlIop::fileExists(const char *filename) {
	struct stat buf;
	if (stat(filename, &buf) != -1) {
		return true;
	}
	return false;
}


// read in parameter text and extract names and values to be set for interpreter
void NukeCtlIop::getUserParameters(const char* str, std::vector<std::string> *paramName, std::vector<std::vector<float> > *paramValues, std::vector<int> paramSize) {
	std::vector<float> value;
	std::string name;
	std::string paramLine(str);
	size_t pos = 0, len = paramLine.size();
	std::string build;
	bool isName = true;
	size_t i, j;
	
	while (pos < len) {
    
		// Skip through spaces
		if (paramLine[pos] == ' ' || paramLine[pos] == '\t' || paramLine[pos] == '\r') {
			pos++;
		}
		// If we hit a newline the next value will be a name so isName is set to true.
		else if (paramLine[pos] == '\n') {
      
			isName = true;
			// If a newline is reached then values would have been read
			// If values have been read then we push the name and value onto the vectors
			if (value.size() > 0) {
				for (i = 0; i < paramName->size(); i++) {
					// find the correct parameter
					if ((*paramName)[i] == name) {
						for (j = 0; j < value.size(); j++) {
							if (j < paramSize[i])
								(*paramValues)[i][j] = value[j];
						}
					}
				}
        
				value.clear();
			}
			pos++;
		}
		// If hit equals sign we expect floats so isName is set to false.
		else if (paramLine[pos] == '=') {
			isName = false;
			pos++;
		}
		else {
			// build the string
			build = "";
			while (pos < len && paramLine[pos] != ' ' && paramLine[pos] != '\r' && paramLine[pos] != '\t' && paramLine[pos] != '\n' && paramLine[pos] != '=') {
				build += paramLine[pos++];
			}
			
			if (isName) {
				name = build;
			}
			else {
				// Pushback float value to the vector since there can multiple values
				value.push_back(std::atof(build.c_str()));
			}
		}
		
	}
	
	// If no newline on last iteration
	if (value.size() > 0) {
		paramName->push_back(name);
		paramValues->push_back(value);
	}
}
