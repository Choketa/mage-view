#!/usr/bin/bash

make

mv ./mage /usr/local/bin/
echo 'moved ./mage to /usr/local/bin/ what a deporation'

chmod +x /usr/local/bin/mage
echo 'chmod +x the damn file'

echo 'donzo'

echo 'Make sure you have /usr/local/bin/ added to your $PATH pls'
echo 'Also, run mage <filename>'
echo 'only .jpg, .jpeg and .png filetypes work'
