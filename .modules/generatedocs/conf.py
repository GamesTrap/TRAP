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


# -- Project information -----------------------------------------------------

project = 'TRAP'
copyright = '2019-2021 Jan "GamesTrap" Schürkamp All Rights Reserved'
author = 'Jan "GamesTrap" Schuerkamp'

# The full version, including alpha/beta/rc tags
release = '0.7.38'

highlight_language = 'cpp'
primary_domain = 'cpp'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.

extensions = ['sphinx.ext.githubpages',
              'sphinxcontrib.inlinesyntaxhighlight']

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
html_theme = 'sphinx_rtd_theme'
html_theme_path = ['_themes']
html_logo = 'Branding/TRAPWhiteLogo.png'
html_favicon = 'Branding/TRAPWhiteLogo64x64.ico'
html_theme_options = {
    'analytics_id': 'UA-154200595-2',  #  Provided by Google in your dashboard
    'display_version': True,
    'prev_next_buttons_location': 'none',
    'style_external_links': False,
}
html_copy_source = False
html_show_sourcelink = False
html_use_opensearch = 'https://gamestrap.github.io/TRAP/'
html_show_sphinx = False

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = ['theme.css', 'custom.css', 'rtd_dark.css']