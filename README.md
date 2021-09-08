<a id="top"></a>
[![Github Releases](https://img.shields.io/github/release/lingjf/jp.svg)](https://github.com/catchorg/jp/releases)
[![Selftest status](https://github.com/lingjf/jp/actions/workflows/ci.yml/badge.svg)](https://github.com/lingjf/jp/actions/workflows/ci.yml)

# jp
a json compare tool, extract from h2unit

## install

```Shell
    git clone https://github.com/lingjf/jp.git
    cmake .
    make
    make install
```

jp will be installed to `/usr/local/bin` (Linux/macOS) `C:\Windows\system32\` (Windows)

install directory is in `PATH` environment

## usage

```Shell
jp 1.json 2.json [options]
```

## options

* `-c` case insensitive comparison.
* `-f` fold simple json array and object, default is open, toggle fold/unfold.
* `-p` programming c/c++ source code json.
* `-s` select subelement to compare.
