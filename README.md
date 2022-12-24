# **gxkb**
[![Latest release](https://img.shields.io/github/release/zen-tools/gxkb.svg)](https://github.com/zen-tools/gxkb/releases)

`X11` keyboard layout indicator and switcher

![screenshot 1](https://zen-tools.github.io/gxkb/images/gxkb_tray_layouts.png "gxkb layouts")
![screenshot 2](https://zen-tools.github.io/gxkb/images/gxkb_tray_menu.png "gxkb menu")

## **Description**
`gxkb` is a tiny indicator applet which allows to quickly switch between different keyboard layouts in `X`.
A flag corresponding to the country of the active layout is shown in the indicator area.
The applet is written in `C` and uses `GTK+` library and therefore does not depend on any `GNOME` components.

## **Dependencies**

* `GTK3`
* `libwnck-3`
* `libxklavier16`
* `libayatana-appindicator3`

## **Installing**

### Debian and Ubuntu

```bash
sudo apt-get install gxkb
```

### Fedora

```bash
sudo dnf install gxkb
```

## **Building from source**

* Install dependencies

    + Debian

        ```bash
        sudo apt install libwnck-3-dev libxklavier-dev libgtk-3-dev dh-autoreconf dh-make devscripts fakeroot
        ```
        For AppIndicator support:
        ```bash
        sudo apt install libayatana-appindicator3-dev
        ```

* Build

    ```bash
    wget https://github.com/zen-tools/gxkb/archive/master.tar.gz -O gxkb.tar.gz
    tar xzf gxkb.tar.gz
    cd gxkb-master
    ./autogen.sh
    ./configure
    ```
    For AppIndicator support:
    ```bash
    ./configure --enable-appindicator=yes
    ```
    Then:
    ```bash
    make && sudo make install
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
    ./configure --enable-appindicator=no
    ```

* Custom flags support

    Put your flag images in `.local/share/gxkb/flags` in PNG format
    with the names like `<country code>.png`,
    e.g. `ua.png`, `pl.png`, `lt.png`
    and the sizes of 24x24 pixels each

* Scrolling support

    Switch layouts by scrolling while hovering over the flag

* Using Scroll Lock led to indicate alternate layouts

    Can be changed in `.config/gxkb/gxkb.cfg`

## **Configuration**

Configuration is done via config file: `.config/gxkb/gxkb.cfg`

The most interesting options are:
`layouts=ua,pl,lt`
`toggle_option=grp:alt_shift_toggle,grp_led:scroll,terminate:ctrl_alt_bksp`

Instead of `grp:alt_shift_toggle` you can use whatever the following command gives you:
`grep grp:.*toggle /usr/share/X11/xkb/rules/base.lst`

## **Known issues**

* Missing flags:  
  **Q**: _`gxkb` shows a white flag with "?" character instead of the flag of my country._  
  **A**: That means `gxkb` fails to find an image under `/usr/share/gxkb/flags/` directory.
  If the flag does not belong to the terrorist state, you can create a merge request
  or issue with proposition to add the flag into distributive. Also, you can
  override missing/system flags with your own images.

* Custom flags:  
  **Q**: _How can I override the system flags with my own?_  
  **A**: You can place your own flags under `$XDG_DATA_HOME/gxkb/flags` directory.
  If the `XDG_DATA_HOME` environment variable is not defined,
  use `$HOME/.local/share/gxkb/flags` instead.

* In Gnome2/Gnome3, Unity, E17, possibly in KDE3/KDE4:  
  **Q**: _The layout does not get changed properly while switching between windows._  
  **A**: In your DE settings find keyboard layout control settings, disable
  the inheriting of the layouts from parent window and disable splitting
  layouts between windows.

* In Gnome3/Unity:  
  **Q**: _The layout icon is not displayed in system tray area._  
  **A**: Due to different versions of Gnome3 there is no easy answer, Google
  might help to find the right one.
  But in fact `gxkb` works under the hood, so you can use the Gnome3/Unity
  system indicators for icon displaying, just don't forget to disable the
  splitting layouts between different windows.

* In XFCE 4.12:  
  **Q**: _The layout icon is not displayed in system tray area._  
  **A**: In "sessions and startup" settings try to find and disable
  <code>indicator&#8209;application&#8209;service</code>.
  More details [here](https://forum.xfce.org/viewtopic.php?pid=32908#p32908).

* In Unity + AppIndicator:  
  **Q**: _The layout switching does not work._  
  **A**: It can happen when the system layout switcher
  <code>indicator&#8209;keyboard</code> uses the same key combination.
  One possible solution to this may be to assign an unused key combination
  for <code>indicator&#8209;keyboard</code>.
  Another solution may be to remove the package
  <code>indicator&#8209;keyboard</code>, but that will also remove the Unity
  control center, which will be replaced by a Gnome control center.
