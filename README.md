# **gxkb**

`X11` keyboard layout indicator and switcher

![screenshot 1](https://dl.dropboxusercontent.com/u/34413642/gxkb/gxkb%20screenshot.png "gxkb about 1")

![screenshot 2](https://dl.dropboxusercontent.com/u/34413642/gxkb/gxkb%20screenshot%200.png "gxkb about 2")
![screenshot 3](https://dl.dropboxusercontent.com/u/34413642/gxkb/gxkb%20screenshot%201.png "gxkb layouts")
![screenshot 4](https://dl.dropboxusercontent.com/u/34413642/gxkb/gxkb%20screenshot%202.png "gxkb menu")

## **Description**
`gxkb` is a tiny indicator applet which allows to quickly switch between different keyboard layouts in `X`.  
A flag corresponding to the country of the active layout is shown in the indicator area.  
The applet is written in `C` and uses `GTK+` library and therefore does not depend on any `GNOME` components.  

## **Dependencies**

* `GTK2`
* `libwnck22`
* `libxklavier16`

## **Installing**

### Debian

```bash
sudo apt-get install gxkb
```

### Ubuntu

```bash
sudo add-apt-repository ppa:zen-root/gxkb-stable
sudo apt-get update
sudo apt-get install gxkb
```

## **Building from source**

* Install dependencies

    + Debian

        ```bash
        sudo apt-get install libwnck-dev libxklavier-dev libgtk2.0-dev
        ```

* Build

    ```bash
    wget https://github.com/zen-tools/gxkb/archive/v0.7.7.tar.gz -O gxkb-0.7.7.tar.gz
    tar xzf gxkb-0.7.7.tar.gz
    cd gxkb-0.7.7
    ./configure && make && sudo make install
    ```

## **Usage**

* Show version and features

    ```bash
    gxkb -v
    ```

* Run from a terminal

    ```bash
    gxkb &
    ```

* Run from a `dmenu` or `gmrun`

    ```bash
    gxkb
    ```

## **Features**

* [AppIndicator](https://wiki.ubuntu.com/DesktopExperienceTeam/ApplicationIndicators) support

    To switch that off use the following command during building phase:

    ```bash
    ./configure --enable-appindicator=no && make && sudo make install
    ```

* Custom flags support

    Put your flag images in `.local/share/gxkb/flags` in PNG format  
    with the names like `<country code>.png`,
    e.g. `us.png`, `ru.png`, `ua.png`  
    and the sizes of 24x24 pixels each

* Scrolling support

    Switch layouts by scrolling while hovering over the flag

* Sensible defaults

    `Alt-Shift` to switch layouts, `Scroll Lock` led on to indicate alternate
    layouts.  
    Can be changed in `.config/gxkb/gxkb.cfg`.

## **Known issues**

* In Ubuntu with AppIndicator enabled there may be issues when the
  system layout switcher <code>indicator&#8209;keyboard</code> uses the same
  key combination.  
  One possible solution to this may be to assign an unused key combination for
  <code>indicator&#8209;keyboard</code>.  
  Another solution may be to remove the package
  <code>indicator&#8209;keyboard</code>, but that will also remove the Unity
  control center, which will be replaced by a Gnome control center.

* In Elementary OS Freya `gxkb` does not work. Trying to figure out why.

* When using more than one layout switcher during switching windows there may
  be incorrect behavior of layout switching.  
  In Gnome/Unity it can be solved by disabling the option of layout
  inheritance from parent window and disabling the option of splitting layout
  between windows in system layout switcher.
