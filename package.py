import zipfile
import os.path
import os
import sys
import json
import re
import subprocess
import winreg

""" creats a zip file release """

def get_version():
    exp = re.compile('project\s*\(.*VERSION\s+([0-9.]+)')
    with open('CMakeLists.txt') as fp:
        for line in fp:
            ret = exp.match(line)
            if ret:
                return ret.group(1)
    return None
    
    

def build_ue(plugin, ue_base, version, ue_install_path):

    out = ue_base + "\\" + version + "\\PeelVCam"

    exe = rf"{ue_install_path}\Engine\Build\BatchFiles\RunUAT.bat"

    if not os.path.isfile(exe) :      
        raise RuntimeError("Could not find unreal: " + version)
    
    cmd = [exe,
            "BuildPlugin",
            "-Plugin=" + plugin,
            "-TargetPlatforms=Win64",
            "-Package=" + out,
            "-Rocket",
            "-VS2022"
            ]

    try:
        pr = subprocess.Popen(cmd, shell=True)
        pr.wait()

        if pr.returncode != 0:
            raise RuntimeError("Plugin Build Failed")
    except Exception as ex:
        print(ex)


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
fp.close()

# Unreal    
unreal_src_plugin = os.path.abspath("plugins/unreal/Open3DStream/Open3DStream.uplugin")
unreal_dst_base = os.path.join(cwd, "plugins", "unreal")

ue_registry_prefix_path = r'SOFTWARE\EpicGames\Unreal Engine'
ue_registry_prefix_key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, ue_registry_prefix_path, 0, winreg.KEY_READ)
ue_version_index = 0
while True:
    try:
        ue_version = winreg.EnumKey(ue_registry_prefix_key, ue_version_index)
        ue_version_key = winreg.OpenKey(ue_registry_prefix_key, ue_version, 0, winreg.KEY_READ)
        ue_path, _ = winreg.QueryValueEx(ue_version_key, "InstalledDirectory")
        build_ue(unreal_src_plugin, unreal_dst_base, ue_version, ue_path)
        ue_version_index = ue_version_index + 1
    except OSError:
        break

raise RuntimeError("Here")

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
    