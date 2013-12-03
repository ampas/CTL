//
//  NukeCtlTests.m
//  NukeCtlTests
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 ARRI Inc. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "NukeCtlUtils.h"
#import "NukeCtlChanArgMap.h"
#import "NukeCtlTransform.h"
using namespace NukeCtl;

#include <fstream>
#include <sstream>
using namespace std;

#include "Iex.h"
using namespace Iex;

#include "ImathFun.h"
using namespace Imath;

#include <CtlSimdInterpreter.h>
#include <CtlFunctionCall.h>
using namespace Ctl;

#include "DDImage/Channel.h"
#include "DDImage/Row.h"
using namespace DD::Image;

namespace NukeCtl
{
  class TransformFriend {
  public:
    
    // Trampolines for static methods
    static
    const std::vector<std::string>
    parseModulePath(const std::string &mp)
    {
      return Transform::parseModulePath(mp);
    }
    
    static
    void
    verifyModuleName(const string &n)
    {
      return Transform::verifyModuleName(n);
    }
    
    TransformFriend(const Transform& transform)
    : transform_(transform)
    {
    }
    
    SimdInterpreterPtr
    interpreter()
    {
      return transform_.interpreter_;
    }
    
    FunctionCallPtr
    topLevelFunctionInTransform()
    {
      return transform_.functionCall_;
    }
    
    FunctionCallPtr
    topLevelFunctionInTransform(const string& topLevelFunctionName)
    {
      return Transform::topLevelFunctionInTransform(transform_.interpreter_, topLevelFunctionName);
    }
    
    ChanArgMap
    chanArgMap()
    {
      return transform_.argMap_;
    }

    void
    callFunction(size_t n)
    {
      transform_.functionCall_->callFunction(n);
    }
    
  private:
    Transform transform_;
  };
  
  class ChanArgMapFriend {
  public:
    
//    std::map<std::string, std::string>&
//    chanNameToArgNameMap()
//    {
//      return chanArgMap_.chanNameToArgName_;
//    }
    
    std::map<std::string, std::string>&
    ArgNameToChanNameMap()
    {
      return chanArgMap_.argNameToChanName_;
    }
    
    std::map<DD::Image::Channel, char*>&
    chanToInArgDataMap()
    {
      return chanArgMap_.chanToInArgData_;
    }
    
    std::map<char*, DD::Image::Channel>&
    outArgDataToChanMap()
    {
      return chanArgMap_.outArgDataToChan_;
    }
    
    ChanArgMapFriend(ChanArgMap& chanArgMap)
    : chanArgMap_(chanArgMap)
    {
    }
    
    void
    load(Ctl::FunctionCallPtr fn)
    {
      chanArgMap_.load(fn);
    }
    
    void
    copyInputRowToArgData(const DD::Image::Row& in, int x, int r)
    {
      chanArgMap_.copyInputRowToArgData(in, x, r);
    }
    
    void
    copyArgDataToOutputRow(int x, int r, DD::Image::Row& out)
    {
      chanArgMap_.copyArgDataToOutputRow(x, r, out);
    }
    
  private:
    ChanArgMap chanArgMap_;
  };
}

@class NukeCtlTests;

@interface NukeCtlTests : XCTestCase

@end



@implementation NukeCtlTests

template<typename T>
bool
equalLengthVectors(const vector<T> &ref, const vector<T> &test)
{
  return test.size() == ref.size();
}

template<typename T>
bool
equalVectorContents(const vector<T> &ref, const vector<T> &test)
{
  if (! equalLengthVectors(test, ref))
  {
    return false;
  }
  for (vector<string>::const_iterator rIter = ref.begin(), tIter=test.begin(); rIter != ref.end(); ++rIter, ++tIter)
  {
    if (*tIter != *rIter)
    {
      return false;
    }
  }
  return true;
}

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testSplitNullString
{
  // Create it, but don't put any element in there.
  vector<string> expected;
  XCTAssertEqual(split("", ':'), expected, @"splitting an empty string should return a vector with no elements");
}

