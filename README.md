# Galatea Task Manager
## Dependencies
- ncurses

## Compile and install

To build Galatea, run:
```
make
```

To install Galatea in `~/.local/bin`, run:
```
make install
```

To uninstall Galatea from `~/.local/bin`, run:
```
make uninstall
```

Ensure that `~/.local/bin` is present in your `PATH` environment variable.

## Usage

To open the Galatea Text User Interface, simply run:

```
galatea
```
![galateaTUI](pics/TUI.png)

## How are tasks stored?

Tasks are stored in plain text in `~/.config/galatea/`.
