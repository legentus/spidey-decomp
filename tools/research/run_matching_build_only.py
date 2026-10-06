import os,subprocess,sys
repo=r"F:\Spider-Man 2000 Recomp\project main"
env=os.environ.copy()
env["SPIDEY_MSVC_ROOT"]=r"C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS"
env["SPIDEY_FORCE_CLEAN"]="1"
r=subprocess.run(["cmd.exe","/d","/c","build.bat"],cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
print(r.stdout)
sys.exit(r.returncode)
