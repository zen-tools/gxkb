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

* Old, **stable** version:
    ```bash
    sudo apt-get install gxkb
    ```
    Run it like `gxkb & 2>/dev/null` due to an unfortunate bug of applet issuing extra warnings to `stdout`.

* Latest, **unstable** version:
    ```bash
    sudo apt-get -y install gxkb -t unstable
    ```
    Or, if you don't have `unstable` repo enabled:
    ```bash
    printf 'deb http://httpredir.debian.org/debian unstable main non-free contrib' > my_unstable.list
    sudo mv my_unstable.list /etc/apt/sources.list.d
    sudo apt-get update
    sudo apt-get -y install gxkb -t unstable
    sudo rm /etc/apt/sources.list.d/my_unstable.list
    sudo apt-get update
    ```

### Ubuntu
```bash
sudo add-apt-repository ppa:zen-root/gxkb-stable
sudo apt-get update
sudo apt-get install gxkb
```

### Arch

## **Building from source**

* Install dependencies
    + Debian

        ```bash
        sudo apt-get install libwnck-dev libxklavier-dev libgtk2.0-dev
        ```
* Build

    ```bash
    wget https://github.com/zen-tools/gxkb/archive/v0.7.7.tar.gz
    tar xvfz gxkb-0.7.7.tar.gz
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

