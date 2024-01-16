import zipfile
import os.path
import os
import sys
import json
import re
import subprocess

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
    
print("Packacging version: " + version_input)

version  = "Open3DStream_" + version_input
cwd      = os.path.abspath('')
out_root = "Open3DStream"
outzip   = os.path.join(cwd, version + ".zip")
out_dir  = os.path.abspath('usr')

fp = zipfile.ZipFile(outzip, "w", zipfile.ZIP_DEFLATED)

for d, _, f in os.walk(out_dir):
    base = d[len(out_dir)+1:]
    for each_file in f:
        src = os.path.join(d, each_file)
        out = os.path.join(base, each_file)
        print(out)
        fp.write(src, out)

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
fp.write("usr/lib/open3dstreamstatic.lib", unreal_dst_base + "/lib/open3dstreamstatic.lib")

root = "usr/include"
for base, _, files in os.walk(root):
    for file in files:
        sub = base[len(root)+1:]
        src = os.path.join(base, file)
        dst = os.path.join(unreal_dst_base, "lib", "include", sub, file)
        print(dst)
        fp.write(src, dst)

fp.close()    
        
#installer

nsis = "C:/Program Files (x86)/NSIS/makensis.exe"
if not os.path.isfile(nsis):
    raise RuntimeError("Could not find NSIS: " + str(nsis))
    
nsis = '"' + nsis.replace("/", "\\") + '"'
nsi_script = os.path.join(os.path.split(__file__)[0], 'o3ds.nsi')

cmd = nsis + ' /DOUT_FILE=' + version + '.exe /DFILE_VERSION=' + version + ' ' + nsi_script

if subprocess.run(cmd).returncode != 0:
    raise RuntimeError("NSI Error")
    
if not os.path.isfile(version + ".exe"):
    raise RuntimeError("NISI did not produce: " + version + ".exe")
    