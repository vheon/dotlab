#!/usr/bin/env bash

dir=$(dirname $0)

echo "symlinking vim, bin, xmodmap"
ln -s $dir/Xmodmap $HOME/.Xmodmap
ln -s $dir/vim $HOME/.vim
ln -s $dir/vim/vimrc $HOME/.vimrc
ln -s $dir/bin $HOME/bin
ln -s $dir/utils $HOME/utils

echo
echo "loading xmodmap config"
xmodmap ~/.Xmodmap

echo
echo "setting solarized in gnome"
sh $dir/utils/gnome-terminal-colors-solarized-master/set_dark.sh

echo
echo "add \$HOME/bin to PATH"
echo "export PATH=$HOME/bin:$PATH" >> $HOME/.bashrc

echo "source $HOME/utils/util.sh" >> $HOME/.bashrc
echo "source $HOME/utils/netkit_util.sh" >> $HOME/.bashrc
echo
echo "re-source .bashrc"
source $HOME/.bashrc
