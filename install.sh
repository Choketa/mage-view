#!/usr/bin/bash

file=$(ls /usr/local/bin | grep -i mage)

if [[ -z "$file" ]]; then
	echo 'No installation found... installing'
else
	echo 'updating'
	rm /usr/local/bin/mage
fi

xlib=$(ls /usr/include/X11 | grep -i xlib.h)
xext=$(ls /usr/include/X11 | grep -i keysym.h)

if [[ -z "$xlib" ]]; then
	echo 'Xlib missing, aborting'
	exit 1
fi

if [[ -z "$xext" ]]; then
	echo 'Keysym missing, aborting'
	echo 'Please install/reinstall libXext'
	exit 1
fi

if make; then
	echo 'compiled'
else
	echo 'Couldnt build'
	exit 1
fi


if cp ./mage /usr/local/bin; then
	echo 'copied ./mage to /usr/local/bin/ what a deporation'
else
	echo 'Copying failed. Make sure to run this as sudo'
	exit 1
fi

if chmod +x /usr/local/bin/mage; then
	echo 'Chmod +x worked'
else
	echo 'Chmod failed, make sure to run this as sudo'
	exit 1
fi

echo 'Success!!!'

echo 'Make sure you have /usr/local/bin/ added to your $PATH pls'
echo 'Also, run mage <filename>'
echo 'only .jpg, .jpeg and .png filetypes work'
