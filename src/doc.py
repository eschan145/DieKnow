"""Documentation parser from C++ code into Python.
"""

import re

def doc(file_path, lib):
    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()

    pattern = r"(DK_API\s+.*?)(\w+)\s*\(.*?\)\s*\{(.*?)\}"
    functions = re.findall(pattern, content, re.DOTALL)

    docstrings = []

    for function in functions:
        signature = function[0].strip().replace("DK_API ", "")
        name = function[1]
        body = function[2]

        docstring_match = re.search(r"/\*(.*?)\*/", body, re.DOTALL)
        if docstring_match:
            docstring = docstring_match.group(1).strip()

            lines = docstring.split("\n")
            stripped_lines = []
            prev_line_empty = False

            for line in lines:
                stripped_line = line.lstrip()
                if stripped_line == "":
                    if not prev_line_empty:
                        stripped_lines.append("")
                        prev_line_empty = True
                else:
                    stripped_lines.append(stripped_line)
                    prev_line_empty = False

            formatted_docstring = " ".join(stripped_lines).strip().replace("  ", "\n\n")
            formatted_docstring += "\n\nSignature: " + signature;

            docstrings.append((name, formatted_docstring))

    for (name, docstring) in docstrings:
        if hasattr(lib, name):
            func = getattr(lib, name)
            func.__doc__ = docstring
