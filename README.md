# **gxkb**

`X11` keyboard layout indicator and switcher

![screenshot 3](https://zen-tools.github.io/gxkb/images/gxkb_tray_layouts.png "gxkb layouts")
![screenshot 4](https://zen-tools.github.io/gxkb/images/gxkb_tray_menu.png "gxkb menu")

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
    wget https://github.com/zen-tools/gxkb/archive/master.tar.gz -O gxkb.tar.gz
    tar xzf gxkb.tar.gz
    cd gxkb-master
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

* Using Scroll Lock led to indicate alternate layouts

    Can be changed in `.config/gxkb/gxkb.cfg`

## **Configuration**

Configuration is done via config file: `.config/gxkb/gxkb.cfg`

The most interesting options are:  
`layouts=us,ru,ua`  
`toggle_option=grp:alt_shift_toggle,grp_led:scroll,terminate:ctrl_alt_bksp`

Instead of `grp:alt_shift_toggle` you can use whatever the following command gives you:  
`grep grp:.*toggle /usr/share/X11/xkb/rules/base.lst`  

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
