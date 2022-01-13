project = 'Open3DStream'
copyright = '2022, Alastair Macleod'
author = 'Alastair Macleod'

extensions = [ "breathe", "exhale", "sphinx.ext.autosectionlabel", "sphinx.ext.graphviz", "sphinx.ext.autodoc"]
graphviz_output_format = "svg"
breathe_projects = { "o3ds": "../doc/xml" }
breathe_default_project = "o3ds"
autosectionlabel_prefix_document = True

primary_domain = "cpp"
highlight_language="cpp"

exhale_args = {
    "containmentFolder":     "./o3ds",
    "rootFileName":          "o3ds.rst",
    "rootFileTitle":         "Reference",
    "doxygenStripFromPath":  "..",
    "createTreeView":        True,
}

# templates_path = ['_templates']
# exclude_patterns = []
html_theme = 'sphinx_rtd_theme'
# html_static_path = ['_static']
breathe_default_members = ('members', 'undoc-members')