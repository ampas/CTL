// NukeCtl.C

const char* const HELP =
  "<p>Applies Ctl transforms onto an image.</p>";

#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"

#include "NukeTransform.h"
#include "NukeInputParameters.h"

using namespace DD::Image;

static const char* const CLASS = "NukeCtl";

char* readFile(const char*, std::string*);
void saveFile(const char*, const char*);
bool fileExists(const char*);
void getUserParameters(const char*, std::vector<std::string>*, std::vector<std::vector<float> >*, std::vector<int>);

class NukeCtlIop : public PixelIop {

private:
	DD::Image::Knob* moduleKnob;
	DD::Image::Knob* readKnob;
	DD::Image::Knob* textKnob;
	DD::Image::Knob* writeKnob;
	DD::Image::Knob* paramKnob;

	const char *modulePath;
	const char *inFilename;
	const char *outFilename;
	const char *ctlText;
	const char *parameters;
	std::string parameterString;
	
	std::vector<std::string>         paramName;
	std::vector<std::vector<float> > paramValues;
	std::vector<int>		 		 paramSize;
	
public:
	
    NukeCtlIop(Node* node) : PixelIop(node) {
    	modulePath 		= "";
    	inFilename      = "";
    	outFilename     = "";
    	ctlText         = "";
    	parameters      = "";
    	parameterString = "";
    }
    
    static const Iop::Description d;
    void in_channels(int input_number, ChannelSet& channels) const {
    	//channels = Mask_RGBA;
    }
    
    virtual void append(Hash& h) {
    	h.append(__DATE__);
    	h.append(__TIME__);
    }

    void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row &);
    virtual void knobs(Knob_Callback);
    virtual int knob_changed(Knob*);
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
    NukeTransform(in, y, x, r, channels, out, inFilename, paramName, paramValues, paramSize);
}


void NukeCtlIop::knobs(Knob_Callback f) {

	moduleKnob = File_knob(f, &modulePath, "Module Path");
	readKnob   = File_knob(f, &inFilename, "Read CTL File");
	textKnob   = Multiline_String_knob(f, &ctlText, "CTL Text");
	paramKnob  = Multiline_String_knob(f, &parameters, "Input Parameters");
	writeKnob  = Write_File_knob(f, &outFilename, "Write CTL File");

}

// Knob state changed
int NukeCtlIop::knob_changed(Knob *k) {

	char *buffer;
	
	if (k == moduleKnob) {
		return 1;
	}

	// If we read in a file, get the input parameters, display the ctl file to the text knob,
	// and extract the user parameters from the input parameters.
	if (k == readKnob) {
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
	if (k == writeKnob) {
		saveFile(outFilename, textKnob->get_text());
		
		return 1;
	}

	// If the parameters are changed, extract the new information so that the changes
	// can be displayed.
	if (k == paramKnob) {
		getUserParameters(paramKnob->get_text(), &paramName, &paramValues, paramSize);
		return 1;
	}
	
	return 0;
}

static Iop* build(Node* node) {
    return (new NukeWrapper(new NukeCtlIop(node)));
}

const Iop::Description NukeCtlIop::d(CLASS, "Color/NukeCtl", build);


// Read ctl file and copy to buffer
char* readFile(const char *filename, std::string *paramString) {
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
void saveFile(const char *filename, const char *ctlText) {
	size_t size;
	int write = 1;
	std::ofstream outf;
		
	// Check if file exists so it isn't accidentally overwritten
	if (fileExists(filename)) {
        write = Op::message_f('?', "File already exists. Are you sure you would like to overwrite it?");
    }
    
    if (write) {
    	size = strlen(ctlText);
    	outf.open(filename);
    	outf.write(ctlText, size);
    	outf.close();
    }
}


// check to see if file exists
bool fileExists(const char *filename) {
	struct stat buf;
	if (stat(filename, &buf) != -1) {
		return true;
	}
	return false;
}


// read in parameter text and extract names and values to be set for interpreter
void getUserParameters(const char* str, std::vector<std::string> *paramName, std::vector<std::vector<float> > *paramValues, std::vector<int> paramSize) {
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