- (void)testSplitBlankString
{
  vector<string> expected;
  expected.push_back(" ");
  vector<string> test(split(" ", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a blank string should return a vector with one element, containing that blank string");
}

- (void)testSplitDelimOnlyString
{
  vector<string> expected;
  vector<string> test(split(":", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a string whose only element is the delimiter should return a vector with no elements");
}

- (void)testSplitDoubleDelimOnlyString
{
  vector<string> expected;
  vector<string> test(split("::", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a string whose only element is an adjacent pair of delimiters should return a vector with no elements");
}

- (void)testSplitSingleCharString
{
  vector<string> expected;
  expected.push_back("x");
  vector<string> test(split("x", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a single-character string should return a vector with a single element, containing that string");
}

- (void)testSplitTwoElementString
{
  // Create it, but don't put any element in there.
  vector<string> expected;
  expected.push_back("x");
  expected.push_back("y");
  vector<string> test(split("x:y", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a two-element string should return a vector with two elements, in the same order as they occured in the string");
}

- (void)testSplitThreeElementString
{
  // Create it, but don't put any element in there.
  vector<string> expected;
  expected.push_back("x");
  expected.push_back("y");
  expected.push_back("z");
  vector<string> test(split("x:y:z", ':'));
  XCTAssert(equalVectorContents(expected, test), @"splitting a three-element string should return a vector with three elements, in the same order as they occured in the string");
}

- (void)testTrimLeftEmptyStringEmptyDelim
{
  string expected;
  string debris;
  string test;
  string result(trimLeft(test, debris));
  XCTAssert(result == expected, @"left trim of empty string with empty debris field should return empty string");
  
}

- (void)testTrimLeftEmptyStringNonEmptyDelim
{
  string expected;
  string debris(" ");
  string test;
  string result(trimLeft(test, debris));
  XCTAssert(result == expected, @"left trim of empty string with non-empty debris field should return empty string");
  
}

- (void)testTrimLeftEmptyDebris
{
  string expected0(" blah blah blah");
  string expected1("blah blah blah");
  string debris;
  string test0(expected0);
  string result0(trimLeft(test0, debris));
  XCTAssert(result0 == expected0, @"left trim of string with leading debris but empty debris field should return string unmodified");
  string test1(expected1);
  string result1(trimLeft(test1, debris));
  XCTAssert(result1 == expected1, @"left trim of string with no leading debris but empty debris field should return string unmodified");
}

- (void)testTrimLeft
{
  string expected("blah blah blah");
  string debris("\t ");
  string test0(string(" ") + expected);
  string test1(expected);
  XCTAssert(trimLeft(test0, debris) == expected, @"left trim of string with leading debris and matching debris field should return trimmed string");
  XCTAssert(trimLeft(test1, debris) == expected, @"left trim of string with no leading debris and matching debris field should return trimmed string");
}

- (void)testTrimRightEmptyStringEmptyDelim
{
  string expected;
  string debris;
  string test;
  string result(trimRight(test, debris));
  XCTAssert(result == expected, @"right trim of empty string with empty debris field should return empty string");

}

- (void)testTrimRightEmptyStringNonEmptyDelim
{
  string expected;
  string debris(" ");
  string test;
  string result(trimRight(test, debris));
  XCTAssert(result == expected, @"right trim of empty string with non-empty debris field should return empty string");
  
}

- (void)testTrimRightEmptyDebris
{
  string expected0("blah blah blah ");
  string expected1("blah blah blah");
  string debris;
  string test0(expected0);
  XCTAssert(trimRight(test0, debris) == expected0, @"right trim of string with trailing debris but empty debris field should return trimmed string");
  string test1(expected1);
  XCTAssert(trimRight(test1, debris) == expected1, @"right trim of string with no leading debris but empty debris field should return trimmed string");
}

- (void)testTrimRight
{
  string expected("blah blah blah");
  string debris("\t ");
  string test0(expected + string(" "));
  string test1(expected);
  string result(trimRight(test0, debris));
  XCTAssert(result == expected, @"right trim of string with leading debris and matching debris field should return trimmed string");
  XCTAssert(trimRight(test1, debris) == expected, @"right trim of string with no leading debris and matching debris field should return trimmed string");
}

- (void)testFilenameFromEmptyPathname
{
  string test("");
  bool threw = false;
  try {
    filenameFromPathname(test);
  }
  catch(const ArgExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"getting the filename name from an empty pathname should thrown ArgExc");
}

- (void)testFilenameFromRootPath
{
  string expected("/");
  string test("/");
  string result(filenameFromPathname(test));
  XCTAssert(result == expected, @"getting the filename of the root as a pathname is still the root");
}

- (void)testFilenameFromDirectoryPath
{
  string expected("foo");
  string test("/foo/");
  string result(filenameFromPathname(test));
  XCTAssert(result == expected, @"getting the filename of a directory as a pathname (i.e. with a trailing slash) is the last non-empty component of the pathname");
}

- (void)testFilenameFromNonDirectoryPath
{
  string expected("bar");
  string test("/foo/bar");
  string result(filenameFromPathname(test));
  XCTAssert(result == expected, @"getting the filename of a non-directory pathname (i.e. with no trailing slash) is the last component of the pathname");
}

- (void)testModuleNameFromEmptyFilename
{
  string test("");
  bool threw = false;
  try {
    modulenameFromFilename(test);
  }
  catch(const ArgExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"getting the module name from an empty filename should thrown ArgExc");
}

- (void)testModuleNameFromMonadicFilename
{
  string expected("foo");
  string test("foo");
  string result(modulenameFromFilename(test));
  XCTAssert(result == expected, @"getting the module name from a monadic filename should return that filename");
}

- (void)testModuleNameFromDyadicFilename
{
  string expected("README");
  string test("README.txt");
  string result(modulenameFromFilename(test));
  XCTAssert(result == expected, @"getting the module name from a Dyadic filename should return the name component of that filename");
}

- (void)testModuleNameFromTriadicFilename
{
  string expected("foo");
  string test("foo.1.ari");
  string result(modulenameFromFilename(test));
  XCTAssert(result == expected, @"getting the module name from a triadic filename should return the name component of that filename");
}

- (void)testRowReadback
{
  // Imagine a line which is 30 pixels across, but where the first pixel is x=10 and the last is x=39
  int x = 10;
  int r = 39;
  Row srcRow(x, r);
  Row::WritablePtr   redBuf = srcRow.writable(Chan_Red);
  Row::WritablePtr greenBuf = srcRow.writable(Chan_Green);
  Row::WritablePtr  blueBuf = srcRow.writable(Chan_Blue);
  for (int i = x; i <= r; ++i)
  {
    float lf = lerpfactor(i + 0.0, x + 0.0, r + 0.0);
    redBuf[i] = lf;
    greenBuf[i] = 1 - lf;
    blueBuf[i] = redBuf[i] * greenBuf[i];
  }
  for (int i = x; i <= r; ++i)
  {
    float lf = lerpfactor(i + 0.0, x + 0.0, r + 0.0);
    XCTAssertEqualWithAccuracy(  redBuf[i],            lf, 4 * numeric_limits<float>::epsilon(),
                               @"red buffer contents should read back to match what was just written into them");
    XCTAssertEqualWithAccuracy(greenBuf[i],        1 - lf, 4 * numeric_limits<float>::epsilon(),
                               @"red buffer contents should read back to match what was just written into them");
    XCTAssertEqualWithAccuracy( blueBuf[i], lf * (1 - lf), 4 * numeric_limits<float>::epsilon(),
                               @"red buffer contents should read back to match what was just written into them");
  }
}

#define BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS \
try {

#define END_WARINESS_OF_UNCAUGHT_EXCEPTIONS \
}                                           \
catch (const std::exception& e)           \
{                                         \
XCTFail(@"Failure due to uncaught std::exception subclass having been thrown"); \
}                                         \
catch (...)                               \
{                                         \
XCTFail(@"Failure due to uncaught exception (and one not based on std::exception)"); \
}

- (void)writeGenericTransformWithName:(NSString *)name toPath:(NSString *)path
{
  ofstream s([path UTF8String]);
  s << "void\n";
  s << [name UTF8String];
  s << "(input varying float rIn,\ninput varying float gIn,\ninput varying float bIn,\ninput varying float aIn,\noutput varying float rOut,\noutput varying float gOut,\noutput varying float bOut,\noutput varying float aOut)\n{\nrOut=rIn;\ngOut=gIn;\nbOut=bIn;\naOut=aIn;\n}\n" << endl;
}

- (void)writeRGBASwappingTransformWithName:(NSString *)name toPath:(NSString *)path
{
  ofstream s([path UTF8String]);
  s << "void\n";
  s << [name UTF8String];
  s << "(input varying float rIn,\ninput varying float gIn,\ninput varying float bIn,\ninput varying float aIn,\noutput varying float rOut,\noutput varying float gOut,\noutput varying float bOut,\noutput varying float aOut)\n{\nrOut=aIn;\ngOut=bIn;\nbOut=gIn;\naOut=rIn;\n}\n" << endl;
}

- (void)testEmptyModulePath
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  vector<string> expected;
  string test;
  NSString* p = @"/tmp/genericMain.ctl";
  [self writeGenericTransformWithName:@"main" toPath:p];
  Transform transform("", [p UTF8String]);
  TransformFriend amigo(transform);
  XCTAssert(equalVectorContents(amigo.parseModulePath(test), expected), @"parsing an empty module path should return a vector with no elements");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testSingleElementModulePath
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  const string test("/usr/local/ctl/aces-dev/utilities");
  vector<string> expected(1, test);
  NSString* p = @"/tmp/genericMain.ctl";
  [self writeGenericTransformWithName:@"main" toPath:p];
  Transform transform("", [p UTF8String]);
  TransformFriend amigo(transform);
  XCTAssert(equalVectorContents(amigo.parseModulePath(test), expected), @"parsing a module path with one element should return a vector with that element");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testVerificationOfModuleName
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  string badChars("/:;\\");
  for (int i = 0; i < badChars.size(); ++i)
  {
    const string test(string("fo") + badChars[i] + "o");
    bool threw = false;
    try {
      NSString* p = @"/tmp/genericMain.ctl";
      [self writeGenericTransformWithName:@"main" toPath:p];
      Transform transform("", [p UTF8String]);
      TransformFriend amigo(transform);
      amigo.verifyModuleName(test);
    }
    catch(const ArgExc& e)
    {
      threw = true;
    }
    XCTAssert(threw, @"verifying a module name with bad characters embedded should thrown ArgExc");
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingTopLevelMainFunction
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
//  SimdInterpreter i;
  NSString* p = @"/tmp/genericMain.ctl";
  [self writeGenericTransformWithName:@"main" toPath:p];
//  i.loadFile([p UTF8String]);
  Transform transform("", [p UTF8String]);
  TransformFriend amigo(transform);
//  FunctionCallPtr f = amigo.topLevelFunctionInTransform(i, [p UTF8String]);
  FunctionCallPtr f = amigo.topLevelFunctionInTransform([p UTF8String]);
  XCTAssert(f.refcount() != 0, @"top-level 'main' function should be visible");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingTopLevelFooFunction
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  // SimdInterpreter i;
  NSString* p = @"/tmp/genericFoo.ctl";
  [self writeGenericTransformWithName:@"genericFoo" toPath:p];
  try {
    // i.loadFile([p UTF8String]);
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
//    FunctionCallPtr f = amigo.topLevelFunctionInTransform(i, [p UTF8String]);
    FunctionCallPtr f = amigo.topLevelFunctionInTransform([p UTF8String]);
  } catch (const ArgExc &e) {
    cout << "oops: " << e.what() << endl;
    XCTFail("could not find genericFoo as top-level function");
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testNotFindingTopLevelFunction
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  // SimdInterpreter i;
  NSString* p = @"/tmp/genericBar.ctl";
  [self writeGenericTransformWithName:@"genericBar" toPath:p];
  bool threw = false;
  try {
    // i.loadFile([p UTF8String]);
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
//    FunctionCallPtr f = amigo.topLevelFunctionInTransform(i, "genericBaz");
    FunctionCallPtr f = amigo.topLevelFunctionInTransform("genericBaz");
  }
  catch(const ArgExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"top-level function should not be found, neither as 'main' nor 'genericBaz'");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}


- (void)testTopLevelFunctionNotReturningVoidThrows
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  // SimdInterpreter i;
  NSString* p = @"/tmp/intMain.ctl";
  ofstream s([p UTF8String]);
  s << "int main(input varying float rIn, input varying float gIn, input varying float bIn, input varying float aIn, output varying float rOut, output varying float gOut, output varying float bOut, output varying float aOut) {}" << endl;
  bool threw = false;
  try {
    // i.loadFile([p UTF8String]);
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
//    FunctionCallPtr f = amigo.topLevelFunctionInTransform(i, "main");
    FunctionCallPtr f = amigo.topLevelFunctionInTransform("main");
  }
  catch(const BaseExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"top-level function not returning void should have been grounds for an exception, but none was thrown");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}


- (void)testNukeCTLArgAndResultCopyingSteps
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/RGBASwap.ctl";
  [self writeRGBASwappingTransformWithName:@"main" toPath:p];
  try {
    Transform transform("", [p UTF8String]);
    int x = 10;
    int r = 20;
    Row in(x, r);
    ChannelMask mask = Mask_RGBA;
    float   red_test_value = 0.125;
    float green_test_value = 0.250;
    float  blue_test_value = 2.0;
    float alpha_test_value = 4.0;
    foreach(z, mask)
    {
      switch (z) {
        case Chan_Red:
          (in.writable(z))[x] = red_test_value;
          break;
        case Chan_Green:
          (in.writable(z))[x] = green_test_value;
          break;
        case Chan_Blue:
          (in.writable(z))[x] = blue_test_value;
          break;
        case Chan_Alpha:
          (in.writable(z))[x] = alpha_test_value;
          break;
        default:
          break;
      }
    }
    transform.loadArgMap();
    TransformFriend amigo(transform);
    int rr = min(r - x, static_cast<int>(amigo.interpreter()->maxSamples()));
    amigo.chanArgMap().copyInputRowToArgData(in, x, x + rr);
    ChanArgMap amigoChanArgMap(amigo.chanArgMap());
    ChanArgMapFriend pal(amigoChanArgMap);
    FunctionCallPtr functionCall = amigo.topLevelFunctionInTransform();
    XCTAssert(pal.chanToInArgDataMap()[Chan_Red]   == functionCall->findInputArg("rIn")->data(), "Chan_Red not mapping to data buffer for rIn");
    XCTAssert(pal.chanToInArgDataMap()[Chan_Green] == functionCall->findInputArg("gIn")->data(), "Chan_Green not mapping to data buffer for gIn");
    XCTAssert(pal.chanToInArgDataMap()[Chan_Blue]  == functionCall->findInputArg("bIn")->data(), "Chan_Blue not mapping to data buffer for bIn");
    XCTAssert(pal.chanToInArgDataMap()[Chan_Alpha] == functionCall->findInputArg("aIn")->data(), "Chan_Alpha not mapping to data buffer for aIn");
    half*   inRedArgData = reinterpret_cast<half*>(pal.chanToInArgDataMap()[Chan_Red]);
    half* inGreenArgData = reinterpret_cast<half*>(pal.chanToInArgDataMap()[Chan_Green]);
    half*  inBlueArgData = reinterpret_cast<half*>(pal.chanToInArgDataMap()[Chan_Blue]);
    half* inAlphaArgData = reinterpret_cast<half*>(pal.chanToInArgDataMap()[Chan_Alpha]);
    XCTAssertEqualWithAccuracy(static_cast<float>(  inRedArgData[0]),   red_test_value, numeric_limits<half>::epsilon() * 4, "Red data from row not being copied to data buffer for rIn");
    XCTAssertEqualWithAccuracy(static_cast<float>(inGreenArgData[0]), green_test_value, numeric_limits<half>::epsilon() * 4, "Green data from row not being copied to data buffer for gIn");
    XCTAssertEqualWithAccuracy(static_cast<float>( inBlueArgData[0]),  blue_test_value, numeric_limits<half>::epsilon() * 4, "Blue channel from row not being copied to data buffer for bIn");
    XCTAssertEqualWithAccuracy(static_cast<float>(inAlphaArgData[0]), alpha_test_value, numeric_limits<half>::epsilon() * 4, "Alpha channel from row not being copied to data buffer for aIn");
    char*   outRedArgData = nullptr;
    char* outGreenArgData = nullptr;
    char*  outBlueArgData = nullptr;
    char* outAlphaArgData = nullptr;
    map<char*, Channel> outArgDataToChanMap = pal.outArgDataToChanMap();
    for (map<char*, Channel>::iterator i = outArgDataToChanMap.begin(); i != outArgDataToChanMap.end(); ++i)
    {
      switch (i->second) {
        case Chan_Red:
          outRedArgData = i->first;
          break;
        case Chan_Green:
          outGreenArgData = i->first;
          break;
        case Chan_Blue:
          outBlueArgData = i->first;
          break;
        case Chan_Alpha:
          outAlphaArgData = i->first;
          break;
        default:
          cout << "Unexpected channel " << i->second << " in output arg to channel map" << endl;
          break;
      }
    }
    functionCall->callFunction(rr);
    XCTAssertEqualWithAccuracy(reinterpret_cast<half*>(  outRedArgData)[0], static_cast<half>(alpha_test_value), numeric_limits<half>::epsilon() * 4, "aIn not swapping into rOut");
    XCTAssertEqualWithAccuracy(reinterpret_cast<half*>(outGreenArgData)[0], static_cast<half>( blue_test_value), numeric_limits<half>::epsilon() * 4, "bIn not swapping into gOut");
    XCTAssertEqualWithAccuracy(reinterpret_cast<half*>( outBlueArgData)[0], static_cast<half>(green_test_value), numeric_limits<half>::epsilon() * 4, "gIn not swapping into bOut");
    XCTAssertEqualWithAccuracy(reinterpret_cast<half*>(outAlphaArgData)[0], static_cast<half>(  red_test_value), numeric_limits<half>::epsilon() * 4, "rIn not swapping into aOut");
    XCTAssert(  outRedArgData == functionCall->findOutputArg("rOut")->data(), "data buffer for rOut not mapping to Chan_Red");
    XCTAssert(outGreenArgData == functionCall->findOutputArg("gOut")->data(), "data buffer for gOut not mapping to Chan_Green");
    XCTAssert( outBlueArgData == functionCall->findOutputArg("bOut")->data(), "data buffer for bOut not mapping to Chan_Blue");
    XCTAssert(outAlphaArgData == functionCall->findOutputArg("aOut")->data(), "data buffer for aOut not mapping to Chan_Alpha");
    Row out(x, r);
    amigo.chanArgMap().copyArgDataToOutputRow(x, x + rr, out);
    functionCall->callFunction(rr);
    XCTAssertEqualWithAccuracy(out[Chan_Red][x],   alpha_test_value, numeric_limits<half>::epsilon() * 4, "Chan_red   output value does not match Chan_alpha input");
    XCTAssertEqualWithAccuracy(out[Chan_Green][x],  blue_test_value, numeric_limits<half>::epsilon() * 4, "Chan_green output value does not match Chan_blue  input");
    XCTAssertEqualWithAccuracy(out[Chan_Blue][x],  green_test_value, numeric_limits<half>::epsilon() * 4, "Chan_blue  output value does not match Chan_green input");
    XCTAssertEqualWithAccuracy(out[Chan_Alpha][x],   red_test_value, numeric_limits<half>::epsilon() * 4, "Chan_alpha output value does not match Chan_red   input");
  } catch (const ArgExc &e) {
    cout << "oops: " << e.what() << endl;
    XCTFail("could not load argument map, execute, or extract results of RGBA-swapping top-level function");
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

// This is the short version. If this doesn't work, consider whether the long version works or not.
- (void)testNukeCTLArgAndResult
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/RGBASwap.ctl";
  [self writeRGBASwappingTransformWithName:@"main" toPath:p];
  try {
    Transform transform("", [p UTF8String]);
    int x = 10;
    int r = 20;
    Row in(x, r);
    float   red_test_value = 0.125;
    float green_test_value = 0.250;
    float  blue_test_value = 2.0;
    float alpha_test_value = 4.0;
    in.writable(Chan_Red)  [x] =   red_test_value;
    in.writable(Chan_Green)[x] = green_test_value;
    in.writable(Chan_Blue) [x] =  blue_test_value;
    in.writable(Chan_Alpha)[x] = alpha_test_value;
    Row out(x,r);
    transform.loadArgMap();
    transform.execute(in, x, r, out);
    XCTAssertEqualWithAccuracy(out[Chan_Red][x],   alpha_test_value, numeric_limits<half>::epsilon() * 4, "Chan_red   output value does not match Chan_alpha input");
    XCTAssertEqualWithAccuracy(out[Chan_Green][x],  blue_test_value, numeric_limits<half>::epsilon() * 4, "Chan_green output value does not match Chan_blue  input");
    XCTAssertEqualWithAccuracy(out[Chan_Blue][x],  green_test_value, numeric_limits<half>::epsilon() * 4, "Chan_blue  output value does not match Chan_green input");
    XCTAssertEqualWithAccuracy(out[Chan_Alpha][x],   red_test_value, numeric_limits<half>::epsilon() * 4, "Chan_alpha output value does not match Chan_red   input");
  } catch (const ArgExc &e) {
    cout << "oops: " << e.what() << endl;
    XCTFail("could not load argument map, execute, or extract results of RGBA-swapping top-level function");
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}


@end
