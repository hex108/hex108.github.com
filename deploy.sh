#!/bin/bash

set -e
set -u
#set -x

# Now we are at 'raw' branch
CUR_DIR=$(cd "$(dirname "$0")";pwd)
# Check if all content has been committed
[[ -n $(git status -s) ]] && echo 'This branch is not clean, exiting...' && exit 255
# Get latest git commit message
latestGitMessage=$(git log --pretty=format:%s HEAD -1)
# Generate blog using hugo
hugo --theme=even --baseUrl="http://hex108.github.io/"

# Push blog content under `public` directory to 'master' branch
TMP_DIR=~/test
BLOG="hex108.github.com"
cd $TMP_DIR
[ -d $BLOG ] &&	rm -rf $BLOG
git clone https://github.com/hex108/$BLOG
cp -a $CUR_DIR/public/* $TMP_DIR/$BLOG
cd $BLOG
git add .
git commit -am "$latestGitMessage"
git push origin master
