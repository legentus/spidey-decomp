# Spider-Man 2000 Dev Build — BAT Workflow

The normal user workflow is now BAT-first. The PowerShell scripts remain underneath as implementation helpers, but you should not need to type PowerShell commands manually.

## First-time setup

Run:

```text
SETUP_FIRST_TIME.bat
```

Enter the folder containing `SpideyPC.exe`. The path is saved locally in:

```text
spidey_local_config.bat
```

That file is ignored by Git.

## Update your local project

Run:

```text
UPDATE_PROJECT.bat
```

It fetches and fast-forwards your local `dev` branch from:

```text
https://github.com/legentus/spidey-decomp
```

It intentionally refuses to overwrite tracked local modifications.

## Build

Run:

```text
BUILD_DEV.bat
```

This uses the preserved matching Windows toolchain and stages:

```text
out\matching\binkw32.dll
out\matching\spider.pdb
```

when symbols are produced.

## Build + install

Run:

```text
BUILD_AND_INSTALL.bat
```

This builds the current source and safely installs the proxy into the configured retail game folder.

On first install:

```text
binkw32.dll   -> binkw32_.dll   (preserved retail original)
new proxy     -> binkw32.dll
```

## Run

Run:

```text
RUN_GAME.bat
```

A `spidey-decomp` console should appear if the proxy loads successfully.

## Restore stock

Run:

```text
RESTORE_STOCK_GAME.bat
```

This restores the preserved retail Bink DLL.

## Menu

You can also use:

```text
SPIDEY_DEV_MENU.bat
```

for a simple numbered menu covering setup, update, build, install, run, and restore.

## Recommended normal loop

```text
UPDATE_PROJECT.bat
        ↓
BUILD_AND_INSTALL.bat
        ↓
RUN_GAME.bat
```

That is the intended day-to-day development workflow.
