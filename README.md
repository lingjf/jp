# jp
a json compare tool, extract from h2unit

## install

```Shell
    git clone https://github.com/lingjf/jp.git
    cmake .
    make
    make install
```

jp will be installed to */usr/local/bin*

## usage

jp 1.json 2.json [options]

## options

* `-c` case insensitive comparison.
* `-f` fold simple json array and object, default is open, toggle fold/unfold.
* `-p` programming c/c++ source code json.
* `-s` select subelement to compare.
