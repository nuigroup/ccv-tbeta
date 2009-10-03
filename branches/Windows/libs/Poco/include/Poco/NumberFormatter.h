//
// NumberFormatter.h
//
// $Id: //poco/1.3/Foundation/include/Poco/NumberFormatter.h#2 $
//
// Library: Foundation
// Package: Core
// Module:  NumberFormatter
//
// Definition of the NumberFormatter class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_NumberFormatter_INCLUDED
#define Foundation_NumberFormatter_INCLUDED


#include "Poco/Foundation.h"


namespace Poco {


class Foundation_API NumberFormatter
	/// The NumberFormatter class provides static methods
	/// for formatting numeric values into strings.
{
public:
	static std::string format(int value);
		/// Formats an integer value in decimal notation.

	static std::string format(int value, int width);
		/// Formats an integer value in decimal notation,
		/// right justified in a field having at least
		/// the specified width.

	static std::string format0(int value, int width);
		/// Formats an integer value in decimal notation, 
		/// right justified and zero-padded in a field
		/// having at least the specified width.

	static std::string formatHex(int value);
		/// Formats an int value in hexadecimal notation.
		/// The value is treated as unsigned.

	static std::string formatHex(int value, int width);
		/// Formats a int value in hexadecimal notation,
		/// right justified and zero-padded in
		/// a field having at least the specified width.
		/// The value is treated as unsigned.

	static std::string format(unsigned value);
		/// Formats an unsigned int value in decimal notation.

	static std::string format(unsigned value, int width);
		/// Formats an unsigned long int in decimal notation,
		/// right justified in a field having at least the
		/// specified width.

	static std::string format0(unsigned int value, int width);
		/// Formats an unsigned int value in decimal notation, 
		/// right justified and zero-padded in a field having at 
		/// least the specified width.

	static std::string formatHex(unsigned value);
		/// Formats an unsigned int value in hexadecimal notation.

	static std::string formatHex(unsigned value, int width);
		/// Formats a int value in hexadecimal notation,
		/// right justified and zero-padded in
		/// a field having at least the specified width.

	static std::string format(long value);
		/// Formats a long value in decimal notation.

	static std::string format(long value, int width);
		/// Formats a long value in decimal notation,
		/// right justified in a field having at least the 
		/// specified width.

	static std::string format0(long value, int width);
		/// Formats a long value in decimal notation, 
		/// right justified and zero-padded in a field
		/// having at least the specified width.

	static std::string formatHex(long value);
		/// Formats an unsigned long value in hexadecimal notation.
		/// The value is treated as unsigned.

	static std::string formatHex(long value, int width);
		/// Formats an unsigned long value in hexadecimal notation,
		/// right justified and zero-padded in a field having at least the 
		/// specified width.
		/// The value is treated as unsigned.

	static std::string format(unsigned long value);
		/// Formats an unsigned long value in decimal notation.

	static std::string format(unsigned long value, int width);
		/// Formats an unsigned long value in decimal notation,
		/// right justified in a field having at least the specified 
		/// width.

	static std::string format0(unsigned long value, int width);
		/// Formats an unsigned long value in decimal notation, 
		/// right justified and zero-padded
		/// in a field having at least the specified width.

	static std::string formatHex(unsigned long value);
		/// Formats an unsigned long value in hexadecimal notation.

	static std::string formatHex(unsigned long value, int width);
		/// Formats an unsigned long value in hexadecimal notation,
		/// right justified and zero-padded in a field having at least the 
		/// specified width.

#if defined(POCO_HAVE_INT64) && !defined(POCO_LONG_IS_64_BIT)

	static std::string format(Int64 value);
		/// Formats a 64-bit integer value in decimal notation.

	static std::string format(Int64 value, int width);
		/// Formats a 64-bit integer value in decimal notation,
		/// right justified in a field having at least the specified width.

	static std::string format0(Int64 value, int width);
		/// Formats a 64-bit integer value in decimal notation, 
		/// right justified and zero-padded in a field having at least 
		/// the specified width.

	static std::string formatHex(Int64 value);
		/// Formats a 64-bit integer value in hexadecimal notation.
		/// The value is treated as unsigned.

	static std::string formatHex(Int64 value, int width);
		/// Formats a 64-bit integer value in hexadecimal notation,
		/// right justified and zero-padded in a field having at least 
		/// the specified width.
		/// The value is treated as unsigned.

	static std::string format(UInt64 value);
		/// Formats an unsigned 64-bit integer value in decimal notation.

	static std::string format(UInt64 value, int width);
		/// Formats an unsigned 64-bit integer value in decimal notation,
		/// right justified in a field having at least the specified width.

	static std::string format0(UInt64 value, int width);
		/// Formats an unsigned 64-bit integer value in decimal notation, 
		/// right justified and zero-padded in a field having at least the 
		/// specified width.

	static std::string formatHex(UInt64 value);
		/// Formats a 64-bit integer value in hexadecimal notation.

	static std::string formatHex(UInt64 value, int width);
		/// Formats a 64-bit integer value in hexadecimal notation,
		/// right justified and zero-padded in a field having at least 
		/// the specified width.

#endif // defined(POCO_HAVE_INT64) && !defined(POCO_LONG_IS_64_BIT)

	static std::string format(float value);
		/// Formats a float value in decimal floating-point notation,
		/// according to std::printf's %g format with a precision of 8 fractional digits.

	static std::string format(double value);
		/// Formats a double value in decimal floating-point notation,
		/// according to std::printf's %g format with a precision of 16 fractional digits.

	static std::string format(double value, int precision);
		/// Formats a double value in decimal floating-point notation,
		/// according to std::printf's %f format with the given precision.

	static std::string format(double value, int width, int precision);
		/// Formats a double value in decimal floating-point notation,
		/// right justified in a field of the specified width,
		/// with the number of fractional digits given in precision.

	static std::string format(const void* ptr);
		/// Formats a pointer in an eight (32-bit architectures) or
		/// sixteen (64-bit architectures) characters wide
		/// field in hexadecimal notation.
};


} // namespace Poco


#endif // Foundation_NumberFormatter_INCLUDED
