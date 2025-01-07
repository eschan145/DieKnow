"""Documentation parser from C++ code into Python.
"""

import os
import re

def doc(file_path, lib=None, markdown=False):
    """Parse a C++ file and generate its documentation."""

    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()

    pattern = r"(DK_API\s+.*?)(\w+)\s*\(.*?\)\s*\{(.*?)\}"
    functions = re.findall(pattern, content, re.DOTALL)

    docstrings = []

    for function in functions:
        signature = function[0].strip().replace("DK_API ", "")
        name = function[1]
        body = function[2]

        if markdown:
            print("Parsing function", name)

        docstring_match = re.search(r"/\*(.*?)\*/", body, re.DOTALL)
        if docstring_match:
            docstring = docstring_match.group(1).strip()

            lines = docstring.split("\n")
            stripped_lines = []
            prev_line_empty = False
            in_callout = False

            for line in lines:
                stripped_line = line.lstrip()
                if stripped_line.startswith(">"):
                    if not in_callout:
                        stripped_lines.append("\n")
                        in_callout = True
                    stripped_lines.append(stripped_line)
                elif stripped_line == "":
                    if not prev_line_empty:
                        stripped_lines.append("")
                        prev_line_empty = True
                else:
                    if in_callout:
                        stripped_lines.append(stripped_line)
                        in_callout = False
                    else:
                        stripped_lines.append(stripped_line)
                    prev_line_empty = False

            formatted_docstring = "\n".join(stripped_lines).strip().replace("  ", "\n\n")
            if markdown:
                formatted_docstring += "\n\n**Signature**: `" + signature + "`"
            else:
                formatted_docstring += "\n\nSignature: " + signature

            docstrings.append((name, formatted_docstring))

    if markdown:
        base_name = os.path.splitext(os.path.basename(file_path))[0]
        project_root = os.path.dirname(os.path.dirname(file_path))
        output_markdown = os.path.join(project_root, "docs", f"{base_name}.md")

        with open(output_markdown, "w", encoding="utf-8") as md_file:
            md_file.write(f"# Documentation for `{base_name}.cpp`\n\n")
            md_file.write("This documentation is automatically generated each push to GitHub.\n\n")
            for (name, docstring) in docstrings:
                md_file.write(f"### *function* `{name}()`\n\n{docstring}\n\n")

    for (name, docstring) in docstrings:
        if hasattr(lib, name):
            func = getattr(lib, name)
            func.__doc__ = docstring
