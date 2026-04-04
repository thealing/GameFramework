import os
import sys

asset_dir = sys.argv[1]
output_dir = sys.argv[2]

rc_file = os.path.join(output_dir, "resources.rc")
header_file = os.path.join(output_dir, "resources.h")

if not os.path.exists(output_dir):
	os.makedirs(output_dir)

assets = []
for root, dirs, files in os.walk(asset_dir):
	for file in files:
		full_path = os.path.join(root, file)
		rel_path = os.path.relpath(full_path, asset_dir).replace(os.sep, "\\\\")
		rc_path = os.path.relpath(full_path, output_dir).replace(os.sep, "\\\\")
		assets.append((rel_path, rc_path))

with open(rc_file, "w") as rc:
	for i, (rel_path, rc_path) in enumerate(assets):
		rc.write(f'{i + 1} RCDATA "{rc_path}"\n')

with open(header_file, "w") as h:
	h.write(f"#define ASSET_COUNT {len(assets)}\n\n")
	h.write("const char* g_asset_paths[ASSET_COUNT] = {\n")
	for rel_path, _ in assets:
		h.write(f'\t"{rel_path}",\n')
	h.write("};\n")