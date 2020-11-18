/*
MIT License

Copyright (c) 2020 Jan "GamesTrap" Schürkamp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>

#include <ModernFileDialogs.h>

#define SaveFileExamples 1
#define OpenFileExamples 1
#define SelectFolderExamples 1

int main()
{
#ifdef SaveFileExamples
	std::cout << SaveFile("TestTitle") << std::endl;
	std::cout << SaveFile("TestTitle", "TestFile.Test") << std::endl;
	std::cout << SaveFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"} }) << std::endl;
	std::cout << SaveFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"}, {"Another Test File", "*.ATS"} }) << std::endl;
	std::cout << SaveFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"}, {"Another Test File", "*.ATS"} }, false) << std::endl;
#endif

#ifdef OpenFileExamples
	for (const auto& str : OpenFile("TestTitle"))
		std::cout << str << std::endl;
	for (const auto& str : OpenFile("TestTitle", "TestFile.Test"))
		std::cout << str << std::endl;
	for (const auto& str : OpenFile("TestTitle", "TestFile.Test", {{"Test File", "*.Test;*.TS"}}))
		std::cout << str << std::endl;
	for (const auto& str : OpenFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"}, {"Another Test File", "*.ATS"} }))
		std::cout << str << std::endl;
	for (const auto& str : OpenFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"}, {"Another Test File", "*.ATS"} }, true))
		std::cout << str << std::endl;
	for (const auto& str : OpenFile("TestTitle", "TestFile.Test", { {"Test File", "*.Test;*.TS"}, {"Another Test File", "*.ATS"} }, true, false))
		std::cout << str << std::endl;
#endif

#ifdef SelectFolderExamples
	std::cout << SelectFolder("TestTitle") << std::endl;
	std::cout << SelectFolder("TestTitle", "TestPath") << std::endl;
#endif
}