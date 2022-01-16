import zipfile
import os.path
import os
import sys
import json

if len(sys.argv) == 1 :
    print("Usage: package.py version")
    exit()

version_input = sys.argv[1]

version = "Open3DStream_" + sys.argv[1]

cwd = os.path.abspath('')

out_root = "Open3DStream"

outzip = os.path.join(cwd, version + ".zip")

fp = zipfile.ZipFile(outzip, "w", zipfile.ZIP_DEFLATED)

release = os.path.abspath('build_release')

# FbxStream
fp.write(os.path.join(release, "apps", "FbxStream", "FbxStream.exe"), "FbxStream\\FbxStream.exe")
fp.write(r"C:\Program Files\Autodesk\FBX\FBX SDK\2020.0.1\lib\vs2017\x64\release\libfbxsdk.dll", "FbxStream\\libfbxsdk.dll")

# Maya
for i in os.listdir(os.path.join(release, "plugins", "maya")):
    if not i.endswith(".mll"):
        continue        
    fp.write(os.path.join(release, "plugins", "maya", i), "maya/" + i)
    
# Mobu
for i in os.listdir(os.path.join(release, "plugins", "mobu")):
    if not i.endswith(".dll"):
        continue        
    fp.write(os.path.join(release, "plugins", "mobu", i), "mobu/" + i)
    

# Unreal    
unreal_base = os.path.abspath("plugins/unreal/Open3DStream")
for base, _, files in os.walk(unreal_base):
    sub = base[len(unreal_base)+1:]
    #if os.path.split(sub)[0] in ["Source", "Resources", "lib", "Content"]:
       
    for file in files: 
        if len(sub) == 0:
            if file.endswith(".uplugin"):
                print(os.path.join(base, file))
                fp.write(os.path.join(base, file), os.path.join("unreal", "Open3dStream", file))
            continue

        print(os.path.join(base, file))
        fp.write(os.path.join(base, file), os.path.join("unreal", "Open3dStream", sub, file))
    
fp.close()    