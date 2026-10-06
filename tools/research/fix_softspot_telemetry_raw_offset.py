from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\research\add_softspot_hit_telemetry.py")
s=p.read_text(encoding="utf-8")
old="\tif (player)\n\t\tplayerWebMode =\n\t\t\t(int)player->field_8F8;"
new="\tif (player)\n\t\tplayerWebMode =\n\t\t\t(int)*(volatile unsigned char*)(\n\t\t\t\t(unsigned char*)player +\n\t\t\t\t0x8F8);"
if old not in s: raise SystemExit("old field_8F8 helper text not found")
p.write_text(s.replace(old,new,1),encoding="utf-8")
print("helper synced to raw +0x8F8 read")
