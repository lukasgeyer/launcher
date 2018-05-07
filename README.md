# Launcher
A minimalist, yet powerful, search bar based launcher.

## Overview
Lauchner allows for browsing through a user-defined collection of items using a minimalist search bar. Items can be grouped to share a set of common attributes and a search bar allows for tag-based, kind-of boolean expression filtering, which can be used to narrow down the desired item.

## Configuration
Launcher is configured using at least one file, `launcher.xml`. The file is expected to be at the same location as the executable, but any other location can be specified using the proper command line parameter (see below). The basic building block is an `item`, which has a `name`, an `url` and optionally one or more `tag` elements. In addition, it may have a `color` element, which can be used to determine the color that should be used to display that item. One or more `item` elements can be grouped using a `group` element, which may have its own `tag` and `color` elements. Each item in that group will then share those attributes. For the `tag` element this relation is cumulative, for `color` it is absolute (so a color specified on an item will override the color specified on a group).

The `url` element of an item may also contain a positional (`{0}, {1}, {2}, ...`) or match-all wildcard (`{*}`), which can be used to pass information from the search bar to an item. Be aware that parameters are used to filter matching items, so an item with _n_ parameters will only be shown if _n_ parameters are provided. This feature can be used to create items with the same name, but a different URL with different amounts of parameters (in which case only one of them will be displayed).

The configuration file may also contain one or more `import` elements, which can be used to include one or more additional configuration files, each specified using a `file` element. If one of the files is modified it will be reloaded automatically. If a file failed to load it will be retried at a later time.

The position of the search bar can be configured by holding down the shift key. If the cursor is placed in the upper left corner of the search bar click-and-drag can be used to resize the bar.

Any error during operation will be indicated with an exclamation mark in the upper right corner. The exclamation mark will vanish if the error condition is no longer present. A click on the mark will manually dismiss the error.

### Example
```
<?xml version="1.0" encoding="UTF-8"?>
<items>
   <group>
      <name>Launcher</name>
      <tag>Launcher</tag>
      <color>#0000ff</color>
      <item>
         <name>Code</name>
         <url>https://github.com/lukasgeyer/launcher</url>
      </item>
      <item>
         <name>Issues</name>
         <url>https://github.com/lukasgeyer/launcher/issues</url>
      </item>
      <item>
         <name>Issues</name>
         <url>https://github.com/lukasgeyer/launcher/issues/{0}</url>
      </item>
      <item>
         <name>Qt</name>
         <url>https://www.qt.io/</url>
         <tag>Environment</tag>
         <color>green</color>
      </item>
   </group>
   <item>
      <name>GitHub</name>
      <url>https://github.com/search?q={*}</url>
   </item>
   <import>
      <file>additional.xml</file>
   </import>
</items>
```

### Commmand Line
The following options can be supplied on the command line:

| Option            | Description                                                    |
|-------------------|----------------------------------------------------------------|
| `--help`          | Displays the help text.                                        |
| `--version`       | Displays the version.                                          |
| `--source <file>` | Use `<file>` as configuration file (instead of `launcher.xml`) |

## Searching
The key sequence `STRG + SHIFT + Space` will bring up the search bar, which allows for entering a series of space-separated words used to define the (case-insensitive) filter expression. The first word is always treated as the name or the tag of an item that should be displayed. Any subsequent word will be treated as parameter to that item. If a word starts with `:` it is in any case interpreted as name, if it starts with `@` as a tag. The words `and`, `or` and `not` can be used to formulate a kind-of boolean expression (to satisfy the developer in all of us `&&`, `||` and `!` can be used alternatively). If no operator is supplied `and` will be assumed. Any word following those words will be also interpreted as name or the tag of an item, not a parameter.

A name or tag is treated as a regular expression. For convenience, the wildcards `?` and `*` can also be used to match a single or any character.

If the search expression yields at least one result a click on the item will open the related URL. Alternatively the up/down as well as the page-up/-down key can be used to navigate through the result list and the `Enter` can be used to open the related URL. If no item is selected in the list `SHIFT+Enter` will open any URL in the list.

### Example
| Expression            | Description                                                                                      |
|-----------------------|--------------------------------------------------------------------------------------------------|
| `GitHub`              | Matches any item with the name or a tag starting with 'GitHub'                                   |
| `Code @Launcher`      | Matches any item with the name or a tag starting with 'Code' and a tag starting with 'Launcher'  |
| `:Code @Launcher`     | Matches any item with the name starting with 'Code' and a tag starting with 'Launcher'           |
| `GitHub or @Launcher` | Matches any item with the name or a tag starting with 'GitHub' or a tag starting with 'Launcher' |
| `Issues 12`           | Matches any item with the name or a tag starting with 'Issues' which has one parameter           |

## Contributing
### Prerequisites
- A C++14 capable compiler (GCC, Clang, MSVC2015 or alike).
- Qt 5.6+

### Building
- qmake
- make / nmake / jom

### Contributing
Please stick to the local style if you expect your pull request to be accepted.

### License
Launcher is released under the GPLv3.
