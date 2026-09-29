# Matching Windows Build and Baseline Install

This project currently runs reconstructed Spider-Man 2000 PC code through a Bink proxy DLL. The retail executable remains in place while selected functions are redirected into the rebuilt DLL.

## 1. Clone and select the development branch

```powershell
git clone https://github.com/legentus/spidey-decomp.git
cd spidey-decomp
git checkout dev
```

## 2. Build the matching Windows proxy

Run:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build_matching.ps1
```

The script will:

- download the preserved compiler/toolchain used by upstream CI to `C:\vs` if it is not already present;
- stamp the current Git commit into the runtime version string;
- run the existing `build.bat` / `spider.mak` matching build;
- stage the result as `out\matching\binkw32.dll`;
- copy symbols to `out\matching\spider.pdb` when produced;
- print the SHA-256 of the generated proxy.

Do not commit generated DLLs, PDBs, or retail game files.

## 3. Install against a retail Spider-Man 2000 PC directory

Replace the example path with the folder containing `SpideyPC.exe`:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install_dev_proxy.ps1 -GameDir "C:\Games\Spider-Man 2000"
```

On first install, the helper preserves the retail Bink DLL as:

```text
binkw32_.dll
```

and installs the rebuilt proxy as:

```text
binkw32.dll
```

## 4. Baseline launch

Launch `SpideyPC.exe` normally.

Expected proof that the reconstructed DLL executed:

- a console window is allocated;
- its title contains `spidey-decomp - <commit>`;
- startup prints `spidey-decomp starting <commit>`;
- runtime validation runs before patches are applied.

Do not make gameplay-source changes until this baseline launch is confirmed. If validation prints failures or the game crashes, preserve the entire console output and record the exact built DLL SHA-256.

## 5. Restore the retail DLL

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\restore_stock_bink.ps1 -GameDir "C:\Games\Spider-Man 2000"
```

This removes the active proxy and restores the preserved retail `binkw32.dll`.

## GitHub Actions status

The `dev` workflow is configured to build automatically and also supports manual dispatch. At the time this document was written, the fork had not reported any `dev` workflow runs yet. If the repository's Actions page shows workflows disabled for the new fork, enable them there; the local build path above does not depend on Actions.
