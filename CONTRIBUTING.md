# How to contribute efficiently

## Table of contents

- [Reporting bugs](#reporting-bugs)

**Please read the first section before reporting a bug!**

## Reporting bugs

**Always open *one* issue for *one* bug!**. If you notice
several bugs and want to report them, make sure to create one new issue for
each of them.

If you're reporting a new bug, you'll make our life simpler (and the
fix will come sooner) by following these guidelines:

### Search first in the existing database

Issues are often reported several times by various users. It's good practice to
**search first in the [issue tracker](https://github.com/GamesTrap/TRAP/issues)
before reporting your issue**. If you don't find a relevant match or if you're
unsure, don't hesitate to **open a new issue**. The bugsquad will handle it
from there if it's a duplicate.

### Specify the platform

TRAP™ runs on a variety of platforms and operating systems and devices.
**In your bug reports, please always specify:**

- Operating system and version (e.g. Windows 11, macOS 13.0, Ubuntu 22.04)
- TRAP version (e.g. 1.0, 1.0.3, or the Git commit hash if you're using a development branch)

For bugs that are likely OS-specific and/or graphics-related, please also specify:

- Device (CPU model including architecture, e.g. x86_64, arm64, etc.)
- GPU model (and the driver version in use if you know it)

**Bug reports not including the required information may be closed at the
maintainers' discretion.** If in doubt, always include all the requested
information; it's better to include too much information than not enough
information.

### Specify steps to reproduce

Many bugs can't be reproduced unless specific steps are taken. Please **specify
the exact steps** that must be taken to reproduce the condition, and try to
keep them as minimal as possible. If you're describing a procedure to follow
in the editor, don't hesitate to include screenshots.

Making your bug report easy to reproduce will make it easier to fix the bug.

### Provide a simple example project

Sometimes, unexpected behavior can happen in your project. In such case,
understand that:

- What happens to you may not happen to other users.
- We can't take the time to look at your project, understand how it is set up
  and then figure out why it's failing.
- On the contributors' end, recreating a test project from scratch takes valuable
  time that can be saved by uploading a *minimal* project.

To speed up our work, **please upload a minimal project** that isolates
and reproduces the issue. This is always the **best way for us to fix it**.
We recommend attaching a ZIP file with the minimal project directly to the bug report,
by drag and dropping the file in the GitHub edition field. This ensures the file
can remain available for a long period of time. Only use third-party file hosts
if your ZIP file isn't accepted by GitHub because it's too large.

We recommend always attaching a minimal reproduction project, even if the issue
may seem simple to reproduce manually.

Now that you've read the guidelines, click the link below to create a
bug report:

- **[Report a bug](https://github.com/GamesTrap/TRAP/issues/new?assignees=&labels=&template=bug_report.yml)**
