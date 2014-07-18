//
//  NukeCtlTests.m
//  NukeCtlTests
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 ARRI Inc. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "NukeCtlUtils.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#import "NukeCtlTransform.h"
// #import "NukeCtlChanArgMap.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

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
    
    TransformFriend(Transform& transform)
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
      string topLevelFunctionName(transform_.topLevelFunctionNameInTransform());
      return interpreter()->newFunctionCall(topLevelFunctionName);
    }
    
    static
    bool
    matchesCTLCannotFindFunctionExceptionText(const exception&e , const string &functionName)
    {
      return Transform::matchesCTLCannotFindFunctionExceptionText(e, functionName);
    }
    
    static
    bool
    matchesCTLCannotFindModuleExceptionText(const exception &e)
    {
      return Transform::matchesCTLCannotFindModuleExceptionText(e);
    }
    
    static
    string
    missingModuleFromException(const exception &e)
    {
      return Transform::missingModuleFromException(e);
    }
    
  private:
    Transform& transform_;
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

- (void)testRecognizingCTLCannotFindFunctionExceptionText
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/genericFoo.ctl";
  [self writeGenericTransformWithName:@"genericFoo" toPath:p];
  SimdInterpreter interpreter;
  interpreter.loadFile([p UTF8String]);
  bool threwRecognizedError = false;
  try {
    FunctionCallPtr f = interpreter.newFunctionCall("main");
  }
  catch(const ArgExc& e)
  {
    threwRecognizedError = TransformFriend::matchesCTLCannotFindFunctionExceptionText(e, "main");
  }
  XCTAssert(threwRecognizedError, @"top-level function should not be found, neither as 'main' nor 'genericBaz'");
  
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testRecognizingCTLCannotFindModuleExceptionText
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  bool threwRecognizedError = false;
  try {
    SimdInterpreter interpreter;
    interpreter.loadModule("zzz");
  }
  catch(const ArgExc& e)
  {
    threwRecognizedError = TransformFriend::matchesCTLCannotFindModuleExceptionText(e);
  }
  XCTAssert(threwRecognizedError, @"Failure to find CTL module should throw recognizable error");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testExtractingCTLCannotFindModuleExceptionText
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  bool threwRecognizedError = false;
  try {
    SimdInterpreter interpreter;
    interpreter.loadModule("zzz");
  }
  catch(const ArgExc& e)
  {
    threwRecognizedError = TransformFriend::missingModuleFromException(e) == "zzz";
  }
  XCTAssert(threwRecognizedError, @"Failure to find CTL module should throw error from which we can extract module name");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingTopLevelMainFunctionInGenericCode
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/genericMain.ctl";
  [self writeGenericTransformWithName:@"main" toPath:p];
  Transform transform("", [p UTF8String]);
  TransformFriend amigo(transform);
  FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  XCTAssert(f.refcount() != 0, @"top-level 'main' function should be visible");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingTopLevelFooFunctionInGenericCode
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/genericFoo.ctl";
  [self writeGenericTransformWithName:@"genericFoo" toPath:p];
  try {
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
    FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  } catch (const ArgExc &e) {
    cout << "oops: " << e.what() << endl;
    XCTFail("could not find genericFoo as top-level function");
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testNotFindingTopLevelFunctionInGenericCode
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/genericBar.ctl";
  [self writeGenericTransformWithName:@"genericBaz" toPath:p];
  bool threw = false;
  try {
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
    FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  }
  catch(const ArgExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"top-level function should not be found, neither as 'main' nor 'genericBaz'");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingRRTWhenModulePathSet
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/rrt.ctl";
  bool threw = false;
  try {
  Transform transform("/tmp", [p UTF8String]);
  TransformFriend amigo(transform);
  FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  XCTAssert(f.refcount() != 0, @"top-level 'main' function should be visible");
  }
  catch(const BaseExc& e)
  {
    threw = true;
  }
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testFindingRRTWhenModulePathNotSet
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/rrt.ctl";
  ofstream s([p UTF8String]);
  bool threw = false;
  try {
    Transform transform("/var/tmp", [p UTF8String]);
    TransformFriend amigo(transform);
    FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  }
  catch(const BaseExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"RRT failing to find utilities modules should have been grounds for an exception, but none was thrown");
  END_WARINESS_OF_UNCAUGHT_EXCEPTIONS
}

- (void)testTopLevelFunctionNotReturningVoidThrows
{
  BEGIN_WARINESS_OF_UNCAUGHT_EXCEPTIONS
  NSString* p = @"/tmp/intMain.ctl";
  ofstream s([p UTF8String]);
  s << "int main(input varying float rIn, input varying float gIn, input varying float bIn, input varying float aIn, output varying float rOut, output varying float gOut, output varying float bOut, output varying float aOut) {}" << endl;
  bool threw = false;
  try {
    Transform transform("", [p UTF8String]);
    TransformFriend amigo(transform);
    FunctionCallPtr f = amigo.topLevelFunctionInTransform();
  }
  catch(const BaseExc& e)
  {
    threw = true;
  }
  XCTAssert(threw, @"top-level function not returning void should have been grounds for an exception, but none was thrown");
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
    TransformFriend amigo(transform);
    FunctionCallPtr functionCall = amigo.topLevelFunctionInTransform();
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
