import zipfile
import os.path
import os
import sys
import json
import re

""" creats a zip file release """

def get_version():
    exp = re.compile('#define\s+O3DS_VERSION\s+"([0-9.]+)"')
    with open('src/o3ds/o3ds.h') as fp:
        for line in fp:
            ret = exp.match(line)
            if ret:
                return ret.group(1)
    return None
    
    
version_input = get_version()
if version_input is None:
    raise RuntimeError("Could not determine version")
    
    

version = "Open3DStream_0." + version_input

cwd = os.path.abspath('')

out_root = "Open3DStream"

outzip = os.path.join(cwd, version + ".zip")

fp = zipfile.ZipFile(outzip, "w", zipfile.ZIP_DEFLATED)

release = os.path.abspath('build_release')

root = "usr"
for base, _, files in os.walk(root):
    sub = base[len(root)+1:]
    
    for file in files:
        src = os.path.join(base, file)
        dst = os.path.join(sub, file)
        print(dst)
        fp.write(src, dst)
        
fp.write("lib/flatbuffers.lib", "lib/flatbuffers.lib")
fp.write("lib/nng.lib", "lib/nng.lib")

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
    
fp.write("lib/nng.lib", unreal_dst_base + "/lib/nng.lib")
fp.write("lib/flatbuffers.lib", unreal_dst_base + "/lib/flatbuffers.lib")
fp.write("usr/lib/open3dstreamstatic.lib", unreal_dst_base + "/lib/open3dstreamstatic.lib")

for i in os.listdir("src/o3ds"):
    if i.endswith(".h"):
        dst = unreal_dst_base + "/lib/include/o3ds/" + i
        print(dst)
        fp.write("src/o3ds/" + i, dst)

root = "include"
for base, _, files in os.walk("include"):
    for file in files:
        sub = base[len(root)+1:]
        src = os.path.join(base, file)
        dst = os.path.join(unreal_dst_base, "lib", "include", sub, file)
        print(dst)
        fp.write(src, dst)

fp.close()    
        