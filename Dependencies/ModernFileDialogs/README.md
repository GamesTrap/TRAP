# ModernFileDialogs

[![Build Status](   https://travis-ci.com/GamesTrap/ModernFileDialogs.svg?branch=main)](https://travis-ci.com/GamesTrap/ModernFileDialogs)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/GamesTrap/ModernFileDialogs)](https://github.com/GamesTrap/ModernFileDialogs)
[![GitHub repo size](https://img.shields.io/github/repo-size/GamesTrap/ModernFileDialogs)](https://github.com/GamesTrap/ModernFileDialogs)
[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/GamesTrap/ModernFileDialogs?include_prereleases)](https://github.com/GamesTrap/ModernFileDialogs/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/GamesTrap/ModernFileDialogs/total)](https://github.com/GamesTrap/ModernFileDialogs/releases)
[![GitHub issues](https://img.shields.io/github/issues/GamesTrap/ModernFileDialogs)](https://github.com/GamesTrap/ModernFileDialogs/issues?q=is%3Aopen+is%3Aissue)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/GamesTrap/ModernFileDialogs)](https://github.com/GamesTrap/ModernFileDialogs/pulls?q=is%3Aopen+is%3Apr)
[![GitHub](https://img.shields.io/github/license/GamesTrap/ModernFileDialogs)](https://github.com/GamesTrap/ModernFileDialogs/blob/master/LICENSE)

ModernFileDialogs (Cross-platform C++17)  
MessageBox, OpenFileDialog, SaveFileDialog & SelectFolderDialog  
ASCII UTF-8

This README will be updated with more info eventually.

## Setup

First clone the repository with `git clone https://github.com/GamesTrap/ModernFileDialogs`.

ModernFileDialogs uses _Premake 5_ as a build generation tool. Follow [these](https://premake.github.io/download.html) instructions in order to install it.

Then, follow the steps relevant to your operating system.

### Windows

Premake can generate project files for several Visual Studio versions.
For example, run `premake vs2019` to generate the `.sln` and `.vcxproj` files for Visual Studio 2019.

### Linux

Premake can generate makefile project files.
For example, run `premake gmake2` to generate the `Makefile` files.

## License

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
