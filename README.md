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

Compare two json files
```Shell
jp a.json b.json [options]
```

Scan json files in directory then compare them in pairs
```Shell
jp dir [options]
```

## options

* `-c` case insensitive comparison.
* `-f` toggle (un)fold simple json array and object.
* `-s` select subelement to compare, like [jq](https://github.com/stedolan/jq)
