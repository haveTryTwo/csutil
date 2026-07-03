#!/bin/sh

ps aux | grep rpc_ | grep -v grep | grep -v vim | awk -F' ' '{print $2}' | xargs -r kill -9
