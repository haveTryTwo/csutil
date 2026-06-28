#!/bin/sh

ps aux | grep rpc_server_book | grep -v grep | grep -v vim | awk -F' ' '{print $2}' | xargs kill -9
