#!/usr/bin/env bash

dir=$(dirname $0)

echo "symlinking vim, bin, xmodmap"
ln -s $dir/Xmodmap $HOME/.Xmodmap
ln -s $dir/vim $HOME/.vim
ln -s $dir/vim/vimrc $HOME/.vimrc
ln -s $dir/bin $HOME/bin

echo "loading xmodmap config"
xmodmap ~/.Xmodmap

echo "setting solarized in gnome"
sh $dir/utils/gnome-terminal-colors-solarized-master/set_dark.sh

