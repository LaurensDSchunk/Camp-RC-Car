import os
from typing import Any

# These symbols are injected globally by PlatformIO
Import: Any
env: Any

Import("env")

print("Creating HTML Assets...")

assets_path = os.path.join(env.get("PROJECT_DIR"), "include/html")
output_header = os.path.join(env.get("PROJECT_DIR"), "include/html", "HTMLAssets.h")

header_content = "#ifndef HTML_ASSETS_H\n#define HTML_ASSETS_H\n\n// Automatically generated file. DO NOT EDIT.\n\n"

for asset in os.listdir(assets_path):
    print(asset)
    if not asset.endswith(".html"): continue

    with open(os.path.join(assets_path, asset), "r", encoding="utf-8") as file:
        define_name = asset.upper().replace(".", "_")
        file_content = file.read()
        header_content += f"const char* const {define_name} = R\"rawhtml({file_content})rawhtml\";\n"

header_content += "\n\n#endif"

with open(output_header, "w", encoding="utf-8") as f:
    f.write(header_content)