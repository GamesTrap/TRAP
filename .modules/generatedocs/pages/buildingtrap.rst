.. _buildingtrap:

=============
Building TRAP
=============

------------
Requirements
------------

| TRAP Engine requires the `VulkanSDK <https://vulkan.lunarg.com/sdk/home/>`_.
| TRAP Engine requires a GPU that is Vulkan 1.2, OpenGL 4.6 or newer compatible.

| First clone the repository with :code:`git clone --recursive https://github.com/GamesTrap/TRAP`.

.. note::
   If the repository has already been cloned non-recursively, use :code:`git submodule update --init` to clone the necessary submodules.

| TRAP uses `Premake 5 <https://premake.github.io/>`_ as a build generation tool.
| Follow `these <https://premake.github.io/download.html>`_ instructions in order to install it.

| Then, follow the steps relevant to your operating system.

-------
Windows
-------

| Premake 5 can generate project files for several Visual Studio versions.
| For example, run :code:`premake vs2019` to generate the :code:`.sln` and :code:`.vcxproj` files for Visual Studio 2019.

-----
Linux
-----

| Premake 5 can generate makefile project files.
| For example, run :code:`premake gmake2` to generate the :code:`Makefile` files.