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
copyright = '2019-2024 Jan "GamesTrap" Schürkamp. All Rights Reserved.'
author = 'Jan "GamesTrap" Schuerkamp'

# The full version, including alpha/beta/rc tags
#Retrieve version number from Base.h
basedotHPath = '../../TRAP/src/Core/Base.h'
basedothExists = exists(basedotHPath)
if basedothExists:
    with open(basedotHPath) as f:
        for line in f:
            if line.startswith('inline constexpr u32 TRAP_VERSION = TRAP_MAKE_VERSION<'):
                #Remove 'inline constexpr u32 TRAP_VERSION = TRAP_MAKE_VERSION<'
                line = line[len('inline constexpr u32 TRAP_VERSION = TRAP_MAKE_VERSION<'):]
                #Remove '>();\n' from end
                line = line[:-len('>();\n')]
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
              'sphinx_design',
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
#     'analytics_id': '',  #  Provided by Google in your dashboard
#     'display_version': True,
#     'prev_next_buttons_location': 'none',
#     'style_external_links': False,
# }
html_copy_source = False
html_show_sourcelink = False
html_use_opensearch = 'https://gamestrap.github.io/TRAP-Docs/'
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
    "footer_icons": [
        {
            "name": "GitHub",
            "url": "https://github.com/GamesTrap/TRAP",
            "html": """
                <svg stroke="currentColor" fill="currentColor" stroke-width="0" viewBox="0 0 16 16">
                    <path fill-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68 0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.013 8.013 0 0 0 16 8c0-4.42-3.58-8-8-8z"></path>
                </svg>
            """,
            "class": "",
        },
    ],
}

#SphinxExt-OpenGraph
ogp_site_url = 'https://gamestrap.github.io/TRAP-Docs/'
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
    "doxygenStripFromPath": "../../",
    "createTreeView": True,
    #"contentsDirectives": False,
    "contentsSpecifiers": [":local:", ":backlinks: none", ":class: this-will-duplicate-information-and-it-is-still-useful-here"],
}
