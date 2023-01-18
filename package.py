import zipfile
import os.path
import os
import sys
import json
import re

""" creats a zip file release """

def get_version():
    exp = re.compile('project\s*\(.*VERSION\s+([0-9.]+)')
    with open('CMakeLists.txt') as fp:
        for line in fp:
            ret = exp.match(line)
            if ret:
                return ret.group(1)
    return None
    
    
version_input = get_version()
if version_input is None:
    raise RuntimeError("Could not determine version")
    
print(version_input)

exit()

version = "Open3DStream_0." + version_input

cwd = os.path.abspath('')

out_root = "Open3DStream"

outzip = os.path.join(cwd, version + ".zip")

fp = zipfile.ZipFile(outzip, "w", zipfile.ZIP_DEFLATED)

out_dir = os.path.abspath('out')

for d, _, f in os.walk(out_dir):
    base = d[len(out_dir)+1:]
    for each_file in f:
        src = os.path.join(d, each_file)
        out = os.path.join(base, each_file)
        print(out)
        fp.write(src, out)


fp.write(os.path.join(cwd, "usr", "lib", "flatbuffers.lib"), "lib/flatbuffers.lib")
fp.write(os.path.join(cwd, "usr", "lib", "nng.lib"), "lib/nng.lib")

# Unreal    
unreal_src_base = os.path.abspath("plugins/unreal/Open3DStream")
unreal_dst_base = os.path.join("plugins/unreal", "Open3DStream")
for base, _, files in os.walk(unreal_src_base):
    sub = base[len(unreal_src_base)+1:]
    #if os.path.split(sub)[0] in ["Source", "Resources", "lib", "Content"]:
       
    for file in files: 
        if len(sub) == 0:
            if file.endswith(".uplugin"):
                dst = os.path.join(unreal_dst_base, file)
                print(dst)
                fp.write(os.path.join(base, file), dst)
            continue

        src = os.path.join(base, file)
        dst = os.path.join(unreal_dst_base, sub, file)
        print(dst)
        fp.write(src, dst)
    
fp.write("usr/lib/nng.lib", unreal_dst_base + "/lib/nng.lib")
fp.write("usr/lib/flatbuffers.lib", unreal_dst_base + "/lib/flatbuffers.lib")
fp.write("out/lib/open3dstreamstatic.lib", unreal_dst_base + "/lib/open3dstreamstatic.lib")

for i in os.listdir("out/include/o3ds"):
    if i.endswith(".h"):
        dst = unreal_dst_base + "/lib/include/o3ds/" + i
        print(dst)
        fp.write("src/o3ds/" + i, dst)

#root = "usr/include"
#for base, _, files in os.walk(root):
#    for file in files:
#        sub = base[len(root)+1:]
#        src = os.path.join(base, file)
#        dst = os.path.join(unreal_dst_base, "lib", "include", sub, file)
#        print(dst)
#        fp.write(src, dst)

fp.close()    
        