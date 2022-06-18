# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))
from os.path import exists
from io import open

# -- Project information -----------------------------------------------------

project = 'TRAP'
copyright = '2019-2022 Jan "GamesTrap" Schürkamp All Rights Reserved'
author = 'Jan "GamesTrap" Schuerkamp'

# The full version, including alpha/beta/rc tags
#Retrieve version number from Base.h
basedotHPath = '../../TRAP/src/Core/Base.h'
basedothExists = exists(basedotHPath)
if basedothExists:
    with open(basedotHPath) as f:
        for line in f:
            if line.startswith('constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION('):
                #Remove 'constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION('
                line = line[len('constexpr uint32_t TRAP_VERSION = TRAP_MAKE_VERSION('):]
                #Remove ');\n' from end
                line = line[:-len(');\n')]
                #Replace ', ' with '.'
                line = line.replace(', ', '.')
                release = line
                break
else:
    release = 'Unknown'

highlight_language = 'cpp'
primary_domain = 'cpp'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.

extensions = ['sphinx.ext.githubpages',
              #'sphinxcontrib.inlinesyntaxhighlight',
              'sphinx_inline_tabs',
              'sphinxext.opengraph',
              'sphinx_copybutton',
              'myst_parser',
              'breathe',
              'exhale']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'furo'
html_theme_path = ['_themes']
# html_logo = 'Branding/TRAPWhiteLogo.png'
html_favicon = 'Branding/TRAPWhiteLogo64x64.ico'
# html_theme_options = {
#     'analytics_id': 'UA-154200595-2',  #  Provided by Google in your dashboard
#     'display_version': True,
#     'prev_next_buttons_location': 'none',
#     'style_external_links': False,
# }
html_copy_source = False
html_show_sourcelink = False
html_use_opensearch = 'https://gamestrap.github.io/TRAP/'
html_show_sphinx = False

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
# html_css_files = ['custom.css']
# html_js_files = ['custom.js']

#Extension settings

#Furo
html_theme_options = {
    "light_logo": "TRAPDarkLogo.png",
    "dark_logo": "TRAPWhiteLogo.png",
    "sidebar_hide_name": True,
    "announcement": "This documentation is still WIP and may contain errors and/or outdated code!", #Remove this once website is in a usable state
    # "top_of_page_button": "edit",
    # "source_repository": "https://github.com/GamesTrap/TRAP",
    # "source_branch": "master",
    # "source_directory": ".modules/generatedocs",
}

#SphinxExt-OpenGraph
ogp_site_url = 'https://gamestrap.github.io/TRAP/'
ogp_site_name = 'TRAP Documentation'
ogp_image = '_static/TRAPWhiteLogo.png'

#MyST-Parser
myst_enable_extensions = [
    #"amsmath",
    "colon_fence",
    "deflist",
    #"dollarmath",
    "fieldlist",
    "html_admonition",
    "html_image",
    #"linkify",
    #"replacements",
    #"smartquotes",
    "strikethrough",
    "substitution",
    "tasklist",
]

#Breathe
breathe_projects = {
    "TRAP": "doxygen_docs/xml"
}
breathe_default_project = "TRAP"

#Exhale
exhale_args = {
    "containmentFolder": "./api",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "API Reference",
    "doxygenStripFromPath": "..",
    "createTreeView": True,
    #"contentsDirectives": False,
    "contentsSpecifiers": [":local:", ":backlink: none", ":class: this-will-duplicate-information-and-it-is-still-useful-here"],
}